/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <string.h>
#include <mama/mama.h>
#include <subscriptionimpl.h>
#include <transportimpl.h>
#include <msgimpl.h>
#include <queueimpl.h>
#include <wombat/queue.h>
#include "qpidbridgefunctions.h"
#include "transport.h"
#include "qpidcommon.h"
#include "qpiddefs.h"
#include "publisher.h"
#include "endpointpool.h"


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
qpidBridgeMamaSubscription_create (subscriptionBridge* subscriber,
                                   const char*         source,
                                   const char*         symbol,
                                   mamaTransport       tport,
                                   mamaQueue           queue,
                                   mamaMsgCallbacks    callback,
                                   mamaSubscription    subscription,
                                   void*               closure)
{
    qpidSubscription*       impl        = NULL;
    qpidTransportBridge*    transport   = NULL;
    mama_status             status      = MAMA_STATUS_OK;
    pn_data_t*              data        = NULL;
    const char*             uuid        = NULL;
    const char*             outgoingAddress = NULL;

    if ( NULL == subscriber || NULL == subscription || NULL == tport )
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "qpidBridgeMamaSubscription_create(): something NULL");
        return MAMA_STATUS_NULL_ARG;
    }

    status = mamaTransport_getBridgeTransport (tport,
                                               (transportBridge*) &transport);

    if (MAMA_STATUS_OK != status || NULL == transport)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "qpidBridgeMamaSubscription_create(): something NULL");
        return MAMA_STATUS_NULL_ARG;
    }

    outgoingAddress = qpidBridgeMamaTransportImpl_getOutgoingAddress (
                        (transportBridge) transport);
    uuid = qpidBridgeMamaTransportImpl_getUuid (
                        (transportBridge) transport);

    /* Allocate memory for qpid subscription implementation */
    impl = (qpidSubscription*) calloc (1, sizeof (qpidSubscription));
    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }

    mamaQueue_getNativeHandle(queue, &impl->mQpidQueue);
    impl->mMamaCallback        = callback;
    impl->mMamaSubscription    = subscription;
    impl->mMamaQueue           = queue;
    impl->mTransport           = (transportBridge) transport;
    impl->mClosure             = closure;
    impl->mIsNotMuted          = 1;
    impl->mIsTportDisconnected = 1;
    impl->mSubject             = NULL;

    /* Use a standard centralized method to determine a topic key */
    qpidBridgeCommon_generateSubjectKey (NULL,
                                         source,
                                         symbol,
                                         &impl->mSubject);

    /* Parse the collapsed string to extract the standardized values */
    qpidBridgeCommon_parseSubjectKey (impl->mSubject,
                                      &impl->mRoot,
                                      &impl->mSource,
                                      &impl->mTopic,
                                      impl->mTransport);

    /* Generate subject URI based on standardized values */
    qpidBridgeCommon_generateSubjectUri (outgoingAddress,
                                         impl->mRoot,
                                         impl->mSource,
                                         impl->mTopic,
                                         uuid,
                                         &impl->mUri);

    /* Register the endpoint */
    endpointPool_registerWithoutIdentifier (transport->mSubEndpoints,
                                            impl->mSubject,
                                            &impl->mEndpointIdentifier,
                                            impl);

    if (QPID_TRANSPORT_TYPE_P2P ==
                qpidBridgeMamaTransportImpl_getType (
                        impl->mTransport))
    {
        /* Notify the publisher that you have an interest in this topic */
        pn_message_clear        (transport->mMsg);

        /* Set the message meta data to reflect a subscription request */
        qpidBridgePublisherImpl_setMessageType (transport->mMsg,
                                                QPID_MSG_SUB_REQUEST);

        /* Set the outgoing address as provided by the transport configuration */
        pn_message_set_address  (transport->mMsg,
                                 transport->mOutgoingAddress);

        /* Set the reply address */
        pn_message_set_reply_to (transport->mMsg,
                                 transport->mReplyAddress);

        /* Get the proton message's body data for writing */
        data = pn_message_body  (transport->mMsg);

        /* Add in the subject key as the only string inside */
        pn_data_put_string      (data, pn_bytes (strlen (impl->mSubject),
                                                 (char*) impl->mSubject));

        /* Send out the subscription registration of interest message */
        if (NULL != transport->mOutgoingAddress)
        {
            pn_messenger_put    (transport->mOutgoing, transport->mMsg);

            if (0 != pn_messenger_send (transport->mOutgoing,
                    QPID_MESSENGER_SEND_TIMEOUT))
            {
                const char* qpid_error = PN_MESSENGER_ERROR (transport->mOutgoing);
                mama_log (MAMA_LOG_LEVEL_SEVERE,
                          "qpidBridgeMamaSubscription_create(): "
                          "pn_messenger_send Error:[%s]", qpid_error);
                return MAMA_STATUS_PLATFORM;
            }
        }
    }
    else
    {
        if (pn_messenger_subscribe (transport->mIncoming,
                                    impl->mUri) <= 0)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "qpidBridgeMamaSubscription_create(): "
                      "Error Subscribing to %s : %s",
                      impl->mUri,
                      PN_MESSENGER_ERROR(transport->mIncoming));
            return MAMA_STATUS_PLATFORM;
        }
    }

    mama_log (MAMA_LOG_LEVEL_FINEST,
              "qpidBridgeMamaSubscription_create(): "
              "created interest for %s.",
              impl->mSubject);

    /* Mark this subscription as valid */
    impl->mIsValid = 1;

    *subscriber =  (subscriptionBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaSubscription_createWildCard (subscriptionBridge*     subscriber,
                                           const char*             source,
                                           const char*             symbol,
                                           mamaTransport           transport,
                                           mamaQueue               queue,
                                           mamaMsgCallbacks        callback,
                                           mamaSubscription        subscription,
                                           void*                   closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaSubscription_mute (subscriptionBridge subscriber)
{
    qpidSubscription* impl = (qpidSubscription*) subscriber;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl->mIsNotMuted = 0;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaSubscription_destroy (subscriptionBridge subscriber)
{
    qpidSubscription*            impl            = NULL;
    qpidTransportBridge*         transportBridge = NULL;
    mamaSubscription             parent          = NULL;
    void*                        closure         = NULL;
    wombat_subscriptionDestroyCB destroyCb       = NULL;

    if (NULL == subscriber)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl            = (qpidSubscription*) subscriber;
    parent          = impl->mMamaSubscription;
    closure         = impl->mClosure;
    destroyCb       = impl->mMamaCallback.onDestroy;
    transportBridge = (qpidTransportBridge*) impl->mTransport;

    /* Remove the subscription from the transport's subscription pool. */
    if (NULL != transportBridge && NULL != transportBridge->mSubEndpoints
        && NULL != impl->mSubject)
    {
        endpointPool_unregister (transportBridge->mSubEndpoints,
                                 impl->mSubject,
                                 impl->mEndpointIdentifier);
    }

    if (NULL != impl->mMsg)
    {
        pn_message_free (impl->mMsg);
    }

    if (NULL != impl->mSubject)
    {
        free ((void*)impl->mSubject);
    }

    if (NULL != impl->mRoot)
    {
        free ((void*)impl->mRoot);
    }

    if (NULL != impl->mSource)
    {
        free ((void*)impl->mSource);
    }

    if (NULL != impl->mTopic)
    {
        free ((void*)impl->mTopic);
    }

    if (NULL != impl->mUri)
    {
        free ((void*)impl->mUri);
    }

    if (NULL != impl->mEndpointIdentifier)
    {
        free ((void*)impl->mEndpointIdentifier);
    }

    free (impl);

    /*
     * Invoke the subscription callback to inform that the bridge has been
     * destroyed.
     */
    if (NULL != destroyCb)
        (*(wombat_subscriptionDestroyCB)destroyCb)(parent, closure);

    return MAMA_STATUS_OK;
}

int
qpidBridgeMamaSubscription_isValid (subscriptionBridge subscriber)
{
    qpidSubscription* impl = (qpidSubscription*) subscriber;

    if (NULL != impl)
    {
        return impl->mIsValid;
    }
    return 0;
}

int
qpidBridgeMamaSubscription_hasWildcards (subscriptionBridge subscriber)
{
    return 0;
}

mama_status
qpidBridgeMamaSubscription_getPlatformError (subscriptionBridge subscriber,
                                             void** error)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

int
qpidBridgeMamaSubscription_isTportDisconnected (subscriptionBridge subscriber)
{
	qpidSubscription* impl = (qpidSubscription*) subscriber;
	if (NULL == impl)
	{
		return MAMA_STATUS_NULL_ARG;
	}
    return impl->mIsTportDisconnected;
}

mama_status
qpidBridgeMamaSubscription_setTopicClosure (subscriptionBridge subscriber,
                                            void* closure)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaSubscription_muteCurrentTopic (subscriptionBridge subscriber)
{
    /* As there is one topic per subscription, this can act as an alias */
    return qpidBridgeMamaSubscription_mute (subscriber);
}

