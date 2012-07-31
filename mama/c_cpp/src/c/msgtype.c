/* $Id: msgtype.c,v 1.14.4.1.16.4 2011/08/10 14:53:25 nicholasmarriott Exp $
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
#include <mama/msg.h>
#include <mama/msgtype.h>
#include <mama/reservedfields.h>


mamaMsgType 
mamaMsgType_typeForMsg (const mamaMsg msg)
{
    int32_t result = MAMA_MSG_TYPE_UNKNOWN;

    if (mamaMsg_getI32 (msg,
                    MamaFieldMsgType.mName,
                    MamaFieldMsgType.mFid,
                    &result) != MAMA_STATUS_OK)
        result = MAMA_MSG_TYPE_UNKNOWN;

    return (mamaMsgType) result;
}

const char* 
mamaMsgType_stringForMsg (const mamaMsg msg)
{
    return mamaMsgType_stringForType (mamaMsgType_typeForMsg (msg)) ;
}


const char* 
mamaMsgType_stringForType (mamaMsgType type)
{
    switch  (type)
    {
    case MAMA_MSG_TYPE_UPDATE:               return "UPDATE";
    case MAMA_MSG_TYPE_INITIAL:              return "INITIAL";
    case MAMA_MSG_TYPE_CANCEL:               return "CANCEL";
    case MAMA_MSG_TYPE_ERROR:                return "ERROR";
    case MAMA_MSG_TYPE_CORRECTION:           return "CORRECTION";
    case MAMA_MSG_TYPE_CLOSING:              return "CLOSING";
    case MAMA_MSG_TYPE_RECAP:                return "RECAP";
    case MAMA_MSG_TYPE_DELETE:               return "DELETE";
    case MAMA_MSG_TYPE_EXPIRE:               return "EXPIRE";
    case MAMA_MSG_TYPE_SNAPSHOT:             return "SNAPSHOT";
    case MAMA_MSG_TYPE_PREOPENING:           return "PREOPENING";
    case MAMA_MSG_TYPE_QUOTE:                return "QUOTE";
    case MAMA_MSG_TYPE_TRADE:                return "TRADE";
    case MAMA_MSG_TYPE_ORDER:                return "ORDER";
    case MAMA_MSG_TYPE_BOOK_INITIAL:         return "BOOK_INITIAL";
    case MAMA_MSG_TYPE_BOOK_UPDATE:          return "BOOK_UPDATE";
    case MAMA_MSG_TYPE_BOOK_CLEAR:           return "BOOK_CLEAR";
    case MAMA_MSG_TYPE_BOOK_RECAP:           return "BOOK_RECAP";
    case MAMA_MSG_TYPE_BOOK_SNAPSHOT:        return "BOOK_SNAPSHOT";
    case MAMA_MSG_TYPE_NOT_PERMISSIONED:     return "NOT_PERMISSIONED";
    case MAMA_MSG_TYPE_NOT_FOUND:            return "NOT_FOUND";
    case MAMA_MSG_TYPE_END_OF_INITIALS:      return "END_OF_INITIALS";
    case MAMA_MSG_TYPE_WOMBAT_REQUEST:       return "WOMBAT_REQUEST";
    case MAMA_MSG_TYPE_WOMBAT_CALC:          return "WOMBAT_CALC";
    case MAMA_MSG_TYPE_SEC_STATUS:           return "SECURITY_STATUS";
    case MAMA_MSG_TYPE_DDICT_SNAPSHOT:       return "DDICT_SNAPSHOT";
    case MAMA_MSG_TYPE_MISC:                 return "MISC";
    case MAMA_MSG_TYPE_TIBRV:                return "TIBRV";
    case MAMA_MSG_TYPE_FEATURE_SET:          return "FEATURE_SET";
    case MAMA_MSG_TYPE_SYNC_REQUEST:         return "SYNC_REQUEST";
    case MAMA_MSG_TYPE_REFRESH:              return "REFRESH";
    case MAMA_MSG_TYPE_WORLD_VIEW:           return "WORLD_VIEW";
    case MAMA_MSG_TYPE_NULL:                 return "NULL";    
    case MAMA_MSG_TYPE_UNKNOWN:              return "UNKNOWN";
    default:                                 return "error";
    }

}

