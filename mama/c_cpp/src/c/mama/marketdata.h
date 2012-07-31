/* $Id: marketdata.h,v 1.11.14.1.4.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaMartketDataH__
#define MamaMartketDataH__

/* 
 * This file provides type definitions and function declarations for
 * MAMA types related to market data applications.
 */

#include "mama/config.h"
#include "mama/types.h"
#include "mama/msgtype.h"

#if defined(__cplusplus)
extern "C" {
#endif


typedef enum mamaMdDataType
{
    MAMA_MD_DATA_TYPE_STANDARD   = 0,
    MAMA_MD_DATA_TYPE_ORDER_BOOK = 1,
    MAMA_MD_DATA_TYPE_NEWS_STORY = 2,
    MAMA_MD_DATA_TYPE_WORLDVIEW  = 3,
    MAMA_MD_DATA_TYPE_PROPERTY   = 4,
    MAMA_MD_DATA_TYPE_USAGE_LOG  = 5,
    MAMA_MD_DATA_TYPE_NEWS_QUERY = 6,
    MAMA_MD_DATA_TYPE_TEMPLATE   = 7
} mamaDataType;


typedef enum mamaMdMsgType
{
    /** General update (indices, funds)*/
    MAMA_MD_MSG_TYPE_GENERAL            = 0,

    /** Trade cancellation. */
    MAMA_MD_MSG_TYPE_CANCEL             = 2,  

    /** Trade error */
    MAMA_MD_MSG_TYPE_ERROR              = 3,  

    /** Trade correction. */
    MAMA_MD_MSG_TYPE_CORRECTION         = 4,  

    /** Closing summary */
    MAMA_MD_MSG_TYPE_CLOSING            = 5,  

    /** Symbol deleted*/
    MAMA_MD_MSG_TYPE_SYMBOL_DELETE      = 7,

    /** Action related to this symbol, such as a name change or symbol
     * deletion due to option/future expiration, etc. */
    MAMA_MD_MSG_TYPE_SYMBOL_ACTION      = 8,  

    /** Pre-opening summary (e.g. morning "roll") */
    MAMA_MD_MSG_TYPE_PREOPENING         = 12, 

    /** Quote update. */
    MAMA_MD_MSG_TYPE_QUOTE              = 13, 

    /** Trade update. */
    MAMA_MD_MSG_TYPE_TRADE              = 14, 

    /** Orderbook update*/
    MAMA_MD_MSG_TYPE_BOOK_UPDATE        = 15,

    /** Order book initial value. This message is sent rather than
     * MAMA_MD_MSG_TYPE_INITIAL for order books.
     */
    MAMA_MD_MSG_TYPE_BOOK_INITIAL       = 16,

    /** Orderbook clear*/
    MAMA_MD_MSG_TYPE_BOOK_CLEAR         = 18,

    /** Order imbalance or noimbalance update */
    MAMA_MD_MSG_TYPE_IMBALANCE          = 22,
    
    /** Security status update*/
    MAMA_MD_MSG_TYPE_SECURITY_STATUS    = 23,

    /** News headline */
    MAMA_MD_MSG_TYPE_NEWS_HEADLINE      = 24,

    /** News Story*/
    MAMA_MD_MSG_TYPE_NEWS_STORY         = 25,

    /** News query*/
    MAMA_MD_MSG_TYPE_NEWS_QUERY         = 26,
    
    /** Miscellaneous. */
    MAMA_MD_MSG_TYPE_MISC               = 100

} mamaMdMsgType;


/**
 * Extract the market data message type from the message.
 *
 * @param msg The message.
 */
MAMAExpDLL
extern mamaMdMsgType 
mamaMdMsgType_typeForMsg (const mamaMsg msg);

/**
 * Extract the market data message type as a string from a message.
 *
 * @param msg The message.
 */
MAMAExpDLL
extern const char* 
mamaMdMsgType_stringForMsg (const mamaMsg msg);

/**
 * Convert a mamaMdMsgType to a string.
 *
 * @param type The mamaMdMsgType.
 */
MAMAExpDLL
extern const char* 
mamaMdMsgType_stringForType (mamaMdMsgType type);

/**
 * Convert a mamaMdMsgType to a reasonable backward-compatible mamaMsgType.
 *
 * @param type The mamaMdMsgType.
 */
MAMAExpDLL
extern mamaMsgType
mamaMdMsgType_compatMsgType (mamaMdMsgType type);

#if defined(__cplusplus)
}
#endif

#endif  /* MamaMartketDataH__ */
