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

#ifndef MamdaSecurityStatusH
#define MamdaSecurityStatusH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing the status of a security
     * such as whether or not it is halted or closed for trading.
     */
    enum MamdaSecurityStatus
    {
        SECURITY_STATUS_NONE,        /* toString() returns "None"   */
        SECURITY_STATUS_NORMAL,      /* toString() returns "Normal"  */
        SECURITY_STATUS_CLOSED,      /* toString() returns "Closed"  */
        SECURITY_STATUS_HALTED,      /* toString() returns "Halted" */
        SECURITY_STATUS_NOT_EXIST,   /* toString() returns "NotExist" */
        SECURITY_STATUS_DELETED,     /* toString() returns "Deleted" */
        SECURITY_STATUS_AUCTION,     /* toString() returns "Auction" */
        SECURITY_STATUS_CROSSING,    /* toString() returns "Crossing" */
        SECURITY_STATUS_SUSPENDED,   /* toString() returns "Suspended" */
        SECURITY_STATUS_AT_LAST,     /* toString() returns "AtLast" */
        SECURITY_STATUS_UNKNOWN = 99 /* toString() returns "Unknown" */
    };

    /**
     * Convert a MamdaSecurityStatus to an appropriate, displayable
     * string.
     *
     * @param securityStatus The security status as an enumerated type.
     */
    MAMDAExpDLL const char* toString (MamdaSecurityStatus  securityStatus);

    /**
     * Convert a string representation of a security status to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param securityStatusStr The security status as a string.
     *
     * @return The security status as an enumerated type.
     */
    MAMDAExpDLL MamdaSecurityStatus mamdaSecurityStatusFromString (
        const char* securityStatusStr);

}

#endif // MamdaSecurityStatusH
