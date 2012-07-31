/* $Id: clientmanageresponder.c,v 1.13.4.1.6.1.2.5 2011/10/10 16:03:18 emmapollock Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "mama/mama.h"
#include "mama/clientmanage.h"
#include "subscriptionimpl.h"
#include "clientmanageresponder.h"
#include "list.h"
#include "syncresponder.h"
#include "bridge.h"
#include "transportimpl.h"
#include "publisherimpl.h"

/** The clients don't really pubish on this topic they simply use the 
 *  publisher for request reply. For WMW we don't want all the clients
 *  connecting to each other.
 */

/**
 * The CmResponder (Client Management Responder) provides the ability to
 * execute arbitrary commands upon receiving a message on MAMA_CM_TOPIC
 * ("__MAMA_CM").
 *
 * Currently it only responds to sync requests; however, in the future it may
 * respond to other commands as well. The sync request command implemenation
 * sends a message with all of the parameters required for the client to
 * respond to a sync command.
 *
 * Other commands could request stats from the clients, etc.
 */

typedef struct mamaCmResponderImpl_
{
	/* The number of sub transport bridges services by this responder. */
	int		numberTransportBridges;

	/* Array of publishers, one for each sub transport bridge. */
    	mamaPublisher    *publishers;
    
	/* Array of subscriptions, one for each sub transport bridge. */
	mamaSubscription *subscriptions;

	/* The actual transport. */
	mamaTransport    mTransport;    

	/* The list of pending commands from all subscriptions. */
    wList            mPendingCommands;
} mamaCmResponderImpl;


static void MAMACALLTYPE
createCB( mamaSubscription subscription, void* closure);
static void MAMACALLTYPE
errorCB( mamaSubscription subscription, mama_status status, 
         void* platform, const char* subject, void* closure);
static void MAMACALLTYPE
msgCB( mamaSubscription subscription, mamaMsg msg, void* closure, 
       void* itemClosure);
static void MAMACALLTYPE
destroyCB( mamaSubscription subscription, void* closure);

/**
 * Destroy the pending command
 */
static void commandDestroyCB (wList list, void *element, void *closure)
{
    mamaCommand *command = (mamaCommand*)element;
    /**
     * Since we don't know anything about the command, it provides a
     * destructor method.
     */
    command->mDtor(command->mHandle);
}

/*	Description	:	This function will destroy the cm responder including deleting the arrays of publishers
 *					and subscribers used when received and responding to sync requests.
 *	Arguments	:	responder [I] The responder to destroy.
 *	Returns		:	MAMA_STATUS_NULL_ARG - if the responder isn't valid.
 *					MAMA_STATUS_OK
 */
mama_status mamaCmResponder_destroy(mamaCmResponder responder)
{
	/* Returns */
    mama_status ret = MAMA_STATUS_NULL_ARG;

	/* Cast the responder to an impl. */
    mamaCmResponderImpl *impl = (mamaCmResponderImpl *)responder;
	if(impl != NULL)
	{		
		/* Publishers and subscribers are held for each sub transport bridge, each must be freed in
		 * turn before the corresponding arrays can be deleted.
		 * Note that these are enumerate separately in case something went wrong during creation and
		 * one array wasn't created successfully.
		 */
		ret = MAMA_STATUS_OK;

		/* Free the subscriptions. */
		if(impl->subscriptions != NULL)
		{
			/* Enumerate all subscribers. */
			mamaSubscription *nextSubscription	= impl->subscriptions;
			int nextBridgeIndex					= 0;
			for(; nextBridgeIndex<impl->numberTransportBridges; nextBridgeIndex++, nextSubscription++)
			{
				/* Free the subscription. */
				if(*nextSubscription != NULL)
				{
					/* Destroy the subscription, note that the first failure return code will be preserved.
					 */
					mama_status sd = mamaSubscription_destroyEx(*nextSubscription);
					if(ret == MAMA_STATUS_OK)
					{
						ret = sd;
					}
				}
			}

			/* Free the array itself. */
			free(impl->subscriptions);
			impl->subscriptions = NULL;
		}

		/* Free the publishers. */
		if(impl->publishers != NULL)
		{
			/* Enumerate all publishers. */
			mamaPublisher *nextPublisher	= impl->publishers;			
			int nextBridgeIndex				= 0;
			for(; nextBridgeIndex<impl->numberTransportBridges; nextBridgeIndex++, nextPublisher++)
			{				
				/* Free the publisher, note that the first failure return code will be preserved. */
				if(*nextPublisher != NULL)
				{
					mama_status pd = mamaPublisher_destroy(*nextPublisher);
					if(ret == MAMA_STATUS_OK)
					{
						ret = pd;
					}
				}
			}

			/* Free the array of publishers. */
			free(impl->publishers);
            impl->publishers = NULL;
		}

		/* Destroy all pending commands. */
		if(impl->mPendingCommands != NULL)
		{
			list_destroy(impl->mPendingCommands, commandDestroyCB, impl);
		}

		/* Free the impl itself. */
		free(impl);
	}

	return ret;
}

/*	Description	:	This function will populate the responder impl object by creating the arrays of publishers
 *					and subscribers. Note that one of each of these objects must be created for each of the sub
 *					transport bridges that are contained in the main transport. There will be multiple sub
 *					transports used when load balancing.
 *	Arguments	:	impl [I] The responder to populate.
 *	Returns		:	MAMA_STATUS_NO_BRIDGE_IMPL
 *					MAMA_STATUS_OK
 */
mama_status populateCmResponder(mamaCmResponderImpl *impl)
{
	/* Returns */
    mama_status ret = MAMA_STATUS_NO_BRIDGE_IMPL;
    mamaQueue internalQueue = NULL;

	/* Get the default event queue from the bridgeImpl. */
	mamaBridgeImpl *bridgeImpl = mamaTransportImpl_getBridgeImpl(impl->mTransport);
	if(bridgeImpl != NULL)
	{
		/* The same callbacks will be sent to each subscriber regardless of the bridge
		 * transport being used.
		 */
		mamaMsgCallbacks callbacks;
		memset(&callbacks, 0, sizeof(callbacks));
		callbacks.onCreate  = createCB;
		callbacks.onError   = errorCB;
		callbacks.onMsg     = msgCB;    
        callbacks.onDestroy = destroyCB;

        ret = mamaBridgeImpl_getInternalEventQueue((mamaBridge)bridgeImpl, &internalQueue);
		if (ret == MAMA_STATUS_OK)
		{
			/* Enumerate all the sub transport bridges in the transport. */
			int nextTransportIndex				= 0;
			mamaPublisher *nextPublisher		= impl->publishers;
			mamaSubscription *nextSubscription	= impl->subscriptions;	
			while((ret == MAMA_STATUS_OK) && (nextTransportIndex < impl->numberTransportBridges))
			{
				/* The publisher allows commands to respond to point to point requests
				 * without creating a new publisher, this must be created using the correct
				 * transport bridge.
				 */
				ret = mamaPublisher_createByIndex(
					nextPublisher,
					impl->mTransport,
					nextTransportIndex,
					MAMA_CM_PUB_TOPIC,
					NULL,
					NULL);
				if(ret == MAMA_STATUS_OK)
				{
					/* Allocate the subscription */
					ret = mamaSubscription_allocate(nextSubscription);
					if(ret == MAMA_STATUS_OK)
					{
						/* Set the subscription's transport index to ensure that the right transport is used. */
						mamaSubscription_setTransportIndex(*nextSubscription, nextTransportIndex);
			      
						/* Create the subscription */
						ret = mamaSubscription_createBasic(
							*nextSubscription, 
							impl->mTransport, 
							internalQueue, 
							&callbacks, 
							MAMA_CM_TOPIC, 
							impl);
                        if(ret == MAMA_STATUS_OK)
                        {
                            /* We don't want the CM subs to show up on the stats logger */
                            mamaSubscription_setLogStats(*nextSubscription, 0);
                        }

					}
				}

				/* Increment the counts for the next iteration */
				nextTransportIndex ++;
				nextPublisher ++;
				nextSubscription ++;
			}			
		}    
	}

	return ret;
}

/*	Description	:	This function will create the cm responder, the responder will listen for management messages from
 *					the currently active feed handlers. This is done by creating a publisher and a subscription for each
 *					sub transport bridge in the main transport.
 *	Arguments	:	responder				[O] To return the new responder.
 *					tport					[I] The main transport.
 *					numberTransportBridges	[I] The number of transport bridges.
 *	Returns		:	MAMA_STATUS_NOMEM
 *					MAMA_STATUS_OK
 */
mama_status mamaCmResponder_create(mamaCmResponder *responder, mamaTransport tport, int numberTransportBridges)
{
	/* Returns */
    mama_status ret = MAMA_STATUS_NOMEM;

	/* Allocate the impl structure. */
    mamaCmResponderImpl *impl = (mamaCmResponderImpl *)calloc (1, sizeof(mamaCmResponderImpl));
	if(impl != NULL)
	{		
		/* Save arguments in the structure. */
		impl->numberTransportBridges	= numberTransportBridges;
		impl->mTransport				= tport;

		/* Create the array of publisher pointers, one for each sub transport bridge. */
		impl->publishers = (mamaPublisher *)calloc(numberTransportBridges, sizeof(mamaPublisher));
		if(impl->publishers != NULL)
		{
			/* Create the array of basic subscriptions, one for each sub transport bridge. */
			impl->subscriptions = (mamaSubscription *)calloc(numberTransportBridges, sizeof(mamaSubscription));
			if(impl->subscriptions != NULL)
			{		
				/* Populate the arrays of publishers and subscriptions. */
				ret = populateCmResponder(impl);
				if(ret == MAMA_STATUS_OK)
				{
					/* Create the list which will contain commands that have not completed execution. */				 
					impl->mPendingCommands = list_create(sizeof(mamaCommand));
				}
			}
		}

		/* If something has gone wrong delete the impl structure. */
		if(ret != MAMA_STATUS_OK)
		{
			/* Destroy the responder. */
			mamaCmResponder_destroy((mamaCmResponder)impl);
			impl = NULL;

			/* Write a log message. */
			mama_log (MAMA_LOG_LEVEL_ERROR, "mamaCmResponder_create(): Could not"
                                         " create subscription.");     
		}
	}

	/* Write back the impl in the mamaCmResponder pointer. */
	*responder = (mamaCmResponder)impl;
    
    return ret;
}




static void MAMACALLTYPE
createCB( mamaSubscription subscription, void* closure)
{
    mama_log (MAMA_LOG_LEVEL_FINE, "mamaCmResponder::createCB(): "
              "Created CM subscription");
}

static void MAMACALLTYPE
errorCB (mamaSubscription subscription, mama_status status, 
         void* platform, const char* subject, void* closure)
{
    mama_log (MAMA_LOG_LEVEL_FINE, "mamaCmResponder::errorCB(): "
              "Error creating CM subscription %d\n", status);
}

/**
 * We pass this function to commands so they can tell the CmResponder to
 * remove them from the pending list and clean up when the command completes.
 */
static void 
endCB (mamaCommand *command, void *closure)
{
    mamaCmResponderImpl* impl = (mamaCmResponderImpl*) closure;
    list_remove_element (impl->mPendingCommands, command);
    command->mDtor (command->mHandle);
    list_free_element (impl->mPendingCommands, command);
}

static void MAMACALLTYPE
msgCB (mamaSubscription subscription, mamaMsg msg, void* closure, 
       void* itemClosure)
{
    mama_u16_t command_id = 0;
    mamaCmResponderImpl *impl = (mamaCmResponderImpl*)closure;	
    mamaCommand* command = 
        (mamaCommand*)list_allocate_element(impl->mPendingCommands);

	/* Get the transport index in use by the supplied subscription. */
	int transportIndex = 0;

    /*
     * The command ID field tells us what command to execute.
     *
     * Add new values for new commands.
     */
    mamaMsg_getU16 (msg, NULL, MAMA_CM_COMMAND_ID, &command_id);

    /* TODO: We can convert this into a command lookup for various Client
     * management/wadmin type commands.
     */
    switch (command_id)
    {
    case MAMA_COMMAND_SYNC:
		{
			mama_log (MAMA_LOG_LEVEL_FINE, "mamaCmResponder::msgCb(): "
					  "CM  SYNC Received" );
            mamaSubscription_getTransportIndex(subscription, &transportIndex);
			mamaSyncCommand_create (command, msg, impl->mTransport, transportIndex, impl->publishers[transportIndex], endCB, impl);
            list_push_back(impl->mPendingCommands, command);
			mamaSyncCommand_run (command);
		}
        break;
    default:
        list_free_element (impl->mPendingCommands, command);
        mama_log (MAMA_LOG_LEVEL_FINE, "mamaCmResponder::msgCB(): "
                  "Bad CM command: %d", command);
    }
}

static void MAMACALLTYPE
destroyCB (mamaSubscription subscription, void* closure)
{
    /* Deallocate the subscription. */
    mamaSubscription_deallocate(subscription);
}
