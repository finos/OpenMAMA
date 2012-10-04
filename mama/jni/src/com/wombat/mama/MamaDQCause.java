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
 * Causes of data quality events. Provides an enumeration that represents
 * the cause of a data quality event.<p>
 *
 * The cause is supplied only by some middlewares.
 * The following middlewares are supported:<p>
 *
 * <ul>
 * <li>tibrv
 * </ul>
 */

public class MamaDQCause
{
    /** <ul>
     *  <li>tibrv: Slow consumer advisory
     *  </ul>
     */
    public static final short DQ_SUBSCRIBER  = 7;
    /** <ul> 
      * <li>tibrv: Unused
      * </ul>
      */
    public static final short DQ_PUBLISHER   = 8;
    /** <ul> 
      * <li>tibrv: Data loss advisory 
      * </ul>
      */
    public static final short DQ_NETWORK     = 9;
    /** <ul> 
      * <li>tibrv: Data quality event not covered by other categories 
      * </ul>
      */
    public static final short DQ_UNKNOWN     = 10;
    /** <ul> 
      * <li>tibrv: Disconnection advisory 
      * </ul>
      */
    public static final short DQ_DISCONNECT  = 12;
    /** <ul> 
      * <li>tibrv: Connection advisory 
      * </ul>
      */
    public static final short DQ_CONNECT     = 13;

    public static String toString (short cause)
    {
        switch (cause)
        {
        case DQ_SUBSCRIBER: return "Data Quality Subscriber";
        case DQ_PUBLISHER:  return "Data Quality Publisher";
        case DQ_NETWORK:    return "Data Quality Network";
        case DQ_UNKNOWN:    return "Data Quality Unknown";
        case DQ_DISCONNECT: return "Data Quality Disconnect";
        case DQ_CONNECT:    return "Data Quality Connect";
        }
        return "Error";
    }
}
