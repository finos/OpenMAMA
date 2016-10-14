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

#include <mama/mama.h>
#include "reservedfieldsimpl.h"

#define CREATE_FIELD(x, type) do {\
    if (!MamaReservedField ## x) {\
        mamaFieldDescriptor_create (&MamaReservedField ## x,\
                                    MamaField ## x.mFid,\
                                    MAMA_FIELD_TYPE_ ## type,\
                                    MamaField ## x.mName);\
    }\
} while (0)

#define DESTROY_FIELD(x) do {\
    if (MamaReservedField ## x) {\
        mamaFieldDescriptor_destroy (MamaReservedField ## x);\
        MamaReservedField ## x = NULL;\
    }\
} while (0)

const long WOMBAT_MAX_RESERVED_FID = 100;

const MamaReservedField MamaFieldMsgType          = { "MdMsgType",          1 };
const MamaReservedField MamaFieldMsgStatus        = { "MdMsgStatus",        2 };
const MamaReservedField MamaFieldFieldIndex       = { "MdFieldIndex",       3 };
const MamaReservedField MamaFieldSubscMsgTypeOld  = { "MdSubscType",        5 };
const MamaReservedField MamaFieldSubscSubjectOld  = { "MdSubscSubject",     6 };
const MamaReservedField MamaFieldMsgNum           = { "MdMsgNum",           7 };
const MamaReservedField MamaFieldMsgTotal         = { "MdMsgTotal",         8 };
const MamaReservedField MamaFieldTibrvResult      = { "MdTibrvResult",      9 };
const MamaReservedField MamaFieldSeqNum           = { "MdSeqNum",          10 };
const MamaReservedField MamaFieldFeedName         = { "MdFeedName",        11 };
const MamaReservedField MamaFieldFeedHost         = { "MdFeedHost",        12 };
const MamaReservedField MamaFieldFeedGroup        = { "MdFeedGroup",       13 };
const MamaReservedField MamaFieldSyncPattern      = { "MdRvSyncPattern",   14 };
const MamaReservedField MamaFieldItemSeqNum       = { "MdItemSeq",         15 };
const MamaReservedField MamaFieldSendTime         = { "MamaSendTime",      16 };
const MamaReservedField MamaFieldAppDataType      = { "MamaAppDataType",   17 };
const MamaReservedField MamaFieldAppMsgType       = { "MamaAppMsgType",    18 };
const MamaReservedField MamaFieldSenderId         = { "MamaSenderId",      20 };
const MamaReservedField MamaFieldMsgQual          = { "wMsgQual",          21 };
const MamaReservedField MamaFieldConflateCount    = { "wConflateCount",    22 };
const MamaReservedField MamaFieldConflateQuoteCount
                                                = { "wConflateQuoteCount", 23 };
const MamaReservedField MamaFieldConflateTradeCount
                                                = { "wConflateTradeCount", 24 };
const MamaReservedField MamaFieldRvDaemon         = { "MdRvDaemon",        51 };
const MamaReservedField MamaFieldRvNetwork        = { "MdRvNetwork",       52 };
const MamaReservedField MamaFieldRvService        = { "MdRvService",       53 };
const MamaReservedField MamaFieldElvinSource      = { "MdElvinSource",     56 };
const MamaReservedField MamaFieldElvinSourceOld   = { "MdSubscSource",     64 };
const MamaReservedField MamaFieldSubscriptionType = { "MdSubscriptionType",60 };
const MamaReservedField MamaFieldSubscMsgType     = { "MdSubscMsgType",    61 };
const MamaReservedField MamaFieldSubscSourceHost  = { "MdSubscSourceHost", 63 };
const MamaReservedField MamaFieldSubscSourceApp   = { "MdSubscSourceApp",  64 };
const MamaReservedField MamaFieldSubscSourceUser  = { "MdSubscSourceUser", 65 };
const MamaReservedField MamaFieldServiceLevel     = { "MdServiceLevel",    66 };
const MamaReservedField MamaFieldSubscSourceIp    = { "MdSubscSourceIp",   67 };
const MamaReservedField MamaFieldSubscSourceAppClass
                                              = { "MdSubscSourceAppClass", 68 };
const MamaReservedField MamaFieldInterfaceVersion = { "InterfaceVersion",  69 };
const MamaReservedField MamaFieldUpdateTopic      = { "MdUpdateTopic",     70 };
const MamaReservedField MamaFieldSubscSymbol      = { "MdSubscSymbol",     71 };
const MamaReservedField MamaFieldTportName        = { "MamaTportName",     72 };
const MamaReservedField MamaFieldSubscNamespace   = { "MdSubscNamespace",  73 };
const MamaReservedField MamaFieldSymbolList       = { "MamaSymbolList",    81 };
const MamaReservedField MamaFieldTemplate         = { "MamaTemplate",      90 };
const MamaReservedField MamaFieldTemplateLen      = { "MamaTemplateLen",   91 };

/* This one should be overridable as a property, if we need it at all. */
const MamaReservedField MamaFieldEntitleCode    = { "wEntitleCode",      496 };

mamaFieldDescriptor MamaReservedFieldMsgType            = NULL;
mamaFieldDescriptor MamaReservedFieldMsgStatus          = NULL;
mamaFieldDescriptor MamaReservedFieldFieldIndex         = NULL;
mamaFieldDescriptor MamaReservedFieldMsgNum             = NULL;
mamaFieldDescriptor MamaReservedFieldMsgTotal           = NULL;
mamaFieldDescriptor MamaReservedFieldSeqNum             = NULL;
mamaFieldDescriptor MamaReservedFieldFeedName           = NULL;
mamaFieldDescriptor MamaReservedFieldFeedHost           = NULL;
mamaFieldDescriptor MamaReservedFieldFeedGroup          = NULL;
mamaFieldDescriptor MamaReservedFieldItemSeqNum         = NULL;
mamaFieldDescriptor MamaReservedFieldSendTime           = NULL;
mamaFieldDescriptor MamaReservedFieldAppDataType        = NULL;
mamaFieldDescriptor MamaReservedFieldAppMsgType         = NULL;
mamaFieldDescriptor MamaReservedFieldSenderId           = NULL;
mamaFieldDescriptor MamaReservedFieldMsgQual            = NULL;
mamaFieldDescriptor MamaReservedFieldConflateQuoteCount = NULL;
mamaFieldDescriptor MamaReservedFieldSymbolList         = NULL;
mamaFieldDescriptor MamaReservedFieldEntitleCode        = NULL;


void initReservedFields (void)
{
    CREATE_FIELD (MsgType,            U8);
    CREATE_FIELD (MsgStatus,          U8);
    CREATE_FIELD (FieldIndex,         U16);
    CREATE_FIELD (MsgNum,             U8);
    CREATE_FIELD (MsgTotal,           U8);
    CREATE_FIELD (SeqNum,             U32);
    CREATE_FIELD (FeedName,           STRING);
    CREATE_FIELD (FeedHost,           STRING);
    CREATE_FIELD (FeedGroup,          STRING);
    CREATE_FIELD (ItemSeqNum,         U32);
    CREATE_FIELD (SendTime,           TIME);
    CREATE_FIELD (AppDataType,        U8);
    CREATE_FIELD (AppMsgType,         U8);
    CREATE_FIELD (SenderId,           U64);
    CREATE_FIELD (MsgQual,            U8);
    CREATE_FIELD (ConflateQuoteCount, U32);
    CREATE_FIELD (SymbolList,         VECTOR_STRING);
    CREATE_FIELD (EntitleCode,        U32);
}

void cleanupReservedFields (void)
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
    DESTROY_FIELD (SymbolList);
    DESTROY_FIELD (EntitleCode);
}
