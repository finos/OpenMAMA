/* $Id: //commsof_depot/platform/products/6.0/trunk/mamda/c_cpp/src/cpp/MamdaErrorCode.h#1 $
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

#ifndef MamdaErrorCodeH
#define MamdaErrorCodeH


#include <mamda/MamdaErrorListener.h>
#include <string.h>


namespace Wombat
{

static const char* stringForErrorCode (const MamdaErrorCode& errorCode)
{
    switch (errorCode)
    {
        case MAMDA_ERROR_NO_ERROR:              return "No Error";
        case MAMDA_ERROR_LINE_DOWN:             return "Line Down";
        case MAMDA_ERROR_NO_SUBSCRIBERS:        return "No Subscribers";
        case MAMDA_ERROR_BAD_SYMBOL:            return "Bad Symbol";
        case MAMDA_ERROR_EXPIRED:               return "Expired";
        case MAMDA_ERROR_TIME_OUT:              return "Time Out";
        case MAMDA_ERROR_MISC:                  return "Misc";
        case MAMDA_ERROR_STALE:                 return "Stale";
        case MAMDA_ERROR_PLATFORM_STATUS:       return "Platform Status";
        case MAMDA_ERROR_ENTITLEMENT:           return "Not Entitled";
        case MAMDA_ERROR_NOT_FOUND:             return "Not Found";
        case MAMDA_ERROR_POSSIBLY_STALE:        return "Possibly Stale";
        case MAMDA_ERROR_NOT_PERMISSIONED:      return "Not Permissioned";
        case MAMDA_ERROR_TOPIC_CHANGE:          return "Topic Change";
        case MAMDA_ERROR_BANDWIDTH_EXCEEDED:    return "Bandwidth Exceeded";
        case MAMDA_ERROR_DELETE:                return "Delete";
        case MAMDA_ERROR_EXCEPTION:             return "Exception";
        default:                                return "Unknown";
    }
}

static MamdaErrorCode errorCodeForMamaStatus (MamaStatus status)
{
    switch (status.getStatus())
    {
        case MAMA_STATUS_OK:                   return MAMDA_ERROR_NO_ERROR;
        case MAMA_STATUS_NO_SUBSCRIBERS:       return MAMDA_ERROR_NO_SUBSCRIBERS;
        case MAMA_STATUS_BAD_SYMBOL:           return MAMDA_ERROR_BAD_SYMBOL;
        case MAMA_STATUS_EXPIRED:              return MAMDA_ERROR_EXPIRED;
        case MAMA_STATUS_TIMEOUT:              return MAMDA_ERROR_TIME_OUT;
        case MAMA_STATUS_PLATFORM:	       return MAMDA_ERROR_PLATFORM_STATUS;
        case MAMA_STATUS_NOT_ENTITLED:         return MAMDA_ERROR_ENTITLEMENT;
        case MAMA_STATUS_NOT_FOUND:            return MAMDA_ERROR_NOT_FOUND;
        case MAMA_STATUS_NOT_PERMISSIONED:     return MAMDA_ERROR_NOT_PERMISSIONED;
        case MAMA_STATUS_BANDWIDTH_EXCEEDED:   return MAMDA_ERROR_BANDWIDTH_EXCEEDED;
        case MAMA_STATUS_DELETE:               return MAMDA_ERROR_DELETE;
        default:                               return MAMDA_ERROR_MISC;
    }
}

static MamdaErrorSeverity severityForErrorCode (MamdaErrorCode errorCode)
{
    switch (errorCode)
    {
        case MAMDA_ERROR_NO_ERROR:    		return MAMDA_SEVERITY_OK;
        case MAMDA_ERROR_NOT_FOUND: 		return MAMDA_SEVERITY_LOW;
        default:                                return MAMDA_SEVERITY_HIGH;
    }
}


}

#endif // MamdaErrorCodeH


