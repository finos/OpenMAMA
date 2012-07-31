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
 * MamdaTradeCorrection is an interface that provides access to
 * fields related to trade corrections.
 */

public interface MamdaTradeCorrection extends MamdaBasicEvent
{
    /**
     * Get the original trade sequence number.
     * @see MamdaBasicEvent#getEventSeqNum()
     */
    public long    getOrigSeqNum();

    /**
     * @return Returns the OrigSeqNum Field State.
     */
    public short getOrigSeqNumFieldState();

    /**
     * Get the original trade price.
     * @see MamdaTradeReport#getTradePrice()
     */
    public double  getOrigPrice();

    /**
     * @return Returns the OrigPrice Field State.
     */
    public short getOrigPriceFieldState();

    /**
     * Get the original trade volume.
     * @see MamdaTradeReport#getTradeVolume()
     */
    public double  getOrigVolume();

    /**
     * @return Returns the OrigVolume Field State.
     */
    public short getOrigVolumeFieldState();

    /**
     * Get the original trade participant identifier.
     * @see MamdaTradeReport#getTradePartId()
     */
    public String  getOrigPartId();

    /**
     * @return Returns the OrigPartId Field State.
     */
    public short getOrigPartIdFieldState();

    /**
     * Get original trade qualifier.
     * @see MamdaTradeReport#getTradeQual()
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
     * Get original trade condition.
     * @see MamdaTradeReport#getTradeCondition()
     */
    public String  getOrigCondition();

    /**
     * @return Returns the OrigCondition Field State.
     */
    public short getOrigConditionFieldState();

    /**
     * Get the original trade sellers days.
     * @see MamdaTradeReport#getTradeSellersSaleDays()
     */
    public long    getOrigSellersSaleDays();

    /**
     * @return Returns the OrigSellersSaleDays Field State.
     */
    public short getOrigSellersSaleDaysFieldState();

    /**
     * Get the original stock stop indicator.
     * @see MamdaTradeReport#getTradeStopStock()
     */
    public char    getOrigStopStock();

    /**
     * @return Returns the OrigStopStock FieldState.
     */
    public short getOrigStopStockFieldState();

    /**
     * @return Returns the OrigTradeId.
     */
    public String getOrigTradeId(); 

    /**
     * @return Returns the OrigPartId Field State.
     */
    public short getOrigTradeIdFieldState();  

    /**
     * Get the corrected trade id.
     *
     */
    public String  getCorrTradeId();

    /**
     * @return Returns the CorrTradeId Field State.
     */
    public short getCorrTradeIdFieldState();

    /**
     * Get the corrected trade price.
     * @see MamdaTradeReport#getTradePrice()
     */
    public double  getCorrPrice();

    /**
     * @return Returns the CorrPrice Field State.
     */
    public short getCorrPriceFieldState();

    /**
     * Get the corrected trade volume.
     * @see MamdaTradeReport#getTradeVolume()
     */
    public double  getCorrVolume();

    /**
     * @return Returns the CorrVolume Field State.
     */
    public short getCorrVolumeFieldState();

    /**
     * Get the corrected trade participant identifier.
     * @see MamdaTradeReport#getTradePartId()
     */
    public String  getCorrPartId();

    /**
     * @return Returns the CorrPartId Field State.
     */
    public short getCorrPartIdFieldState();

    /**
     * Get corrected trade qualifier.
     * @see MamdaTradeReport#getTradeQual()
     */
    public String  getCorrQual();

    /**
     * @return Returns the CorrQual Field State.
     */
    public short getCorrQualFieldState();

    /**
     * Get corrected trade condition.
     * @see MamdaTradeReport#getTradeCondition()
     */
    public String getCorrQualNative();

    /**
     * @return Returns the CorrQualNative Field State.
     */
    public short getCorrQualNativeFieldState();

    /**
     * Get corrected trade condition.
     * @see MamdaTradeReport#getTradeCondition()
     */
    public String  getCorrCondition();

    /**
     * @return Returns the CorrCondition Field State.
     */
    public short getCorrConditionFieldState();

    /**
     * Get the corrected trade sellers days.
     * @see MamdaTradeReport#getTradeSellersSaleDays()
     */
    public long    getCorrSellersSaleDays();

    /**
     * @return Returns the CorrSellersSaleDays Field State.
     */
    public short getCorrSellersSaleDaysFieldState();

    /**
     * Get the original stock stop indicator.
     * @see MamdaTradeReport#getTradeStopStock()
     */
    public char    getCorrStopStock();

    /**
     * @return Returns the CorrStopStock Field State.
     */
    public short getCorrStopStockFieldState();

    /**
     * getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker
     * @see MamdaTradeReport#getCorrShortSaleCircuitBreaker()
     */
    public char getCorrShortSaleCircuitBreaker();  

    /**
     * @return Returns the FieldState, always MODIFIED.
     */
    public short getCorrShortSaleCircuitBreakerFieldState();    

}
