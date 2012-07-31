/* $Id: reservedfields.h,v 1.25.12.1.4.5 2011/08/29 11:52:44 ianbell Exp $
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

#ifndef MamaReservedFieldsH__
#define MamaReservedFieldsH__

#include "mama/mama.h"
#include "mama/config.h"
 
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct MamaReservedField_
{
    char*           mName;
    unsigned short  mFid;
} MamaReservedField;

MAMAExpDLL
extern const long WOMBAT_MAX_RESERVED_FID;                         /*   = 100 */

MAMAExpDLL
extern const MamaReservedField MamaFieldMsgType;                   /* FID   1 */
MAMAExpDLL
extern const MamaReservedField MamaFieldMsgStatus;                 /* FID   2 */
MAMAExpDLL
extern const MamaReservedField MamaFieldFieldIndex;                /* FID   3 */
#ifndef IGNORE_DEPRECATED_FIELDS
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscMsgTypeOld;           /* FID   5 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSubjectOld;           /* FID   6 */
#endif
MAMAExpDLL
extern const MamaReservedField MamaFieldMsgNum;                    /* FID   7 */
MAMAExpDLL
extern const MamaReservedField MamaFieldMsgTotal;                  /* FID   8 */
MAMAExpDLL
extern const MamaReservedField MamaFieldTibrvResult;               /* FID   9 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSeqNum;                    /* FID  10 */
MAMAExpDLL
extern const MamaReservedField MamaFieldFeedName;                  /* FID  11 */
MAMAExpDLL
extern const MamaReservedField MamaFieldFeedHost;                  /* FID  12 */
MAMAExpDLL
extern const MamaReservedField MamaFieldFeedGroup;                 /* FID  13 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSyncPattern;               /* FID  14 */
MAMAExpDLL
extern const MamaReservedField MamaFieldItemSeqNum;                /* FID  15 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSendTime;                  /* FID  16 */
MAMAExpDLL
extern const MamaReservedField MamaFieldAppDataType;               /* FID  17 */
MAMAExpDLL
extern const MamaReservedField MamaFieldAppMsgType;                /* FID  18 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSenderId;                  /* FID  20 */
MAMAExpDLL
extern const MamaReservedField MamaFieldMsgQual;                   /* FID  21 */
MAMAExpDLL
extern const MamaReservedField MamaFieldConflateCount;             /* FID  22 */
MAMAExpDLL
extern const MamaReservedField MamaFieldConflateQuoteCount;        /* FID  23 */
MAMAExpDLL
extern const MamaReservedField MamaFieldConflateTradeCount;        /* FID  24 */

/************************************************************************
 * Internal MAMA API fields (not passed to applications).
 ************************************************************************/

/* TIB/RV fields */
MAMAExpDLL
extern const MamaReservedField MamaFieldRvDaemon;                  /* FID  51 */
MAMAExpDLL
extern const MamaReservedField MamaFieldRvNetwork;                 /* FID  52 */
MAMAExpDLL
extern const MamaReservedField MamaFieldRvService;                 /* FID  53 */
/* For Elvin, we put the "source" as a separate field (it's a
 * doublecheck for the other field whose *name* acts as efficient
 * content-based filter). */
MAMAExpDLL
extern const MamaReservedField MamaFieldElvinSource;               /* FID  55 */
#ifndef IGNORE_DEPRECATED_FIELDS
MAMAExpDLL
extern const MamaReservedField MamaFieldElvinSourceOld;            /* FID  64 */
#endif
/* General subscription-related fields. */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscriptionType;          /* FID  60 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscMsgType;              /* FID  61 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSourceHost;           /* FID  63 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSourceApp;            /* FID  64 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSourceUser;           /* FID  65 */
MAMAExpDLL
extern const MamaReservedField MamaFieldServiceLevel;              /* FID  66 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSourceIp;             /* FID  67 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSourceAppClass;       /* FID  68 */
MAMAExpDLL
extern const MamaReservedField MamaFieldInterfaceVersion;          /* FID  69 */
MAMAExpDLL
extern const MamaReservedField MamaFieldUpdateTopic;               /* FID  70 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscSymbol;               /* FID  71 */
MAMAExpDLL
extern const MamaReservedField MamaFieldTportName;                 /* FID  72 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSubscNamespace;            /* FID  73 */
/* This field is used to pass back a list of symbol from the FH
 * cache and newly announced symbols as a series of recap and
 * update messages on a subscription of type MAMA_SUBSC_TYPE_SYMBOL_LIST
 * on the well-known topic SYMBOL_LIST
 */
MAMAExpDLL
extern const MamaReservedField MamaFieldSymbolList;                /* FID  81 */
MAMAExpDLL
extern const MamaReservedField MamaFieldTemplate;                  /* FID  90 */
MAMAExpDLL
extern const MamaReservedField MamaFieldTemplateLen;               /* FID  91 */

MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldMsgType;               /* FID   1 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldMsgStatus;             /* FID   2 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldFieldIndex;            /* FID   3 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldMsgNum;                /* FID   7 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldMsgTotal;              /* FID   8 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldSeqNum;                /* FID  10 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldFeedName;              /* FID  11 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldFeedHost;              /* FID  12 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldFeedGroup;             /* FID  13 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldItemSeqNum;            /* FID  15 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldSendTime;              /* FID  16 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldAppDataType;           /* FID  17 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldAppMsgType;            /* FID  18 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldSenderId;              /* FID  20 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldMsgQual;               /* FID  21 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldConflateQuoteCount;    /* FID  23 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldEntitleCode;           /* FID 496 */
MAMAExpDLL
extern mamaFieldDescriptor MamaReservedFieldSymbolList;            /* FID  81 */


#if defined(__cplusplus)
}
#endif

#endif /* MamaReservedFieldsH__*/
