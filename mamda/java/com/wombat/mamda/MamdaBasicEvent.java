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

package com.wombat.mamda;

import com.wombat.mama.*;

/**
 * MamdaBasicEvent is a superclass interface that provides access to
 * common event related fields.  Events types include trades, quotes,
 * order book updates, closing summaries, etc.  See the individual
 * subclasses for specific information about each event type.
 * 
 * Note: Different types of time stamps are available representing the
 * time that the event (trade, quote, etc) actually occurred, the time
 * the data source/exchange reported it, and the time the feed
 * handler applied an action to the given data item (record, order
 * book, etc.).  Many feeds to not provide a distinction between the
 * event time and the source time (they may be the same) and the
 * granularity of time stamps also varies between data sources.
 */

public interface MamdaBasicEvent
{
    /**
     * @return Source time.  Typically, the exchange generated feed
     * time stamp.  This is often the same as the "event time",
     * because many feeds do not distinguish between the actual event
     * time and when the exchange sent the message.
     */
    public MamaDateTime getSrcTime();

    /**
     * return Source time Field State
     * MODIFIED (2) value indicates the fied was updated in last tick
     * NOT MODIFIED (1) value indicate that there was no change in the last tick
     * NOT_INITIALISED (0) value indicates that the field has never been updated
     */
    public short getSrcTimeFieldState();

    /**
     * @return Activity time.  A feed handler generated time stamp
     * representing when the data item was last updated.
     */
    public MamaDateTime getActivityTime();

    /**
     * return Activity time Field State
     * MODIFIED (2) value indicates the fied was updated in last tick
     * NOT MODIFIED (1) value indicate that there was no change in the last tick
     * NOT_INITIALISED (0) value indicates that the field has never been updated
     */
    public short getActivityTimeFieldState();

    /**
     * @return Source sequence number.  The exchange generated
     * sequence number.
     */
    public long   getEventSeqNum();

    /**
     * return source sequence number Field State
     * MODIFIED (2) value indicates the fied was updated in last tick
     * NOT MODIFIED (1) value indicate that there was no change in the last tick
     * NOT_INITIALISED (0) value indicates that the field has never been updated
     */
    public short getEventSeqNumFieldState();

    /**
     * @return Event time.  Typically, when the event actually
     * occurred.  This is often the same as the "source time", because
     * many feeds do not distinguish between the actual event time and
     * when the exchange sent the message.
     */
    public MamaDateTime getEventTime();

    /**
     * return event time Field State
     * MODIFIED (2) value indicates the fied was updated in last tick
     * NOT MODIFIED (1) value indicate that there was no change in the last tick
     * NOT_INITIALISED (0) value indicates that the field has never been updated
     */
    public short getEventTimeFieldState();
}
