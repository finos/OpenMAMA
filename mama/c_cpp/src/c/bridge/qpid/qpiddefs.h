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
#include <proton/version.h>
#include <proton/driver.h>
#include <proton/message.h>
#include <proton/util.h>
#include <proton/messenger.h>

#include "endpointpool.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Maximum topic length */
#define     MAX_SUBJECT_LENGTH              256
#define     MAX_URI_LENGTH                  1024

/* Default timeout for send working threads */
#define     QPID_MESSENGER_SEND_TIMEOUT     -1
#define     QPID_MESSENGER_TIMEOUT          1 /* milliseconds */

/* Message types */
typedef enum qpidMsgType_
{
    QPID_MSG_PUB_SUB        =               0x00,
    QPID_MSG_INBOX_REQUEST,
    QPID_MSG_INBOX_RESPONSE,
    QPID_MSG_SUB_REQUEST,
    QPID_MSG_TERMINATE      =               0xff
} qpidMsgType;

/* Message types */
typedef enum qpidTransportType_
{
    QPID_TRANSPORT_TYPE_P2P,
    QPID_TRANSPORT_TYPE_BROKER
} qpidTransportType;

#define PN_MESSENGER_ERROR(messenger)                                          \
    pn_error_text(pn_messenger_error(messenger))

/* Keys for application property map */
#define QPID_KEY_MSGTYPE        "MAMAT"
#define QPID_KEY_INBOXNAME      "MAMAI"
#define QPID_KEY_REPLYTO        "MAMAR"
#define QPID_KEY_TARGETSUBJECT  "MAMAS"

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
    transportBridge     mTransport;
    const char*         mSource;
    const char*         mTopic;
    const char*         mRoot;
    const char*         mSubject;
    const char*         mUri;
    void*               mClosure;
    int                 mIsNotMuted;
    int                 mIsValid;
    int                 mIsTportDisconnected;
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
    const char*         mUuid;
    wthread_t           mQpidDispatchThread;
    int                 mIsDispatching;
    mama_status         mQpidDispatchStatus;
    endpointPool_t      mSubEndpoints;
    endpointPool_t      mPubEndpoints;
    qpidTransportType   mQpidTransportType;
    wtable_t            mKnownSources;
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
