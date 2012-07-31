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

#include "MamdaUtils.h"
#include "mamda/MamdaCommonFields.h"
#include "mamda/MamdaDataException.h"
#include "version.h"

namespace Wombat
{

    void getSymbolAndPartId (
        const MamaMsg&  msg,
        const char*&    symbol,
        const char*&    partId)
    {
        symbol = NULL;
        partId = NULL;

        if(!msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, symbol))
        {        
            if(!msg.tryString (MamdaCommonFields::INDEX_SYMBOL, symbol))
            {
                if (!msg.tryString (MamdaCommonFields::SYMBOL, symbol))
                {
                    mama_log (MAMA_LOG_LEVEL_FINEST, 
                              "getSymbolAndPartId: cannot find symbol field in message");
                }
            }
        }


        if (!msg.tryString (MamdaCommonFields::PART_ID, partId))
        {
            // No explicit part ID in message, but maybe in symbol.
            if (NULL == symbol)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,  
                          "getSymbolAndPartId: cannot find symbol or part id fields in message");
                return;
            }   

            const char* lastDot = strrchr (symbol, '.');

            if (lastDot!= NULL)
            {
                if (lastDot+1 != '\0')
                {
                    partId = lastDot+1;
                }
                else
                {                 
                    mama_log (MAMA_LOG_LEVEL_FINEST, 
                              "getSymbolAndPartId: cannot find part id field in message");
                }
            }
            else
                mama_log (MAMA_LOG_LEVEL_FINEST, 
                          "getSymbolAndPartId: cannot find part id field in message");
        }
    }


    void getTimeStamps (
        const MamaMsg&  msg,
        MamaDateTime&   srcTime,
        MamaDateTime&   activityTime,
        MamaDateTime&   lineTime,
        MamaDateTime&   sendTime)
    {
        msg.tryDateTime (MamdaCommonFields::SRC_TIME,      srcTime);
        msg.tryDateTime (MamdaCommonFields::ACTIVITY_TIME, activityTime);
        msg.tryDateTime (MamdaCommonFields::LINE_TIME,     lineTime);
        msg.tryDateTime (MamdaCommonFields::SEND_TIME,     sendTime);
    }

} // namespace
