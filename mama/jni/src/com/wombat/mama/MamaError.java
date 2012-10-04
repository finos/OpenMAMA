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
* Class containing constants and utility methods for Mama internal errors.
*/
public class MamaError
{
    /** The last error in the current thread*/
    public static final int ERROR_DEFAULT         = 0;

    public static final int ERROR_INITIAL_TIMEOUT = 1;

    public static final int ERROR_RECAP_TIMEOUT   = 2;
    
    public static final int ERROR_NOT_ENTITLED    = 3;
    
    public static final int ERROR_NO_RESOLVER     = 4;
    
    public static final int ERROR_UNKNOWN         = 999;

    /** 
      Get a stringified representation of the error.

      @param quality A valid quality short value.
      @return The string representation of the quality.
     */
    public static String toString (int error)
    {
        switch (error)
        {
        case ERROR_DEFAULT:         return "DEFAULT";
        case ERROR_INITIAL_TIMEOUT: return "INITIAL_TIMEOUT";
        case ERROR_RECAP_TIMEOUT:   return "RECAP_TIMEOUT";
        case ERROR_NOT_ENTITLED:    return "NOT_ENTITLED";
        case ERROR_NO_RESOLVER:     return "NO_RESOLVER";
        case ERROR_UNKNOWN:         return "UNKNOWN";
        }
        return "Error";  // throw?
    }
}
