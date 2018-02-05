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

#ifndef MamaCPPReservedFieldsH
#define MamaCPPReservedFieldsH


#include <mama/mamacpp.h>
#include <mama/reservedfields.h>

namespace Wombat
{

    class MamaFieldDescriptor;


    class MAMACPPExpDLL MamaReservedFields
    {

    public:
            static void  initReservedFields    ();
            static void  uninitReservedFields  ();
            
            static const MamaFieldDescriptor*  MsgType;
            static const MamaFieldDescriptor*  MsgStatus;
            static const MamaFieldDescriptor*  FieldIndex;
            static const MamaFieldDescriptor*  MsgNum;
            static const MamaFieldDescriptor*  MsgTotal;
            static const MamaFieldDescriptor*  SeqNum;
            static const MamaFieldDescriptor*  FeedName;
            static const MamaFieldDescriptor*  FeedHost;
            static const MamaFieldDescriptor*  FeedGroup;
            static const MamaFieldDescriptor*  ItemSeqNum;
            static const MamaFieldDescriptor*  SendTime;
            static const MamaFieldDescriptor*  AppDataType;
            static const MamaFieldDescriptor*  AppMsgType;
            static const MamaFieldDescriptor*  SenderId;
            static const MamaFieldDescriptor*  MsgQual;
            static const MamaFieldDescriptor*  ConflateQuoteCount;
            static const MamaFieldDescriptor*  EntitleCode;
            static const MamaFieldDescriptor*  SymbolList;

    };
    
} // namespace Wombat

#endif // MamaReservedFieldsH

