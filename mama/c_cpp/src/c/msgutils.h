/* $Id: msgutils.h,v 1.14.24.4 2011/09/01 16:34:38 emmapollock Exp $
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
#ifndef MsgUtilsH__
#define MsgUtilsH__

#include "mama/subscmsgtype.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define DQ_SUBSCRIBER   7
#define DQ_PUBLISHER    8
#define DQ_NETWORK      9
#define DQ_UNKNOWN      10
#define DQ_DISCONNECT   12
#define DQ_CONNECT      13

mama_status
msgUtils_getIssueSymbol            (mamaMsg msg, const char** result);

mama_status
msgUtils_setStatus                 (mamaMsg msg, short status);

mama_status
msgUtils_msgTotal                  (mamaMsg msg, short* result);

mama_status 
msgUtils_msgNum                    (mamaMsg msg, short* result);

mama_status
msgUtils_msgSubscMsgType           (mamaMsg msg, short* result);

mama_status 
msgUtils_createSubscriptionMessage (mamaSubscription  subscription,
                                    mamaSubscMsgType  subscMsgType,
                                    mamaMsg*          msg,
                                    const char*       issueSymbol);

/**
 * Create a message requesting a  subscription to the specified subject
 *
 * @param subsc
 * @return the subscribe message.
 */
mama_status
msgUtils_createSubscribeMsg        (mamaSubscription subsc, mamaMsg* result);

/**
 * Create a RESUBSCRIBE message  in response to a sync request.
 * @return  the resubscribe message.
 */
mama_status
msgUtils_createResubscribeMessage  (mamaMsg* result);

mama_status
msgUtils_createRefreshMsg          (mamaSubscription subsc, mamaMsg* result);

mama_status
msgUtils_createTimeoutMsg          (mamaMsg* msg);

mama_status
msgUtils_createEndOfInitialsMsg    (mamaMsg* msg);

/**
 * Create a recovery message in when required to attempt recovery
 * of an item..
 * @return the recover request message.
 */
mama_status
msgUtils_createRecoveryRequestMsg  (mamaSubscription subsc, 
                                    short            reason,
                                    mamaMsg*         result, 
                                    const char*      issueSymbol);

mama_status
msgUtils_setSubscSubject           (mamaMsg msg, const char* sendSubject);

mama_status 
msgUtils_createUnsubscribeMsg      (mamaSubscription subsc, mamaMsg* msg);

mama_status
msgUtils_createDictionarySubscribe (mamaSubscription subscription, 
                                    mamaMsg*         msg);

mama_status
msgUtils_createSnapshotSubscribe   (mamaSubscription subsc, mamaMsg* msg);

mama_status
msgUtils_msgTotal                  (mamaMsg msg, short* result);

mama_status
msgUtils_msgNum                    (mamaMsg msg, short* result);

#if defined(__cplusplus)
}
#endif

#endif /* MsgUtilsH__*/
