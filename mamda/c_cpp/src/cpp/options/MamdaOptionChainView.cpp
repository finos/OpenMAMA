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

#include <mamda/MamdaOptionChainView.h>
#include <mamda/MamdaOptionChain.h>
#include <mamda/MamdaOptionContract.h>
#include <mamda/MamdaOptionExpirationDateSet.h>
#include <mamda/MamdaQuoteHandler.h>
#include <mamda/MamdaTradeHandler.h>
#include <mama/mamacpp.h>

// Find DBL_MIN and DBL_MAX:
#ifdef HAVE_LIMITS_H
# include <limits.h>
# ifdef HAVE_VALUES_H
#  include <values.h>
# endif
#endif

#include <float.h>

namespace Wombat
{

    static const double DEFAULT_JITTER_MARGIN = 0.5; /* percent */

    struct MamdaOptionChainView::MamdaOptionChainViewImpl
    {
        MamdaOptionChainViewImpl (
            MamdaOptionChainView&  view,
            MamdaOptionChain&      chain);

        bool isVisible         (const MamdaOptionContract&  contract) const;

        void resetRange ();

        void filterExpirations (MamdaOptionExpirationDateSet&        result,
                                const MamdaOptionExpirationDateSet&  initialSet);

        void filterStrikes     (MamdaOptionExpirationDateSet&        dateSet);

        bool strikeInRange     (double               strikePrice) const;

        bool expirationInRange (const MamaDateTime&  expirationDate) const;

        class UnderlyingQuoteHandler : public MamdaQuoteHandler
        {
        public:
            UnderlyingQuoteHandler (MamdaOptionChain& chain)
                : mChain (chain)
            {
            }

            virtual ~UnderlyingQuoteHandler() {}

            void onQuoteRecap (
                MamdaSubscription*      subscription,
                MamdaQuoteListener&     listener,
                const MamaMsg&          msg,
                const MamdaQuoteRecap&  recap)
            {
            }

            void onQuoteUpdate (
                MamdaSubscription*      subscription,
                MamdaQuoteListener&     listener,
                const MamaMsg&          msg,
                const MamdaQuoteUpdate& quote,
                const MamdaQuoteRecap&  recap)
            {
            }

            void onQuoteGap (
                MamdaSubscription*      subscription,
                MamdaQuoteListener&     listener,
                const MamaMsg&          msg,
                const MamdaQuoteGap&    event,
                const MamdaQuoteRecap&  recap)
            {
            }

            void onQuoteClosing (
                MamdaSubscription*        subscription,
                MamdaQuoteListener&       listener,
                const MamaMsg&            msg,
                const MamdaQuoteClosing&  event,
                const MamdaQuoteRecap&    recap)
            {
            }

            void onQuoteOutOfSequence (
                MamdaSubscription*              subscription,
                MamdaQuoteListener&             listener,
                const MamaMsg&                  msg,
                const MamdaQuoteOutOfSequence&  event,
                const MamdaQuoteRecap&          recap)
            {
            }

            void onQuotePossiblyDuplicate (
                MamdaSubscription*                  subscription,
                MamdaQuoteListener&                 listener,
                const MamaMsg&                      msg,
                const MamdaQuotePossiblyDuplicate&  event,
                const MamdaQuoteRecap&              recap)
            {
            }

        private:
            MamdaOptionChain&  mChain;
        };

        class UnderlyingTradeHandler : public MamdaTradeHandler
        {
        public:
            UnderlyingTradeHandler (MamdaOptionChain& chain)
                : mChain (chain)
            {
            }

            void onTradeRecap (
                MamdaSubscription*      subscription,
                MamdaTradeListener&     listener,
                const MamaMsg&          msg,
                const MamdaTradeRecap&  recap)
            {
            }

            void onTradeReport (
                MamdaSubscription*      subscription,
                MamdaTradeListener&     listener,
                const MamaMsg&          msg,
                const MamdaTradeReport& trade,
                const MamdaTradeRecap&  recap)
            {
            }

            void onTradeGap (
                MamdaSubscription*              subscription,
                MamdaTradeListener&             listener,
                const MamaMsg&                  msg,
                const MamdaTradeGap&            event,
                const MamdaTradeRecap&          recap)
            {
            }

            void onTradeCancelOrError (
                MamdaSubscription*              subscription,
                MamdaTradeListener&             listener,
                const MamaMsg&                  msg,
                const MamdaTradeCancelOrError&  event,
                const MamdaTradeRecap&          recap)
            {
            }

            void onTradeCorrection (
                MamdaSubscription*              subscription,
                MamdaTradeListener&             listener,
                const MamaMsg&                  msg,
                const MamdaTradeCorrection&     event,
                const MamdaTradeRecap&          recap)
            {
            }

            void onTradeClosing (
                MamdaSubscription*              subscription,
                MamdaTradeListener&             listener,
                const MamaMsg&                  msg,
                const MamdaTradeClosing&        event,
                const MamdaTradeRecap&          recap)
            {
            }

            void onTradeOutOfSequence (
                MamdaSubscription*              subscription,
                MamdaTradeListener&             listener,
                const MamaMsg&                  msg,
                const MamdaTradeOutOfSequence&  event,
                const MamdaTradeRecap&          recap)
            {
            }

            void onTradePossiblyDuplicate (
                MamdaSubscription*                  subscription,
                MamdaTradeListener&                 listener,
                const MamaMsg&                      msg,
                const MamdaTradePossiblyDuplicate&  event,
                const MamdaTradeRecap&              recap)
            {
            }

        private:
            MamdaOptionChain&  mChain;
        };

        MamdaOptionChainView&         mView;
        MamdaOptionChain&             mChain;

        MamdaOptionAtTheMoneyCompareType  mAtTheMoneyType;
        double                        mStrikeMargin;
        int                           mNumStrikes;
        int                           mExpirationDays;
        int                           mNumExpirations;
        double                        mJitterMargin;

        MamdaOptionExpirationDateSet  mExpirationDateSet;
        MamaDateTime                  mLowExpireDate;
        MamaDateTime                  mHighExpireDate;
        double                        mLowStrike;
        double                        mHighStrike;

        // The following "underlying" handlers are used if/when we need to
        // check the strike range.  Which, if either, are active depends
        // on the value of mAtTheMoneyType.
        UnderlyingQuoteHandler        mQuoteHandler;
        UnderlyingTradeHandler        mTradeHandler;
    };

    MamdaOptionChainView::MamdaOptionChainView (MamdaOptionChain& chain)
        : mImpl (*new MamdaOptionChainViewImpl (*this, chain))
    {
        mImpl.resetRange ();
    }

    MamdaOptionChainView::~MamdaOptionChainView ()
    {
        delete &mImpl;
    }

    const char* MamdaOptionChainView::getSymbol() const
    {
        return mImpl.mChain.getSymbol();
    }

    void MamdaOptionChainView::setAtTheMoneyType (
        MamdaOptionAtTheMoneyCompareType  atTheMoneyType)
    {
        if (mImpl.mAtTheMoneyType != atTheMoneyType)
        {
            mImpl.mAtTheMoneyType = atTheMoneyType;
            mImpl.resetRange();
        }
    }

    void MamdaOptionChainView::setStrikeRangePercent (
        double  percentMargin)
    {
        if (mImpl.mStrikeMargin != percentMargin)
        {
            mImpl.mStrikeMargin = percentMargin;
            mImpl.resetRange();
        }
    }

    void MamdaOptionChainView::setStrikeRangeNumber (
        int  number)
    {
        if (mImpl.mNumStrikes != number)
        {
            mImpl.mNumStrikes = number;
            mImpl.resetRange();
        }
    }

    void MamdaOptionChainView::setExpirationRangeDays (
        int  expirationDays)
    {
        mImpl.mExpirationDays = expirationDays;
        mImpl.resetRange();
    }

    void MamdaOptionChainView::setNumberOfExpirations (
        int  numExpirations)
    {
        mImpl.mNumExpirations = numExpirations;
        mImpl.resetRange();
    }

    void MamdaOptionChainView::setJitterMargin (
        double  percentMargin)
    {
        mImpl.mJitterMargin = percentMargin;
        mImpl.resetRange();
    }

    bool MamdaOptionChainView::isVisible (
        const MamdaOptionContract&  contract) const
    {
        return mImpl.isVisible (contract);
    }

    const MamdaOptionExpirationDateSet& MamdaOptionChainView::getExpireDateSet() const
    {
        return mImpl.mExpirationDateSet;
    }

    void MamdaOptionChainView::onOptionChainRecap (
        MamdaSubscription*         subscription,
        MamdaOptionChainListener&  listener,
        const MamaMsg&             msg,
        MamdaOptionChain&          chain)
    {
        mImpl.resetRange();
    }

    void MamdaOptionChainView::onOptionContractCreate (
        MamdaSubscription*         subscription,
        MamdaOptionChainListener&  listener,
        const MamaMsg&             msg,
        MamdaOptionContract&       contract,
        MamdaOptionChain&          chain)
    {
        mImpl.resetRange();
    }

    void MamdaOptionChainView::onOptionSeriesUpdate (
        MamdaSubscription*              subscription,
        MamdaOptionChainListener&       listener,
        const MamaMsg&                  msg,
        const MamdaOptionSeriesUpdate&  event,
        MamdaOptionChain&               chain)
    {
        mImpl.resetRange();
    }

    void MamdaOptionChainView::onOptionChainGap (
        MamdaSubscription*         subscription,
        MamdaOptionChainListener&  listener,
        const MamaMsg&             msg,
        MamdaOptionChain&          chain)
    {
    }


    MamdaOptionChainView::MamdaOptionChainViewImpl::MamdaOptionChainViewImpl (
        MamdaOptionChainView&  view,
        MamdaOptionChain&      chain)
        : mView             (view)
        , mChain            (chain)
        , mAtTheMoneyType   (MAMDA_AT_THE_MONEY_COMPARE_MID_QUOTE)
        , mStrikeMargin     (0.0)
        , mNumStrikes       (0)
        , mExpirationDays   (0)
        , mNumExpirations   (0)
        , mJitterMargin     (DEFAULT_JITTER_MARGIN)
        , mLowStrike        (0.0)
        , mHighStrike       (0.0)
        , mQuoteHandler     (chain)
        , mTradeHandler     (chain)
    {
    }

    bool MamdaOptionChainView::MamdaOptionChainViewImpl::isVisible (
        const MamdaOptionContract&  contract) const
    {
        double               strikePrice = contract.getStrikePrice();
        const MamaDateTime&  expireDate  = contract.getExpireDate();

        return ((mLowExpireDate.compare(expireDate) <= 0) &&
                    (mHighExpireDate.compare(expireDate) >= 0) &&
                    (mLowStrike <= strikePrice) &&
                    (strikePrice <= mHighStrike));
    }

    void MamdaOptionChainView::MamdaOptionChainViewImpl::resetRange ()
    {
        filterExpirations (mExpirationDateSet, mChain.getAllExpirations());
        filterStrikes     (mExpirationDateSet);

        if (mExpirationDateSet.empty())
        {
            mLowExpireDate.clear();
            mHighExpireDate.setToNow();
            mHighExpireDate.addSeconds (366 * 24 * 60 * 60);  // one year
        }
        else
        {
            mLowExpireDate  = mExpirationDateSet.begin()->first;
            mHighExpireDate = mExpirationDateSet.rbegin()->first;
        }

    }

    void MamdaOptionChainView::MamdaOptionChainViewImpl::filterExpirations (
        MamdaOptionExpirationDateSet&        result,
        const MamdaOptionExpirationDateSet&  initialSet)
    {
        if (mExpirationDays > 0)
        {
            // Find the subset of expirations between now and some
            // number of days into the future.
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "filterExpirations: trying to find expirations within %d days",
                      mExpirationDays);

            MamaDateTime futureDate;
            futureDate.setToNow();
            futureDate.addSeconds (mExpirationDays * 24 * 60 * 60);
            initialSet.getExpirationsBefore (result, futureDate);
        }
        else if (mNumExpirations > 0)
        {
            // Explicit number of expiration dates.
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "filterExpirations: trying to find %d expirationss",
                      mNumExpirations);
            initialSet.getExpirations (result, mNumExpirations);
        }
        else
        {
            // All expiration months.  We *copy* the initial set
            // because we may reduce the set to a particular range of
            // strike prices.
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "filterExpirations: taking all expirations",
                      mNumExpirations);
            result = initialSet;
        }
    }

    void MamdaOptionChainView::MamdaOptionChainViewImpl::filterStrikes (
        MamdaOptionExpirationDateSet&  dateSet)
    {
        mLowStrike  = DBL_MIN;
        mHighStrike = DBL_MAX;

        // First calculate the high/low strikes
        StrikeSet strikeSet;
        if (mStrikeMargin > 0.0)
        {
            mChain.getStrikesWithinPercent (strikeSet, 
                                            mStrikeMargin,
                                            mAtTheMoneyType);
        }
        if (strikeSet.empty())
        {
            if (mNumExpirations > 0)
            {
                mChain.getStrikesWithinRangeSize (strikeSet, 
                                                  mNumStrikes,
                                                  mAtTheMoneyType);
            }
        }

        if (strikeSet.empty())
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "filterStrikes: no strikes or underlying (yet?)");
            return;
        }
        else
        {
            if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
            {
                char strikeSetStr[256];
                int  offset = 0;
                StrikeSet::const_iterator i;

                for (i = strikeSet.begin(); i != strikeSet.end(); ++i)
                {
                    snprintf (strikeSetStr+offset, 256-offset, "%g ", *i);
                    offset = strlen(strikeSetStr);
                }

                mama_log (MAMA_LOG_LEVEL_FINE,
                          "filterStrikes: got %d strikes in range: %s",
                          strikeSet.size(), strikeSetStr);
            }
        }

        mLowStrike  = *strikeSet.begin();
        mHighStrike = *strikeSet.rbegin();

        MamdaOptionExpirationDateSet::iterator expireEnd  = dateSet.end();
        MamdaOptionExpirationDateSet::iterator expireIter = dateSet.begin();

        while (expireIter != expireEnd)
        {
            // Filter the strike prices for one expiration date
            MamdaOptionExpirationStrikes* expireStrikes = expireIter->second;
            expireStrikes->trimStrikes (strikeSet);
            ++expireIter;
        }
    }

    bool MamdaOptionChainView::MamdaOptionChainViewImpl::strikeInRange (
        double  strikePrice) const
    {
        // To be completed.
        return true;
    }

    bool MamdaOptionChainView::MamdaOptionChainViewImpl::expirationInRange (
        const MamaDateTime&  expirationDate) const
    {
        // To be completed.
        return true;
    }

} // namespace
