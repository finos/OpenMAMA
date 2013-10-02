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

#ifndef MAMA_BRIDGE_QPID_QPIDDEFS_H__
#define MAMA_BRIDGE_QPID_QPIDDEFS_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <wombat/wSemaphore.h>
#include <wombat/wtable.h>
#include <list.h>

/* Qpid include files */
#include <proton/driver.h>
#include <proton/message.h>
#include <proton/util.h>
#include <proton/messenger.h>

#include "endpointpool.h"

/* If this version of proton has provided a version header file (build system
 * to provide this macro) */
#ifdef HAVE_QPID_PROTON_VERSION_H
#include <proton/version.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Maximum topic length */
#define     MAX_SUBJECT_LENGTH              256

/* Default timeout for send working threads */
#define     QPID_MESSENGER_SEND_TIMEOUT     -1

/* Message types */
typedef enum qpidMsgType_
{
    QPID_MSG_PUB_SUB        =               0x00,
    QPID_MSG_INBOX_REQUEST,
    QPID_MSG_INBOX_RESPONSE,
    QPID_MSG_SUB_REQUEST,
    QPID_MSG_TERMINATE      =               0xff
} qpidMsgType;

/* If a proton version header has been parsed at this point (version >= 0.5) */
#ifdef _PROTON_VERSION_H

#if (PN_VERSION_MAJOR > 0 || PN_VERSION_MINOR > 4)
#define PN_MESSENGER_SEND(messenger)                                           \
    pn_messenger_send(messenger, QPID_MESSENGER_SEND_TIMEOUT)
#define PN_MESSENGER_ERROR(messenger)                                          \
    pn_error_text(pn_messenger_error(messenger))
#define PN_MESSENGER_STOP(messenger)                                           \
    qpidBridgeMamaTransportImpl_stopProtonMessenger (messenger)
#define PN_MESSENGER_FREE(messenger)                                           \
    qpidBridgeMamaTransportImpl_freeProtonMessenger (messenger)
#endif

/* Place other version specific macros here */

#else

/* Earliest supported version is 0.4 - header was added in 0.5 */
#define     PN_VERSION_MAJOR                0
#define     PN_VERSION_MINOR                4

#define PN_MESSENGER_SEND(messenger) pn_messenger_send(messenger)
#define PN_MESSENGER_ERROR(messenger) pn_messenger_error(messenger)
#define PN_MESSENGER_STOP(messenger) /* disabled as it deadlocks in this ver */
#define PN_MESSENGER_FREE(messenger) /* disabled as it deadlocks in this ver */

#endif /* _PROTON_VERSION_H */


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidMsgNode_ qpidMsgNode;
typedef struct qpidMsgPool_ qpidMsgPool;

typedef struct qpidSubscription_
{
    mamaMsgCallbacks    mMamaCallback;
    mamaSubscription    mMamaSubscription;
    mamaQueue           mMamaQueue;
    void*               mQpidQueue;
    mamaTransport       mTransport;
    const char*         mSymbol;
    char*               mSubjectKey;
    void*               mClosure;
    int                 mIsNotMuted;
    int                 mIsValid;
    pn_message_t*       mMsg;
    const char*         mEndpointIdentifier;
} qpidSubscription;

typedef struct qpidTransportBridge_
{
    int                 mIsValid;
    int                 mQpidRecvBlockSize;
    mamaTransport       mTransport;
    pn_message_t*       mMsg;
    pn_messenger_t*     mIncoming;
    pn_messenger_t*     mOutgoing;
    qpidMsgPool*        mQpidMsgPool;
    unsigned int        mQpidMsgPoolIncSize;
    const char*         mIncomingAddress;
    const char*         mOutgoingAddress;
    const char*         mReplyAddress;
    const char*         mName;
    wthread_t           mQpidDispatchThread;
    int                 mIsDispatching;
    mama_status         mQpidDispatchStatus;
    endpointPool_t      mSubEndpoints;
    endpointPool_t      mPubEndpoints;
} qpidTransportBridge;

struct qpidMsgNode_
{
  pn_message_t*         mMsg;
  qpidMsgNode*          mNext;
  qpidMsgNode*          mPrev;
  qpidMsgPool*          mMsgPool;
  qpidMsgType           mMsgType;
  qpidSubscription*     mQpidSubscription;
};

struct qpidMsgPool_
{
  qpidTransportBridge*  mQpidTransportBridge;
  qpidMsgNode*          mFreeList;      /* Linked list of free Messages */
  qpidMsgNode*          mOpenList;      /* Linked list of open Messages */
  unsigned int          mNumMsgs;       /* Number of buffer in the pool */
  unsigned int          mNumFree;       /* Number of free buffers */
  wthread_mutex_t       mLock;
};

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_QPID_QPIDDEFS_H__ */
