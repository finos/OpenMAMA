/* $Id: msgutils.c,v 1.36.22.5 2011/10/02 19:02:17 ianbell Exp $
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

#include <mama/mama.h>
#include <mama/reservedfields.h>
#include <mama/msg.h>
#include <mama/subscmsgtype.h>
#include <msgutils.h>
#include <bridge.h>
#include <subscriptionimpl.h>
#include <string.h>
#include <msgimpl.h>

/**
 * Utility functions for manipulating messages. Used internally only.
 */

mama_status
msgUtils_getIssueSymbol (mamaMsg msg, const char** result)
{
    mama_status status = MAMA_STATUS_OK;

    if(MAMA_STATUS_OK!=(status=mamaMsg_getString( msg, NULL, 305, result)))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Could not get wIssueSymbol [%s]."
                  "Trying wIndexSymbol.", mamaMsgStatus_stringForStatus (status));
        /*The symbol might be in wIndexSymbol*/
        status = mamaMsg_getString (msg, NULL, 293, result);
    }

    return status;
}

mama_status
msgUtils_setStatus (mamaMsg msg, short status)
{
    return mamaMsgImpl_setStatus (msg, status);
}

mama_status
msgUtils_msgTotal (mamaMsg msg, short *result)
{
    int32_t val;
    mama_status status;

    status = mamaMsg_getI32 (msg,
                             MamaFieldMsgTotal.mName,
                             MamaFieldMsgTotal.mFid,
                             &val);

    *result = (short)val;

    return status;
}

mama_status
msgUtils_msgNum (mamaMsg msg, short *result)
{
    int32_t val;
    mama_status status;

    status = mamaMsg_getI32 (msg,
                             MamaFieldMsgNum.mName,
                             MamaFieldMsgNum.mFid,
                             &val);

    *result = (short)val;

    return status;
}

mama_status
msgUtils_msgSubscMsgType (mamaMsg msg, short *result)
{
    int32_t val = 0;
    mama_status status =
        mamaMsg_getI32 (msg,
                        MamaFieldSubscMsgType.mName,
                        MamaFieldSubscMsgType.mFid,
                        &val);

#ifndef IGNORE_DEPRECATED_FIELDS
    if (status != MAMA_STATUS_OK)
    {
        /* Try the old (deprecated) field. */
        status = mamaMsg_getI32 (msg,
                                 MamaFieldSubscMsgTypeOld.mName,
                                 MamaFieldSubscMsgTypeOld.mFid,
                                 &val);
    }
#endif

    *result = (short)val;
    return status;
}

/**
 * Create a message requesting a  subscription to the specified subject
 *
 * @param subsc
 * @return the subscribe message.
 */
mama_status
msgUtils_createSubscribeMsg (mamaSubscription subsc, mamaMsg *result)
{
    return msgUtils_createSubscriptionMessage(
        subsc, MAMA_SUBSC_SUBSCRIBE, result, NULL);
}

/**
 * Create a RESUBSCRIBE message  in response to a sync request.
 * @return  the resubscribe message.
 */
mama_status
msgUtils_createResubscribeMessage (mamaMsg *result)
{
    return msgUtils_createSubscriptionMessage(
        NULL, MAMA_SUBSC_RESUBSCRIBE, result, NULL);
}

mama_status
msgUtils_createRefreshMsg( mamaSubscription subsc, mamaMsg *result)
{
    return msgUtils_createSubscriptionMessage(
        subsc, MAMA_SUBSC_REFRESH, result, NULL);
}

mama_status
msgUtils_createTimeoutMsg (mamaMsg *msg)
{
    mamaMsg_create (msg);

    mamaMsg_addU8 (*msg,
                    MamaFieldMsgType.mName,
                    MamaFieldMsgType.mFid,
                    MAMA_MSG_TYPE_INITIAL);

    mamaMsg_addI32 (*msg,
                    MamaFieldMsgStatus.mName,
                    MamaFieldMsgStatus.mFid,
                    MAMA_MSG_STATUS_TIMEOUT);

    return MAMA_STATUS_OK;
}

mama_status
msgUtils_createEndOfInitialsMsg (mamaMsg *msg)
{
    mamaMsg_create (msg);
    mamaMsg_addU8 (*msg,
                    MamaFieldMsgType.mName,
                    MamaFieldMsgType.mFid,
                    MAMA_MSG_TYPE_END_OF_INITIALS);
    mamaMsg_addI32 (*msg,
                    MamaFieldMsgStatus.mName,
                    MamaFieldMsgStatus.mFid,
                    MAMA_MSG_STATUS_OK);
    return MAMA_STATUS_OK;
}

/**
 * Create a recovery message in when required to attempt recovery
 * of an item..
 * @return the recover request message.
 */
mama_status
msgUtils_createRecoveryRequestMsg (mamaSubscription  subscription,
                                   short             reason,
                                   mamaMsg*          result,
                                   const char*       issueSymbol)
{
    return msgUtils_createSubscriptionMessage (subscription,
                                               reason,
                                               result,
                                               issueSymbol);
}

mama_status
msgUtils_createSubscriptionMessage(
    mamaSubscription  subscription,
    mamaSubscMsgType  subscMsgType,
    mamaMsg*          msg,
    const char*       issueSymbol)
{
    uint8_t appDataType = 0;
    char*   ipaddr;

    mama_status status = mamaMsg_create (msg);
    if (status != MAMA_STATUS_OK)
    {
        return status;
    }

    if (subscription != NULL)
    {
        mamaBridgeImpl* bridge = mamaSubscription_getBridgeImpl (subscription);
        const char* source = NULL;
        const char* symbol = NULL;
        mamaSubscriptionType type = MAMA_SUBSC_TYPE_NORMAL;

        mamaMsgImpl_useBridgePayload    (*msg, bridge);

        mamaSubscription_getSource      (subscription, &source);

        /* For group subs the issue symbol needs to be passed in as it
           is not the same as the subscribe symbol */
        if (issueSymbol)
        {
            symbol = issueSymbol;
        }
        else
        {
            mamaSubscription_getSubscSymbol (subscription, &symbol);
        }

        mamaSubscription_getSubscriptionType (subscription, &type);
        mamaSubscription_getAppDataType      (subscription, &appDataType);

        status = mamaMsgImpl_setSubscInfo (
            *msg,
            mamaSubscription_getSubscRoot (subscription),
            source,
            symbol,
            1);
        if (status != MAMA_STATUS_OK)
            return status;


        status = mamaMsg_addI32 (
            *msg,
            MamaFieldSubscriptionType.mName,
            MamaFieldSubscriptionType.mFid,
            type);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    status = mamaMsg_addI32 (
        *msg,
        MamaFieldSubscMsgType.mName,
        MamaFieldSubscMsgType.mFid,
        subscMsgType);
    if (status != MAMA_STATUS_OK)
    {
        return status;
    }

    status = mamaMsg_addU8 (
        *msg,
        MamaFieldAppDataType.mName,
        MamaFieldAppDataType.mFid,
        appDataType);
    if (status != MAMA_STATUS_OK)
    {
        return status;
    }

#ifndef IGNORE_DEPRECATED_FIELDS
    status = mamaMsg_addI32 (
        *msg,
        MamaFieldSubscMsgTypeOld.mName,
        MamaFieldSubscMsgTypeOld.mFid,
        subscMsgType);
    if (status != MAMA_STATUS_OK)
        return status;
#endif

    mama_getIpAddress ((const char**)&ipaddr);

    if (ipaddr != NULL && strlen( ipaddr ) > 0)
    {
        status = mamaMsg_addString (
            *msg,
            MamaFieldSubscSourceHost.mName,
            MamaFieldSubscSourceHost.mFid,
            ipaddr);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
msgUtils_setSubscSubject (mamaMsg msg,  const char* sendSubject)
{
    mama_status status = mamaMsg_addString (
        msg,
        MamaFieldSubscSymbol.mName,
        MamaFieldSubscSymbol.mFid,
        sendSubject);

#ifndef IGNORE_DEPRECATED_FIELDS
    if (status != MAMA_STATUS_OK)
    {
        status = mamaMsg_addString (
            msg,
            MamaFieldSubscSubjectOld.mName,
            MamaFieldSubscSubjectOld.mFid,
            sendSubject);
    }
#endif

    return status;
}

mama_status
msgUtils_createUnsubscribeMsg (mamaSubscription subsc, mamaMsg *msg)
{
    return msgUtils_createSubscriptionMessage(
        subsc, MAMA_SUBSC_UNSUBSCRIBE, msg, NULL);
}

mama_status
msgUtils_createDictionarySubscribe(
    mamaSubscription subscription,
    mamaMsg *msg )
{
    return msgUtils_createSubscriptionMessage(
        subscription, MAMA_SUBSC_DDICT_SNAPSHOT, msg, NULL);
}

mama_status
msgUtils_createSnapshotSubscribe (mamaSubscription subsc, mamaMsg *msg)
{
    return msgUtils_createSubscriptionMessage(
        subsc, MAMA_SUBSC_SNAPSHOT, msg, NULL);
}

