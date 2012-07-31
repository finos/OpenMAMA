/* $Id$
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

#include <mamda/MamdaSecurityStatus.h>
#include <string.h>

namespace Wombat
{

    static const char* SECURITY_STATUS_STR_NONE       = "None";
    static const char* SECURITY_STATUS_STR_NORMAL     = "Normal";
    static const char* SECURITY_STATUS_STR_CLOSED     = "Closed";
    static const char* SECURITY_STATUS_STR_HALTED     = "Halted";
    static const char* SECURITY_STATUS_STR_NOT_EXIST  = "NotExists";
    static const char* SECURITY_STATUS_STR_DELETED    = "Deleted";
    static const char* SECURITY_STATUS_STR_AUCTION    = "Auction";
    static const char* SECURITY_STATUS_STR_CROSSING   = "Crossing";
    static const char* SECURITY_STATUS_STR_SUSPENDED  = "Suspended";
    static const char* SECURITY_STATUS_STR_AT_LAST    = "AtLast";

    const char* toString (MamdaSecurityStatus  securityStatus)
    {
        switch (securityStatus)
        {
            case SECURITY_STATUS_NONE:      return SECURITY_STATUS_STR_NONE;
            case SECURITY_STATUS_NORMAL:    return SECURITY_STATUS_STR_NORMAL;
            case SECURITY_STATUS_CLOSED:    return SECURITY_STATUS_STR_CLOSED;
            case SECURITY_STATUS_HALTED:    return SECURITY_STATUS_STR_HALTED;
            case SECURITY_STATUS_NOT_EXIST: return SECURITY_STATUS_STR_NOT_EXIST;
            case SECURITY_STATUS_DELETED:   return SECURITY_STATUS_STR_DELETED;
            case SECURITY_STATUS_AUCTION:   return SECURITY_STATUS_STR_AUCTION;
            case SECURITY_STATUS_CROSSING:  return SECURITY_STATUS_STR_CROSSING;
            case SECURITY_STATUS_SUSPENDED: return SECURITY_STATUS_STR_SUSPENDED;
            case SECURITY_STATUS_AT_LAST:   return SECURITY_STATUS_STR_AT_LAST;
            case SECURITY_STATUS_UNKNOWN:
            default:                        return "Unknown";
        }
    }

    MamdaSecurityStatus mamdaSecurityStatusFromString (const char*  securityStatus)
    {
        if (securityStatus == NULL)
        {
           return SECURITY_STATUS_UNKNOWN;
        }

        // Older FH configurations sent strings:
        if (strcmp (securityStatus, SECURITY_STATUS_STR_NONE)      == 0)    return SECURITY_STATUS_NONE;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_NORMAL)    == 0)    return SECURITY_STATUS_NORMAL;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_CLOSED)    == 0)    return SECURITY_STATUS_CLOSED;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_HALTED)    == 0)    return SECURITY_STATUS_HALTED;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_NOT_EXIST) == 0)    return SECURITY_STATUS_NOT_EXIST;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_DELETED)   == 0)    return SECURITY_STATUS_DELETED;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_AUCTION)   == 0)    return SECURITY_STATUS_AUCTION;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_CROSSING)  == 0)    return SECURITY_STATUS_CROSSING;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_SUSPENDED) == 0)    return SECURITY_STATUS_SUSPENDED;
        if (strcmp (securityStatus, SECURITY_STATUS_STR_AT_LAST)   == 0)    return SECURITY_STATUS_AT_LAST;

        // A misconfigured FH might send numbers as strings:
        if (strcmp (securityStatus,"0") == 0)   return SECURITY_STATUS_NONE;
        if (strcmp (securityStatus,"1") == 0)   return SECURITY_STATUS_NORMAL;
        if (strcmp (securityStatus,"2") == 0)   return SECURITY_STATUS_CLOSED;
        if (strcmp (securityStatus,"3") == 0)   return SECURITY_STATUS_HALTED;
        if (strcmp (securityStatus,"4") == 0)   return SECURITY_STATUS_NOT_EXIST;
        if (strcmp (securityStatus,"5") == 0)   return SECURITY_STATUS_DELETED;
        if (strcmp (securityStatus,"6") == 0)   return SECURITY_STATUS_AUCTION;
        if (strcmp (securityStatus,"7") == 0)   return SECURITY_STATUS_CROSSING;
        if (strcmp (securityStatus,"8") == 0)   return SECURITY_STATUS_SUSPENDED;
        if (strcmp (securityStatus,"9") == 0)   return SECURITY_STATUS_AT_LAST;

        return SECURITY_STATUS_UNKNOWN;
    }

}
