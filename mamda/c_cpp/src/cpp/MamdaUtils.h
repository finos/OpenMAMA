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

#ifndef MamdaUtilsH
#define MamdaUtilsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>
#include <stdio.h>
#include <string.h>

namespace Wombat
{

    /**
     * Get the symbol and (if applicable) the participant ID from this
     * message.
     *
     * @param msg  The message to interrogate for symbol and participant ID.
     *
     * @param symbol (out) The resultant symbol (may be NULL is not
     * present and mustExist is false).
     *
     * @param partId (out) The resultant participant ID (may be NULL
     * if not present or not applicable).
     *
     * @param mustExist Whether it is an error for the symbol not
     * exist in the message.  If true, then a MamdaDataException will
     * be thrown.
     *
     */
    MAMDAExpDLL void getSymbolAndPartId (const MamaMsg&  msg,
                                         const char*&    symbol,
                                         const char*&    partId);


    MAMDAExpDLL void getTimeStamps (const MamaMsg&  msg,
                                    MamaDateTime&   srcTime,
                                    MamaDateTime&   activityTime,
                                    MamaDateTime&   lineTime,
                                    MamaDateTime&   sendTime);
}

#endif // MamdaUtilsH
