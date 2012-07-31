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

#include <mamda/MamdaOptionContract.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaTradeRecap.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaQuoteRecap.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mamda/MamdaFundamentalHandler.h>
#include <mamda/MamdaFundamentals.h>
#include <mama/mamacpp.h>

static const int MAX_SYMBOL_LEN      = 256;
static const int MAX_PART_ID_LEN     = 5;
static const int MAX_EXPIRE_DATE_LEN = 11;

namespace Wombat
{

    struct MamdaOptionContract::MamdaOptionContractImpl
    {
        MamdaOptionContractImpl();
        char                       mSymbol [MAX_SYMBOL_LEN];
        char                       mExchange [MAX_PART_ID_LEN];
        MamaDateTime               mExpireDate;
        bool                       mGotExpireDate;
        double                     mStrikePrice;
        bool                       mGotStrikePrice;
        MamdaOptionPutCall         mPutCall;
        bool                       mGotPutCall;
        uint32_t                   mOpenInterest;
        bool                       mGotOpenInterest;
        MamdaOptionExerciseStyle   mExerciseStyle;
        bool                       mGotExerciseStyle;
        bool                       mRecapRequired;
        bool                       mInScope;
        vector<MamdaMsgListener*>  mMsgListeners;
        MamdaTradeListener         mTradeListener;
        MamdaQuoteListener         mQuoteListener;
        MamdaFundamentalListener   mFundamentalListener;
        void*                      mCustomObject;
        int64_t                    mSeqNum;
    };

    /* MLS Temporary Hack */
    int64_t MamdaOptionContract::getSeqNum()
    {
        return mImpl.mSeqNum;
    }

    void MamdaOptionContract::setSeqNum (int64_t num)
    {
        mImpl.mSeqNum = num;
    }

    MamdaOptionContract::MamdaOptionContract (
        const char*          symbol,
        const char*          exchange,
        const MamaDateTime&  expireDate,
        double               strikePrice,
        MamdaOptionPutCall   putCall)
        : mImpl (*new MamdaOptionContractImpl)
    {
        snprintf (mImpl.mSymbol,     MAX_SYMBOL_LEN,      symbol);
        snprintf (mImpl.mExchange,   MAX_PART_ID_LEN,     exchange);

        mImpl.mExpireDate        = expireDate;
        mImpl.mGotExpireDate     = true;
        mImpl.mStrikePrice       = strikePrice;
        mImpl.mGotStrikePrice    = true;
        mImpl.mPutCall           = putCall;
        mImpl.mGotPutCall        = true;
        mImpl.mGotOpenInterest   = false;
        mImpl.mGotExerciseStyle  = false;
        mImpl.mRecapRequired     = false;
        mImpl.mSeqNum            = 0;
    }

    MamdaOptionContract::MamdaOptionContract (
        const char*          symbol,
        const char*          exchange)
        : mImpl (*new MamdaOptionContractImpl)
    {
        snprintf (mImpl.mSymbol,     MAX_SYMBOL_LEN,      symbol);
        snprintf (mImpl.mExchange,   MAX_PART_ID_LEN,     exchange);

        mImpl.mExpireDate.clear();
        mImpl.mGotExpireDate     = false;
        mImpl.mStrikePrice       = 0.0;
        mImpl.mGotStrikePrice    = false;
        mImpl.mPutCall           = MAMDA_PUT_CALL_UNKNOWN;
        mImpl.mGotPutCall        = false;
        mImpl.mGotOpenInterest   = false;
        mImpl.mGotExerciseStyle  = false;
        mImpl.mRecapRequired     = false;
        mImpl.mSeqNum            = 0;
    }

    MamdaOptionContract::~MamdaOptionContract ()
    {
        delete &mImpl;
    }

    void MamdaOptionContract::setExpireDate (
        const  MamaDateTime& expireDate)
    {
        mImpl.mExpireDate    = expireDate;
        mImpl.mGotExpireDate = true;
    }

    void MamdaOptionContract::setStrikePrice 
        (double  strikePrice)
    {
        mImpl.mStrikePrice    = strikePrice;
        mImpl.mGotStrikePrice = true;
    }

    void MamdaOptionContract::setPutCall (
        MamdaOptionPutCall  putCall)
    {
        mImpl.mPutCall    = putCall;
        mImpl.mGotPutCall = true;
    }

    void MamdaOptionContract::setOpenInterest (
        uint32_t  openInterest)
    {
        mImpl.mOpenInterest    = openInterest;
        mImpl.mGotOpenInterest = true;
    }

    void MamdaOptionContract::setExerciseStyle (
        MamdaOptionExerciseStyle  exerciseStyle)
    {
        mImpl.mExerciseStyle    = exerciseStyle;
        mImpl.mGotExerciseStyle = true;
    }

    void MamdaOptionContract::setRecapRequired (
        bool  recapRequired)
    {
         mImpl.mRecapRequired = recapRequired;
    }

    const char* MamdaOptionContract::getSymbol () const
    {
        return mImpl.mSymbol;
    }

    const char* MamdaOptionContract::getExchange () const
    {
        return mImpl.mExchange;
    }

    const MamaDateTime& MamdaOptionContract::getExpireDate () const
    {
        return mImpl.mExpireDate;
    }

    const char* MamdaOptionContract::getExpireDateStr () const
    {
        return mImpl.mExpireDate.getAsString();
    }

    bool MamdaOptionContract::gotExpireDate() const
    {
        return mImpl.mGotExpireDate;
    }

    double MamdaOptionContract::getStrikePrice () const
    {
        return mImpl.mStrikePrice;
    }

    bool MamdaOptionContract::gotStrikePrice() const
    {
        return mImpl.mGotStrikePrice;
    }

    MamdaOptionPutCall MamdaOptionContract::getPutCall () const
    {
        return mImpl.mPutCall;
    }

    bool MamdaOptionContract::gotPutCall() const
    {
        return mImpl.mGotPutCall;
    }

    uint32_t MamdaOptionContract::getOpenInterest () const
    {
        return mImpl.mOpenInterest;
    }

    bool MamdaOptionContract::gotOpenInterest() const
    {
        return mImpl.mGotOpenInterest;
    }

    MamdaOptionExerciseStyle MamdaOptionContract::getExerciseStyle () const
    {
        return mImpl.mExerciseStyle;
    }

    bool MamdaOptionContract::gotExerciseStyle() const
    {
        return mImpl.mGotExerciseStyle;
    }

    bool MamdaOptionContract::getRecapRequired() const
    {
        return mImpl.mRecapRequired;
    }

    void MamdaOptionContract::addMsgListener (
        MamdaMsgListener*  listener)
    {
        mImpl.mMsgListeners.push_back (listener);
    }

    void MamdaOptionContract::addTradeHandler (
        MamdaTradeHandler*  handler)
    {
        mImpl.mTradeListener.addHandler (handler);
    }

    void MamdaOptionContract::addQuoteHandler (
        MamdaQuoteHandler*  handler)
    {
        mImpl.mQuoteListener.addHandler (handler);
    }

    void MamdaOptionContract::addFundamentalHandler (
        MamdaFundamentalHandler*  handler)
    {
        mImpl.mFundamentalListener.addHandler (handler);
    }

    void MamdaOptionContract::setCustomObject (void* object)
    {
        mImpl.mCustomObject = object;
    }

    MamdaTradeRecap& MamdaOptionContract::getTradeInfo () const
    {
        return mImpl.mTradeListener;
    }

    MamdaQuoteRecap& MamdaOptionContract::getQuoteInfo () const
    {
        return mImpl.mQuoteListener;
    }

    MamdaFundamentals& MamdaOptionContract::getFundamentalsInfo () const
    {
        return mImpl.mFundamentalListener;
    }

    void* MamdaOptionContract::getCustomObject () const
    {
        return mImpl.mCustomObject;
    }

    vector<MamdaMsgListener*>& MamdaOptionContract::getMsgListeners ()
    {
        return mImpl.mMsgListeners;
    }

    MamdaTradeListener& MamdaOptionContract::getTradeListener ()
    {
        return mImpl.mTradeListener;
    }

    MamdaQuoteListener& MamdaOptionContract::getQuoteListener ()
    {
        return mImpl.mQuoteListener;
    }

    MamdaFundamentalListener& MamdaOptionContract::getFundamentalListener ()
    {
        return mImpl.mFundamentalListener;
    }

    MamdaOptionContract::MamdaOptionContractImpl::MamdaOptionContractImpl()
        : mStrikePrice      (0.0)
        , mPutCall          (MAMDA_PUT_CALL_UNKNOWN)
        , mOpenInterest     (0)
        , mExerciseStyle    (MAMDA_EXERCISE_STYLE_UNKNOWN)
        , mInScope          (false)
        , mCustomObject     (NULL)
    {
    }

} // namespace
