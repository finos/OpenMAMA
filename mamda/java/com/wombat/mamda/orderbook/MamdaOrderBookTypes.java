/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;


public class MamdaOrderBookTypes
{
    /**
     * An enumeration for actions on an order book.  Note: price level
     * actions differ from entry actions because, for example, a price
     * level message with ACTION_UPDATE may consist of entries with
     * ACTION_ADD, ACTION_UPDATE or ACTION_DELETE.
     */
    public static final char MAMDA_BOOK_ACTION_ADD     = 'A';    /** A new price level. */
    public static final char MAMDA_BOOK_ACTION_UPDATE  = 'U';    /** An updated price level. */
    public static final char MAMDA_BOOK_ACTION_DELETE  = 'D';    /** A deleted price level. */
    public static final char MAMDA_BOOK_ACTION_UNKNOWN = 'Z';    /** Erroneous action (should not occur). */

    /**
     * An enumeration for the side order book side.  "Bid" (or "buy")
     * orders occur on one side and "ask" (or "sell") orders occur on
     * the other.
     */

    public static final char MAMDA_BOOK_SIDE_BID      = 'B';    /** A bid (buy) order. */
    public static final char MAMDA_BOOK_SIDE_ASK      = 'A';    /** An ask (sell) order. */
    public static final char MAMDA_BOOK_SIDE_UNKNOWN  = 'Z';    /** Erroneous side (should not occur). */

    /**
     * An enumeration for a reason for a change.  Some of the values
     * of Reason can mean the same thing, as far as their affect on
     * the order book. If possible, a feed will send MODIFY, CANCEL or
     * TRADE actions so that downstream applications that are
     * interested in such data can handle it; other applications can
     * treat such actions in the same way as an UPDATE action (or as a
     * DELETE action if the size is zero).
     */
    public static final char MAMDA_BOOK_REASON_MODIFY  = 'M';
    public static final char MAMDA_BOOK_REASON_CANCEL  = 'C';
    public static final char MAMDA_BOOK_REASON_TRADE   = 'T';
    public static final char MAMDA_BOOK_REASON_CLOSE   = 'c';
    public static final char MAMDA_BOOK_REASON_DROP    = 'N';   // dropped out of top-N
    public static final char MAMDA_BOOK_REASON_MISC    = 'm';
    public static final char MAMDA_BOOK_REASON_UNKNOWN = 'Z';

    /**
     * When a message contains properties (ie. for order books). The properties
     * may either be an update or a recap. The default when the type field is
     * not present is UPDATE
     */
    public static final char MAMDA_PROP_MSG_TYPE_UPDATE = 'U';
    public static final char MAMDA_PROP_MSG_TYPE_RECAP  = 'R';
    
    /**
     * An enumeration for the type of level. "Limit" level orders are set at a
     * specific price. "Market" level orders are set at the current market price.
     */
    public static final char MAMDA_BOOK_LEVEL_LIMIT    = 'L';
    public static final char MAMDA_BOOK_LEVEL_MARKET   = 'M';
    public static final char MAMDA_BOOK_LEVEL_UNKNOWN  = 'U';

}
