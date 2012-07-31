/* $Id: msgstatus.c,v 1.9.24.5 2011/08/29 11:52:43 ianbell Exp $
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
#include <mama/msgstatus.h>
#include "msgimpl.h"

mamaMsgStatus
mamaMsgStatus_statusForMsg (const mamaMsg msg)
{
    mamaMsgStatus status;
    mamaMsgImpl_getStatus (msg, &status);
    
    return status;
}

const char *
mamaMsgStatus_stringForMsg (const mamaMsg msg)
{
    return mamaMsgStatus_stringForStatus (
                                mamaMsgStatus_statusForMsg (msg)) ;
}

const char *
mamaMsgStatus_stringForStatus (const mamaMsgStatus status)
{
    switch  (status)
    {
    case MAMA_MSG_STATUS_OK:                   return "OK";
    case MAMA_MSG_STATUS_LINE_DOWN:            return "LINE_DOWN";
    case MAMA_MSG_STATUS_NO_SUBSCRIBERS:       return "NO_SUBSCRIBERS";
    case MAMA_MSG_STATUS_BAD_SYMBOL:           return "BAD_SYMBOL";
    case MAMA_MSG_STATUS_EXPIRED:              return "EXPIRED";
    case MAMA_MSG_STATUS_TIMEOUT:              return "TIMEOUT";
    case MAMA_MSG_STATUS_MISC:                 return "MISC";
    case MAMA_MSG_STATUS_STALE:                return "STALE";
    case MAMA_MSG_STATUS_TIBRV_STATUS:         return "PLATFORM_SPECIFIC_STATUS";    
    case MAMA_MSG_STATUS_NOT_ENTITLED:         return "NOT_ENTITLED";
    case MAMA_MSG_STATUS_NOT_FOUND:            return "NOT_FOUND";
    case MAMA_MSG_STATUS_POSSIBLY_STALE:       return "POSSIBLY_STALE";
    case MAMA_MSG_STATUS_NOT_PERMISSIONED:     return "NOT_PERMISSIONED";
    case MAMA_MSG_STATUS_TOPIC_CHANGE:         return "TOPIC_CHANGE";
    case MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED:   return "BANDWIDTH_EXCEEDED";
    case MAMA_MSG_STATUS_DUPLICATE:            return "DUPLICATE";
    case MAMA_MSG_STATUS_UNKNOWN:              return "UNKNOWN";
    default: return "error";
    }
}

