/* $Id$
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

#ifndef MamdaQuoteRecapH
#define MamdaQuoteRecapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaFieldState.h>
#include <mama/types.h>

namespace Wombat
{

    /**
     * MamdaQuoteRecap is an interface that provides access to quote
     * related fields.
     */
    class MAMDAExpDLL MamdaQuoteRecap : public MamdaBasicRecap
    {
    public:

        /**
         * Get the quote bid price.
         * 
         * @return Bid price.  The highest price that the representative
         * party/group is willing to pay to buy the security.  For most
         * feeds, this size is represented in round lots.
         */
        virtual const MamaPrice&  getBidPrice() const = 0;
        
        /**
         * Get the quote bid size.
         *
         * @return Total share size available for the current bid price.
         * Note: many feeds provide this size in terms of round lots.

         */
        virtual mama_quantity_t  getBidSize() const = 0;

        /**
         * Get the quote bid depth.
         *
         * @return The total size available at the current best bid price.
         * Only supported by some exchanges.  The total size may differ
         * from the "best" size in that the total may be aggregated from
         * multiple sources.
         */
        virtual mama_quantity_t  getBidDepth() const = 0;

        /**
         * Get the quote bid participant identifier.
         *
         * @return The identifier of the market participant (e.g. exchange
         * or market maker) contributing the bid price field.
         */
        virtual const char*  getBidPartId() const = 0;

        /**
         * Get the bid closing price.
         *
         * @return Today's closing bid price, after the market has closed
         * and the stock has traded today.  If the market is not not
         * closed or the stock did not trade today, zero is returned and
         * the previous close price is available using
         * getBidPrevClosePrice().  This value is always zero at the start
         * of a trading day.
         */
        virtual const MamaPrice&  getBidClosePrice() const = 0;

        /**
         * Get the bid closing date.
         *
         * @return Date of the BidClosePrice, if that value is non-zero.
         * This is always the most recent day that the market was open,
         * including today.  This is different to BidPrevCloseDate.
         */
        virtual const MamaDateTime&  getBidCloseDate() const = 0;

        /**
         * Get the previous bid closing price.
         *
         * @return The previous closing price, prior to any trade today.
         */
        virtual const MamaPrice&  getBidPrevClosePrice() const = 0;

        /**
         * Get the previous bid closing date.
         *
         * @return Date of the previous closing price, prior to any trade
         * today.
         */
        virtual const MamaDateTime&  getBidPrevCloseDate() const = 0;

        /**
         * Get the high bid price for the day.
         * 
         * @return High bid price.
         */
        virtual const MamaPrice&  getBidHigh() const = 0;

        /**
         * Get the low bid price for the day.
         * 
         * @return Low bid price.
         */
        virtual const MamaPrice&  getBidLow() const = 0;

        /**
         * Get the quote ask price.
         *
         * @return Ask price.  The lowest price that the representative
         * party/group is willing to take to sell the security.  For most
         * feeds, this size is represented in round lots.
         */
        virtual const MamaPrice&  getAskPrice() const = 0;

        /**
         * Get the quote ask size.
         *
         * @return Total share size available for the current ask price.
         * Note: many feeds provide this size in terms of round lots.
         */
        virtual mama_quantity_t  getAskSize() const = 0;

        /**
         * Get the quote ask depth.
         *
         * @return The total size available at the current best ask price.
         * Only supported by some exchanges.  The total size may differ
         * from the "best" size in that the total may be aggregated from
         * multiple sources.
         */
        virtual mama_quantity_t  getAskDepth() const = 0;

        /**
         * Get the quote ask participant identifier.
         *
         * @return The identifier of the market participant (e.g. exchange
         * or market maker) contributing the ask price field.
         */
        virtual const char*  getAskPartId() const = 0;

        /**
         * Get the ask quote closing price.
         *
         * @return Today's closing ask price, after the market has closed
         * and the stock has traded today.  If the market is not not
         * closed or the stock did not trade today, zero is returned and
         * the previous close price is available using 
         * <code>getAskPrevClosePrice()</code>.  This value is always zero
         * at the start of a trading day.
         */
        virtual const MamaPrice&  getAskClosePrice() const = 0;

        /**
         * Get the ask quote closing date.
         *
         * @return Date of the AskClosePrice, if that value is non-zero.
         * This is always the most recent day that the market was open,
         * including today.  This is different to AskPrevCloseDate.
         */
        virtual const MamaDateTime&  getAskCloseDate() const = 0;

        /**
         * Get the previous ask closing price.
         *
         * @return The previous closing price, prior to any trade today.
         */
        virtual const MamaPrice&  getAskPrevClosePrice() const = 0;

        /**
         * Get the previous ask closing date.
         *
         * @return Date of previous closing price, prior to any trade
         * today.
         */
        virtual const MamaDateTime&  getAskPrevCloseDate() const = 0;

        /**
         * Get the high ask price for the day.
         * 
         * @return High ask price.
         */
        virtual const MamaPrice&  getAskHigh() const = 0;

        /**
         * Get the low ask price for the day.
         * 
         * @return Low ask price.
         */
        virtual const MamaPrice&  getAskLow() const = 0;

        /**
         * Get the quote mid price.
         *
         * @return The mid price of the current quote.  Usually, this is
         * the average of the bid and ask prices, but some exchanges
         * provide this field explicitly (e.g. LSE).
         */
        virtual const MamaPrice&  getQuoteMidPrice() const = 0;

        /**
         * Get the quote count.
         *
         * @return The number of quotes generated for this security during
         * the current trading session.
         */
        virtual mama_u32_t  getQuoteCount() const = 0;

        /**
         * Get quote qualifier as a string.
         *
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
         *     <td>OPRA only. Quote relates to a trading rotation (Where a
         *     participant rotates through various clients that they are trading
         *     for)</td>
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
         *     <td>This indicates that the market participant's Ask is in
         *     automatic mode - we generally send a combination of the last four
         *     (e.g. ManualAsk:AutomaticBid or ManualAsk;ManualBid)</td>
         *   </tr>
         *   <tr>
         *     <td>AutomaticBid</td>
         *     <td>This indicates that the market participant's Ask is in
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
        virtual const char*  getQuoteQualStr() const = 0;

        /**
         * Get the native feed quote qualifier.
         * @return Native quote qualifier (a.k.a. "quote condition").
         * Feed-specific quote qualifier code(s).  This field is provided
         * primarily for completeness and/or troubleshooting.  
         * @see getQuoteQual.
         */
        virtual const char*  getQuoteQualNative() const = 0;

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
        virtual char  getShortSaleBidTick() const = 0;
        
        /**
         * Get the quote ask time.
         *
         * @return The quote ask time.
         */
        virtual const MamaDateTime&  getAskTime() const = 0;
        
        /**
         * Get the quote bid time.
         *
         * @return The quote bid time.
         */
        virtual const MamaDateTime&  getBidTime() const = 0;
        
        /**
         * Get the quote date.
         *
         * @return The quote date.
         */
        virtual const MamaDateTime&  getQuoteDate() const = 0;
        
        /**
         * Get the quote ask indicator.
         *
         * @return The quote ask indicator.
         */
        virtual const char*  getAskIndicator() const = 0;
        
        /**
         * Get the quote bid indicator.
         *
         * @return The quote bid indicator.
         */
        virtual const char*  getBidIndicator() const = 0;
        
        /**
         * Get the quote ask update count.
         *
         * @return The quote ask update count.
         */
        virtual mama_u32_t  getAskUpdateCount() const = 0;
        
        /**
         * Get the quote bid update count.
         *
         * @return The quote bid update count.
         */
        virtual mama_u32_t  getBidUpdateCount() const = 0;
        
        /**
         * Get the quote ask yield.
         *
         * @return The quote ask yield.
         */
        virtual double  getAskYield() const = 0;
        
        /**
         * Get the quote bid yield.
         *
         * @return The quote bid yield.
         */
        virtual double  getBidYield() const = 0;

        /**
         * Get the ask sizes list.
         *
         * @return The ask sizes list.
         */
        virtual const char*  getAskSizesList() const = 0;

        /**
         * Get the bid sizes list.
         *
         * @return The bid sizes list.
         */
        virtual const char*  getBidSizesList() const = 0;

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
        virtual char  getShortSaleCircuitBreaker() const = 0;


        /* FieldState Accessors ******/
       
        /**
         * Get the quote bid price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidPriceFieldState() const = 0;
        
        /**
         * Get the quote bid size fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidSizeFieldState() const = 0;

        /**
         * Get the quote bid depth fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidDepthFieldState() const = 0;

        /**
         * Get the quote participant identifier fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidPartIdFieldState() const = 0;

        /**
         * Get the quote ask price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskPriceFieldState() const = 0;

        /**
         * Get the quote ask size fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskSizeFieldState() const = 0;

        /**
         * Get the quote ask depth fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskDepthFieldState() const = 0;

        /**
         * Get the quote ask participant identifier fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskPartIdFieldState() const = 0;

        /**
         * Get the quote mid price fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getQuoteMidPriceFieldState() const = 0;

        /**
         * Get the quote qualifier fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getQuoteQualStrFieldState() const = 0;

        /**
         * Get the native feed quote qualifier fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getQuoteQualNativeFieldState() const = 0;

        /**
         * Get the short sale bid tick fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getShortSaleBidTickFieldState() const = 0;
        
        /**
         * Get the quote ask time fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskTimeFieldState() const = 0;
        
        /**
         * Get the quote bid time fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidTimeFieldState() const = 0;
        
        /**
         * Get the quote date fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getQuoteDateFieldState() const = 0;
        
        /**
         * Get the quote ask indicator fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskIndicatorFieldState() const = 0;
        
        /**
         * Get the quote bid indicator fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidIndicatorFieldState() const = 0;
        
        /**
         * Get the quote ask update count fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskUpdateCountFieldState() const = 0;
        
        /**
         * Get the quote bid update count fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidUpdateCountFieldState() const = 0;
        
        /**
         * Get the quote ask yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskYieldFieldState() const = 0;
        
        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidYieldFieldState() const = 0;     

        /**
         * Get the ask sizes list fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getAskSizesListFieldState() const = 0;

        /**
         * Get the bid sizes list fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getBidSizesListFieldState() const = 0;
        
        /**
         * @return The ShortSaleCircuitBreaker Field State.
         * An enumeration representing field state.
         */
        virtual MamdaFieldState  getShortSaleCircuitBreakerFieldState() const = 0;
        
        virtual ~MamdaQuoteRecap() {};

    };

} // namespace

#endif // MamdaQuoteRecapH
