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

#ifndef MamdaQuoteUpdateH
#define MamdaQuoteUpdateH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaQuoteUpdate is an interface that provides access to fields
     * related to quote updates.
     */
    class MAMDAExpDLL MamdaQuoteUpdate : public MamdaBasicEvent
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
         * Get the quote participant identifier.
         *
         * @return The identifier of the market participant (e.g. exchange
         * or market maker) contributing the bid price field.
         */
        virtual const char*  getBidPartId() const = 0;

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
         * Get the quote mid price.
         *
         * @return The mid price of the current quote.  Usually, this is
         * the average of the bid and ask prices, but some exchanges
         * provide this field explicitly (e.g. LSE).
         */
        virtual const MamaPrice&  getQuoteMidPrice  () const = 0;

        /**
         * Get the quote qualifier. 
         *
         * @return A normalized set of qualifiers for the last quote for
         * the security. This field may contain multiple string values,
         * separated by the colon(:) character.  
         *
         * @see MamdaQuoteRecap#getQuoteQualStr()
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
        virtual char getShortSaleBidTick () const = 0;
        
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
        
        virtual ~MamdaQuoteUpdate() {};
    };

} // namespace

#endif // MamdaQuoteUpdateH
