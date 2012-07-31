/* $Id: marketdata.c,v 1.3.34.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mama/marketdata.h"
#include "mama/msgtype.h"
#include "mama/reservedfields.h"
#include "mama/msg.h"


mamaMdMsgType
mamaMdMsgType_typeForMsg (const mamaMsg msg)
{
    uint8_t val;
    mama_status status;

    status = mamaMsg_getU8 (msg,
                            MamaFieldAppMsgType.mName,
                            MamaFieldAppMsgType.mFid,
                            &val);

    if (status == MAMA_STATUS_OK)
        return (mamaMdMsgType)val;
    else
        return (mamaMdMsgType)0;
}

const char*
mamaMdMsgType_stringForMsg (const mamaMsg msg)
{
    return mamaMdMsgType_stringForType (mamaMdMsgType_typeForMsg (msg));
}

const char*
mamaMdMsgType_stringForType (mamaMdMsgType type)
{
    switch (type)
    {
    case MAMA_MD_MSG_TYPE_GENERAL:
        return "GENERAL";

    case MAMA_MD_MSG_TYPE_CANCEL:
        return "CANCEL";

    case MAMA_MD_MSG_TYPE_ERROR:
        return "ERROR";

    case MAMA_MD_MSG_TYPE_CORRECTION:
        return "CORRECTION";

    case MAMA_MD_MSG_TYPE_CLOSING:
        return "CLOSING";

    case MAMA_MD_MSG_TYPE_SYMBOL_DELETE:
        return "SYMBOL_DELETE";

    case MAMA_MD_MSG_TYPE_SYMBOL_ACTION:
        return "SYMBOL_ACTION";

    case MAMA_MD_MSG_TYPE_PREOPENING:
        return "PREOPENING";

    case MAMA_MD_MSG_TYPE_QUOTE:
        return "QUOTE";

    case MAMA_MD_MSG_TYPE_TRADE:
        return "TRADE";

    case MAMA_MD_MSG_TYPE_BOOK_UPDATE:
        return "BOOK_UPDATE";

    case MAMA_MD_MSG_TYPE_BOOK_INITIAL:
        return "BOOK_INITIAL";

    case MAMA_MD_MSG_TYPE_BOOK_CLEAR:
        return "BOOK_CLEAR";

    case MAMA_MD_MSG_TYPE_IMBALANCE:
        return "IMBALANCE";

    case MAMA_MD_MSG_TYPE_SECURITY_STATUS:
        return "SECURITY_STATUS";

    case MAMA_MD_MSG_TYPE_NEWS_HEADLINE:
        return "NEWS_HEADLINE";

    case MAMA_MD_MSG_TYPE_NEWS_STORY:
        return "NEWS_STORY";

    case MAMA_MD_MSG_TYPE_NEWS_QUERY:
        return "NEWS_QUERY";

    case MAMA_MD_MSG_TYPE_MISC:
        return "MISC";
    }
    return "UNKNOWN";
}

mamaMsgType
mamaMdMsgType_compatMsgType (mamaMdMsgType type)
{
    switch (type)
    {
    case MAMA_MD_MSG_TYPE_CANCEL:
        return MAMA_MSG_TYPE_CANCEL;

    case MAMA_MD_MSG_TYPE_ERROR:
        return MAMA_MSG_TYPE_ERROR;

    case MAMA_MD_MSG_TYPE_CORRECTION:
        return MAMA_MSG_TYPE_CORRECTION;

    case MAMA_MD_MSG_TYPE_CLOSING:
        return MAMA_MSG_TYPE_CLOSING;

    case MAMA_MD_MSG_TYPE_SYMBOL_ACTION:
        return MAMA_MSG_TYPE_MISC;

    case MAMA_MD_MSG_TYPE_PREOPENING:
        return MAMA_MSG_TYPE_PREOPENING;

    case MAMA_MD_MSG_TYPE_QUOTE:
        return MAMA_MSG_TYPE_QUOTE;

    case MAMA_MD_MSG_TYPE_TRADE:
        return MAMA_MSG_TYPE_TRADE;

    case MAMA_MD_MSG_TYPE_BOOK_INITIAL:
        return MAMA_MSG_TYPE_BOOK_INITIAL;

    case MAMA_MD_MSG_TYPE_MISC:
        return MAMA_MSG_TYPE_MISC;

    default:
        return MAMA_MSG_TYPE_MISC;
    }
    return MAMA_MSG_TYPE_MISC;
}
