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
import com.wombat.mama.MamaPrice;
import com.wombat.mama.MamaDateTime;

/**
 * MamdaQuoteRecap is an interface that provides access to quote
 * related fields.
 */

public interface MamdaQuoteRecap extends MamdaBasicRecap
{
    /**
     * Get the bid price.
     *
     * @return Bid price.  The highest price that the representative
     * party/group is willing to pay to buy the security.
     * For most feeds, this size is represented in round lots.
     */
    MamaPrice  getBidPrice();

    /**
     * @return the bid price Field State
     */
     short  getBidPriceFieldState();

    /**
     * Get the bid size.
     *
     * @return Total share size available for the current bid price.
     */
    double  getBidSize();

    /**
     * @return the bid size Field State
     */
     short  getBidSizeFieldState();

    /**
     * Get the bid participant identifier.
     *
     * @return The identifier of the market participant (e.g.
     * exchange or market maker) contributing the bid price field.
     */
    String  getBidPartId();

    /**
     * @return the bid part ID Field State
     */
     short  getBidPartIdFieldState();

    /**
     * Date corresponding to the last quote, as reported by the feed.
     */
    MamaDateTime getEventDate();

    /**
     * @return event date Field State
     */
     short  getEventDateFieldState();

    /**
     * Time corresponding to the last quote, as reported by the feed.
     */
    MamaDateTime getEventTime();

    /**
     * @return the event time Field State
     */
     short  getEventTimeFieldState();

    /**
     * Get the ask price.
     *
     * @return Ask price.  The lowest price that the representative
     * party/group is willing to take to sell the security.  For
     * most feeds, this size is represented in round lots.
     */
    MamaPrice  getAskPrice();

    /**
     * @return the ask price Field State
     */
     short  getAskPriceFieldState();

    /**
     * Get the ask size.
     *
     * @return Total share size available for the current ask price.
     */
    double    getAskSize();

    /**
     * @return the ask size Field State
     */
     short  getAskSizeFieldState();

    /**
     * Get the ask participant identifier.
     *
     * @return The identifier of the market participant (e.g.
     * exchange or market maker) contributing the ask price field.
     */
    String  getAskPartId();

    /**
     * @return the ask part ID Field State
     */
     short  getAskPartIdFieldState();

    /**
     * Get the quote mid price.
     *
     * @return The mid price of the current quote.  Usually, this is
     * the average of the bid and ask prices, but some exchanges
     * provide this field explicitly (e.g. LSE).
     */
    MamaPrice  getQuoteMidPrice();

    /**
     * @return the quote mid price Field State
     */
     short  getQuoteMidPriceFieldState();

    /**
     * Get the quote count.
     *
     * @return The number of quotes generated for this security during
     * the current trading session.
     */
    long    getQuoteCount();

    /**
     * @return the quote count Field State
     */
     short  getQuoteCountFieldState();

    /**
     * Get the normalized quote qualifier.
     *
     * @return A normalized set of qualifiers for the last quote for
     * the security. This field may contain multiple string values,
     * separated by the colon(:) character. 
     *
     * @see MamdaQuoteUpdate#getQuoteQual
     */
    String  getQuoteQual();

    /**
     * @return the quote qual Field State
     */
     short  getQuoteQualFieldState();

    /**
     * @see MamdaQuoteUpdate#getQuoteQualNative()
     */
    String getQuoteQualNative ();

    /**
     * @return the quote qual native Field State
     */
     short  getQuoteQualNativeFieldState();

    /**
     * @see MamdaQuoteUpdate#getShortSaleBidTick()
     */
    char getShortSaleBidTick ();

    /**
     * @return the short sale bid tick Field State
     */
     short  getShortSaleBidTickFieldState();

    /**
     * getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker
     * @see MamdaQuoteUpdate#getShortSaleCircuitBreaker()
     */
    public char getShortSaleCircuitBreaker();  

    /**
     * @return Returns the FieldState, always MODIFIED.
     */
    public short getShortSaleCircuitBreakerFieldState();    

    MamaDateTime    getAskTime();
    /**
     * @return the ask time Field State
     */
    short  getAskTimeFieldState();

    MamaDateTime    getBidTime();
    /**
     * @return the bid time Field State
     */
    short  getBidTimeFieldState();

    String          getAskIndicator();
    /**
     * @return the ask indicator Field State
     */
    short  getAskIndicatorFieldState();

    String          getBidIndicator();
    /**
     * @return the bid indicator Field State
     */
    short  getBidIndicatorFieldState();

    long            getAskUpdateCount();
    /**
     * @return the ask update count Field State
     */
    short  getAskUpdateCountFieldState();
     
    long            getBidUpdateCount();
    /**
     * @return the bid update count Field State
     */
    short  getBidUpdateCountFieldState();
    
    double          getAskYield();
    /**
     * @return the ask yield Field State
     */
    short  getAskYieldFieldState();
    
    double          getBidYield();  
    /**
     * @return the bid yield Field State
     */
    short  getBidYieldFieldState();

    /**
     * @return the quote bid depth
     **/
    double getAskDepth();

    /**
     * @return the quote bid depth
     **/
    double getBidDepth();

    /**
     * @return the ask depth Field State
     */
    short getAskDepthFieldState();
    /**
     * @return the bid depth Field State
     */
    short getBidDepthFieldState();
     
    String          getBidSizesList();  
    /**
     * @return the bid sizes list Field State
     */
    short  getBidSizesListFieldState();
     
    String          getAskSizesList();  
    /**
     * @return the ask sizes list Field State
     */
    short  getAskSizesListFieldState();
}
