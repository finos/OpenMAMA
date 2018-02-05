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

#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static bool initialised = false;

    const MamaFieldDescriptor*  MamdaCommonFields::SYMBOL        = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::ISSUE_SYMBOL  = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::INDEX_SYMBOL  = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::PART_ID       = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::SEQ_NUM       = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::SRC_TIME      = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::LINE_TIME     = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::ACTIVITY_TIME = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::SEND_TIME     = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::PUB_ID        = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::MSG_QUAL      = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::MSG_SEQ_NUM   = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::MSG_NUM       = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::MSG_TOTAL     = NULL;
    const MamaFieldDescriptor*  MamdaCommonFields::SENDER_ID     = NULL;


    void MamdaCommonFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        SYMBOL           = dictionary.getFieldByName ("wSymbol");
        ISSUE_SYMBOL     = dictionary.getFieldByName ("wIssueSymbol");
        INDEX_SYMBOL     = dictionary.getFieldByName ("wIndexSymbol");
        PART_ID          = dictionary.getFieldByName ("wPartId");
        SEQ_NUM          = dictionary.getFieldByName ("wSeqNum");
        SRC_TIME         = dictionary.getFieldByName ("wSrcTime");
        LINE_TIME        = dictionary.getFieldByName ("wLineTime");
        ACTIVITY_TIME    = dictionary.getFieldByName ("wActivityTime");
        PUB_ID           = dictionary.getFieldByName ("wPubId");
        MSG_QUAL         = dictionary.getFieldByName ("wMsgQual");

        MSG_SEQ_NUM      = new MamaFieldDescriptor   (MamaReservedFieldSeqNum);
        MSG_NUM          = new MamaFieldDescriptor   (MamaReservedFieldMsgNum);
        MSG_TOTAL        = new MamaFieldDescriptor   (MamaReservedFieldMsgTotal);
        SEND_TIME        = new MamaFieldDescriptor   (MamaReservedFieldSendTime);
        SENDER_ID        = new MamaFieldDescriptor   (MamaReservedFieldSenderId);

        initialised = true;
    }

    void MamdaCommonFields::reset ()
    {
        initialised      = false;
        SYMBOL           = NULL;
        ISSUE_SYMBOL     = NULL;
        INDEX_SYMBOL     = NULL;
        PART_ID          = NULL;
        SEQ_NUM          = NULL;
        SRC_TIME         = NULL;
        LINE_TIME        = NULL;
        ACTIVITY_TIME    = NULL;
        PUB_ID           = NULL;
        MSG_QUAL         = NULL;

        if (MSG_SEQ_NUM)
        {
            delete MSG_SEQ_NUM;
            MSG_SEQ_NUM = NULL;
        }

        if (MSG_NUM)
        {
            delete MSG_NUM;
            MSG_NUM = NULL;
        }

        if (MSG_TOTAL)
        {
            delete MSG_TOTAL;
            MSG_TOTAL = NULL;
        }

        if (SEND_TIME)
        {
            delete SEND_TIME;
            SEND_TIME = NULL;
        }

        if (SENDER_ID)
        {
            delete SENDER_ID;
            SENDER_ID = NULL;
        }
    }

    bool MamdaCommonFields::isSet ()
    {
        return initialised;
    }

}
