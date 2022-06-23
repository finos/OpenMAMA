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

#include <stdint.h>
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
#include "codec.h"
#include "publisher.h"
#include "mama/integration/endpointpool.h"
#include "qpidcommon.h"
#include <proton/connection.h>
#include <mama/integration/bridge/base.h>

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
    const char*             mUri;
    pn_message_t*           mQpidRawMsg;
    msgBridge               mMamaBridgeMsg;
    mamaPublisher           mParent;
    mamaPublisherCallbacks  mCallbacks;
    void*                   mCallbackClosure;
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
qpidBridgePublisherImpl_enqueueMessageForAddress (mamaMsg              msg,
                                                  const char*          url,
                                                  qpidPublisherBridge* impl,
                                                  qpidP2pEndpoint*     endpoint);


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
                                       mamaPublisher        parent)
{
    qpidPublisherBridge*    impl            = NULL;
    qpidTransportBridge*    transport       = NULL;
    mama_status             status          = MAMA_STATUS_OK;
    const char*             outgoingAddress = NULL;
    const char*             uuid            = NULL;

    if (NULL == result || NULL == tport || NULL == parent)
    {
        return MAMA_STATUS_NULL_ARG;
    }

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
    impl->mParent    = parent;

    /* Create an underlying bridge message with no parent to be used in sends */
    status = baseBridgeMamaMsgImpl_createMsgOnly (&impl->mMamaBridgeMsg);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_createByIndex(): "
                  "Could not create qpid bridge message for publisher: %s.",
                  mamaStatus_stringForStatus (status));
        free (impl);
        return MAMA_STATUS_NOMEM;
    }

    /* Get the outgoing address format string */
    outgoingAddress = qpidBridgeMamaTransportImpl_getOutgoingAddress (
                            (transportBridge) impl->mTransport);

    /* Get the transport UUID which is unique to the transport in case we need
     * it during format string expansion */
    uuid = qpidBridgeMamaTransportImpl_getUuid (
                 (transportBridge) impl->mTransport);

    /* Collapse subject key to single string based on supplied values
     *
     * _MD requests do not use the topic on the wire as the responder may not
     * necessarily be listening for requests on that topic until the first
     * request comes in. */
    if (NULL != root && 0 == strcmp (root, MAMA_ROOT_MARKET_DATA))
    {
        qpidBridgeCommon_generateSubjectKey (root,
                                             source,
                                             NULL,
                                             &impl->mSubject);
    }
    else
    {
        qpidBridgeCommon_generateSubjectKey (root,
                                             source,
                                             topic,
                                             &impl->mSubject);
    }

    /* Parse the collapsed string to extract the standardized values */
    qpidBridgeCommon_parseSubjectKey (impl->mSubject,
                                      &impl->mRoot,
                                      &impl->mSource,
                                      &impl->mTopic,
                                      (transportBridge) impl->mTransport);

    if (NULL != outgoingAddress)
    {
        /* Generate subject URI based on standardized values */
        qpidBridgeCommon_generateSubjectUri (outgoingAddress,
                                             impl->mRoot,
                                             impl->mSource,
                                             impl->mTopic,
                                             uuid,
                                             &impl->mUri);
    }

    /* Create a reusable proton message */
    impl->mQpidRawMsg = pn_message ();

    /* Populate the publisherBridge pointer with the publisher implementation */
    *result = (publisherBridge) impl;

    return status;
}

mama_status
qpidBridgeMamaPublisher_destroy (publisherBridge publisher)
{
    qpidPublisherBridge* impl = (qpidPublisherBridge*) publisher;

    /* Take a copy of the callbacks - we'll need those */
    mamaPublisherCallbacks callbacks;
    mamaPublisher          parent  = NULL;
    void*                  closure = NULL;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Take a copy of the callbacks - we'll need those */
    callbacks = impl->mCallbacks;
    parent    = impl->mParent;
    closure   = impl->mCallbackClosure;

    if (NULL != impl->mQpidRawMsg)
    {
        pn_message_free (impl->mQpidRawMsg);
    }
    if (NULL != impl->mSubject)
    {
        free ((void*) impl->mSubject);
    }
    if (NULL != impl->mUri)
    {
        free ((void*) impl->mUri);
    }
    if (NULL != impl->mRoot)
    {
        free ((void*) impl->mRoot);
    }
    if (NULL != impl->mSource)
    {
        free ((void*) impl->mSource);
    }
    if (NULL != impl->mTopic)
    {
        free ((void*) impl->mTopic);
    }
    if (NULL != impl->mMamaBridgeMsg)
    {
        baseBridgeMamaMsg_destroy (impl->mMamaBridgeMsg, 0);
    }

    if (NULL != callbacks.onDestroy)
    {
        (*callbacks.onDestroy)(parent, closure);
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
    baseMsgType             type          = BASE_MSG_PUB_SUB;
    int                     err           = 0;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_send(): No publisher.");
        return MAMA_STATUS_NULL_ARG;
    }
    else if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the bridge message type if specified already by inbox handlers */
    baseBridgeMamaMsgImpl_getMsgType (impl->mMamaBridgeMsg, &type);

    switch (type)
    {
    case BASE_MSG_INBOX_REQUEST:
        /* Use the publisher's default send subject */
        baseBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                          impl->mSubject,
                                          impl->mSource);

        /* Use the publisher's default send destination for request */
        qpidBridgePublisherImpl_enqueueMessageForAddress (
                msg,
                (char*) impl->mUri,
                impl,
                NULL);
        break;
    case BASE_MSG_INBOX_RESPONSE:
        /* The url should already be set for inbox responses as the replyTo */
        baseBridgeMamaMsgImpl_getDestination (impl->mMamaBridgeMsg, &url);

        /* Send out this message to the URL already provided */
        qpidBridgePublisherImpl_enqueueMessageForAddress (msg, url, impl, NULL);
        break;
    default:
        /* Use the publisher's default send subject */
        baseBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
                                          impl->mSubject,
                                          impl->mSource);

        if (QPID_TRANSPORT_TYPE_P2P ==
                qpidBridgeMamaTransportImpl_getType (
                        (transportBridge) impl->mTransport))
        {
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
                qpidP2pEndpoint* endpoint = (qpidP2pEndpoint*) targets[targetInc];
                qpidBridgePublisherImpl_enqueueMessageForAddress (msg, endpoint->mUrl, impl, endpoint);
            }
        }
        else
        {
            qpidBridgePublisherImpl_enqueueMessageForAddress (msg, impl->mUri, impl, NULL);
        }
        break;
    }

    /* Note the messages don't actually get published until here */
    err = pn_messenger_send (impl->mTransport->mOutgoing, -1);
    if (err && err != PN_TIMEOUT)
    {
        qpidError = PN_MESSENGER_ERROR (impl->mTransport->mOutgoing);
        mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidBridgeMamaPublisher_send(): "
                  "Qpid Error:[%s][%d]",
                  qpidError, err);

        return MAMA_STATUS_PLATFORM;
    }

    /* Reset the message type for the next publish */
    baseBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg, BASE_MSG_PUB_SUB);

    return status;
}

/* Send reply to inbox. */
mama_status
qpidBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                          mamaMsg           request,
                                          mamaMsg           reply)
{
    qpidPublisherBridge*    impl            = (qpidPublisherBridge*) publisher;
    const char*             inboxSubject    = NULL;
    const char*             replyTo         = NULL;
    msgBridge               bridgeMsg       = NULL;
    mama_status             status          = MAMA_STATUS_OK;

    if (NULL == publisher || NULL == request|| NULL == reply)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Set properties for the outgoing bridge message */
    baseBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                      BASE_MSG_INBOX_RESPONSE);

    /* Target is for MD subscriptions to respond to this particular topic */
    baseBridgeMamaMsgImpl_setTargetSubject (impl->mMamaBridgeMsg,
                                            impl->mSubject);

    /* Get the incoming bridge message from the mamaMsg */
    status = mamaMsgImpl_getBridgeMsg (request, &bridgeMsg);
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
    status = baseBridgeMamaMsgImpl_getInboxName (bridgeMsg,
                                                 (char**) &inboxSubject);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not get inbox name [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    status = baseBridgeMamaMsgImpl_getReplyTo (bridgeMsg, (char**) &replyTo);
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
    status = baseBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
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
    status = baseBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg,
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

    if (NULL == publisher || NULL == inbox || NULL == reply)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Set properties for the outgoing bridge message */
    baseBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                      BASE_MSG_INBOX_RESPONSE);

    /* Target is for MD subscriptions to respond to this particular topic */
    baseBridgeMamaMsgImpl_setTargetSubject (impl->mMamaBridgeMsg,
                                            impl->mSubject);

    /* Get properties from the incoming bridge message */
    status = baseBridgeMamaMsgReplyHandleImpl_getInboxName (
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

    status = baseBridgeMamaMsgReplyHandleImpl_getReplyTo (
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
    status = baseBridgeMamaMsg_setSendSubject (impl->mMamaBridgeMsg,
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
    status = baseBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg,
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

    if (NULL == impl || NULL == inbox || NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the inbox which you want the publisher to respond to */
    inboxImpl = mamaInboxImpl_getInboxBridge (inbox);
    replyAddr = baseBridgeMamaInboxImpl_getReplySubject (inboxImpl);

    /* Mark this as being a request from an inbox */
    status = baseBridgeMamaMsgImpl_setMsgType (impl->mMamaBridgeMsg,
                                               BASE_MSG_INBOX_REQUEST);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaPublisher_sendFromInboxByIndex(): "
                  "Failed to set message type [%s]",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    /* Update meta data in outgoing message to reflect the inbox name */
    baseBridgeMamaMsgImpl_setInboxName (impl->mMamaBridgeMsg,
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
    baseBridgeMamaMsgImpl_setReplyTo (impl->mMamaBridgeMsg,
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

mama_status
qpidBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                          mamaQueue               queue,
                                          mamaPublisherCallbacks* cb,
                                          void*                   closure)
{
    qpidPublisherBridge*    impl        = (qpidPublisherBridge*) publisher;

    if (NULL == impl || NULL == cb)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Take a copy of the callbacks */
    impl->mCallbacks = *cb;
    impl->mCallbackClosure = closure;

    return MAMA_STATUS_OK;
}

/*=========================================================================
 =                  Public implementation functions                      =
 =========================================================================*/

void
qpidBridgePublisherImpl_setMessageType (pn_message_t* message, baseMsgType type)
{
    /* Get the properties */
    pn_data_t* properties = pn_message_properties (message);

    /* Ensure position is at the start */
    pn_data_rewind (properties);

    /* Container for application properties is a map */
    pn_data_put_map(properties);
    pn_data_enter(properties);

    /* Add the type */
    pn_data_put_string(properties,pn_bytes(strlen(QPID_KEY_MSGTYPE),QPID_KEY_MSGTYPE));
    pn_data_put_ubyte(properties,type);

    pn_data_exit (properties);

    return;
}

/*=========================================================================
 =                  Private implementation functions                     =
 =========================================================================*/

void
qpidBridgePublisherImpl_enqueueMessageForAddress (mamaMsg              msg,
                                                  const char*          url,
                                                  qpidPublisherBridge* impl,
                                                  qpidP2pEndpoint*     endpoint)
{
    const char*     qpidError   = NULL;
    pn_message_t*   pnMsg       = NULL;
    pn_link_t*      pnLink      = NULL;
    pn_state_t      pnLinkState = PN_STATE_ERR;
    mama_bool_t     linkReady   = 1;
    char urlCopy[MAMA_MAX_TOTAL_SYMBOL_LEN];

    if (NULL == impl || NULL == url)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                  "Null closure or url received.");
        return;
    }
    strncpy (urlCopy, url, sizeof(urlCopy));

    /* Make a copy of the pointer - baseBridgeMamaMsgImpl_pack may modify */
    pnMsg = impl->mQpidRawMsg;

    /* Update the address with the current url */
    baseBridgeMamaMsgImpl_setDestination (impl->mMamaBridgeMsg, urlCopy);

    /* Pack the provided MAMA message into a proton message */
    qpidBridgeMsgCodec_pack (impl->mMamaBridgeMsg,
                             msg,
                             &pnMsg);

    /* This will ensure link checking only happens for pub / sub and only after
     * the first message has already been enqueued for send */
    if (NULL != endpoint && endpoint->mMsgCount > 0)
    {
        pnLink = pn_messenger_get_link (impl->mTransport->mOutgoing, urlCopy, 1);
        if (NULL != pnLink)
        {
            pnLinkState = pn_link_state (pnLink);
        }
        if (NULL == pnLink || (pnLinkState & PN_REMOTE_UNINIT))
        {
            endpoint->mErrorCount++;
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                      "Link last seen as down while sending on topic %s to %s "
                      "[%d] (attempt %d of %d to re-establish).",
                      impl->mSubject,
                      urlCopy,
                      (int)pnLinkState,
                      endpoint->mErrorCount,
                      QPID_PUBLISHER_RETRIES);
            if (endpoint->mErrorCount >= QPID_PUBLISHER_RETRIES)
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                          "Not publishing message for %s to %s: link is down [%d].",
                          impl->mSubject,
                          urlCopy,
                          (int)pnLinkState);
                if (NULL != pnLink) {
                    pn_link_free (pnLink);
                }

                endpointPool_unregister (impl->mTransport->mPubEndpoints,
                                         impl->mSubject,
                                         urlCopy);
                free (endpoint);
                linkReady = 0;
            }
        }
        else
        {
            endpoint->mErrorCount = 0;
        }
    }

    /* Enqueue the message in the messenger send queue if link is up */
    if (linkReady)
    {
        if (pn_messenger_put (impl->mTransport->mOutgoing, pnMsg))
        {
            qpidError = PN_MESSENGER_ERROR (impl->mTransport->mOutgoing);
            mama_log (MAMA_LOG_LEVEL_SEVERE,
                      "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                      "Qpid Error:[%s]",
                      qpidError);
            if (NULL != endpoint)
            {
                endpoint->mErrorCount++;
            }
            return;
        }
        else
        {
            if (NULL != endpoint)
            {
                endpoint->mMsgCount++;
            }
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                      "Publishing message for %s to %s.",
                      impl->mSubject,
                      urlCopy);
        }
    } else {
        mama_log (MAMA_LOG_LEVEL_WARN,
                      "qpidBridgePublisherImpl_enqueueMessageForAddress(): "
                      "Cannot publish message for %s to %s - link not ready.",
                      impl->mSubject,
                      urlCopy);
    }
}
