/* $Id:
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

package com.wombat.mama;

/**
* Class containing constants and utility methods for dealing with subscription
* level quality events.
*/
public class MamaQuality
{
    /** The quality of the subscription is ok. 
        The integrity of the data can be guaranteed.*/
    public static final short QUALITY_OK            = 0;

    /** The quality of the subscription is possibly stale.
        The integrity of the data must be treated as suspect. The middleware
        has informed MAMA that data is being lost but a gap has not yet been
        detected.*/
    public static final short QUALITY_MAYBE_STALE   = 1;

    /** The quality of the subscription is stale. 
        A gap has been detected in the message stream. The integrity of the
        data has been compromised. Data should be treated as invalid until a
        recap is received.*/
    public static final short QUALITY_STALE         = 2;

    /** As with <code>QUALITY_MAYBE_STALE</code>*/
    public static final short QUALITY_PARTIAL_STALE = 3;
    
    public static final short QUALITY_FORCED_STALE  = 4;

    /** Messages with duplicate sequence numbers have been received for the 
        subscription. */
    public static final short QUALITY_DUPLICATE     = 5; 
    
    /** Invalid quality state value.*/
    public static final short QUALITY_UNKNOWN       = 99;

    /** 
      Get a stringified representation of the quality.

      @param quality A valid quality short value.
      @return The string representation of the quality.
     */
    public static String toString (short quality)
    {
        switch (quality)
        {
        case QUALITY_OK:            return "OK";
        case QUALITY_MAYBE_STALE:   return "MaybeStale";
        case QUALITY_STALE:         return "Stale";
        case QUALITY_PARTIAL_STALE: return "PartialStale";
        case QUALITY_FORCED_STALE:  return "ForcedStale";
        case QUALITY_DUPLICATE:     return "Duplicate";
        case QUALITY_UNKNOWN:       return "Quality Unknown";
        }
        return "Error";  // throw?
    }
}
