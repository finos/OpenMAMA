/* $Id: msgstatus.h,v 1.11.24.7 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MamaMsgStatusH__
#define MamaMsgStatusH__

#if defined(__cplusplus)
extern "C" {
#endif

#include <mama/msg.h>

/**
 * Utility class (enum) for interpreting message status.
 */

typedef enum mamaMsgStatus_
{
    /** OK */
    MAMA_MSG_STATUS_OK                  =   0,

    /** The feed handler has detected a Line Down */
    MAMA_MSG_STATUS_LINE_DOWN           =   1,

    /** The feed handler does not have any subscribers to the subject */
    MAMA_MSG_STATUS_NO_SUBSCRIBERS      =   2,

    /** The symbol does not exist */
    MAMA_MSG_STATUS_BAD_SYMBOL          =   3,

    /** Expired */
    MAMA_MSG_STATUS_EXPIRED             =   4,

    /** A time out occurred */
    MAMA_MSG_STATUS_TIMEOUT             =   5,

    /** Miscellaneous status. Not an error */
    MAMA_MSG_STATUS_MISC                =   6,

    /** The subject is stale. Messages may have been dropped */
    MAMA_MSG_STATUS_STALE               =   7,

    MAMA_MSG_STATUS_TIBRV_STATUS        =   8,

    /** Error in the underlying messaging API */
    MAMA_MSG_STATUS_PLATFORM_STATUS     =   8,

    /** Not entitled to a subject */
    MAMA_MSG_STATUS_NOT_ENTITLED        =   9,

    /** Not found */
    MAMA_MSG_STATUS_NOT_FOUND           =   10,

    /** Messages may have been dropped */
    MAMA_MSG_STATUS_POSSIBLY_STALE      =   11,

    /** Not permissioned for the subject */
    MAMA_MSG_STATUS_NOT_PERMISSIONED    =   12,

    /** Topic renamed */
    MAMA_MSG_STATUS_TOPIC_CHANGE        =   13,

    /** Bandwidth exceeded */
    MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED  =   14,

    /** Message with duplicate sequence number */
    MAMA_MSG_STATUS_DUPLICATE           =   15,

    /** Unknown status */
    MAMA_MSG_STATUS_UNKNOWN             =   99

} mamaMsgStatus;

/**
 * Extract the status from the supplied message.
 *
 * @param msg The message.
 */
MAMAExpDLL
mamaMsgStatus
mamaMsgStatus_statusForMsg (const mamaMsg msg);

/**
 * Return the status as a string given a message.
 *
 * @return The string.
 */
MAMAExpDLL
const char*
mamaMsgStatus_stringForMsg (const mamaMsg msg);

/**
 * Return a text description of the message's status.
 *
 * @return The description.
 */
MAMAExpDLL
const char*
mamaMsgStatus_stringForStatus (mamaMsgStatus type);

#if defined(__cplusplus)
}
#endif

#endif  /* MAMA_MSG_STATUS_H__ */

