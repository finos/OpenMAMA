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
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <bridge.h>
#include <inboximpl.h>
#include <msgimpl.h>
#include "qpidbridgefunctions.h"
#include "transport.h"
#include "qpiddefs.h"
#include "msg.h"
#include "codec.h"
#include "inbox.h"
#include "subscription.h"
#include "publisher.h"
#include "endpointpool.h"

/*=========================================================================
 =                Typedefs, structs, enums and globals                   =
 =========================================================================*/

typedef struct qpidPublisherBridge
{
    qpidTransportBridge*    mTransport;
    const char*             mTopic;
    const char*             mSource;
    const char*             mRoot;
    const char*             mSubject;
    pn_message_t*           mQpidRawMsg;
    msgBridge               mMamaBridgeMsg;
} qpidPublisherBridge;

/*=========================================================================
 =                  Private implementation prototypes                    =
 =========================================================================*/

/**
 * This function takes a MAMA Message, transcodes it to a proton message for
 * distribution using the bridge codec functions and enqueues it for sending
 * in the outgoing transport messenger.
 *
 * @param msg   The MAMA message to enqueue for sending.
 * @param url   The URL to eneueue the message for sending to.
 * @param impl  The related qpid publisher bridge.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static void
qpidBridgePublisherImpl_enqueueMessageForAddress (mamaMsg               msg,
                                                  const char*           url,
                                                  qpidPublisherBridge*  impl);

/**
 * When a qpid publisher is created, it calls this function to generate a
 * standard subject key using qpidBridgeMamaSubscriptionImpl_generateSubjectKey
 * with different parameters depending on if it's a market data publisher,
 * basic publisher or a data dictionary publisher.
 *
 * @param msg   The MAMA message to enqueue for sending.
 * @param url   The URL to eneueue the message for sending to.
 * @param impl  The related qpid publisher bridge.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidBridgeMamaPublisherImpl_buildSendSubject (qpidPublisherBridge* impl);

/*=========================================================================
 =               Public interface implementation functions               =
 =========================================================================*/

mama_status
qpidBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                       mamaTransport        tport,
                                       int                  tportIndex,
                                       const char*          topic,
                                       const char*          source,
                                       const char*          root,
                                       void*                nativeQueueHandle,
                                       mamaPublisher        parent)
{
    qpidPublisherBridge*    impl        = NULL;
    qpidTransportBridge*    transport   = NULL;
    mama_status             status      = MAMA_STATUS_OK;

    transport = qpidBridgeMamaTransportImpl_getTransportBridge (tport);
    if (NULL == transport)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE,
                  "qpidBridgeMamaPublisher_createByIndex(): "
                  "Could not find transport.");
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (qpidPublisherBridge*) calloc (1, sizeof (qpidPublisherBridge));
    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_createByIndex(): "
                  "Could not allocate mem publisher.");
        return MAMA_STATUS_NOMEM;
    }

    /* Initialize the publisher members */
    impl->mTransport = transport;

    /* Create an underlying bridge message with no parent to be used in sends */
    status = qpidBridgeMamaMsg_create (&impl->mMamaBridgeMsg, NULL);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_createByIndex(): "
                  "Could not create qpid bridge message for publisher: %s.",
                  mamaMsgStatus_stringForStatus (status));
        free (impl);
        return MAMA_STATUS_NOMEM;
    }

    if (NULL != topic)
    {
        impl->mTopic = topic;
    }

    if (NULL != source)
    {
        impl->mSource = source;
    }

    if (NULL != root)
    {
        impl->mRoot = root;
    }

    /* Generate a topic name based on the publisher details */
    status = qpidBridgeMamaPublisherImpl_buildSendSubject (impl);

    /* Create a reusable proton message */
    impl->mQpidRawMsg = pn_message ();

    /* Populate the publisherBridge pointer with the publisher implementation */
    *result = (publisherBridge) impl;

    return status;
}

mama_status
qpidBridgeMamaPublisher_create (publisherBridge*    result,
                                mamaTransport       tport,
                                const char*         topic,
                                const char*         source,
                                const char*         root,
                                void*               nativeQueueHandle,
                                mamaPublisher       parent)
{
    return qpidBridgeMamaPublisher_createByIndex (result,
                                                  tport,
                                                  0,
                                                  topic,
                                                  source,
                                                  root,
                                                  nativeQueueHandle,
                                                  parent);
}

mama_status
qpidBridgeMamaPublisher_destroy (publisherBridge publisher)
{

    qpidPublisherBridge* impl = (qpidPublisherBridge*) publisher;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    if (NULL != impl->mQpidRawMsg)
    {
        pn_message_free (impl->mQpidRawMsg);
    }
    if (NULL != impl->mSubject)
    {
        free ((void*) impl->mSubject);
    }
    if (NULL != impl->mMamaBridgeMsg)
    {
        qpidBridgeMamaMsg_destroy (impl->mMamaBridgeMsg, 0);
    }

    free (impl);
    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    mama_status             status        = MAMA_STATUS_OK;
    qpidPublisherBridge*    impl          = (qpidPublisherBridge*) publisher;
    const char*             qpidError     = NULL;
    endpoint_t*             targets       = NULL;
    size_t                  targetCount   = 0;
    size_t                  targetInc     = 0;
    char*                   url           = NULL;
    qpidMsgType             type          = QPID_MSG_PUB_SUB;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_send(): No publisher.");
        return MAMA_STATUS_INVALID_ARG;
    }

    /* Get the bridge message type if specified already by inbox handlers */
    qpidBridgeMamaMsgImpl_getMsgType (impl->mMamaBridgeMsg, &type);

    switch (type)
    {
    case QPID_MSG_INBOX_REQUEST:
        /* Use the publisher's default send subject */
        qpidBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                          impl->mSubject,
                                          impl->mSource);

        /* Use the publisher's default send destination for request */
        qpidBridgePublisherImpl_enqueueMessageForAddress (
                msg,
                (char*) impl->mTransport->mOutgoingAddress,
                impl);
        break;
    case QPID_MSG_INBOX_RESPONSE:
        /* The url should already be set for inbox responses as the replyTo */
        qpidBridgeMamaMsgImpl_getDestination (impl->mMamaBridgeMsg, &url);

        /* Send out this message to the URL already provided */
        qpidBridgePublisherImpl_enqueueMessageForAddress (msg, url, impl);
        break;
    default:
        /* Use the publisher's default send subject */
        qpidBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                          impl->mSubject,
                                          impl->mSource);

        /* For each known downstream destination */
        status = endpointPool_getRegistered (impl->mTransport->mPubEndpoints,
                                             impl->mSubject,
                                             &targets,
                                             &targetCount);

        if (targetCount == 0)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST, "qpidBridgeMamaPublisher_send(): "
                      "No one subscribed to subject '%s', not publishing.",
                      impl->mSubject);
            return MAMA_STATUS_OK;
        }

        /* Push the message out to the send queue for each interested party */
        for (targetInc = 0; targetInc < targetCount; targetInc++)
        {
            url = (char*) targets[targetInc];
            qpidBridgePublisherImpl_enqueueMessageForAddress (msg, url, impl);
        }
        break;
    }

    /* Note the messages don't actually get published until here */
    if (PN_MESSENGER_SEND(impl->mTransport->mOutgoing))
    {
        qpidError = PN_MESSENGER_ERROR (impl->mTransport->mOutgoing);
        mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidBridgeMamaPublisher_send(): "
                  "Qpid Error:[%s]",
                  qpidError);

        return MAMA_STATUS_PLATFORM;
    }

    /* Reset the message type for the next publish */
    qpidBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg, QPID_MSG_PUB_SUB);

    return status;
}

/* Send reply to inbox. */
mama_status
qpidBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                          void*             request,
                                          mamaMsg           reply)
{
    qpidPublisherBridge*    impl            = (qpidPublisherBridge*) publisher;
    mamaMsg                 requestMsg      = (mamaMsg) request;
    const char*             inboxSubject    = NULL;
    const char*             replyTo         = NULL;
    msgBridge               bridgeMsg       = NULL;
    mama_status             status          = MAMA_STATUS_OK;

    if (NULL == publisher)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Set properties for the outgoing bridge message */
    qpidBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                      QPID_MSG_INBOX_RESPONSE);

    /* Target is for MD subscriptions to respond to this particular topic */
    qpidBridgeMamaMsgImpl_setTargetSubject (impl->mMamaBridgeMsg,
                                            impl->mSubject);

    /* Get the incoming bridge message from the mamaMsg */
    status = mamaMsgImpl_getBridgeMsg (requestMsg, &bridgeMsg);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get bridge message from cached"
                  " queue mamaMsg [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Get properties from the incoming bridge message */
    status = qpidBridgeMamaMsgImpl_getInboxName (bridgeMsg,
                                                 (char**) &inboxSubject);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get inbox name [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    status = qpidBridgeMamaMsgImpl_getReplyTo (bridgeMsg, (char**) &replyTo);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get reply to [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    if (NULL == inboxSubject)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "No reply address specified - cannot respond to inbox %s.",
                  inboxSubject);
        return status;
    }

    /* Set the send subject to publish onto the inbox subject */
    status = qpidBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                               inboxSubject,
                                               impl->mSource);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not set send subject '%s' [%s]",
                  inboxSubject,
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Set the destination to the replyTo URL */
    status = qpidBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg,
                                                   replyTo);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not set destination '%s' [%s]",
                  replyTo,
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Fire out the message to the inbox */
    return qpidBridgeMamaPublisher_send (publisher, reply);
}

mama_status
qpidBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                void*               inbox,
                                                mamaMsg             reply)
{
    qpidPublisherBridge*      impl           = (qpidPublisherBridge*) publisher;
    const char*               inboxSubject   = NULL;
    const char*               replyTo        = NULL;
    mama_status               status         = MAMA_STATUS_OK;

    if (NULL == publisher)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Set properties for the outgoing bridge message */
    qpidBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                      QPID_MSG_INBOX_RESPONSE);

    /* Target is for MD subscriptions to respond to this particular topic */
    qpidBridgeMamaMsgImpl_setTargetSubject (impl->mMamaBridgeMsg,
                                            impl->mSubject);

    /* Get properties from the incoming bridge message */
    status = qpidBridgeMamaMsgReplyHandleImpl_getInboxName (
                     inbox,
                     (char**) &inboxSubject);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get inbox name [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    status = qpidBridgeMamaMsgReplyHandleImpl_getReplyTo (
                                                inbox,
                                                (char**) &replyTo);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get reply to [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }


    if (NULL == inboxSubject || NULL == replyTo)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInboxHandle(): "
                  "No reply address specified - cannot respond to inbox %s.",
                  inboxSubject);
        return status;
    }

    /* Set the send subject to publish onto the inbox subject */
    status = qpidBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                               inboxSubject,
                                               impl->mSource);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInboxHandle(): "
                  "Could not set send subject '%s' [%s]",
                  inboxSubject,
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Set the destination to the replyTo URL */
    status = qpidBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg,
                                                   replyTo);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInboxHandle(): "
                  "Could not set destination '%s' [%s]",
                  replyTo,
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Fire out the message to the inbox */
    return qpidBridgeMamaPublisher_send (publisher, reply);
}

/* Send a message from the specified inbox using the throttle. */
mama_status
qpidBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                              int               tportIndex,
                                              mamaInbox         inbox,
                                              mamaMsg           msg)
{
    qpidPublisherBridge*    impl        = (qpidPublisherBridge*) publisher;
    const char*             replyAddr   = NULL;
    inboxBridge             inboxImpl   = NULL;
    mama_status             status      = MAMA_STATUS_OK;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the inbox which you want the publisher to respond to */
    inboxImpl = mamaInboxImpl_getInboxBridge (inbox);
    replyAddr = qpidBridgeMamaInboxImpl_getReplySubject (inboxImpl);

    /* Mark this as being a request from an inbox */
    status = qpidBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                               QPID_MSG_INBOX_REQUEST);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendFromInboxByIndex(): "
                  "Failed to set message type [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Update meta data in outgoing message to reflect the inbox name */
    qpidBridgeMamaMsgImpl_setInboxName (impl->mMamaBridgeMsg,
                                        replyAddr);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendFromInboxByIndex(): "
                  "Failed to set inbox name [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Update meta data in outgoing message to reflect the reply URL */
    qpidBridgeMamaMsgImpl_setReplyTo (impl->mMamaBridgeMsg,
                                      impl->mTransport->mReplyAddress);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendFromInboxByIndex(): "
                  "Failed to set reply to [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    return qpidBridgeMamaPublisher_send (publisher, msg);;
}

mama_status
qpidBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                       mamaInbox        inbox,
                                       mamaMsg          msg)
{
    return qpidBridgeMamaPublisher_sendFromInboxByIndex (publisher,
                                                         0,
                                                         inbox,
                                                         msg);
}

/*=========================================================================
 =                  Public implementation functions                      =
 =========================================================================*/

void
qpidBridgePublisherImpl_setMessageType (pn_message_t* message, qpidMsgType type)
{
    /* Get the properties */
    pn_data_t* properties = pn_message_properties (message);

    /* Ensure position is at the start */
    pn_data_rewind (properties);

    /* Main container should be a list to allow expansion */
    pn_data_put_list (properties);
    pn_data_enter (properties);

    /* Add the type */
    pn_data_put_ubyte (properties, type);

    pn_data_exit (properties);

    return;
}

/*=========================================================================
 =                  Private implementation functions                     =
 =========================================================================*/

void
qpidBridgePublisherImpl_enqueueMessageForAddress (mamaMsg               msg,
                                                  const char*           url,
                                                  qpidPublisherBridge*  impl)
{
    const char*     qpidError   = NULL;
    pn_message_t*   pnMsg       = NULL;

    if (NULL == impl || NULL == url)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgePublisherImpl_sendMessageToAddress(): "
                  "Null closure or url received.");
        return;
    }

    /* Make a copy of the pointer - qpidBridgeMamaMsgImpl_pack may modify */
    pnMsg = impl->mQpidRawMsg;

    /* Update the address with the current url */
    qpidBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg, url);

    /* Pack the provided MAMA message into a proton message */
    qpidBridgeMsgCodec_pack (impl->mMamaBridgeMsg,
                             msg,
                             &pnMsg);

    /* Enqueue the message in the messenger send queue */
    if (pn_messenger_put (impl->mTransport->mOutgoing, pnMsg))
    {
        qpidError = PN_MESSENGER_ERROR (impl->mTransport->mOutgoing);
        mama_log (MAMA_LOG_LEVEL_SEVERE,
                  "qpidBridgePublisherImpl_sendMessageToAddress(): "
                  "Qpid Error:[%s]",
                  qpidError);
        return;
    }

}

mama_status
qpidBridgeMamaPublisherImpl_buildSendSubject (qpidPublisherBridge* impl)
{
    char* keyTarget = NULL;

    /* If this is a special _MD publisher, lose the topic unless dictionary */
    if (impl->mRoot != NULL)
    {
        /*
         * May use strlen here to increase speed but would need to test to
         * verify this is the only circumstance in which we want to consider the
         * topic when a root is specified.
         */
        if (strcmp (impl->mRoot, "_MDDD") == 0)
        {
            qpidBridgeMamaSubscriptionImpl_generateSubjectKey (impl->mRoot,
                                                               impl->mSource,
                                                               impl->mTopic,
                                                               &keyTarget);
        }
        else
        {
            qpidBridgeMamaSubscriptionImpl_generateSubjectKey (impl->mRoot,
                                                               impl->mSource,
                                                               NULL,
                                                               &keyTarget);
        }
    }
    /* If this isn't a special _MD publisher */
    else
    {
        qpidBridgeMamaSubscriptionImpl_generateSubjectKey (NULL,
                                                           impl->mSource,
                                                           impl->mTopic,
                                                           &keyTarget);
    }

    /* Set the subject for publishing here */
    impl->mSubject = keyTarget;

    return MAMA_STATUS_OK;
}
