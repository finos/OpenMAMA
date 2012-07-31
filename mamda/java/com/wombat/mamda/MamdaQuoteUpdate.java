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
 * MamdaQuoteUpdate is an interface that provides access to fields
 * related to quote updates.
 */

public interface MamdaQuoteUpdate extends MamdaBasicEvent
{
    /**
     * @return Bid price.  The highest price that the representative
     * party/group is willing to pay to buy the security.  For most
     * feeds, this size is represented in round lots.
     */
    MamaPrice  getBidPrice();

    /**
     * @return the bid price Field State
     */
    short  getBidPriceFieldState();

    /**
     * @return Total share size available for the current
     * bid price. For most feeds, this size is represented in round
     * lots.
     */
    double    getBidSize();

    /**
     * @return the bid size Field State
     */
    short  getBidSizeFieldState();

    /**
     * @return The identifier of the market participant (e.g. exchange
     * or market maker) contributing the bid price field.
     */
    String  getBidPartId();

    /**
     * @return the bid part ID Field State
     */
    short  getBidPartIdFieldState();

    /**
     * @return Ask price.  The lowest price that the representative
     * party/group is willing to take to sell the security.  For most
     * feeds, this size is represented in round lots.
     */
    MamaPrice  getAskPrice();

    /**
     * @return the ask price Field State
     */
    short  getAskPriceFieldState();

    /**
     * @return Total share size available for the current
     * ask price. For most feeds, this size is represented in round
     * lots.
     */
    double    getAskSize();

    /**
     * @return the ask size Field State
     */
    short  getAskSizeFieldState();

    /**
     * @return The identifier of the market participant (e.g. exchange
     * or market maker) contributing the ask price field.
     */
    String  getAskPartId();

    /**
     * @return the ask part ID Field State
     */
    short  getAskPartIdFieldState();

    /**
     * @return A normalized set of qualifiers for the last quote for
     * the security. This field may contain multiple string values,
     * separated by the colon(:) character. 
     *
     * <table width="100%" border="1">
     *   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
     *   <tr>
     *     <td>Normal</td>
     *     <td>Regular quote; no special condition</td>
     *   </tr>
     *   <tr>
     *     <td>DepthAsk</td>
     *     <td>Depth on ask side</td>
     *   </tr>
     *   <tr>
     *     <td>DepthBid</td>
     *     <td>Depth on bid side</td>
     *   </tr>
     *   <tr>
     *     <td>SlowQuoteOnAskSide</td>
     *     <td>This indicates that a market participants Ask is in a slow
     *     (CTA) mode. While in this mode, automated
     *     execution is not eligible on the Ask side and can be
     *     traded through pursuant to Regulation NMS requirements.</td>
     *   </tr>
     *   <tr>
     *     <td>SlowQuoteOnBidSide</td>
     *     <td>This indicates that a market participants Bid is in a slow
     *     (CTA) mode. While in this mode, automated
     *     execution is not eligible on the Bid side and can be
     *     traded through pursuant to Regulation NMS
     *     requirements.</td>
     *   </tr>
     *   <tr>
     *     <td>Fast</td>
     *     <td>Fast trading</td>
     *   </tr>
     *   <tr>
     *     <td>NonFirm</td>
     *     <td>Non-firm quote</td>
     *   </tr>
     *   <tr>
     *     <td>Rotation</td>
     *     <td>OPRA only. Quote relates to a trading rotation (Where a participant rotates through various clients that they are trading for)</td>
     *   </tr>
     *   <tr>
     *     <td>Auto</td>
     *     <td>Automatic trade</td>
     *   </tr>
     *   <tr>
     *     <td>Inactive</td>
     *     <td>&nbsp;</td>
     *   </tr>
     *   <tr>
     *     <td>SpecBid</td>
     *     <td>Specialist bid</td>
     *   </tr>
     *   <tr>
     *     <td>SpecAsk</td>
     *     <td>Specialist ask</td>
     *   </tr>
     *   <tr>
     *     <td>OneSided</td>
     *     <td>One sided.  No orders, or only market orders, exist on one side
     *     of the book.</td>
     *   </tr>
     *   <tr>
     *     <td>PassiveMarketMaker</td>
     *     <td>Market Maker is both underwriter and buyer of security.</td>
     *   </tr>
     *   <tr>
     *     <td>LockedMarket</td>
     *     <td>Locked market - Bid is equal to Ask for OTCBB issues
     *     </td>
     *   </tr>
     *   <tr>
     *     <td>Crossed</td>
     *     <td>Crossed market - Bid is greater than Ask for OTCBB
     *     </td>
     *   </tr>
     *   <tr>
     *     <td>Synd</td>
     *     <td>Syndicate bid</td>
     *   </tr>
     *   <tr>
     *     <td>PreSynd</td>
     *     <td>Pre-syndicate bid</td>
     *   </tr>
     *   <tr>
     *     <td>Penalty</td>
     *     <td>Penalty bid</td>
     *   </tr>
     *   <tr>
     *     <td>UnsolBid</td>
     *     <td>Unsolicited bid</td>
     *   </tr>
     *   <tr>
     *     <td>UnsolAsk</td>
     *     <td>Unsolicited ask</td>
     *   </tr>
     *   <tr>
     *     <td>UnsolQuote</td>
     *     <td>Unsolicited quote</td>
     *   </tr>
     *   <tr>
     *     <td>Empty</td>
     *     <td>Empty quote (no quote)</td>
     *   </tr>
     *   <tr>
     *     <td>XpressBid</td>
     *     <td>NYSE LiquidityQuote Xpress bid indicator</td>
     *   </tr>
     *   <tr>
     *     <td>XpressAsk</td>
     *     <td>NYSE LiquidityQuote Xpress ask indicator</td>
     *   </tr>
     *   <tr>
     *     <td>BestOrder</td>
     *     <td>&nbsp;</td>
     *   </tr>
     *   <tr>
     *     <td>WillSell</td>
     *     <td>&nbsp;</td>
     *   </tr>
     *   <tr>
     *     <td>WillBuy</td>
     *     <td>&nbsp;</td>
     *   </tr>
     *   <tr>
     *     <td>AnyOrder</td>
     *     <td>&nbsp;</td>
     *   </tr>
     *   <tr>
     *     <td>MktOnly</td>
     *     <td>Market orders only.</td>
     *   </tr>
     *   <tr>
     *     <td>ManualAsk</td>
     *     <td>This indicates that a market participants Ask is in a manual
     *     (NASDAQ) mode. While in this mode, automated
     *     execution is not eligible on the Ask side and can be traded through
     *     pursuant to Regulation NMS requirements.</td>
     *   </tr>
     *   <tr>
     *     <td>ManualBid</td>
     *     <td>This indicates that a market participants Bid is in a manual
     *     (NASDAQ) mode. While in this mode, automated
     *     execution is not eligible on the Bid side and can be
     *     traded through pursuant to Regulation NMS requirements.</td>
     *   </tr>
     *   <tr>
     *     <td>AutomaticAsk</td>
     *     <td>This indicates that the marlet participant's Ask is in
     *     automatic mode - we generally send a combination of the last four
     *     (e.g. ManualAsk:AutomaticBid or ManualAsk;ManualBid)</td>
     *   </tr>
     *   <tr>
     *     <td>AutomaticBid</td>
     *     <td>This indicates that the marlet participant's Ask is in
     *     automatic mode - we generally send a combination of the last four
     *     (e.g. ManualAsk:AutomaticBid or ManualAsk;ManualBid)</td>
     *   </tr>
     *   <tr>
     *     <td>Closing</td>
     *     <td>Closing quote.</td>
     *   </tr>
     *   <tr>
     *     <td>Unknown</td>
     *     <td>Quote condition unknown.</td>
     *   </tr>
     *  </table>
     */
    String  getQuoteQual();

    /**
     * @return the quote qual Field State
     */
    short  getQuoteQualFieldState();

    /**
     * The exchange specific non normalized quote qualifier.
     * 
     * @return The exchange specific quote qualifier.
     * @see #getQuoteQual()
     */
    String getQuoteQualNative ();

    /**
     * @return the quote qual native Field State
     */
    short  getQuoteQualNativeFieldState();

    /**
     * NASDAQ Bid Tick Indicator for Short Sale Rule Compliance.
     * National Bid Tick Indicator based on changes to the bid price of the
     * National Best Bid or Offer (National BBO).
     * 
     * <table width="100%" border="1">
     *   <tr><td><b>Value</b></td><td><b>Meaning</b></td></tr>
     *   <tr>
     *     <td>U</td>
     *     <td>Up Tick. The current Best Bid Price price is higher than the
     *     previous Best Bid Price for the given NNM security.</td>
     *   </tr>
     *   <tr>
     *     <td>D</td>
     *     <td>Down Tick.  The current Best Bid Price price is lower than the
     *     previous Best Bid Price for the given NNM security.</td>
     *   </tr>
     *   <tr>
     *     <td>N</td>
     *     <td>No Tick. The NASD Short Sale Rule does not apply to issue (i.e.
     *     NASDAQ SmallCap listed security).</td>
     *   </tr>
     *   <tr>
     *     <td>Z</td>
     *     <td>Unset - default value within the API</td>
     *   </tr>
     * </table>
     *
     * @return The tick bid indicator.
     */
    char getShortSaleBidTick ();

    /**
     * @return the short sale bid tick Field State
     */
    short  getShortSaleBidTickFieldState();

    /**
     * get the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker 
     * <ul>
     *   <li>return values:</li>
     *   <li>Blank: Short Sale Restriction Not in Effect.</li>
     *   <li>A: Short Sale Restriction Activiated.</li>
     *   <li>C: Short Sale Restriction Continued.</li>
     *   <li>D: Sale Restriction Deactivated.</li>
     *   <li>E: Sale Restriction in Effect.</li>
     * </ul>    
     */    
    public char getShortSaleCircuitBreaker();

    /**
     * @return the reason Field State
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
     * @return the quote Ask depth
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
