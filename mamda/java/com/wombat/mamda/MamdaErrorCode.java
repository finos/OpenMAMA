/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda;

import com.wombat.mama.MamaMsgStatus;

/**
 * MamdaErrorCode defines MAMDA error codes.
 */
public class MamdaErrorCode
{
    public static final short  MAMDA_NO_ERROR = 0;   // never sent

    /** The feed handler has detected a Line Down. */
    public static final short MAMDA_ERROR_LINE_DOWN = 1;

    /** The feed handler does not have any subscribers to the subject */
    public static final short MAMDA_ERROR_NO_SUBSCRIBERS = 2;

    /** The symbol does not exist */
    public static final short MAMDA_ERROR_BAD_SYMBOL = 3;

    /** Expired */
    public static final short MAMDA_ERROR_EXPIRED = 4;

    /** A time out occurred */
    public static final short MAMDA_ERROR_TIMEOUT = 5;

    /** Miscellaneous status. Not an error */
    public static final short MAMDA_ERROR_MISC = 6;

    /** The subject is stale. Messages may have been dropped */
    public static final short MAMDA_ERROR_STALE = 7;

    /** Error in the underlying messaging API */
    public static final short MAMDA_ERROR_PLATFORM_STATUS = 8;

    /** Not entitled to a subject */
    public static final short MAMDA_ERROR_NOT_ENTITLED = 9;

    /** Not found */
    public static final short MAMDA_ERROR_NOT_FOUND = 10;

    /** Messages may have been dropped */
    public static final short MAMDA_ERROR_POSSIBLY_STALE = 11;

    /** Not permissioned for the subject */
    public static final short MAMDA_ERROR_NOT_PERMISSIONED = 12;

    /** Topic renamed */
    public static final short MAMDA_ERROR_TOPIC_CHANGE = 13;

    /** Bandwidth exceeded */
    public static final short MAMDA_ERROR_BANDWIDTH_EXCEEDED = 14;

    public static final short MAMDA_ERROR_EXCEPTION = 999;


    /**
     * Return a text description of the message's status.
     *
     * @return The description.
     */
    public  static String stringForMamdaError( final int type )
    {
        switch ( type )
        {
        case MAMDA_NO_ERROR:                 return "OK";
        case MAMDA_ERROR_LINE_DOWN:          return "LINE_DOWN";
        case MAMDA_ERROR_NO_SUBSCRIBERS:     return "NO_SUBSCRIBERS";
        case MAMDA_ERROR_BAD_SYMBOL:         return "BAD_SYMBOL";
        case MAMDA_ERROR_EXPIRED:            return "EXPIRED";
        case MAMDA_ERROR_TIMEOUT:            return "TIMEOUT";
        case MAMDA_ERROR_MISC:               return "MISC";
        case MAMDA_ERROR_STALE:              return "STALE";
        case MAMDA_ERROR_PLATFORM_STATUS:    return "PLATFORM_SPECIFIC_MAMDA_ERROR";
        case MAMDA_ERROR_NOT_ENTITLED:       return "NOT_ENTITLED";
        case MAMDA_ERROR_NOT_FOUND:          return "NOT_FOUND";
        case MAMDA_ERROR_POSSIBLY_STALE:     return "POSSIBLY_STALE";
        case MAMDA_ERROR_NOT_PERMISSIONED:   return "NOT_PERMISSIONED";
        case MAMDA_ERROR_TOPIC_CHANGE:       return "TOPIC_CHANGE";
        case MAMDA_ERROR_BANDWIDTH_EXCEEDED: return "BANDWIDTH_EXCEEDED";
        case MAMDA_ERROR_EXCEPTION:          return "EXCEPTION PROCESSING MESSAGE";
        default: return "UNKNOWN";
        }
    }

    public static short codeForMamaMsgStatus (short wombatStatus)
    {
        switch (wombatStatus)
        {
        case MamaMsgStatus.STATUS_OK:                   return MAMDA_NO_ERROR;
        case MamaMsgStatus.STATUS_LINE_DOWN:            return MAMDA_ERROR_LINE_DOWN;
        case MamaMsgStatus.STATUS_NO_SUBSCRIBERS:       return MAMDA_ERROR_NO_SUBSCRIBERS;
        case MamaMsgStatus.STATUS_BAD_SYMBOL:           return MAMDA_ERROR_BAD_SYMBOL;
        case MamaMsgStatus.STATUS_EXPIRED:              return MAMDA_ERROR_EXPIRED;
        case MamaMsgStatus.STATUS_TIMEOUT:              return MAMDA_ERROR_TIMEOUT;
        case MamaMsgStatus.STATUS_MISC:                 return MAMDA_ERROR_MISC;
        case MamaMsgStatus.STATUS_STALE:                return MAMDA_ERROR_STALE;
        case MamaMsgStatus.STATUS_PLATFORM_STATUS:      return MAMDA_ERROR_PLATFORM_STATUS;
        case MamaMsgStatus.STATUS_NOT_ENTITLED:         return MAMDA_ERROR_NOT_ENTITLED;
        case MamaMsgStatus.STATUS_NOT_FOUND:            return MAMDA_ERROR_NOT_FOUND;
        case MamaMsgStatus.STATUS_POSSIBLY_STALE:       return MAMDA_ERROR_POSSIBLY_STALE;
        case MamaMsgStatus.STATUS_NOT_PERMISSIONED:     return MAMDA_ERROR_NOT_PERMISSIONED;
        case MamaMsgStatus.STATUS_TOPIC_CHANGE:         return MAMDA_ERROR_TOPIC_CHANGE;
        case MamaMsgStatus.STATUS_BANDWIDTH_EXCEEDED:   return MAMDA_ERROR_BANDWIDTH_EXCEEDED;
        case MamaMsgStatus.STATUS_EXCEPTION:            return MAMDA_ERROR_EXCEPTION;
        }

        return -1;
    }
}
