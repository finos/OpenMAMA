/* $Id$
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

#include <string.h>

#include <mama/mama.h>
#include <mama/msg.h>
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <bridge.h>
#include <msgimpl.h>
#include <inboximpl.h>
#include "subinitial.h"
#include "avisbridgefunctions.h"
#include "transportbridge.h"
#include "msgimpl.h"
#include "avisdefs.h"
#include "sub.h"
#include "../../payload/avismsg/avispayload.h"

typedef struct avisPublisherBridge
{
    mamaTransport  mTransport;
    Elvin*         mAvis;
    const char*    mTopic;
    const char*    mSource;
    const char*    mRoot;
    char*          mSubject;
} avisPublisherBridge;

#define avisPublisher(publisher) ((avisPublisherBridge*) publisher)
#define CHECK_PUBLISHER(publisher) \
    do {  \
       if (avisPublisher(publisher) == 0) return MAMA_STATUS_NULL_ARG; \
       if (avisPublisher(publisher)->mAvis == 0) return MAMA_STATUS_INVALID_ARG; \
       if (!elvin_is_open(avisPublisher(publisher)->mAvis)) return MAMA_STATUS_INVALID_ARG; \
     } while(0)

static
void sendAvisMessage (Elvin* avis,
                      void*  closure)
{
    mamaMsg     msg     = (mamaMsg) closure;
    Attributes* attr    = NULL;
    mama_size_t dataLen = 0;

    mamaMsgImpl_getPayloadBuffer (msg, (const void**)&attr, &dataLen);

    if (!elvin_send (avis, attr))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "sendAvisMessage(): "
                  "Could not send message - %s", avis->error.message);
    }

    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST, "sendAvisMessage(): "
                       "Send message: %s", mamaMsg_toString (msg));
    }

    mamaMsg_destroy (msg);
}

static mama_status
avisBridgeMamaPublisherImpl_buildSendSubject (avisPublisherBridge* impl);

static
mama_status
avisBridgeMamaPublisherImpl_prepareMessage (mamaMsg* msg)
{
    mama_status ret       = MAMA_STATUS_OK;
    char        payloadId = MAMA_PAYLOAD_UNKNOWN;

    ret = mamaMsgImpl_getPayloadId (*msg, &payloadId);

    if (MAMA_STATUS_OK != ret)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisherImpl_prepareMessage(): "
                  "Could not get message payload type. [%d]", ret);

        return ret;
    }

    // TODO: Should replace this with a more run-time way to get the payload ID
    //       from name
    if (MAMA_PAYLOAD_ID_AVIS == payloadId)
    {
        /* Avis message, just perform a copy and detach so that it
         * can be enqueued on the avis dispatch thread. */
        mamaMsg copyMsg = NULL;

        ret = mamaMsg_copy (*msg, &copyMsg);

        if (MAMA_STATUS_OK != ret)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, 
                      "avisBridgeMamaPublisherImpl_prepareMessage(): "
                      "Could not copy message. [%d]", ret);

            return ret;
        }

        *msg = copyMsg;
    }
    else
    {
        /* Non-Avis message - Strategy is to wrap it within the enclosed field
         * of a new Avis message.  If the existing message had a bridged message
         * within it, then this forms the basis for the new message.  Otherwise,
         * a brand new message is constructed. */
        mamaMsg     encMsg     = NULL;
        msgBridge   bridgeMsg  = NULL;
        const void* buf        = NULL;
        mama_size_t bufSize    = 0;

        if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
        {
            mama_log (MAMA_LOG_LEVEL_FINEST, 
                      "sendAvisMessage(): "
                      "Enclosing message: %s", mamaMsg_toString (*msg));
        }

        ret = mamaMsg_getByteBuffer (*msg, &buf, &bufSize);

        if (MAMA_STATUS_OK != ret)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, 
                      "avisBridgeMamaPublisherImpl_prepareMessage(): "
                      "Could not get byte buffer for message. [%d]", ret);

            return ret;
        }

        mamaMsgImpl_getBridgeMsg (*msg, &bridgeMsg);

        if (bridgeMsg)
        {
            msgPayload  payloadMsg = NULL;

            avisBridgeMamaMsg_getNativeHandle (bridgeMsg, (void**) &payloadMsg);

            if (payloadMsg)
            {
                const void* buf     = NULL;
                mama_size_t bufSize = 0;
                Attributes* attributes = NULL;

                avismsgPayload_getByteBuffer (payloadMsg, &buf, &bufSize);

                buf = (const void*)attributes_clone ((Attributes*)buf);

                mamaMsg_create (&encMsg);
                mamaMsgImpl_setMsgBuffer (encMsg, buf, bufSize, MAMA_PAYLOAD_ID_AVIS);
                mamaMsgImpl_setMessageOwner (encMsg, 1);
            }
        }

        if (!encMsg)
        {
            mamaPayloadBridge bridge = mamaInternal_findPayload (MAMA_PAYLOAD_ID_AVIS);
            ret = mamaMsg_createForPayloadBridge(&encMsg, bridge);

            if (MAMA_STATUS_OK != ret)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR, 
                          "avisBridgeMamaPublisherImpl_prepareMessage(): "
                          "Could not create enclosed message. [%d]", ret);

                return ret;
            }
        }

        ret = mamaMsg_addOpaque (encMsg, ENCLOSED_MSG_FIELD_NAME, 0,
                                 buf, bufSize);

        if (MAMA_STATUS_OK != ret)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, 
                      "avisBridgeMamaPublisherImpl_prepareMessage(): "
                      "Could not add enclosed message as opaque field. [%d]", ret);

            return ret;
        }

        *msg = encMsg;
    }

    return MAMA_STATUS_OK;
}


static
mama_status
avisBridgeMamaPublisherImpl_sendMessage (publisherBridge publisher, mamaMsg msg)
{
    if (!elvin_invoke (avisPublisher(publisher)->mAvis, &sendAvisMessage, msg))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisherImpl_sendMessage(): "
                  "Could not send message.");

        mamaMsg_destroy (msg);

        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaPublisher_createByIndex (publisherBridge* result,
                                        mamaTransport    tport,
                                        int              tportIndex,
                                        const char*      topic,
                                        const char*      source,
                                        const char*      root,
                                        mamaPublisher    parent)
{
    Elvin* avis = NULL;
    avisPublisherBridge* publisher = NULL;
    if (!result || !tport || !parent) return MAMA_STATUS_NULL_ARG;

    avis = getAvis(tport);

    CHECK_AVIS(avis);

    *result = NULL;
    publisher = (avisPublisherBridge*) calloc (1, sizeof(avisPublisherBridge));
    if (publisher == NULL)
        return MAMA_STATUS_NOMEM;
    publisher->mTransport = tport;
    publisher->mAvis = avis;

    if (topic != NULL)
        publisher->mTopic = strdup (topic);
    if (source != NULL)
        publisher->mSource = strdup(source);
    if (root != NULL)
        publisher->mRoot = strdup (root);

    avisBridgeMamaPublisherImpl_buildSendSubject (publisher);

    *result = (publisherBridge) publisher;

    return MAMA_STATUS_OK;
}

/* Build up the RV subject. This should only need to be set once for the
   publisher. Duplication of some of the logic  */
static mama_status
avisBridgeMamaPublisherImpl_buildSendSubject (avisPublisherBridge* impl)
{
    mama_status status    = MAMA_STATUS_OK;
    char*       keyTarget = NULL;
    const char* root      = impl->mRoot;
    const char* source    = impl->mSource;
    const char* topic     = impl->mTopic;

    /* If this is a special _MD publisher, lose the topic unless dictionary */
    if (root != NULL && 0 != strcmp (root, "_MDDD"))
    {
        topic = NULL;
    }

    status = avisBridgeMamaSubscriptionImpl_generateSubjectKey (root,
                                                                source,
                                                                topic,
                                                                &keyTarget);

    /* Set the subject for publishing here */
    impl->mSubject = keyTarget;

    return status;
}

/*Send a message.*/
mama_status
avisBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    mama_status ret = MAMA_STATUS_OK;

    if (!msg) return MAMA_STATUS_NULL_ARG;

    CHECK_PUBLISHER(publisher);

    ret = avisBridgeMamaPublisherImpl_prepareMessage (&msg);

    if (MAMA_STATUS_OK != ret)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisher_send(): "
                  "Could not prepare message. [%d]", ret);

        return ret;
    }

    ret = mamaMsg_updateString(msg, SUBJECT_FIELD_NAME, 0, avisPublisher(publisher)->mSubject);

    if (MAMA_STATUS_OK != ret)
    {
        return ret;
    }

    return avisBridgeMamaPublisherImpl_sendMessage (publisher, msg);
}

/* Send reply to inbox. */
mama_status
avisBridgeMamaPublisher_sendReplyToInbox (publisherBridge  publisher,
                                          mamaMsg          request,
                                          mamaMsg          reply)
{
    msgPayload   requestMsg  = NULL;
    mamaMsgReply replyHandle = NULL;
    const char*  replyAddr   = NULL;
    mama_status  status;
    
    if (!request || !reply) return MAMA_STATUS_NULL_ARG;

    CHECK_PUBLISHER(publisher);

    mamaMsg_getNativeHandle(request, (void**) &requestMsg);

    if (!requestMsg) return MAMA_STATUS_NULL_ARG;

    status = avisBridgeMamaPublisherImpl_prepareMessage (&reply);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisher_sendReplyToInbox(): "
                  "Could not prepare message. [%d]", status);

        return status;
    }

    status = mamaMsg_getReplyHandle (request, &replyHandle);

    if (replyHandle)
    {
        replyAddr = ((mamaMsgReplyImpl*)replyHandle)->replyHandle;
    }

    if (status != MAMA_STATUS_OK || (replyAddr == NULL) || replyAddr[0] == '\0')
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisher_sendReplyToInbox(): "
                  "No reply address in message. [%d]", status);

        mamaMsg_destroyReplyHandle (replyHandle);
        mamaMsg_destroy (reply);

        return MAMA_STATUS_INVALID_ARG;
    }

    status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, replyAddr);

    mamaMsg_destroyReplyHandle (replyHandle);

    if (status != MAMA_STATUS_OK)
    {
        mamaMsg_destroy (reply);

        return status;
    }

    return avisBridgeMamaPublisherImpl_sendMessage (publisher, reply);
}

/* Destroy the publisher.*/
mama_status
avisBridgeMamaPublisher_destroy (publisherBridge publisher)
{
    if (!publisher) return MAMA_STATUS_NULL_ARG;
    free ((char*)avisPublisher(publisher)->mSource);
    free ((char*)avisPublisher(publisher)->mTopic);
    free ((char*)avisPublisher(publisher)->mRoot);
    free (avisPublisher(publisher)->mSubject);
    free (avisPublisher(publisher));
    return MAMA_STATUS_OK;
}

/* Send a message from the specified inbox using the throttle. */
mama_status
avisBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge publisher,
                                               int             tportIndex,
                                               mamaInbox       inbox,
                                               mamaMsg         msg)
{
    const char* replyAddr = NULL;
    mama_status status;

    if (!inbox || !msg) return MAMA_STATUS_NULL_ARG;

    CHECK_PUBLISHER(publisher);

    status = avisBridgeMamaPublisherImpl_prepareMessage (&msg);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisher_sendFromInboxByIndex(): "
                  "Could not prepare message. [%d]", status);

        return status;
    }

    // get reply address from inbox
    replyAddr = avisInboxImpl_getReplySubject(mamaInboxImpl_getInboxBridge(inbox));

    // set reply address in msg
    status = mamaMsg_updateString(msg, INBOX_FIELD_NAME, 0, replyAddr);

    if (status != MAMA_STATUS_OK)
    {
        mamaMsg_destroy (msg);

        return status;
    }

    status = mamaMsg_updateString (msg, SUBJECT_FIELD_NAME, 0, 
                                   avisPublisher(publisher)->mSubject);

    if (MAMA_STATUS_OK != status)
    {
        mamaMsg_destroy (msg);

        return status;
    }

    return avisBridgeMamaPublisherImpl_sendMessage (publisher, msg);
}

mama_status
avisBridgeMamaPublisher_sendFromInbox (publisherBridge publisher,
                                        mamaInbox     inbox,
                                        mamaMsg       msg)
{
    return avisBridgeMamaPublisher_sendFromInboxByIndex (
                                            publisher, 0, inbox, msg);
}

mama_status
avisBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge publisher,
                                               void *          inbox,
                                               mamaMsg         reply)
{
    mama_status status;

    if (!inbox | !reply) return MAMA_STATUS_NULL_ARG;

    CHECK_PUBLISHER(publisher);

    status = avisBridgeMamaPublisherImpl_prepareMessage (&reply);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
                  "avisBridgeMamaPublisher_sendReplyToInboxHandle(): "
                  "Could not prepare message. [%d]", status);

        return status;
    }

    status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, (const char*) inbox);

    if (status != MAMA_STATUS_OK)
    {
        mamaMsg_destroy (reply);

        return status;
    }

    return avisBridgeMamaPublisherImpl_sendMessage (publisher, reply);
}


