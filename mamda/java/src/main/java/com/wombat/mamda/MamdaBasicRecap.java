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
 * MamdaBasicRecap is an interface that provides access to common
 * recap-related fields.
 */

public interface MamdaBasicRecap
{
    /**
     * Get the string symbol for the instrument.
     *
     * @return Symbol.  This is the "well-known" symbol for the
     * security, including any symbology mapping performed by the
     * publisher.
     */
    public String getSymbol();

   /**
    * @return symbol Field State
    */
    public short  getSymbolFieldState();

    /**
     * Get the participant identifier.
     *
     * @return Participant ID.  This may be an exchange identifier, a
     * market maker ID, etc., or NULL (if this is not related to any
     * specific participant).
     */
    public String getPartId();

    /**
     * @return participant ID Field State
     */
    public short  getPartIdFieldState();

    /**
     * Source time.  Typically, the exchange generated feed
     */
    public MamaDateTime getSrcTime();

    /**
     * @return source time Field State
     */
    public short  getSrcTimeFieldState();

    /**
     * Activity time.  A feed handler generated time stamp
     * representing when the data item was last updated.
     */
    public MamaDateTime getActivityTime();

    /**
     * @return activity time Field State
     */
    public short  getActivityTimeFieldState();

    /**
     * Get the line time of the update.
     *
     * @return Line time.  A feed handler (or similar publisher) time
     * stamp representing the time that such publisher received the
     * update message pertaining to the event.  If clocks are properly
     * synchronized and the source time (see above) is accurate
     * enough, then the difference between the source time and line
     * time is the latency between the data source and the feed
     * handler.
     */
    public MamaDateTime  getLineTime();

    /**
     * @return line time Field State
     */
    public short  getLineTimeFieldState();

    /**
     * Get the send time of the update.
     *
     * @return Send time.  A feed handler (or similar publisher) time
     * stamp representing the time that such publisher sent the
     * current message.  The difference between the line time and send
     * time is the latency within the feed handler itself.  Also, if
     * clocks are properly synchronized then the difference between
     * the send time and current time is the latency within the market
     * data distribution framework (i.e. MAMA and the underlying
     * middleware).
     */
    public MamaDateTime getSendTime();

    /**
     * @return send time Field State
     */
    public short  getSendTimeFieldState();

}
