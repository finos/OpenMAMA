/* $Id: reservedfields.c,v 1.23.12.2.4.5 2011/08/29 11:52:44 ianbell Exp $
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
    mamaFieldDescriptor_create (&MamaReservedFieldMsgType,
                                1,
                                MAMA_FIELD_TYPE_U8,
                                "MdMsgType");
    mamaFieldDescriptor_create (&MamaReservedFieldMsgStatus,
                                2,
                                MAMA_FIELD_TYPE_U8,
                                "MdMsgStatus");
    mamaFieldDescriptor_create (&MamaReservedFieldFieldIndex,
                                3,
                                MAMA_FIELD_TYPE_VECTOR_U16,
                                "MdFieldIndex");
    mamaFieldDescriptor_create (&MamaReservedFieldMsgNum,
                                7,
                                MAMA_FIELD_TYPE_U8,
                                "MdMsgNum");
    mamaFieldDescriptor_create (&MamaReservedFieldMsgTotal,
                                8,
                                MAMA_FIELD_TYPE_U8,
                                "MdMsgTotal");
    mamaFieldDescriptor_create (&MamaReservedFieldSeqNum,
                                10,
                                MAMA_FIELD_TYPE_U32,
                                "MdSeqNum");
    mamaFieldDescriptor_create (&MamaReservedFieldFeedName,
                                11,
                                MAMA_FIELD_TYPE_STRING,
                                "MdFeedName");
    mamaFieldDescriptor_create (&MamaReservedFieldFeedHost,
                                12,
                                MAMA_FIELD_TYPE_STRING,
                                "MdFeedHost");
    mamaFieldDescriptor_create (&MamaReservedFieldFeedGroup,
                                13,
                                MAMA_FIELD_TYPE_STRING,
                                "MdFeedGroup");
    mamaFieldDescriptor_create (&MamaReservedFieldItemSeqNum,
                                15,
                                MAMA_FIELD_TYPE_U32,
                                "MdItemSeq");
    mamaFieldDescriptor_create (&MamaReservedFieldSendTime,
                                16,
                                MAMA_FIELD_TYPE_TIME,
                                "MamaSendTime");
    mamaFieldDescriptor_create (&MamaReservedFieldAppDataType,
                                17,
                                MAMA_FIELD_TYPE_U8,
                                "MamaAppDataType");
    mamaFieldDescriptor_create (&MamaReservedFieldAppMsgType,
                                18,
                                MAMA_FIELD_TYPE_U8,
                                "MamaAppMsgType");
    mamaFieldDescriptor_create (&MamaReservedFieldSenderId,
                                20,
                                MAMA_FIELD_TYPE_U64,
                                "MamaSenderId");
    mamaFieldDescriptor_create (&MamaReservedFieldMsgQual,
                                21,
                                MAMA_FIELD_TYPE_U8,
                                "wMsgQual");
    mamaFieldDescriptor_create (&MamaReservedFieldConflateQuoteCount,
                                23,
                                MAMA_FIELD_TYPE_U32,
                                "wConflateQuoteCount");
    mamaFieldDescriptor_create (&MamaReservedFieldSymbolList,
                                81,
                                MAMA_FIELD_TYPE_VECTOR_STRING,
                                "MamaSymbolList");    
    mamaFieldDescriptor_create (&MamaReservedFieldEntitleCode,
                                496,
                                MAMA_FIELD_TYPE_U32,
                                "wEntitleCode");
}

void cleanupReservedFields (void)
{
    if (MamaReservedFieldMsgType)
        mamaFieldDescriptor_destroy (MamaReservedFieldMsgType);
    if (MamaReservedFieldMsgStatus)
        mamaFieldDescriptor_destroy (MamaReservedFieldMsgStatus);
    if (MamaReservedFieldFieldIndex)
        mamaFieldDescriptor_destroy (MamaReservedFieldFieldIndex);
    if (MamaReservedFieldMsgNum)
        mamaFieldDescriptor_destroy (MamaReservedFieldMsgNum);
    if (MamaReservedFieldMsgTotal)
        mamaFieldDescriptor_destroy (MamaReservedFieldMsgTotal);
    if (MamaReservedFieldSeqNum)
        mamaFieldDescriptor_destroy (MamaReservedFieldSeqNum);
    if (MamaReservedFieldFeedName)
        mamaFieldDescriptor_destroy (MamaReservedFieldFeedName);
    if (MamaReservedFieldFeedHost)
        mamaFieldDescriptor_destroy (MamaReservedFieldFeedHost);
    if (MamaReservedFieldFeedGroup)
        mamaFieldDescriptor_destroy (MamaReservedFieldFeedGroup);
    if (MamaReservedFieldItemSeqNum)
        mamaFieldDescriptor_destroy (MamaReservedFieldItemSeqNum);
    if (MamaReservedFieldSendTime)
        mamaFieldDescriptor_destroy (MamaReservedFieldSendTime);
    if (MamaReservedFieldAppDataType)
        mamaFieldDescriptor_destroy (MamaReservedFieldAppDataType);
    if (MamaReservedFieldAppMsgType)
        mamaFieldDescriptor_destroy (MamaReservedFieldAppMsgType);
    if (MamaReservedFieldSenderId)
        mamaFieldDescriptor_destroy (MamaReservedFieldSenderId);
    if (MamaReservedFieldMsgQual)
        mamaFieldDescriptor_destroy (MamaReservedFieldMsgQual);
    if (MamaReservedFieldSymbolList)
        mamaFieldDescriptor_destroy (MamaReservedFieldSymbolList);    
    if (MamaReservedFieldEntitleCode)
        mamaFieldDescriptor_destroy (MamaReservedFieldEntitleCode);
    if (MamaReservedFieldConflateQuoteCount)
        mamaFieldDescriptor_destroy (MamaReservedFieldConflateQuoteCount);
}
