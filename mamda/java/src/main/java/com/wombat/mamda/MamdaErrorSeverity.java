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

/**
 * MamdaErrorSeverity defines MAMDA error severities.  The severity is
 * a hint that can be used to determine what action to take (e.g.,
 * destroy the subscription).
 */
public class MamdaErrorSeverity
{
    public static final short  MAMDA_SEVERITY_OK   = 0;   // never sent
    public static final short  MAMDA_SEVERITY_LOW  = 1;
    public static final short  MAMDA_SEVERITY_HIGH = 2;

    public static short severityForErrorCode (short code)
    {
        switch (code)
        {
            case MamdaErrorCode.MAMDA_NO_ERROR:        return MAMDA_SEVERITY_OK;
            case MamdaErrorCode.MAMDA_ERROR_NOT_FOUND: return MAMDA_SEVERITY_LOW;
            default:                                   return MAMDA_SEVERITY_HIGH;
        }
    }
}
