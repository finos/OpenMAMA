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

#include "mama/mamacpp.h"
#include <mama/MamaReservedFields.h>
#include <mama/MamaFieldDescriptor.h>

#define CREATE_FIELD(x) do {\
    if (!x) {\
        x = new MamaFieldDescriptor (MamaReservedField ## x);\
    }\
} while (0)

#define DESTROY_FIELD(x) do {\
    delete x; x = NULL;\
} while (0)

namespace Wombat
{
    const MamaFieldDescriptor*  MamaReservedFields::MsgType            = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgStatus          = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FieldIndex         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgNum             = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgTotal           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SeqNum             = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedName           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedHost           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::FeedGroup          = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::ItemSeqNum         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SendTime           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::AppDataType        = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::AppMsgType         = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SenderId           = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::MsgQual            = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::ConflateQuoteCount = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::EntitleCode        = NULL;
    const MamaFieldDescriptor*  MamaReservedFields::SymbolList         = NULL;

    void MamaReservedFields::initReservedFields()
    {
        CREATE_FIELD (MsgType);
        CREATE_FIELD (MsgStatus);
        CREATE_FIELD (FieldIndex);
        CREATE_FIELD (MsgNum);
        CREATE_FIELD (MsgTotal);
        CREATE_FIELD (SeqNum);
        CREATE_FIELD (FeedName);
        CREATE_FIELD (FeedHost);
        CREATE_FIELD (FeedGroup);
        CREATE_FIELD (ItemSeqNum);
        CREATE_FIELD (SendTime);
        CREATE_FIELD (AppDataType);
        CREATE_FIELD (AppMsgType);
        CREATE_FIELD (SenderId);
        CREATE_FIELD (MsgQual);
        CREATE_FIELD (ConflateQuoteCount);
        CREATE_FIELD (EntitleCode);
        CREATE_FIELD (SymbolList);
    }

    void MamaReservedFields::uninitReservedFields()
    {    
        DESTROY_FIELD (MsgType);
        DESTROY_FIELD (MsgStatus);
        DESTROY_FIELD (FieldIndex);
        DESTROY_FIELD (MsgNum);
        DESTROY_FIELD (MsgTotal);
        DESTROY_FIELD (SeqNum);
        DESTROY_FIELD (FeedName);
        DESTROY_FIELD (FeedHost);
        DESTROY_FIELD (FeedGroup);
        DESTROY_FIELD (ItemSeqNum);
        DESTROY_FIELD (SendTime);
        DESTROY_FIELD (AppDataType);
        DESTROY_FIELD (AppMsgType);
        DESTROY_FIELD (SenderId);
        DESTROY_FIELD (MsgQual);
        DESTROY_FIELD (ConflateQuoteCount);
        DESTROY_FIELD (EntitleCode);
        DESTROY_FIELD (SymbolList);
    } 

}
// namespace Wombat


