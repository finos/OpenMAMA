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

#include <wombat/port.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mamda/MamdaFundamentalFields.h>
#include <mamda/MamdaFundamentalHandler.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <sstream>
#include <iostream>
#include <string>

using std::string;

namespace Wombat
{

    struct FundamentalFieldUpdate
    {
        virtual void onUpdate (
            MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
            const MamaMsgField&                                      field) = 0;
    };

    class MamdaFundamentalListener::MamdaFundamentalListenerImpl
        : public MamaMsgFieldIterator
    {
    public:
        MamdaFundamentalListenerImpl  (MamdaFundamentalListener&  listener);
        ~MamdaFundamentalListenerImpl () {}

      

        void handleFundamentalMessage     (MamdaSubscription*  subscription,
				                           const MamaMsg&      msg,
				                           short               msgType);

        void handleRecap                  (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void handleUpdate                 (MamdaSubscription*  subscription,
                                           const MamaMsg&      msg);

        void onField                      (const MamaMsg&      msg,
                                           const MamaMsgField& field,
                                           void*               closure);

        void updateFundamentalFields      (const MamaMsg&      msg);
        void updateFieldStates            ();

        string getDividendFrequency       (string  divFreq);
        string getDividendFrequency       (int     divFreq);

        MamdaFundamentalListener&    mListener;
        MamdaFundamentalHandler*     mHandler;
        
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        // Basic Event Fields 
        string        mSymbol;             MamdaFieldState     mSymbolFieldState;      
        bool          mSymbolIsModified;    
        string        mPartId;             MamdaFieldState     mPartIdFieldState; 
        MamaDateTime  mSrcTime;            MamdaFieldState     mSrcTimeFieldState;     
        MamaDateTime  mActTime;            MamdaFieldState     mActivityTimeFieldState;
        MamaDateTime  mLineTime;           MamdaFieldState     mLineTimeFieldState;    
        MamaDateTime  mSendTime;           MamdaFieldState     mSendTimeFieldState;      
        
        // Fundamental Fields 
        // The following fields are used for caching the last reported
        // fundamental equity pricing/analysis attributes, indicators and ratios.
        // The reason for cahcing these values is to allow a configuration that 
        // passes the minimum amount of data  (unchanged fields not sent).
        string      mCorpActType;          MamdaFieldState      mCorpActTypeFieldState;
        double      mDividendPrice;        MamdaFieldState      mDividendPriceFieldState;
        string      mDivFreq;              MamdaFieldState      mDivFreqFieldState;
        string      mDivExDate;            MamdaFieldState      mDivExDateFieldState;
        string      mDivPayDate;           MamdaFieldState      mDivPayDateFieldState;
        string      mDivRecordDate;        MamdaFieldState      mDivRecordDateFieldState;
        string      mDivCurrency;          MamdaFieldState      mDivCurrencyFieldState;
        int32_t     mSharesOut;            MamdaFieldState      mSharesOutFieldState;
        int32_t     mSharesFloat;          MamdaFieldState      mSharesFloatFieldState;
        int32_t     mSharesAuth;           MamdaFieldState      mSharesAuthFieldState;
        double      mEarnPerShare;         MamdaFieldState      mEarnPerShareFieldState;
        double      mVolatility;           MamdaFieldState      mVolatilityFieldState;
        double      mPeRatio;              MamdaFieldState      mPeRatioFieldState;
        double      mYield;                MamdaFieldState      mYieldFieldState;
        string      mMrktSegmNative;       MamdaFieldState      mMrktSegmNativeFieldState;
        string      mMrktSectNative;       MamdaFieldState      mMrktSectNativeFieldState;
        string      mMarketSegment;        MamdaFieldState      mMarketSegmentFieldState;
        string      mMarketSector;         MamdaFieldState      mMarketSectorFieldState;
        double      mHistVolatility;       MamdaFieldState      mHistVolatilityFieldState;
        double      mRiskFreeRate;         MamdaFieldState      mRiskFreeRateFieldState;

        static void initFieldUpdaters ();
        static void initFieldUpdater  (const MamaFieldDescriptor*  fieldDesc,
                                       FundamentalFieldUpdate*     updater);

        static void updateField (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                                 const MamaMsgField&                                      field);

        static FundamentalFieldUpdate**  mFieldUpdaters;
        static volatile uint16_t         mFieldUpdatersSize;
        static wthread_static_mutex_t           mFundamentalFieldUpdaterLockMutex;
        static bool                      mUpdatersComplete;

        struct FieldUpdateSrcTime;
        struct FieldUpdateActTime;
        struct FieldUpdateLineTime;
        struct FieldUpdateCorpActType;
        struct FieldUpdateDividendPrice;
        struct FieldUpdateDivFreq;
        struct FieldUpdateDivExDate;
        struct FieldUpdateDivPayDate;
        struct FieldUpdateDivRecordDate;
        struct FieldUpdateDivCurrency;
        struct FieldUpdateSharesOut;
        struct FieldUpdateSharesFloat;
        struct FieldUpdateSharesAuth;
        struct FieldUpdateEarnPerShare;
        struct FieldUpdateVolatility;
        struct FieldUpdatePeRatio;
        struct FieldUpdateYield;
        struct FieldUpdateMrktSegmNative;
        struct FieldUpdateMrktSectNative;
        struct FieldUpdateMarketSegment;
        struct FieldUpdateMarketSector;
        struct FieldUpdateHistVolatility;
        struct FieldUpdateRiskFreeRate;
    };

    MamdaFundamentalListener::MamdaFundamentalListener ()
        : mImpl (*new MamdaFundamentalListenerImpl(*this))
    {
    }

    MamdaFundamentalListener::~MamdaFundamentalListener()
    {
	    /* Do not call wthread_mutex_destroy for the FieldUpdaterLockMutex here.  
	       If we do, it will not be initialized again if another listener is created 
	       after the first is destroyed. */
        /* wthread_mutex_destroy (&mImpl.mFundamentalFieldUpdaterLockMutex); */
        delete &mImpl;
    }

    void MamdaFundamentalListener::addHandler (MamdaFundamentalHandler*  handler)
    {
        mImpl.mHandler = handler;
    }

    const char* MamdaFundamentalListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaFundamentalListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaFundamentalListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaFundamentalListener::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaFundamentalListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaFundamentalListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const char* MamdaFundamentalListener::getCorporateActionType() const
    {
        return mImpl.mCorpActType.c_str();
    }

    double MamdaFundamentalListener::getDividendPrice() const
    {
        return mImpl.mDividendPrice;
    }

    const char* MamdaFundamentalListener::getDividendFrequency() const
    {
        return mImpl.mDivFreq.c_str();
    }

    const char* MamdaFundamentalListener::getDividendExDate() const
    {
        return mImpl.mDivExDate.c_str();
    }

    const char* MamdaFundamentalListener::getDividendPayDate() const
    {
        return mImpl.mDivPayDate.c_str();
    }

    const char* MamdaFundamentalListener::getDividendRecordDate() const
    {
        return mImpl.mDivRecordDate.c_str();
    }

    const char* MamdaFundamentalListener::getDividendCurrency() const
    {
        return mImpl.mDivCurrency.c_str();
    }

    long MamdaFundamentalListener::getSharesOut() const
    {
        return mImpl.mSharesOut;
    }

    long MamdaFundamentalListener::getSharesFloat() const
    {
        return mImpl.mSharesFloat;
    }

    long MamdaFundamentalListener::getSharesAuthorized() const
    {
        return mImpl.mSharesAuth;
    }

    double MamdaFundamentalListener::getEarningsPerShare() const
    {
        return mImpl.mEarnPerShare;
    }

    double MamdaFundamentalListener::getVolatility() const
    {
        return mImpl.mVolatility;
    }

    double MamdaFundamentalListener::getPriceEarningsRatio() const
    {
        return mImpl.mPeRatio;
    }

    double MamdaFundamentalListener::getYield() const
    {
        return mImpl.mYield;
    }

    const char* MamdaFundamentalListener::getMarketSegmentNative() const
    {
        return mImpl.mMrktSegmNative.c_str();
    }

    const char* MamdaFundamentalListener::getMarketSectorNative() const
    {
        return mImpl.mMrktSectNative.c_str();
    }

    const char* MamdaFundamentalListener::getMarketSegment() const
    {
        return mImpl.mMarketSegment.c_str();
    }

    const char* MamdaFundamentalListener::getMarketSector() const
    {
        return mImpl.mMarketSector.c_str();
    }

    double MamdaFundamentalListener::getHistoricalVolatility() const
    {
        return mImpl.mHistVolatility;
    }

    double MamdaFundamentalListener::getRiskFreeRate() const
    {
        return mImpl.mRiskFreeRate;
    }

    /*      FieldAccessors      */
    MamdaFieldState MamdaFundamentalListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getActivityTimeFieldState() const
    {
        return mImpl.mActivityTimeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getCorporateActionTypeFieldState() const
    {
        return mImpl.mCorpActTypeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendPriceFieldState() const
    {
        return mImpl.mDividendPriceFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendFrequencyFieldState() const
    {
        return mImpl.mDivFreqFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendExDateFieldState() const
    {
        return mImpl.mDivExDateFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendPayDateFieldState() const
    {
        return mImpl.mDivPayDateFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendRecordDateFieldState() const
    {
        return mImpl.mDivRecordDateFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getDividendCurrencyFieldState() const
    {
        return mImpl.mDivCurrencyFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getSharesOutFieldState() const
    {
        return mImpl.mSharesOutFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getSharesFloatFieldState() const
    {
        return mImpl.mSharesFloatFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getSharesAuthorizedFieldState() const
    {
        return mImpl.mSharesAuthFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getEarningsPerShareFieldState() const
    {
        return mImpl.mEarnPerShareFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getVolatilityFieldState() const
    {
        return mImpl.mVolatilityFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getPriceEarningsRatioFieldState() const
    {
        return mImpl.mPeRatioFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getYieldFieldState() const
    {
        return mImpl.mYieldFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getMarketSegmentNativeFieldState() const
    {
        return mImpl.mMrktSegmNativeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getMarketSectorNativeFieldState() const
    {
        return mImpl.mMrktSectNativeFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getMarketSegmentFieldState() const
    {
        return mImpl.mMarketSegmentFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getMarketSectorFieldState() const
    {
        return mImpl.mMarketSectorFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getHistoricalVolatilityFieldState() const
    {
        return mImpl.mHistVolatilityFieldState;
    }

    MamdaFieldState MamdaFundamentalListener::getRiskFreeRateFieldState() const
    {
        return mImpl.mRiskFreeRateFieldState;
    }
    /*      End FieldState Accessors    */


    void MamdaFundamentalListener::onMsg (
        MamdaSubscription* subscription,
        const MamaMsg&     msg,
        short              msgType)
    { 
        // If msg is a trade-related message, invoke the
        // appropriate callback:
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";
            msg.tryString (MamdaCommonFields::ISSUE_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaFundamentalListener (%s.%s(%s)) onMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        switch (msgType)
        {
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
        case MAMA_MSG_TYPE_UPDATE:
            mImpl.handleFundamentalMessage (subscription, msg, msgType);
            break;
        }
    }




    MamdaFundamentalListener::MamdaFundamentalListenerImpl::MamdaFundamentalListenerImpl(
        MamdaFundamentalListener&  listener)
        : mListener       (listener)
        , mHandler        (NULL)
        , mDividendPrice  (0.0)
        , mSharesOut      (0)
        , mSharesFloat    (0)
        , mSharesAuth     (0)
        , mEarnPerShare   (0.0)
        , mVolatility     (0.0)
        , mPeRatio        (0.0)
        , mYield          (0.0)
        , mHistVolatility (0.0)
        , mRiskFreeRate   (0.0)
    {
    }


    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::handleFundamentalMessage (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        // Ensure that the field handling is set up (once for all
        // MamdaFundamentalListener instances).
        if (!mUpdatersComplete)
        {
            wthread_static_mutex_lock (&mFundamentalFieldUpdaterLockMutex);

            if (!mUpdatersComplete)
            {
                if (!MamdaFundamentalFields::isSet())
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                               "MamdaFundamentalListener: MamdaFundamentalFields::setDictionary() "
                               "has not been called.");
                     wthread_static_mutex_unlock (&mFundamentalFieldUpdaterLockMutex);
                     return;
                }

                try
                {
                    initFieldUpdaters ();
                }
                catch (MamaStatus &e)
                {
                    mama_log (MAMA_LOG_LEVEL_WARN,
                              "MamdaFundamentalListener: Could not set field updaters: %s",
                              e.toString ());
                    wthread_static_mutex_unlock (&mFundamentalFieldUpdaterLockMutex);
                    return;
                }
                mUpdatersComplete = true;
            }

            wthread_static_mutex_unlock (&mFundamentalFieldUpdaterLockMutex);
        }

        // Handle fields in message:
        updateFieldStates       ();
        updateFundamentalFields (msg);

        // Handle according to message type:
        switch (msgType)
        {
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
            handleRecap (subscription, msg);
            break;
        case MAMA_MSG_TYPE_UPDATE:
            handleUpdate (subscription, msg);
            break;
        }
    }

    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        // should probably check if cache has changed 
        // prior to calling any handlers
        if (mHandler)
        {
            mHandler->onFundamentals (subscription, 
                                      mListener, 
                                      msg, 
                                      mListener);
        }
    }


    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        // should probably check if cache has changed 
        // prior to calling any handlers
        if (mHandler)
        {
            mHandler->onFundamentals (subscription, 
                                      mListener, 
                                      msg, 
                                      mListener);
        }
    }

    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::updateFieldStates()
    {
      if (mSymbolFieldState == MODIFIED)    
        mSymbolFieldState = NOT_MODIFIED; 

      if (mPartIdFieldState == MODIFIED)   
        mPartIdFieldState = NOT_MODIFIED;

      if (mSrcTimeFieldState == MODIFIED)    
        mSrcTimeFieldState = NOT_MODIFIED;

      if (mActivityTimeFieldState == MODIFIED)
        mActivityTimeFieldState = NOT_MODIFIED;

      if (mLineTimeFieldState == MODIFIED)     
        mLineTimeFieldState = NOT_MODIFIED;  

      if (mSendTimeFieldState == MODIFIED)    
        mSendTimeFieldState = NOT_MODIFIED; 
      
      if (mCorpActTypeFieldState == MODIFIED)    
        mCorpActTypeFieldState = NOT_MODIFIED;

      if (mDividendPriceFieldState == MODIFIED)
        mDividendPriceFieldState = NOT_MODIFIED;

      if (mDivFreqFieldState == MODIFIED)        
        mDivFreqFieldState = NOT_MODIFIED;

      if (mDivExDateFieldState == MODIFIED)
        mDivExDateFieldState = NOT_MODIFIED;

      if (mDivPayDateFieldState == MODIFIED) 
        mDivPayDateFieldState = NOT_MODIFIED;

      if (mDivRecordDateFieldState == MODIFIED)
        mDivRecordDateFieldState = NOT_MODIFIED;

      if (mDivCurrencyFieldState == MODIFIED)    
        mDivCurrencyFieldState = NOT_MODIFIED;

      if (mSharesOutFieldState == MODIFIED)    
        mSharesOutFieldState = NOT_MODIFIED;

      if (mSharesFloatFieldState == MODIFIED)
        mSharesFloatFieldState = NOT_MODIFIED;

      if (mSharesAuthFieldState == MODIFIED)   
        mSharesAuthFieldState = NOT_MODIFIED;

      if (mEarnPerShareFieldState == MODIFIED)
        mEarnPerShareFieldState = NOT_MODIFIED;

      if (mVolatilityFieldState == MODIFIED)    
        mVolatilityFieldState = NOT_MODIFIED;

      if (mPeRatioFieldState == MODIFIED)     
        mPeRatioFieldState = NOT_MODIFIED;

      if (mYieldFieldState == MODIFIED)    
        mYieldFieldState = NOT_MODIFIED;

      if (mMrktSegmNativeFieldState == MODIFIED) 
        mMrktSegmNativeFieldState = NOT_MODIFIED;

      if (mMrktSectNativeFieldState == MODIFIED) 
        mMrktSectNativeFieldState = NOT_MODIFIED;

      if (mMarketSegmentFieldState == MODIFIED)  
        mMarketSegmentFieldState = NOT_MODIFIED;

      if (mMarketSectorFieldState == MODIFIED)   
        mMarketSectorFieldState = NOT_MODIFIED;

      if (mHistVolatilityFieldState == MODIFIED)
        mHistVolatilityFieldState = NOT_MODIFIED;

      if (mRiskFreeRateFieldState == MODIFIED)   
        mRiskFreeRateFieldState = NOT_MODIFIED;
    } 
      
    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::updateFundamentalFields (
        const MamaMsg&  msg)
    { 
        const char* symbol = NULL;
        const char* partId = NULL;
      
        getSymbolAndPartId (msg, symbol, partId);

        if (symbol) 
        {
          mSymbol = symbol;
          mSymbolFieldState = MODIFIED;
        }
        if (partId)
        {
          mPartId = partId;
          mPartIdFieldState = MODIFIED;
        }

        // Iterate over all of the fields in the message.
        msg.iterateFields (*this, NULL, NULL);
    }

    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::onField (
        const MamaMsg&       msg,
        const MamaMsgField&  field,
        void*                closure)
    {
        uint16_t fid = field.getFid();
        if (fid <= mFieldUpdatersSize)
        {
            FundamentalFieldUpdate* updater = mFieldUpdaters[fid];
            if (updater)
            {
                updater->onUpdate (*this, field);
            }
        }
    }

    string MamdaFundamentalListener::MamdaFundamentalListenerImpl::getDividendFrequency (
        int  divFreq)
    {
        switch(divFreq)
        {
            case 0:
                return MamdaFundamentalFields::DIV_FREQ_NONE;
                break;
            case 1:
                return MamdaFundamentalFields::DIV_FREQ_MONTHLY;
                break;
            case 3:
                return MamdaFundamentalFields::DIV_FREQ_QUARTERLY;
                break;
            case 6:
                return MamdaFundamentalFields::DIV_FREQ_SEMI_ANNUALLY;
                break;
            case 12:
                return MamdaFundamentalFields::DIV_FREQ_ANNUALLY;
                break;
            case 13:
                return MamdaFundamentalFields::DIV_FREQ_SPECIAL;
                break;
            case 14:
                return MamdaFundamentalFields::DIV_FREQ_IRREGULAR;
                break;
            case 99:
                return MamdaFundamentalFields::DIV_FREQ_INVALID;
                break;
            default:
                ostringstream stringbuilder;
                stringbuilder << divFreq;
                return stringbuilder.str();
        }
    }

    string MamdaFundamentalListener::MamdaFundamentalListenerImpl::getDividendFrequency (
        string  divFreq)
    {
        string rtnString;

        if (divFreq.compare("0") == 0)
        { 
            // No dividend payments are made
            rtnString = MamdaFundamentalFields::DIV_FREQ_NONE;
        }
        else  if (divFreq.compare("1") == 0)
        { 
            // Monthly
            rtnString = MamdaFundamentalFields::DIV_FREQ_MONTHLY;
        }
        else if (divFreq.compare("3") == 0)
        { 
            // Quarterly
            rtnString = MamdaFundamentalFields::DIV_FREQ_QUARTERLY;
        }
        else if (divFreq.compare("6") == 0)
        { 
            // Semi-Annually
            rtnString = MamdaFundamentalFields::DIV_FREQ_SEMI_ANNUALLY;
        }
        else if (divFreq.compare("12") == 0)
        { 
            // Annually
            rtnString = MamdaFundamentalFields::DIV_FREQ_ANNUALLY;
        }
        else if (divFreq.compare("13") == 0)
        { 
            // Payment frequency is special.
            rtnString = MamdaFundamentalFields::DIV_FREQ_SPECIAL;
        }
        else if (divFreq.compare("14") == 0)
        { 
            // Irregular payment frequency.
            rtnString = MamdaFundamentalFields::DIV_FREQ_IRREGULAR;
        }
        else if (divFreq.compare("99") == 0)
        { 
            // Invalid dividend frequency period.
            rtnString = MamdaFundamentalFields::DIV_FREQ_INVALID;
        }
        else
        {
            // Already a string;
            rtnString = divFreq;
        }

        return rtnString;
    }

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateSrcTime
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            field.getDateTime (impl.mSrcTime);
            impl.mSrcTimeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateActTime
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            field.getDateTime (impl.mActTime);
            impl.mActivityTimeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateLineTime
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            field.getDateTime (impl.mLineTime);
            impl.mLineTimeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateCorpActType
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mCorpActType = field.getString();
            impl.mCorpActTypeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDividendPrice
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mDividendPrice = field.getF64();
            impl.mDividendPriceFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDivFreq
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    impl.mDivFreq = impl.getDividendFrequency (field.getU32());
                    impl.mDivFreqFieldState = MODIFIED;
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    impl.mDivFreq = impl.getDividendFrequency (field.getString());
                    impl.mDivFreqFieldState = MODIFIED;
                    break;
                default:
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wDivFreq.  "
                              "Expected string or integer.", 
                              field.getType());
                    break;
            }
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDivExDate
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mDivExDate = field.getString();
            impl.mDivExDateFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDivPayDate
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mDivPayDate = field.getString();
            impl.mDivPayDateFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDivRecordDate
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mDivRecordDate = field.getString();
            impl.mDivRecordDateFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateDivCurrency
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mDivCurrency = field.getString();
            impl.mDivCurrencyFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateSharesOut
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mSharesOut = field.getI32();
            impl.mSharesOutFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateSharesFloat
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mSharesFloat = field.getI32();
            impl.mSharesFloatFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateSharesAuth
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mSharesAuth = field.getI32();
            impl.mSharesAuthFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateEarnPerShare
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mEarnPerShare = field.getF64();
            impl.mEarnPerShareFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateVolatility
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mVolatility = field.getF64();
            impl.mVolatilityFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdatePeRatio
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mPeRatio = field.getF64();
            impl.mPeRatioFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateYield
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mYield = field.getF64();
            impl.mYieldFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateMrktSegmNative
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mMrktSegmNative = field.getString();
            impl.mMrktSegmNativeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateMrktSectNative
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mMrktSectNative = field.getString();
            impl.mMrktSectNativeFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateMarketSegment
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                        const MamaMsgField&                                     field)
        {

           const char* marketSegment;
           int segment;

            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    segment = field.getU32();
                    if (0 == segment)
                    {
                        impl.mMarketSegment = MamdaFundamentalFields::MRKT_SEGMENT_NONE;
                        impl.mMarketSegmentFieldState = MODIFIED;
                    }
                    else
                    {
                        ostringstream stringbuilder;
                        stringbuilder << segment;
                        impl.mMarketSegment = stringbuilder.str();
                        impl.mMarketSegmentFieldState = MODIFIED;
                    }
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    marketSegment = field.getString();
                    if (strcmp (marketSegment, "0") == 0)
                    {
                        // No market segment is known/available for this security
                        impl.mMarketSegment = MamdaFundamentalFields::MRKT_SEGMENT_NONE;
                        impl.mMarketSegmentFieldState = MODIFIED;
                    }
                    else
                    {
                        // Already a string;
                        impl.mMarketSegment = marketSegment;
                        impl.mMarketSegmentFieldState = MODIFIED;
                    }
                    break;
                default:
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wMarketSegment.  "
                              "Expected string or integer.", 
                              field.getType());
                    break;
            }
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateMarketSector
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                        const MamaMsgField&                                     field)
        {
            const char* marketSector;
            int sector;

            switch (field.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    sector = field.getU32();
                    if (0 == sector)
                    {
                        impl.mMarketSector = MamdaFundamentalFields::MRKT_SECTOR_NONE;
                        impl.mMarketSectorFieldState = MODIFIED;
                    }
                    else
                    {
                        ostringstream stringbuilder;
                        stringbuilder << sector;
                        impl.mMarketSector = stringbuilder.str();
                        impl.mMarketSectorFieldState = MODIFIED;
                    }        
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    marketSector = field.getString();
                    if (strcmp(marketSector,"0") == 0)
                    {
                        // No market sector is known/available for this security
                        impl.mMarketSector = MamdaFundamentalFields::MRKT_SECTOR_NONE;
                        impl.mMarketSectorFieldState = MODIFIED;
                    }
                    else
                    {
                        // Already a string;
                        impl.mMarketSector = marketSector;
                        impl.mMarketSectorFieldState = MODIFIED;
                    }
                    break;
                default:
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wMarketSector.  "
                              "Expected string or integer.", 
                              field.getType());
            }
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateHistVolatility
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mHistVolatility = field.getF64();
            impl.mHistVolatilityFieldState = MODIFIED;
        }
    };

    struct MamdaFundamentalListener::MamdaFundamentalListenerImpl::FieldUpdateRiskFreeRate
        : public FundamentalFieldUpdate
    {
        void onUpdate (MamdaFundamentalListener::MamdaFundamentalListenerImpl&  impl,
                       const MamaMsgField&                                      field)
        {
            impl.mRiskFreeRate = field.getF64();
            impl.mRiskFreeRateFieldState = MODIFIED;
        }
    };


    FundamentalFieldUpdate** MamdaFundamentalListener::
        MamdaFundamentalListenerImpl::mFieldUpdaters = NULL;

    volatile uint16_t MamdaFundamentalListener::
        MamdaFundamentalListenerImpl::mFieldUpdatersSize = 0;

    wthread_static_mutex_t MamdaFundamentalListener::MamdaFundamentalListenerImpl::
        mFundamentalFieldUpdaterLockMutex = WSTATIC_MUTEX_INITIALIZER;

    bool MamdaFundamentalListener::
        MamdaFundamentalListenerImpl::mUpdatersComplete = false;


    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::initFieldUpdaters ()
    {
        if (!mFieldUpdaters)
        {
            mFieldUpdaters = 
                new FundamentalFieldUpdate* [MamdaFundamentalFields::getMaxFid() +1];

            mFieldUpdatersSize = MamdaFundamentalFields::getMaxFid();

            /* Use uint32_t instead of uint16_t to avoid infinite loop if max FID = 65535 */
            for (uint32_t i = 0; i <= mFieldUpdatersSize; ++i)
            {
                mFieldUpdaters[i] = NULL;
            }
        }

        initFieldUpdater (MamdaCommonFields::SRC_TIME,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateSrcTime);

        initFieldUpdater (MamdaCommonFields::ACTIVITY_TIME,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateActTime);

        initFieldUpdater (MamdaCommonFields::LINE_TIME,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateLineTime);

        initFieldUpdater (MamdaFundamentalFields::CORP_ACT_TYPE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateCorpActType);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_PRICE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDividendPrice);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_FREQ,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDivFreq);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_EX_DATE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDivExDate);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_PAY_DATE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDivPayDate);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_REC_DATE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDivRecordDate);

        initFieldUpdater (MamdaFundamentalFields::DIVIDEND_CURRENCY,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateDivCurrency);

        initFieldUpdater (MamdaFundamentalFields::SHARES_OUT,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateSharesOut);

        initFieldUpdater (MamdaFundamentalFields::SHARES_FLOAT,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateSharesFloat);

        initFieldUpdater (MamdaFundamentalFields::SHARES_AUTH,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateSharesAuth);

        initFieldUpdater (MamdaFundamentalFields::EARN_PER_SHARE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateEarnPerShare);

        initFieldUpdater (MamdaFundamentalFields::VOLATILITY,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateVolatility);

        initFieldUpdater (MamdaFundamentalFields::PRICE_EARN_RATIO,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdatePeRatio);

        initFieldUpdater (MamdaFundamentalFields::YIELD,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateYield);

        initFieldUpdater (MamdaFundamentalFields::MRKT_SEGM_NATIVE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateMrktSegmNative);

        initFieldUpdater (MamdaFundamentalFields::MRKT_SECT_NATIVE,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateMrktSectNative);

        initFieldUpdater (MamdaFundamentalFields::MRKT_SEGMENT,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateMarketSegment);

        initFieldUpdater (MamdaFundamentalFields::MRKT_SECTOR,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateMarketSector);

        initFieldUpdater (MamdaFundamentalFields::RISK_FREE_RATE,
                          new MamdaFundamentalListener::
	                      MamdaFundamentalListenerImpl::FieldUpdateHistVolatility);

        initFieldUpdater (MamdaFundamentalFields::HIST_VOLATILITY,
                          new MamdaFundamentalListener::
		                  MamdaFundamentalListenerImpl::FieldUpdateRiskFreeRate);
    }

    void MamdaFundamentalListener::MamdaFundamentalListenerImpl::initFieldUpdater (
        const MamaFieldDescriptor*  fieldDesc,
        FundamentalFieldUpdate*     updater)
    {
        if (!fieldDesc)
            return;

        uint16_t fid = fieldDesc->getFid();
        if (fid <= mFieldUpdatersSize)
        {
            mFieldUpdaters[fid] = updater;
        }
    }

} // namespace
