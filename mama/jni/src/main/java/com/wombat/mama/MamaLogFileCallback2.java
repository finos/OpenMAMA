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

package com.wombat.mama;

/* This interface extends the MamaLogFileCallback and supplies and
 * additional function to be used with setLogCallback. It will be
 * invoked every time a mama log is written.
 */
public interface MamaLogFileCallback2 extends MamaLogFileCallback
{
    /*
     * This function will be called every time a mama log is written.
     *
     * @param level [I] The mama log leve.
     * @param message [I] The message to be logged.
     */
    void onLog(MamaLogLevel level, String message);    
}
