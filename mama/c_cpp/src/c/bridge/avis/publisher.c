/* $Id: publisher.c,v 1.1.2.10 2011/10/02 19:02:18 ianbell Exp $
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
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <bridge.h>
#include <inboximpl.h>
#include "subinitial.h"
#include "avisbridgefunctions.h"
#include "transportbridge.h"
#include "msgimpl.h"
#include "avisdefs.h"

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




static mama_status
avisBridgeMamaPublisherImpl_buildSendSubject (avisPublisherBridge* impl);


mama_status
avisBridgeMamaPublisher_createByIndex (publisherBridge* result,
                                        mamaTransport    tport,
                                        int              tportIndex,
                                        const char*      topic,
                                        const char*      source,
                                        const char*      root,
                                        void*            nativeQueueHandle,
                                        mamaPublisher    parent)
{
    Elvin* avis = getAvis(tport);
    avisPublisherBridge* publisher = NULL;

    if (!result || !tport) return MAMA_STATUS_NULL_ARG;
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

mama_status
avisBridgeMamaPublisher_create (publisherBridge* result,
                                 mamaTransport    tport,
                                 const char*      topic,
                                 const char*      source,
                                 const char*      root,
                                 void*            nativeQueueHandle,
                                 mamaPublisher    parent)
{
    return avisBridgeMamaPublisher_createByIndex (result,
                                                  tport,
                                                  0,
                                                  topic,
                                                  source,
                                                  root,
                                                  nativeQueueHandle,
                                                  parent);
}

/* Build up the RV subject. This should only need to be set once for the
   publisher. Duplication of some of the logic  */
static mama_status
avisBridgeMamaPublisherImpl_buildSendSubject (avisPublisherBridge* impl)
{
    char lSubject[256];

    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (impl->mRoot != NULL && impl->mSource != NULL )
    {
            snprintf (lSubject, sizeof(lSubject),"%s.%s",
                      impl->mRoot, impl->mSource);
    }
    else if (impl->mSource != NULL && impl->mTopic != NULL)
    {
        snprintf (lSubject, sizeof(lSubject), "%s.%s",
                  impl->mSource, impl->mTopic);
    }
    else if (impl->mTopic != NULL)
    {
        snprintf (lSubject, sizeof(lSubject), "%s",
                  impl->mTopic);
    }

    impl->mSubject = strdup(lSubject);

    return MAMA_STATUS_OK;
}

/*Send a message.*/
mama_status
avisBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    mama_size_t        dataLen;
    mama_status        status;
    Attributes* attributes = NULL;
    
    CHECK_PUBLISHER(publisher);

    status = mamaMsgImpl_getPayloadBuffer (msg, (const void**)&attributes, &dataLen);
    if (attributes == NULL)
        return MAMA_STATUS_INVALID_ARG;

    mamaMsg_updateString(msg, SUBJECT_FIELD_NAME, 0, avisPublisher(publisher)->mSubject);

    if (!elvin_send(avisPublisher(publisher)->mAvis, attributes)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaPublisher_send(): "
                  "Could not send message.");
        return MAMA_STATUS_PLATFORM;
    }

    mama_log (MAMA_LOG_LEVEL_FINEST, "avisBridgeMamaPublisher_send(): "
                   "Send message. %s", mamaMsg_toString(msg));
    return MAMA_STATUS_OK;
}

/* Send reply to inbox. */
mama_status
avisBridgeMamaPublisher_sendReplyToInbox (publisherBridge  publisher,
                                           mamaMsg          request,
                                           mamaMsg          reply)
{
    Attributes* requestMsg = NULL;
    Attributes* replyMsg = NULL;
    const char*  replyAddr  = NULL;
    mama_size_t        dataLen;
    mama_status  status;
    
    CHECK_PUBLISHER(publisher);

    mamaMsg_getNativeHandle(request, (void**) &requestMsg);
    mamaMsgImpl_getPayloadBuffer (reply, (const void**)&replyMsg, &dataLen);

    if (!requestMsg || !replyMsg) return MAMA_STATUS_NULL_ARG;

    status = mamaMsg_getString(request, INBOX_FIELD_NAME, 0, &replyAddr);
    if ((status != MAMA_STATUS_OK) || (replyAddr == NULL) || (strlen(replyAddr) == 0)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaPublisher_sendReplyToInbox(): "
                  "No reply address in message.");
        return MAMA_STATUS_INVALID_ARG;
    }

    status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, replyAddr);
    if (status != MAMA_STATUS_OK)
        return status;

    if (!elvin_send(avisPublisher(publisher)->mAvis, replyMsg)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaPublisher_send(): "
                  "Could not send message.");
        return MAMA_STATUS_PLATFORM;
    }

    return MAMA_STATUS_OK;
}

/* Destroy the publisher.*/
mama_status
avisBridgeMamaPublisher_destroy (publisherBridge publisher)
{
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
  //  CHECK_PUBLISHER(publisher);
    if (avisPublisher(publisher) == 0)
    	return MAMA_STATUS_NULL_ARG;
    if (avisPublisher(publisher)->mAvis == 0)
    	return MAMA_STATUS_INVALID_ARG;
    if (!elvin_is_open(avisPublisher(publisher)->mAvis))
    	return MAMA_STATUS_INVALID_ARG;

    // get reply address from inbox
    replyAddr = avisInboxImpl_getReplySubject(mamaInboxImpl_getInboxBridge(inbox));

    // set reply address in msg
    status = mamaMsg_updateString(msg, INBOX_FIELD_NAME, 0, replyAddr);
    if (status != MAMA_STATUS_OK)
        return status;

    return avisBridgeMamaPublisher_send(publisher, msg);
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
    CHECK_PUBLISHER(publisher);

    status = mamaMsg_updateString(reply, SUBJECT_FIELD_NAME, 0, (const char*) inbox);
    if (status != MAMA_STATUS_OK)
        return status;

    return  avisBridgeMamaPublisher_sendFromInboxByIndex(publisher, 0, (mamaInbox) inbox, reply);
}
