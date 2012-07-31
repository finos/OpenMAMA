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

#ifndef MamdaQuoteOutOfSequenceH
#define MamdaQuoteOutOfSequenceH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaQuoteOutOfSequence is an interface that provides access to fields
     * related to quote updates.
     */

    class MAMDAExpDLL MamdaQuoteOutOfSequence : public MamdaBasicEvent
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
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBidPriceFieldState() const = 0;

        /**
         * Get the quote bid size.
         *
         * @return Total share size available for the current bid price.
         * Note: many feeds provide this size in terms of round lots.
    
         */
        virtual mama_quantity_t  getBidSize() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBidSizeFieldState() const = 0;

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
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBidDepthFieldState() const = 0;

        /**
         * Get the quote bid participant identifier.`
         *
         * @return The identifier of the market participant (e.g. exchange
         * or market maker) contributing the bid price field.
         */
        virtual const char*  getBidPartId() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBidPartIdFieldState() const = 0;

        /**
         * Get the quote ask price.
         *
         * @return Ask price.  The lowest price that the representative
         * party/group is willing to take to sell the security.  For most
         * feeds, this size is represented in round lots.
         */
        virtual const MamaPrice&  getAskPrice() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAskPriceFieldState() const = 0;

        /**
         * Get the quote ask size.
         *
         * @return Total share size available for the current ask price.
         * Note: many feeds provide this size in terms of round lots.
         */
        virtual mama_quantity_t  getAskSize() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAskSizeFieldState() const = 0;

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
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAskDepthFieldState() const = 0;

        /**
         * Get the quote ask participant identifier.
         *
         * @return The identifier of the market participant (e.g. exchange
         * or market maker) contributing the ask price field.
         */
        virtual const char*  getAskPartId() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAskPartIdFieldState() const = 0;

        /**
         * Get the quote mid price.
         *
         * @return The mid price of the current quote.  Usually, this is
         * the average of the bid and ask prices, but some exchanges
         * provide this field explicitly (e.g. LSE).
         */
        virtual const MamaPrice&  getQuoteMidPrice  () const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getQuoteMidPriceFieldState() const = 0;

        /**
         * Get the quote qualifiers.
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
         *     <td>Fast</td>
         *     <td>Fast trading</td>
         *   </tr>
         *   <tr>
         *     <td>NonFirm</td>
         *     <td>Non-firm quote</td>
         *   </tr>
         *   <tr>
         *     <td>Rotation</td>
         *     <td>?</td>
         *   </tr>
         *   <tr>
         *     <td>Auto</td>
         *     <td>Automatic trade</td>
         *   </tr>
         *   <tr>
         *     <td>Inactive</td>
         *     <td>?</td>
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
         *     <td>Locked</td>
         *     <td>Locked market - Bid is equal to Ask for OTCBB issues
         *     (Recaps only)</td>
         *   </tr>
         *   <tr>
         *     <td>Crossed</td>
         *     <td>Crossed market - Bid is greater than Ask for OTCBB
         *     (Recaps only)</td>
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
         *     <td>Firm</td>
         *     <td>?</td>
         *   </tr>
         *   <tr>
         *     <td>RangeInd</td>
         *     <td>?</td>
         *   </tr>
         *   <tr>
         *     <td>XpressBid</td>
         *     <td>NYSE LiquidityQuote Xpress bid indicator</td>
         *   </tr>
         *   <tr>
         *     <td>XpressAsk</td>
         *     <td>NYSE LiquidityQuote Xpress ask indicator</td>
         *   </tr>
         *  </table>
         */
        virtual const char*  getQuoteQualStr() const = 0;

        /**
         * Get the quote bid yield fieldState
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getQuoteQualStrFieldState() const = 0;

        virtual ~MamdaQuoteOutOfSequence() {};
    };

} // namespace

#endif // MamdaQuoteOutOfSequenceH
