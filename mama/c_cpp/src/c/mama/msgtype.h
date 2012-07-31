/* $Id: msgtype.h,v 1.21.22.5 2011/08/29 11:52:44 ianbell Exp $
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

#ifndef MamaMsgTypeH__
#define MamaMsgTypeH__

/* 
 * This file provides MAMA message type information.
 */

#include "mama/types.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaMsgType
{
    /**
     * General update (includes, funds). For direct feeds, the handler sends
     * more specific message types for stocks and order books; however,
     * aggregated feeds will send MAMA_MSG_TYPE_UPDATE for stocks and order
     * books as well.
     */
    MAMA_MSG_TYPE_UPDATE            =   0,  

    /**
     * Initial value. The initial image (full record) for normal subscriptions. 
     * The initial value is the first message (several messages for multi-part 
     * initialvalues) to arrive with all of the information in the cache. 
     * Multi-part initial values only occur on Mama for TIBRV. If the 
     * requiresInitial parameter to createSubscription is 0, no initial value is 
     * sent.
     */
    MAMA_MSG_TYPE_INITIAL           =   1,  

    /** Trade cancel. */
    MAMA_MSG_TYPE_CANCEL            =   2,  

    /** An error occurred .*/
    MAMA_MSG_TYPE_ERROR             =   3,  

    /** Trade correction. */
    MAMA_MSG_TYPE_CORRECTION        =   4,  

    /** Closing summary. */
    MAMA_MSG_TYPE_CLOSING           =   5,  

    /**
     * Refresh/recap of some/all fields. When the client detects a sequence
     * number gap, it requests a recap from the feed handler. The feed handler
     * may also send recaps in the event of a correction or other event that
     * requires publishing the full record.
     */
    MAMA_MSG_TYPE_RECAP             =   6,  

    /**
     * The feed handler will not send any more updates for the symbol,
     * to the client in question.
     */
    MAMA_MSG_TYPE_DELETE            =   7,  

    /** Expired option or future. */
    MAMA_MSG_TYPE_EXPIRE            =   8,  

    /**
     * A snapshot is the same as an initial value; however, the client will not
     * receive any subsequent updates. Clients request snapshots by creating
     * snapshot subscriptions.
     */
    MAMA_MSG_TYPE_SNAPSHOT          =   9,  

    /** Pre-opening summary (e.g. morning "roll"). */
    MAMA_MSG_TYPE_PREOPENING        =   12, 

    /** Quote updates. */
    MAMA_MSG_TYPE_QUOTE             =   13, 

    /** Trade updates. */
    MAMA_MSG_TYPE_TRADE             =   14, 

    /** Order updates.     */
    MAMA_MSG_TYPE_ORDER             =   15, 
    
    /** 
    * Order book initial value. This message is sent rather than
     * MAMA_MSG_TYPE_INITIAL for order books.
     */
    MAMA_MSG_TYPE_BOOK_INITIAL      =   16, 
    
    /**
     * Order book update. Sent for order books rather than MAMA_MSG_TYPE_UPDATE. 
     */
    MAMA_MSG_TYPE_BOOK_UPDATE       =   17, 
    
    /** Order book clear. All the entries should be removed from the book. */
    MAMA_MSG_TYPE_BOOK_CLEAR        =   18, 
    
    /** Order book recap. Sent rather than MAMA_MSG_TYPE_RECAP for order 
        books */
    MAMA_MSG_TYPE_BOOK_RECAP        =   19, 

    /** Order book recap. Sent rather than MAMA_MSG_TYPE_SNAPSHOT for order 
        books */
    MAMA_MSG_TYPE_BOOK_SNAPSHOT     =   20, 

    /** Not permissioned on the feed */
    MAMA_MSG_TYPE_NOT_PERMISSIONED  =   21, 

    /**
     * The symbols was not found but may show up later. This indicates
     * that the symbol is not currently in the feed handler's cache,
     * but may get added later. The feed handler must be configured
     * with OrderBookNotFoundAction and RecordNotFoundAction set to
     * not_found to enable this behavior.
     */
    MAMA_MSG_TYPE_NOT_FOUND         =   22,
    
    /**
     * End of group of initial values.  Marks the last initial value for 
     * group subscriptions. 
     */
    MAMA_MSG_TYPE_END_OF_INITIALS   =   23, 
    
    /** A service request */
    MAMA_MSG_TYPE_WOMBAT_REQUEST    =   24,  

    /** A calculated result. */
    MAMA_MSG_TYPE_WOMBAT_CALC       =   25,  

    /** Security status update **/
    MAMA_MSG_TYPE_SEC_STATUS        =   26,  

    /** Data dictionary. This message contains the data dictionary. */
    MAMA_MSG_TYPE_DDICT_SNAPSHOT    =   50, 
    
    /** Miscellaneous                    */
    MAMA_MSG_TYPE_MISC              =   100, 

    /** 
     * Returned if an RV error is encountered the MsgStatus will be 
     * the tibrv_status
     */
    MAMA_MSG_TYPE_TIBRV             =   101, 

    /**
     * MAMA applications do NOT need to process the following
     * message types. These are all internal messages.
     */
    
    /** The set of features and related params for a particular publisher. */
    MAMA_MSG_TYPE_FEATURE_SET           =   150,
    
    /** Subscription synchronization request. */
    MAMA_MSG_TYPE_SYNC_REQUEST          =   170, 
    
    /** Subscription refresh. */
    MAMA_MSG_TYPE_REFRESH               =   171, 
    
    /** World View request. */
    MAMA_MSG_TYPE_WORLD_VIEW            =   172,
        
    /** News query. */
    MAMA_MSG_TYPE_NEWS_QUERY            =   173,
        
    /** Keep alive message. */
    MAMA_MSG_TYPE_NULL                  =   175, 

    MAMA_MSG_TYPE_ENTITLEMENTS_REFRESH  =   176,
        
    /** Unknown, not covered by any of the above. */ 
    MAMA_MSG_TYPE_UNKNOWN               =   199
} mamaMsgType;

/**
 * Extract the type from the message 
 *
 * @param msg The message.
 */
MAMAExpDLL
extern mamaMsgType 
mamaMsgType_typeForMsg (const mamaMsg msg);

/**
 * Extract the type as a string from a message.
 *
 * @param msg The message.
 */
MAMAExpDLL
extern const char* 
mamaMsgType_stringForMsg (const mamaMsg msg);


/**
 * Convert a mamaMsgType to a string.
 *
 * @param type The mamaMsgType.
 */
MAMAExpDLL
extern const char* 
mamaMsgType_stringForType (mamaMsgType type);

#if defined(__cplusplus)
}
#endif

#endif  /* MamaMsgTypeH__ */
