/* $Id: types.h,v 1.46.22.7 2011/08/22 16:28:27 emmapollock Exp $
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

#ifndef MamaTypesH__
#define MamaTypesH__


#include "wombat/port.h"
#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int8_t          mama_i8_t;
typedef uint8_t         mama_u8_t;
typedef int16_t         mama_i16_t;
typedef uint16_t        mama_u16_t;
typedef int32_t         mama_i32_t;
typedef uint32_t        mama_u32_t;
typedef int64_t         mama_i64_t;
typedef uint64_t        mama_u64_t;
typedef int8_t          mama_bool_t;
typedef float           mama_f32_t;
typedef double          mama_f64_t;
typedef double          mama_quantity_t;
typedef uint16_t        mama_fid_t;
typedef size_t          mama_size_t;
typedef uint32_t        mama_seqnum_t;

#define MAMA_QUANTITY_EPSILON   ((mama_f64_t)0.00000000001)

/**
 * Macro to determine if a quantity is zero
 */
#define mama_isQuantityNone(q)                      \
        ((q) < MAMA_QUANTITY_EPSILON)

/**
 * Macro to determine if a quantity equality
 */
#define mama_isQuantityEqual(lhs, rhs)              \
        ((lhs) < (rhs) ? (rhs) - (lhs) < MAMA_QUANTITY_EPSILON : \
                         (lhs) - (rhs) < MAMA_QUANTITY_EPSILON)

/*
 * The bridge implementation required when creating many of the core
 * MAMA objects.
 */
typedef struct mamaBridgeImpl_* mamaBridge;

/*
 * The payload bridge implementation required for accessing messages
 */
typedef struct mamaPayloadBridgeImpl_* mamaPayloadBridge;

/**
 * Flexible date/time format
 */
typedef   mama_u64_t*   mamaDateTime;

/**
 * Time zone utility type
 */
typedef   void*         mamaTimeZone;

/**
 * Flexible price format (contains display hints as well as value)
 */
typedef   void*         mamaPrice;

/**
 * Opaque definitions for all major MAMA data types. Must be allocated
 * through corresponding functions from the API.
 */
typedef struct mamaMsgImpl_*                    mamaMsg;
typedef struct mamaMsgFieldImpl_*               mamaMsgField;
typedef struct mamaMsgIteratorImpl_*            mamaMsgIterator;
typedef struct mamaTransportImpl_*              mamaTransport;
typedef struct mamaSubscriptionImpl_*           mamaSubscription;
typedef struct mamaManagedSubscriptionImpl_*    mamaManagedSubscription;
typedef struct mamaSubscriptionManagerImpl_*    mamaSubscriptionManager;
typedef struct mamaDictionaryImpl_*             mamaDictionary;
typedef struct mamaPublisherImpl_*              mamaPublisher;
typedef struct mamaDQPublisherImpl_*            mamaDQPublisher;
typedef struct mamaDQPublisherManagerImpl_*     mamaDQPublisherManager;
typedef struct mamaInboxImpl_*                  mamaInbox;
typedef struct mamaQueueImpl_*                  mamaQueue;
typedef struct mamaDispatcherImpl_*             mamaDispatcher;
typedef struct mamaTimerImpl*                   mamaTimer;
typedef struct mamaFieldDescriptorImpl_*        mamaFieldDescriptor;
typedef struct mamaIoImpl*                      mamaIo;
typedef struct mamaSymbolSourceFileImpl_*       mamaSymbolSourceFile;
typedef struct mamaSymbolStoreImpl_*            mamaSymbolStore;
typedef struct mamaMsgQualImpl_*                mamaMsgQual;
typedef struct mamaSourceManagerImpl_*          mamaSourceManager;
typedef struct mamaSourceImpl_*                 mamaSource;
typedef struct mamaSymbologyImpl_*              mamaSymbology;
typedef struct mamaSymbologyManagerImpl_*       mamaSymbologyManager;
typedef struct mamaSourceGroupCbHandleImpl_*    mamaSourceGroupCbHandle;
typedef struct mamaConnectionImpl_*             mamaConnection;
typedef struct mamaServerConnectionImpl_*       mamaServerConnection;
typedef struct mamaConflationManager_*          mamaConflationManager;
typedef struct mamaStatsLoggerImpl_*            mamaStatsLogger;
typedef struct mamaStatImpl_*                   mamaStat;
typedef struct mamaStatsCollectorImpl_*         mamaStatsCollector;
typedef struct mamaStatsGeneratorImpl_*         mamaStatsGenerator;
typedef struct mamaMsgReplyImpl_*               mamaMsgReply;

#if defined(__cplusplus)
}
#endif

#endif /* MamaTypesH__ */
