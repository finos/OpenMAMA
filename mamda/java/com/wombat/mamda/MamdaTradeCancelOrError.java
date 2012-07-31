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

/**
 * MamdaTradeCancelOrError is an interface that provides access to
 * fields related to trade cancellations and errors.
 */

public interface MamdaTradeCancelOrError extends MamdaBasicEvent
{
    /**
     * Return whether this event is a trade cancel.  If false, the
     * event is a trade error.
     */
    public boolean getIsCancel();

    /**
     * @return Returns the IsCancel Field State.
     */
    public short getIsCancelFieldState();

    /**
     * Original feed-generated sequence for a correction/cancel/error.
     */
    public long    getOrigSeqNum();

    /**
     * @return Returns the OrigSeqNum Field State.
     */
    public short getOrigSeqNumFieldState();

    /**
     * Original trade price in a correction/cancel/error.
     */
    public double  getOrigPrice();

    /**
     * @return Returns the OrigPrice Field State.
     */
    public short getOrigPriceFieldState();

    /**
     * Original trade size in a correction/cancel/error.
     */
    public double  getOrigVolume();

    /**
     * @return Returns the OrigVolume Field State.
     */
    public short getOrigVolumeFieldState();

    /**
     * Original trade participant identifier in a
     * correction/cancel/error.
     */
    public String  getOrigPartId();

    /**
     * @return Returns the OrigPartId Field State.
     */
    public short getOrigPartIdFieldState();

    /**
     * @return Returns the OrigTradeId.
     */
    public String getOrigTradeId(); 

    /**
     * @return Returns the OrigPartId Field State.
     */
    public short getOrigTradeIdFieldState();  

    /**
     * A normalized set of qualifiers for the original trade for the
     * security in a correction/cancel/error.
     */
    public String  getOrigQual();

    /**
     * @return Returns the OrigQual Field State.
     */
    public short getOrigQualFieldState();

    /**
     * Get original trade qualifier (non normalized).
     * @see MamdaTradeReport#getTradeQual()
     */
    public String getOrigQualNative();

    /**
     * @return Returns the OrigQualNative Field State.
     */
    public short getOrigQualNativeFieldState();

    /**
     * Feed-specific trade qualifier code(s) for original trade.
     * This field is provided primarily for completeness and/or
     * troubleshooting.
     */
    public String  getOrigCondition();

    /**
     * @return Returns the OrigCondition Field State.
     */
    public short getOrigConditionFieldState();

    /**
     * Seller's sale days for original trade. Used when the trade
     * qualifier is "Seller". Specifies the number of days that may
     * elapse before delivery of the security.
     */
    public long    getOrigSellersSaleDays();

    /**
     * @return Returns the OrigSellersSaleDays Field State.
     */
    public short getOrigSellersSaleDaysFieldState();

    /**
     * Stopped stock indicator for original trade.
     * Condition related to certain NYSE trading rules.
     * This is not related to a halted security
     * status.  (0 == N/A; 1 == Applicable)
     */
    public char    getOrigStopStock();

    /**
     * @return Returns the OrigStopStock Field State.
     */
    public short getOrigStopStockFieldState();

    /**
     * getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker
     * @see MamdaTradeReport#getOrigShortSaleCircuitBreaker()
     */
    public char getOrigShortSaleCircuitBreaker();  

    /**
     * @return Returns the FieldState, always MODIFIED.
     */
    public short getOrigShortSaleCircuitBreakerFieldState();  

}
