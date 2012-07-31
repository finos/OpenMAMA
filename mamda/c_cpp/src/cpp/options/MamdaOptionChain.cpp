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

#include <mamda/MamdaOptionChain.h>
#include <mamda/MamdaOptionContract.h>
#include <mamda/MamdaOptionExchangeUtils.h>
#include <mamda/MamdaOptionExpirationDateSet.h>
#include <mamda/MamdaOptionExpirationStrikes.h>
#include <mamda/MamdaOptionTypes.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaTradeListener.h>
#include <mama/mamacpp.h>

// Find DBL_MIN and DBL_MAX:
#ifdef HAVE_LIMITS_H
# include <limits.h>
# ifdef HAVE_VALUES_H
#  include <values.h>
# endif
#endif

#include <float.h>
#include <set>
#include <string>
#include <cstring>
#include <iostream>

using std::set;
using std::string;
using std::cout;
using std::endl;
using std::strcmp;

namespace Wombat
{

    struct contract_less_than
    {
        bool operator() (MamdaOptionContract* contract1,
                         MamdaOptionContract* contract2) const
        {
            // Expiration month
            const MamaDateTime& expiration1 = contract1->getExpireDate();
            const MamaDateTime& expiration2 = contract2->getExpireDate();

            int comp = expiration1.compare(expiration2);

            if (comp == 0)
            {
                // Strike price
                double strike1    = contract1->getStrikePrice();
                double strike2    = contract2->getStrikePrice();
                double strikeDiff = strike1 - strike2;

                if (strikeDiff > 0)
                    comp = 1;
                else if (strikeDiff < 0)
                    comp = -1;
            }

            if (comp == 0)
            {
                // Exchange
                const char* exchange1 = contract1->getExchange();
                const char* exchange2 = contract2->getExchange();
                comp = strcmp (exchange1, exchange2);
            }

	        if (comp == 0)
	        {
	            // Symbol
                const char* symbol1 = contract1->getSymbol();
                const char* symbol2 = contract2->getSymbol();
                comp = strcmp (symbol1, symbol2);
	        }

            return (comp < 0);
        }
    };

    typedef set<MamdaOptionContract*, contract_less_than>      ContractSet;

    struct MamdaOptionChain::MamdaOptionChainImpl
    {
        MamdaOptionChainImpl();
        ~MamdaOptionChainImpl();

        void addContract (
            const char*                       contractSymbol,
	        MamdaOptionContract*              contract);

        void processNewContractDetails (
            const char*                       contractSymbol,
	        MamdaOptionContract*              contract);

        double getAtTheMoney (
            MamdaOptionAtTheMoneyCompareType  compareType);

        void getStrikesWithinPercent (
            StrikeSet&                        strikeSet,
            double                            percentage,
            MamdaOptionAtTheMoneyCompareType  compareType);

        void getStrikesWithinRangeSize (
            StrikeSet&                        strikeSet,
            int                               rangeLength,
            MamdaOptionAtTheMoneyCompareType  compareType);

        bool getIsPriceWithinPercentOfMoney (
            double                            price,
            double                            percentage,
            MamdaOptionAtTheMoneyCompareType  compareType);

        void dump(); 

        string                        mSymbol;
        ContractSet                   mCallOptions;
        ContractSet                   mPutOptions;
        StrikeSet                     mStrikePrices;
        const MamdaQuoteListener*     mQuoteListener;
        const MamdaTradeListener*     mTradeListener;
        MamdaOptionExpirationDateSet  mExpirationSet;
    };

    MamdaOptionChain::MamdaOptionChain (
        const char*  symbol)
        : mImpl (*new MamdaOptionChainImpl)
    {
        setSymbol (symbol);
    }

    MamdaOptionChain::~MamdaOptionChain ()
    {
        delete &mImpl;
    }

    void MamdaOptionChain::setSymbol (
        const char*  symbol)
    {
        mImpl.mSymbol = symbol;
    }

    const char* MamdaOptionChain::getSymbol () const
    {
        return mImpl.mSymbol.c_str();
    }

    void MamdaOptionChain::setUnderlyingQuoteListener (
        const MamdaQuoteListener*  quoteListener)
    {
        mImpl.mQuoteListener = quoteListener;
    }

    void MamdaOptionChain::setUnderlyingTradeListener (
        const MamdaTradeListener*  tradeListener)
    {
        mImpl.mTradeListener = tradeListener;
    }

    const MamdaQuoteListener* MamdaOptionChain::getUnderlyingQuoteListener () const
    {
        return mImpl.mQuoteListener;
    }

    const MamdaTradeListener* MamdaOptionChain::getUnderlyingTradeListener () const
    {
        return mImpl.mTradeListener;
    }

    void MamdaOptionChain::addContract (
        const char*                       contractSymbol,
        MamdaOptionContract*              contract)
    {
        mImpl.addContract (contractSymbol, contract);
    }

    void MamdaOptionChain::processNewContractDetails (
        const char*                       contractSymbol,
        MamdaOptionContract*              contract)
    {
        mImpl.processNewContractDetails (contractSymbol, contract);
    }

    void MamdaOptionChain::removeContract (const char* contractSymbol)
    {
        // don't forget to delete[] the char array.
    }

    double MamdaOptionChain::getAtTheMoney (
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        return mImpl.getAtTheMoney (compareType);
    }

    void MamdaOptionChain::getStrikesWithinPercent (
        StrikeSet&                        strikeSet,
        double                            percentage,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        mImpl.getStrikesWithinPercent (strikeSet, percentage, compareType);
    }

    void MamdaOptionChain::getStrikesWithinRangeSize (
        StrikeSet&                        strikeSet,
        int                               rangeLength,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        mImpl.getStrikesWithinRangeSize (strikeSet, rangeLength, compareType);
    }

    bool MamdaOptionChain::getIsPriceWithinPercentOfMoney (
        double                            price,
        double                            percentage,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        return mImpl.getIsPriceWithinPercentOfMoney (price, percentage,
                                                      compareType);
    }

    struct MamdaOptionChain::iterator::iteratorImpl
    {
        iteratorImpl (ContractSet& aSet)
            : mSet (aSet)
        {
            mSetNextIter = aSet.begin();
        }
        ContractSet&           mSet;
        ContractSet::iterator  mSetNextIter;
    };

    struct MamdaOptionChain::const_iterator::constIteratorImpl
    {
        constIteratorImpl (ContractSet& aSet)
            : mSet (aSet)
        {
            mSetNextIter = aSet.begin();
        }
        ContractSet&           mSet;
        ContractSet::iterator  mSetNextIter;
    };

    /*
     * iterator implementation
     */
    MamdaOptionChain::iterator MamdaOptionChain::callIterator()
    {
        return iterator(&mImpl.mCallOptions);
    }

    MamdaOptionChain::iterator MamdaOptionChain::putIterator()
    {
        return iterator(&mImpl.mPutOptions);
    }

    MamdaOptionChain::const_iterator MamdaOptionChain::callIterator() const
    {
        return const_iterator(&mImpl.mCallOptions);
    }

    MamdaOptionChain::const_iterator MamdaOptionChain::putIterator() const
    {
        return const_iterator(&mImpl.mPutOptions);
    }

    MamdaOptionChain::iterator::iterator(void* aSetVoid)
    {
        ContractSet* aSet = (ContractSet*)aSetVoid;
        mIterImpl = new iteratorImpl(*aSet);
    }

    MamdaOptionChain::iterator::iterator(const iterator& copy)
    {
        mIterImpl = new iteratorImpl (copy.mIterImpl->mSet);
    }

    MamdaOptionChain::iterator::~iterator()
    {
        delete mIterImpl;
    }

    MamdaOptionChain::iterator& MamdaOptionChain::iterator::operator= (const iterator& rhs)
    {
        if (&rhs != this)
        {
            delete mIterImpl;
            mIterImpl = new iteratorImpl(rhs.mIterImpl->mSet);
        }
        return *this;
    }

    bool MamdaOptionChain::iterator::hasNext()
    {
        ContractSet::iterator& nextIter = mIterImpl->mSetNextIter;
        return (nextIter != mIterImpl->mSet.end());
    }

    MamdaOptionContract* MamdaOptionChain::iterator::next()
    {
        MamdaOptionContract* nextContract = *mIterImpl->mSetNextIter;
        ++mIterImpl->mSetNextIter;
        return nextContract;
    }

    /*
     * const_iterator implementation
     */
    MamdaOptionChain::const_iterator::const_iterator (void* aSetVoid)
    {
        ContractSet* aSet = (ContractSet*)aSetVoid;
        mIterImpl = new constIteratorImpl(*aSet);
    }

    MamdaOptionChain::const_iterator::const_iterator (const const_iterator& copy)
    {
        mIterImpl = new constIteratorImpl(copy.mIterImpl->mSet);
    }

    MamdaOptionChain::const_iterator::~const_iterator()
    {
        delete mIterImpl;
    }

    MamdaOptionChain::const_iterator& MamdaOptionChain::const_iterator::operator= (
        const const_iterator& rhs)
    {
        if (&rhs != this)
        {
            delete mIterImpl;
            mIterImpl = new constIteratorImpl(rhs.mIterImpl->mSet);
        }
        return *this;
    }

    bool MamdaOptionChain::const_iterator::hasNext()
    {
        ContractSet::const_iterator& nextIter = mIterImpl->mSetNextIter;
        return (nextIter != mIterImpl->mSet.end());
    }

    const MamdaOptionContract* MamdaOptionChain::const_iterator::next()
    {
        MamdaOptionContract* nextContract = *mIterImpl->mSetNextIter;
        ++mIterImpl->mSetNextIter;
        return nextContract;
    }

    const MamdaOptionExpirationDateSet&  
    MamdaOptionChain::getAllExpirations () const
    {
        return mImpl.mExpirationSet;
    }

    void MamdaOptionChain::dump ()
    {
        mImpl.dump();
    }

    MamdaOptionChain::MamdaOptionChainImpl::MamdaOptionChainImpl()
    {
    }

    MamdaOptionChain::MamdaOptionChainImpl::~MamdaOptionChainImpl()
    {
        // We did not create the MamdaOptionContract objects, so it would
        // be wrong to delete them here.
	    MamdaOptionExpirationDateSet::iterator iter;

	    for (iter = mExpirationSet.begin(); iter != mExpirationSet.end(); iter++)
	    {
            MamdaOptionExpirationStrikes::iterator subIter;
		    for (subIter = iter->second->begin(); subIter != iter->second->end(); subIter++)
		    {
			    delete (*subIter).second;
		    }
		    delete (*iter).second;
	    }
    }

    void MamdaOptionChain::MamdaOptionChainImpl::addContract (
        const char*           contractSymbol,
        MamdaOptionContract*  contract)
    {
        processNewContractDetails (contractSymbol, contract);
    }
     
    void MamdaOptionChain::MamdaOptionChainImpl::processNewContractDetails (
        const char*           contractSymbol,
        MamdaOptionContract*  contract)
    {
        // Not waiting on a recap to update contract details 
        if (!contract->getRecapRequired())
        {
            const char*          exchange       = contract->getExchange();
	        const MamaDateTime&  expireDate     = contract->getExpireDate();
	        double               strikePrice    = contract->getStrikePrice();
	        char                 putCall        = contract->getPutCall();
	        bool                 gotExpireDate  = contract->gotExpireDate();
	        bool                 gotStrikePrice = contract->gotStrikePrice();
	        bool                 gotPutCall     = contract->gotPutCall();

	        if (gotPutCall)
	        {
	            switch (putCall)
	            {
	                case MAMDA_PUT_CALL_CALL:
	                    mCallOptions.insert (ContractSet::value_type(contract));
		                break;
	                case MAMDA_PUT_CALL_PUT:
	                    mPutOptions.insert (ContractSet::value_type(contract));
		                break;
	            }
	        }

	        // Add the contract to the expiration-by-strike set.
	        MamdaOptionExpirationStrikes* expireStrikes = NULL;
	        MamdaOptionStrikeSet*         strikeSet     = NULL;
	        MamdaOptionContractSet*       contractSet   = NULL;

	        if (gotExpireDate)
	        {
	            MamdaOptionExpirationDateSet::iterator foundExpireStrikes =
	                mExpirationSet.find (expireDate);
	            if (foundExpireStrikes == mExpirationSet.end())
	            {
		            expireStrikes = new MamdaOptionExpirationStrikes;
		            mExpirationSet.insert (
		                MamdaOptionExpirationDateSet::value_type(
					              expireDate, expireStrikes));
	            }
	            else
	            {
	                expireStrikes = foundExpireStrikes->second;
	            }
	        }

	        if ((expireStrikes != NULL) && gotStrikePrice)
	        {
	            MamdaOptionExpirationStrikes::const_iterator foundStrikeSet =
	                expireStrikes->find (strikePrice);
	            if ((foundStrikeSet == expireStrikes->end()) && gotExpireDate) 
	            {
		            strikeSet = new MamdaOptionStrikeSet (expireDate, strikePrice);
		            expireStrikes->
		                insert (MamdaOptionExpirationStrikes::value_type(
						                 strikePrice, strikeSet));
	            }
	            else
	            {
	                strikeSet = foundStrikeSet->second;
	            }
	        }

	        if ((strikeSet != NULL) && gotPutCall)
	        {
	            switch (putCall)
	            {
	                case MAMDA_PUT_CALL_CALL:
	                    contractSet = strikeSet->getCallSet();
		                break;
	                case MAMDA_PUT_CALL_PUT:
	                    contractSet = strikeSet->getPutSet();
		                break;
	            }

	            if (MamdaOptionExchangeUtils::isBbo (exchange))
	            {
	                contractSet->setBboContract (contract);
	            }
	            else if (MamdaOptionExchangeUtils::isWombatBbo (exchange))
	            {
	                contractSet->setWombatBboContract (contract);
	            }
	            else
	            {
	                contractSet->setExchangeContract (exchange, contract);
	            }
	        }

	        if (contract->gotStrikePrice())
	        {
	            mStrikePrices.insert(strikePrice);
	        }
        }
    }

    double MamdaOptionChain::MamdaOptionChainImpl::getAtTheMoney (
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        double atTheMoney = 0.0;
        switch (compareType)
        {
            case MAMDA_AT_THE_MONEY_COMPARE_MID_QUOTE:
                if (!mQuoteListener) return 0.0;
                atTheMoney = (mQuoteListener->getBidPrice().getValue() +
                              mQuoteListener->getAskPrice().getValue()) / 2.0;
                break;

            case MAMDA_AT_THE_MONEY_COMPARE_BID:
                if (!mQuoteListener) return 0.0;
                atTheMoney = mQuoteListener->getBidPrice().getValue();
                break;

            case MAMDA_AT_THE_MONEY_COMPARE_ASK:
                if (!mQuoteListener) return 0.0;
                atTheMoney = mQuoteListener->getAskPrice().getValue();
                break;

            case MAMDA_AT_THE_MONEY_COMPARE_LAST_TRADE:
                if (!mTradeListener) return 0.0;
                atTheMoney = mTradeListener->getLastPrice().getValue();
                break;
        }
        return atTheMoney;
    }

    bool MamdaOptionChain::MamdaOptionChainImpl::getIsPriceWithinPercentOfMoney (
        double                            price,
        double                            percentage,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        double atTheMoney  = getAtTheMoney(compareType);
        if (atTheMoney == 0.0)
            return false;

        return (((1.0 - percentage) <= price) || (price <= (1.0 + percentage)));
    }

    void MamdaOptionChain::MamdaOptionChainImpl::getStrikesWithinPercent (
        StrikeSet&                        strikeSet,
        double                            percentage,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "getStrikesWithinPercent: looking for strikes within %g",
                  percentage);

        strikeSet.clear();
        percentage /= 100.0;

        if (percentage <= 0.0)
            return;

        double atTheMoney  = getAtTheMoney(compareType);

        if (atTheMoney == 0.0)
            return;

        double lowPercent  = atTheMoney * (1.0 - percentage);
        double highPercent = atTheMoney * (1.0 + percentage);

        StrikeSet::const_iterator i   = mStrikePrices.lower_bound (lowPercent);
        StrikeSet::const_iterator end = mStrikePrices.upper_bound (highPercent);

        while (i != end)
        {
            strikeSet.insert(*i);
        }
    }

    void MamdaOptionChain::MamdaOptionChainImpl::getStrikesWithinRangeSize (
        StrikeSet&                        strikeSet,
        int                               rangeLength,
        MamdaOptionAtTheMoneyCompareType  compareType)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "getStrikesWithinRangeSize: looking for strikes for range length: %d",
                  rangeLength);

        strikeSet.clear();

        if (rangeLength <= 0.0)
            return;

        double atTheMoney  = getAtTheMoney(compareType);

        if (atTheMoney == 0.0)
            return;

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "getStrikesWithinRangeSize: at-the-money: %g",
                  atTheMoney);

        int    countToMoney     = 0;
        int    countFromMoney   = 0;
        double halfRangeLength  = rangeLength / 2.0;
        double lowerBound       = DBL_MIN;
        double upperBound       = DBL_MAX;

        // First loop determines the upper bound and count the number
        // of strikes that are less than the "money".
        StrikeSet::const_iterator strikeIter = mStrikePrices.begin();
        StrikeSet::const_iterator strikeEnd  = mStrikePrices.end();

        for (; strikeIter != strikeEnd; ++strikeIter)
        {
            double strikePrice = *strikeIter;

            if (strikePrice < atTheMoney)
            {
                // We're still less than the "money", so keep counting up.
                countToMoney++;
            }
            else
            {
                // Check the upper bound. Take a price that is
                // slightly higher than this strike price because we
                // want to include it (Set.subSet excludes the
                // toElement).
                countFromMoney++;
                upperBound = strikePrice + 0.00001;

                if (countFromMoney >= halfRangeLength)
                {
                    // We've passed the upper end of the range
                    break;
                }
            }
        }
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "getStrikesWithinRangeSize: upperBound: %g",
                  upperBound);

        // Second loop determines the lower bound.
        int targetCount = countToMoney - (rangeLength - countFromMoney);
        strikeIter = mStrikePrices.begin();

        for (; strikeIter != strikeEnd; ++strikeIter)
        {
            double strikePrice = *strikeIter;
            if (targetCount > 0)
            {
                // We have not yet hit the lowerBound.
                targetCount--;
            }
            else
            {
                lowerBound = strikePrice;
                break;
            }
        }

        StrikeSet::const_iterator i   = mStrikePrices.lower_bound(lowerBound);
        StrikeSet::const_iterator end = mStrikePrices.upper_bound(upperBound);

        while (i != end)
        {
            strikeSet.insert(*i);
            ++i;
        }

        if (strikeSet.empty())
        {
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "getStrikesWithinRangeSize: empty range!");
        }
        else
        {
            lowerBound = *strikeSet.begin();
            upperBound = *strikeSet.rbegin();

            mama_log (MAMA_LOG_LEVEL_FINER,
                      "getStrikesWithinRangeSize: "
                      "lowerBound=%f atTheMoney=%f upperBound=%f",
                      lowerBound, 
                      atTheMoney, 
                      upperBound);
        }
    }

    void MamdaOptionChain::MamdaOptionChainImpl::dump ()
    {
        int i = 0;

        ContractSet::const_iterator callIter =  mCallOptions.begin();
        ContractSet::const_iterator callEnd  =  mCallOptions.end();
        ContractSet::const_iterator putIter  =  mPutOptions.begin();
        ContractSet::const_iterator putEnd   =  mPutOptions.end();
      
        while ((callIter != callEnd) || (putIter != putEnd))
        {
            cout << i << " | ";
	        if (callIter != callEnd)
	        {
	            const MamdaOptionContract* callContract = *callIter;

	            cout << callContract->getSymbol()                     << " ";
	            cout << callContract->getExchange()                   << " ";
	            cout << callContract->getExpireDate().getAsString()   << " ";
	            cout << callContract->getStrikePrice();
	            
	            callIter++;
	        }
	        else
	        {
	            cout <<  "          ";
	        }
	        cout << " | ";

	        if (putIter != putEnd)
	        {
	            const MamdaOptionContract* callContract = *putIter;

	            cout << callContract->getSymbol()                     << " ";
	            cout << callContract->getExchange()                   << " ";
	            cout << callContract->getExpireDate().getAsString()   << " ";
	            cout << callContract->getStrikePrice();
	            
	            putIter++;
	        }
	        else
	        {
	        }
	        cout << endl;
	        ++i;
        }
    }

} // namespace
