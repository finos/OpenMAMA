/* $Id: subinitial.c,v 1.1.2.4 2011/09/27 11:39:48 emmapollock Exp $
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

#include <wombat/wUuid.h>

#include <mama/mama.h>
#include <bridge.h>
#include <string.h>
#include "avisbridgefunctions.h"
#include "avisdefs.h"
#include "subinitial.h"

static const size_t uuidStringLen = 36;

typedef struct avisInboxImpl
{
    char                      mInbox[MAX_SUBJECT_LENGTH];
    mamaSubscription          mSubscription;
    void*                     mClosure;
    mamaInboxMsgCallback      mMsgCB;
    mamaInboxErrorCallback    mErrCB;
    mamaInboxDestroyCallback  mOnInboxDestroyed;
    mamaInbox                 mParent;
} avisInboxImpl;


#define avisInbox(inbox) ((avisInboxImpl*)(inbox))
#define CHECK_INBOX(inbox) \
        do {  \
           if (avisInbox(inbox) == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisInbox(inbox)->mSubscription == 0) return MAMA_STATUS_INVALID_ARG; \
         } while(0)


static void MAMACALLTYPE
avisInbox_onMsg(
    mamaSubscription    subscription,
    mamaMsg             msg,
    void*               closure,
    void*               itemClosure)
{
   if (!avisInbox(closure)) return;

   if (avisInbox(closure)->mMsgCB)
      (avisInbox(closure)->mMsgCB)(msg, avisInbox(closure)->mClosure);
}

static void MAMACALLTYPE
avisInbox_onCreate(
    mamaSubscription    subscription,
    void*               closure)
{
}

static void MAMACALLTYPE
avisInbox_onDestroy(
    mamaSubscription    subscription,
    void*               closure)
{
   if (!avisInbox(closure)) return;

   if (avisInbox(closure)->mOnInboxDestroyed)
      (avisInbox(closure)->mOnInboxDestroyed)(avisInbox(closure)->mParent, avisInbox(closure)->mClosure);
}

static void MAMACALLTYPE
avisInbox_onError(
    mamaSubscription    subscription,
    mama_status         status,
    void*               platformError,
    const char*         subject,
    void*               closure)
{
   if (!avisInbox(closure)) return;

   if (avisInbox(closure)->mErrCB)
      (avisInbox(closure)->mErrCB)(status, avisInbox(closure)->mClosure);
}


mama_status
avisBridgeMamaInbox_createByIndex (inboxBridge*           bridge,
                                    mamaTransport          transport,
                                    int                    tportIndex,
                                    mamaQueue              queue,
                                    mamaInboxMsgCallback   msgCB,
                                    mamaInboxErrorCallback errorCB,
                                    mamaInboxDestroyCallback onInboxDestroyed,
                                    void*                  closure,
                                    mamaInbox              parent)
{
   wUuid t_uuid;
   char t_str[37];
   mamaMsgCallbacks cb;
   avisInboxImpl* impl = NULL;
   mama_status status = MAMA_STATUS_OK;
   if (!bridge || !transport || !queue || !msgCB) return MAMA_STATUS_NULL_ARG;
   impl = (avisInboxImpl*)calloc(1, sizeof(avisInboxImpl));
   if (!impl)
      return MAMA_STATUS_NOMEM;

   if (MAMA_STATUS_OK != (status = mamaSubscription_allocate(&impl->mSubscription))) {
      mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaInbox_createByIndex(): Failed to allocate subscription ");
      free(impl);
      return status;
   }

   // NOTE: uuid_generate is very expensive, so we use cheaper uuid_generate_time
   wUuid_generate_time(t_uuid);
   wUuid_unparse(t_uuid, t_str);
   snprintf(impl->mInbox, sizeof(impl->mInbox) -1, "_INBOX.%s", t_str);

   cb.onCreate  = &avisInbox_onCreate;
   cb.onError   = &avisInbox_onError;
   cb.onMsg     = &avisInbox_onMsg;
   cb.onDestroy = &avisInbox_onDestroy;
   // not used by basic subscriptions
   cb.onQuality = NULL;
   cb.onGap = NULL;
   cb.onRecapRequest = NULL;

   if (MAMA_STATUS_OK != (status = mamaSubscription_createBasic(impl->mSubscription, transport, queue, &cb, impl->mInbox, impl))) {
      mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaInbox_createByIndex(): Failed to create subscription ");
      mamaSubscription_deallocate(impl->mSubscription);
      free(impl);
      return status;
   }

   impl->mClosure   = closure;
   impl->mMsgCB     = msgCB;
   impl->mErrCB     = errorCB;
   impl->mParent    = parent;
   impl->mOnInboxDestroyed = onInboxDestroyed;

   *bridge = (inboxBridge) impl;
   return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaInbox_create (inboxBridge*           bridge,
                             mamaTransport          transport,
                             mamaQueue              queue,
                             mamaInboxMsgCallback   msgCB,
                             mamaInboxErrorCallback errorCB,
                             mamaInboxDestroyCallback onInboxDestroyed,
                             void*                  closure,
                             mamaInbox              parent)
{
    return avisBridgeMamaInbox_createByIndex (bridge,
                                               transport,
                                               0,
                                               queue,
                                               msgCB,
                                               errorCB,
                                               onInboxDestroyed,
                                               closure,
                                               parent);
}

mama_status
avisBridgeMamaInbox_destroy (inboxBridge inbox)
{
   CHECK_INBOX(inbox);
   mamaSubscription_destroy(avisInbox(inbox)->mSubscription);
   mamaSubscription_deallocate(avisInbox(inbox)->mSubscription);
   free(avisInbox(inbox));
   return MAMA_STATUS_OK;
}


const char*
avisInboxImpl_getReplySubject(inboxBridge inbox)
{
   if (!avisInbox(inbox))
      return NULL;
   return avisInbox(inbox)->mInbox;
}
