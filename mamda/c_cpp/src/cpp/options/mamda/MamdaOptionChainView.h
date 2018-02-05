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

#ifndef MamdaOptionChainViewH
#define MamdaOptionChainViewH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionAtTheMoneyCompareType.h>
#include <mamda/MamdaOptionChainHandler.h>
#include <mamda/MamdaOptionExpirationDateSet.h>

namespace Wombat
{

    /**
     * A class that represents a "view" of a subset of an option chain.
     * The view can be restricted to a percentage or number of strike
     * prices around "the money" as well as to a maximum number of days
     * into the future.  The view will be adjusted to include strike
     * prices within the range as the underlying price moves.  This means
     * that the range of strike prices will change over time.  In order to
     * avoid a "jitter" in the range of strike prices when the underlying
     * price hovers right on the edge of a range boundary, the class also
     * provides a "jitter margin" as some percentage of the underlying
     * price (default is 0.5%).
     */
    class MAMDAOPTExpDLL MamdaOptionChainView : public MamdaOptionChainHandler
    {
    public:
        /**
         * Create a view on the given option chain.  Multiple views are
         * supported on any given option chain.
         *
         * @param chain The <code>MamdaOptionChain</code> on which to provide
         * a view.
         */
        MamdaOptionChainView (MamdaOptionChain& chain);

        /**
         * Destructor.
         */
        virtual ~MamdaOptionChainView ();

        /**
         * Return the symbol for the option chain.
         *
         * @return The symbol for the option chain.
         */
        const char* getSymbol () const;

        /**
         * Set how the underlying price ("at the money") is determined.
         *
         * @param atTheMoneyType The comparator enumeration to determine how at
         * the money is determined.
         * 
         * @see MamdaOptionAtTheMoneyCompareType
         */
        void setAtTheMoneyType (MamdaOptionAtTheMoneyCompareType  atTheMoneyType);

        /**
         * Set the range of strike prices to be included in the view by
         * percentage variation from the underlying price.  The range of
         * strike prices in the view will vary as the underlying varies.
         * The "jitter margin" avoids switching between ranges too often.
         *
         * @param percentMargin Percentage variation the strike price has from the
         * underlying.
         */
        void setStrikeRangePercent (double  percentMargin);

        /**
         * Set the number of strike prices to be included in the view.
         * The "jitter margin" avoids switching between ranges too often.
         *
         * @param number The number of strikes to include in the view.
         */
        void setStrikeRangeNumber (int  number);

        /**
         * Set the range of expiration dates to be included in the view by
         * the maximum number of days until expiration.  Note: a non-zero
         * range overrides a specific number of expirations set by
         * setNumberOfExpirations().
         *
         * @param expirationDays The maximum number of days until expiration.
         */
        void setExpirationRangeDays (int  expirationDays);

        /**
         * Set the number of expiration dates to be included in the view.
         * Note: a non-zero range (set by setExpirationRangeDays())
         * overrides a specific number of expirations.
         *
         * @param numExpirations The number of expiration dates to include in the
         * view.
         */
        void setNumberOfExpirations (int  numExpirations);

        /**
         * Set a "jitter margin" to avoid having the range jump between
         * different strike prices when the underlying price hovers right
         * on the edge of a range boundary.  The underlying is allowed to
         * fluctuate within the jitter margin without the range being
         * reset.  When the underlying moves beyond the jitter margin,
         * the range is reset.
         *
         * @param percentMargin Percentage from the range boundary the underlying
         * price can fluctuate without resetting the range.
         */
        void setJitterMargin (double  percentMargin);

        /**
         * Return whether an option contract falls within this view's
         * parameters.
         *
         * @param contract The contract which is being checked for visibility by
         * the current view.
         */
        bool isVisible (const MamdaOptionContract&  contract) const;

        /**
         * Return the option expiration date set, which contains the
         * subset of strike prices for the view, and the contracts within
         * the strike price.
         *
         * @return The expiration date set for the option.
         */
        const MamdaOptionExpirationDateSet&  getExpireDateSet() const;

        /**
         * Handler option chain recaps and initial values.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener instance which invoked this callback.
         * @param msg          The msg which resulted in the callback being
         * invoked.
         * @param chain        The complete option chain.
         */
        void onOptionChainRecap (
            MamdaSubscription*         subscription,
            MamdaOptionChainListener&  listener,
            const MamaMsg&             msg,
            MamdaOptionChain&          chain);

        /**
         * Handler for option chain structural updates.
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener instance which invoked this
         * callback.    
         * @param msg           The msg which resulted in the callback being
         * invoked.
         * @param contract      The newly created option contract.
         * @param chain         The complete option chain.
         */
        void onOptionContractCreate (
            MamdaSubscription*         subscription,
            MamdaOptionChainListener&  listener,
            const MamaMsg&             msg,
            MamdaOptionContract&       contract,
            MamdaOptionChain&          chain);

        /**
         * Handler for option chain structural updates.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The msg which resulted in the callback being
         * invoked.
         * @param event        Access to the series update event details.
         * @param chain        The complete option chain.
         */
        void onOptionSeriesUpdate (
            MamdaSubscription*              subscription,
            MamdaOptionChainListener&       listener,
            const MamaMsg&                  msg,
            const MamdaOptionSeriesUpdate&  event,
            MamdaOptionChain&               chain);

        /**
         * Handler for gaps.
         *
         * @param subscription The subscription which received the update.
         * @param listener     The listener which invoked this callback.
         * @param msg          The msg which resulted in the callback being
         * invoked.
         * @param chain        The complete option chain.
         */
        void onOptionChainGap (
            MamdaSubscription*              subscription,
            MamdaOptionChainListener&       listener,
            const MamaMsg&                  msg,
            MamdaOptionChain&               chain);

    private:
        struct MamdaOptionChainViewImpl;
        MamdaOptionChainViewImpl& mImpl;
    };

} // namespace

#endif // MamdaOptionChainViewH
