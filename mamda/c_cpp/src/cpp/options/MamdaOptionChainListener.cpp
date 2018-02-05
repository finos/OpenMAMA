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

#include <mamda/MamdaOptionChainListener.h>
#include <mamda/MamdaOptionChainHandler.h>
#include <mamda/MamdaOptionChain.h>
#include <mamda/MamdaOptionContract.h>
#include <mamda/MamdaOptionFields.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaQuoteListener.h>
#include <mamda/MamdaTradeListener.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mamda/MamdaDataException.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

using std::vector;
using std::string;

namespace Wombat
{

    class MamdaOptionChainListener::MamdaOptionChainListenerImpl
    {
    public:
        MamdaOptionChainListenerImpl (MamdaOptionChainListener& self);
        ~MamdaOptionChainListenerImpl ();

        void onMsg (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleMsg (
            MamdaOptionContract&  contract,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleQuoteMsg (
            MamdaOptionContract&  contract,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleTradeMsg (
            MamdaOptionContract&  contract,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        void handleFundamentalMsg (
            MamdaOptionContract&  contract,
            MamdaSubscription*    subscription,
            const MamaMsg&        msg,
            short                 msgType);

        /*!
         * \brief 
         * \throw<MamdaDataException> \n
         * findContract return an option contract (creating it if necessary).
         */
        MamdaOptionContract&  findContract (
            MamdaSubscription*    subscription,
            const MamaMsg&        msg, 
            short                 msgType);

        bool getExpireDate(
            const MamaMsg&        msg,
            const char*           fullSymbol,
	        MamaDateTime&         expireDate);

        bool getStrikePrice(
            const MamaMsg&        msg,
            const char*           fullSymbol,
	        double&               strikePrice);

        bool getPutCall(
            const MamaMsg&        msg,
            const char*           fullSymbol,
	        MamdaOptionPutCall&   putCall);

        bool getOpenInterest(
	        const MamaMsg&            msg,
            const char*               fullSymbol,
	        uint32_t&                 openInterest);

        bool getExerciseStyle(
	        const MamaMsg&            msg,
            const char*               fullSymbol,
	        MamdaOptionExerciseStyle& exerciseStyle);

        void getSymbolAndExchange(
            string                    fullSymbol,
	        string&                   symbol,
	        string&                   exchange);

        void    updateFieldStates();
        
        MamdaOptionChainListener&         mSelf;
        MamdaOptionChain*                 mChain;
        bool                              mOwnChain;
        vector<MamdaOptionChainHandler*>  mHandlers;
        string                            mSymbol;
        string                            mPartId;
        MamaDateTime                      mSrcTime;
        MamaDateTime                      mActivityTime;
        MamaDateTime                      mLineTime;
        MamaDateTime                      mSendTime;
        MamaMsgQual                       mMsgQual;
        int64_t                           mEventSeqNum;
        MamaDateTime                      mEventTime;
        MamdaOptionContract*              mLastActionContract;
        char                              mLastAction;
        bool                              mExpireDateManditory;
        bool                              mStrikePriceManditory;
        bool                              mPutCallManditory;
        vector<MamdaOptionContract*>      mContracts;
        MamaMsgField                      mTmpField;
        
        MamdaFieldState     mSymbolFieldState;      
        MamdaFieldState     mPartIdFieldState;      
        MamdaFieldState     mEventSeqNumFieldState; 
        MamdaFieldState     mEventTimeFieldState;   
        MamdaFieldState     mSrcTimeFieldState;     
        MamdaFieldState     mActivityTimeFieldState;
        MamdaFieldState     mLineTimeFieldState;    
        MamdaFieldState     mSendTimeFieldState;    
        MamdaFieldState     mMsgQualFieldState;     
    };


    MamdaOptionChainListener::MamdaOptionChainListener (
        const char*  underlyingSymbol)
        : mImpl (*new MamdaOptionChainListenerImpl(*this))
    {
        mImpl.mChain    = new MamdaOptionChain(underlyingSymbol);
        mImpl.mOwnChain = true;
    }

    MamdaOptionChainListener::MamdaOptionChainListener (
        MamdaOptionChain*  chain)
        : mImpl (*new MamdaOptionChainListenerImpl(*this))
    {
        mImpl.mChain    = chain;
        mImpl.mOwnChain = false;
    }

    MamdaOptionChainListener::~MamdaOptionChainListener ()
    {
        delete &mImpl;
    }

    void MamdaOptionChainListener::setManditoryFields (
        bool  expireDate, 
        bool  strikePrice, 
        bool  putCall)
    {
      mImpl.mExpireDateManditory  =  expireDate;
      mImpl.mStrikePriceManditory =  strikePrice;
      mImpl.mPutCallManditory     =  putCall;
    }

    void MamdaOptionChainListener::addHandler (
        MamdaOptionChainHandler*  handler)
    {
        mImpl.mHandlers.push_back(handler);
    }

    MamdaOptionChain& MamdaOptionChainListener::getOptionChain ()
    {
        assert(mImpl.mChain!=NULL);
        return *mImpl.mChain;
    }

    void MamdaOptionChainListener::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        mImpl.onMsg (subscription, msg, msgType);
    }

    const char* MamdaOptionChainListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaOptionChainListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaOptionChainListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaOptionChainListener::getActivityTime() const
    {
        return mImpl.mActivityTime;
    }

    const MamaDateTime& MamdaOptionChainListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaOptionChainListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const MamaMsgQual& MamdaOptionChainListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    mama_seqnum_t MamdaOptionChainListener::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime& MamdaOptionChainListener::getEventTime() const
    {
        return mImpl.mEventTime;
    }

    MamdaOptionContract* MamdaOptionChainListener::getOptionContract() const
    {
        return mImpl.mLastActionContract;
    }

    char MamdaOptionChainListener::getOptionAction() const
    {
        return mImpl.mLastAction;
    }

    /*      FieldAccessors      */
    MamdaFieldState MamdaOptionChainListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState  MamdaOptionChainListener::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getActivityTimeFieldState() const
    {
        return mImpl.mActivityTimeFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaOptionChainListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    /*  End FieldAccessors  */

    
    MamdaOptionChainListener::MamdaOptionChainListenerImpl::MamdaOptionChainListenerImpl(
        MamdaOptionChainListener&  self)
        : mSelf                 (self)
        , mChain                (NULL)
        , mOwnChain             (false)
        , mEventSeqNum          (0)
        , mLastActionContract   (NULL)
        , mLastAction           (ACTION_UNKNOWN)
        , mExpireDateManditory  (true)
        , mStrikePriceManditory (true)
        , mPutCallManditory     (true)
    {
    }

    MamdaOptionChainListener::MamdaOptionChainListenerImpl::~MamdaOptionChainListenerImpl()
    {
        if (mOwnChain)
            delete mChain;

        vector<MamdaOptionContract*>::iterator iter;

        for (iter = mContracts.begin(); iter != mContracts.end(); ++iter)
        {   
            delete *iter;
        }
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::onMsg (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {

        if (!MamdaOptionFields::isSet())
        {
             mama_log (MAMA_LOG_LEVEL_WARN,
                       "MamdaOptionChainListener: MamdaOptionFields::setDictionary() "
                       "has not been called");
             return;
        }

        if (msgType == MAMA_MSG_TYPE_END_OF_INITIALS)
        {
            if(subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
            {
                const char *contractSymbol = "N/A";
                msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);
                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                              "MamdaOptionChainListener (%s.%s(%s)) onMsg(). "
                              "msg type: %s msg status %s. (END_OF_INITIALS)\n",
                              subscription->getSource (),
                              subscription->getSymbol (),
                              contractSymbol,
                              msg.getMsgTypeName (),
                              msg.getMsgStatusString ());
            }

            vector<MamdaOptionChainHandler*>::iterator i;

            for (i = mHandlers.begin(); i != mHandlers.end(); ++i)
            {
                (*i)->onOptionChainRecap (subscription, 
                                          mSelf, 
                                          msg,
                                          *mChain);
            }
            return;
        }

        try
        {
            if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
            {
                const char *contractSymbol = "N/A";

                msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);

                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                               "MamdaOptionChainListener (%s.%s(%s)) onMsg(). "
                               "msg type: %s msg status %s\n",
                               subscription->getSource (),
                               subscription->getSymbol (),
                               contractSymbol,
                               msg.getMsgTypeName (),
                               msg.getMsgStatusString ());
            }

            MamdaOptionContract& contract = findContract (subscription, 
                                                          msg, 
                                                          msgType);
            int64_t itemSeqNum = 0;
            msg.tryI64 (MamaFieldItemSeqNum.mName, 
                        MamaFieldItemSeqNum.mFid, 
                        itemSeqNum);
            
            updateFieldStates();

            handleMsg (contract, 
                       subscription, 
                       msg, 
                       msgType);

            switch (msgType)
            {
                case MAMA_MSG_TYPE_INITIAL:
                case MAMA_MSG_TYPE_RECAP:
                    if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
                    {
                        const char *contractSymbol = "N/A";
                        msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, 
			                           contractSymbol);
                        mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                       "MamdaOptionChainListener (%s.%s(%s)) onMsg(). "
                                       "msg type: %s msg status %s RECAP!!!\n",
                                       subscription->getSource (),
                                       subscription->getSymbol (),
                                       contractSymbol,
                                       msg.getMsgTypeName (),
                                       msg.getMsgStatusString ());
                    }
                    contract.setSeqNum (itemSeqNum);
                    handleQuoteMsg (contract, subscription, msg, msgType);
                    handleTradeMsg (contract, subscription, msg, msgType);
                    handleFundamentalMsg (contract, subscription, msg, msgType);
                    break;

                case MAMA_MSG_TYPE_UPDATE:
                    handleQuoteMsg (contract, subscription, msg, msgType);
                    handleTradeMsg (contract, subscription, msg, msgType);
                    handleFundamentalMsg (contract, subscription, msg, msgType);
                    break;

                case MAMA_MSG_TYPE_QUOTE:
                    handleQuoteMsg (contract, subscription, msg, msgType);
                    break;

                case MAMA_MSG_TYPE_TRADE:
                case MAMA_MSG_TYPE_CANCEL:
                case MAMA_MSG_TYPE_ERROR:
                case MAMA_MSG_TYPE_CORRECTION:
                case MAMA_MSG_TYPE_CLOSING:
                    handleTradeMsg (contract, subscription, msg, msgType);
                    break;
            }
        }
        catch (MamdaDataException& e)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MamdaOptionChainListener: caught MamdaDataException: %s",
                      e.what());
        }
        catch (MamaStatus& e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaOptionChainListener: caught MamaStatus: %s",
                      e.toString());
        }
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::updateFieldStates()
    {
        if (mSymbolFieldState == MODIFIED)    
            mSymbolFieldState = NOT_MODIFIED; 
        if (mPartIdFieldState == MODIFIED)   
            mPartIdFieldState = NOT_MODIFIED;
        if (mEventSeqNumFieldState == MODIFIED)    
            mEventSeqNumFieldState = NOT_MODIFIED; 
        if (mEventTimeFieldState == MODIFIED)   
            mEventTimeFieldState = NOT_MODIFIED;
        if (mSrcTimeFieldState == MODIFIED)    
            mSrcTimeFieldState = NOT_MODIFIED;
        if (mActivityTimeFieldState == MODIFIED)
            mActivityTimeFieldState = NOT_MODIFIED;
        if (mLineTimeFieldState == MODIFIED)     
            mLineTimeFieldState = NOT_MODIFIED;  
        if (mSendTimeFieldState == MODIFIED)    
            mSendTimeFieldState = NOT_MODIFIED; 
        if (mMsgQualFieldState == MODIFIED)   
            mMsgQualFieldState = NOT_MODIFIED; 
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::handleMsg (
        MamdaOptionContract&  contract,
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOptionChainListener (%s.%s(%s)) handleMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        vector<MamdaMsgListener*>& msgListeners = contract.getMsgListeners();
        unsigned long size = msgListeners.size();

        for (unsigned long i = 0; i < size; i++)
        {
            MamdaMsgListener* listener = msgListeners[i];

            listener->onMsg (subscription, msg, msgType);
        }
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::handleQuoteMsg (
        MamdaOptionContract&  contract,
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOptionChainListener (%s.%s(%s)) handleQuoteMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        MamdaQuoteListener& quoteListener = contract.getQuoteListener();

        quoteListener.onMsg (subscription, msg, msgType);
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::handleTradeMsg (
        MamdaOptionContract&  contract,
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOptionChainListener (%s.%s(%s)) handleTradeMsg(). "
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        MamdaTradeListener& tradeListener = contract.getTradeListener();

        tradeListener.onMsg (subscription, msg, msgType);
    }

    void MamdaOptionChainListener::MamdaOptionChainListenerImpl::handleFundamentalMsg (
        MamdaOptionContract&  contract,
        MamdaSubscription*    subscription,
        const MamaMsg&        msg,
        short                 msgType)
    {
        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            const char *contractSymbol = "N/A";

            msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, contractSymbol);

            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOptionChainListener (%s.%s(%s)) handleFundamentalMsg()."
                           "msg type: %s msg status %s\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           contractSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString ());
        }

        MamdaFundamentalListener& fundamentalListener 
            = contract.getFundamentalListener();

        fundamentalListener.onMsg (subscription, msg, msgType);
    }


    MamdaOptionContract&MamdaOptionChainListener::MamdaOptionChainListenerImpl::findContract (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        short               msgType)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        const char*               fullSymbol    = NULL;
        string                    symbol;
        string                    exchange;
        MamaDateTime              expireDate;
        double                    strikePrice   = 0.0;
        uint32_t                  openInterest  = 0;
        MamdaOptionPutCall        putCall       = MAMDA_PUT_CALL_UNKNOWN;
        MamdaOptionExerciseStyle  exerciseStyle = MAMDA_EXERCISE_STYLE_UNKNOWN;

        if (!msg.tryString (MamdaOptionFields::CONTRACT_SYMBOL, fullSymbol))
        {
            throw MamdaDataException ("cannot find contract symbol in msg");
        }

        MamdaOptionContract* contract = static_cast<MamdaOptionContract*>
            (subscription->getItemClosure ());

        getSymbolAndExchange (string(fullSymbol), symbol, exchange); 

        if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
        {
            mama_forceLog (MAMA_LOG_LEVEL_FINE,
                           "MamdaOptionChainListener (%s.%s(%s)) findContract()."
                           "msg type: %s msg status: %s Symbol: %s contract: %p\n",
                           subscription->getSource (),
                           subscription->getSymbol (),
                           fullSymbol,
                           msg.getMsgTypeName (),
                           msg.getMsgStatusString (),
                           symbol.c_str(),
                           contract);
        }

        if (contract == NULL)
        {
            // Parse fields from message to create contract.
            bool gotExpireDate   =  getExpireDate  (msg, fullSymbol, expireDate);
            bool gotStrikePrice  =  getStrikePrice (msg, fullSymbol, strikePrice);
            bool gotPutCall      =  getPutCall     (msg, fullSymbol, putCall);

            bool gotAllPrimFields = 
                (gotExpireDate && gotStrikePrice && gotPutCall);

            bool gotAllMandFields = 
                (gotAllPrimFields || 
                 ( (!mExpireDateManditory 
                    || (mExpireDateManditory && gotExpireDate))
                   && (!mStrikePriceManditory 
                       || (mStrikePriceManditory && gotStrikePrice))
                   && (!mPutCallManditory 
                       || (mPutCallManditory && gotPutCall)) ) );

            bool gotOpenInterest  = 
                getOpenInterest  (msg, fullSymbol, openInterest);
            bool gotExerciseStyle = 
                getExerciseStyle (msg, fullSymbol, exerciseStyle);

            // Create contract
            contract = new MamdaOptionContract (symbol.c_str(), exchange.c_str());

            // Populate contract
            if (gotExpireDate)
                contract->setExpireDate(expireDate);

            if (gotStrikePrice)
                contract->setStrikePrice(strikePrice);

            if (gotPutCall)
                contract->setPutCall(putCall);

            if (gotOpenInterest)
                contract->setOpenInterest(openInterest);

            if (gotExerciseStyle)
                contract->setExerciseStyle(exerciseStyle);

            if (subscription->checkDebugLevel (MAMA_LOG_LEVEL_FINE))
            {
                mama_forceLog (MAMA_LOG_LEVEL_FINE,
                               "MamdaOptionChainListener (%s.%s(%s)) findContract() - "
                               "new contract creation - "
                               "ExpireDate: %s StrikePrice: %f PutCall: %c"
                               "OpenInterest: %d ExerciseStyle: %c\n",
                               subscription->getSource (),
                               subscription->getSymbol (),
                               fullSymbol,	
                               contract->getExpireDate().getAsString(),
                               contract->getStrikePrice(),
                               contract->getPutCall(),
                               contract->getOpenInterest(),
                               contract->getExerciseStyle());
            }

            // Add to chain
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "findContract: adding new contract to chain: %s",
                      fullSymbol);

            contract->setRecapRequired   (!gotAllMandFields);
            mChain->addContract          (fullSymbol, contract);
            subscription->setItemClosure (contract);

            mLastActionContract = contract;
            mLastAction         = ACTION_ADD;

            mContracts.push_back (contract);

            mama_log (MAMA_LOG_LEVEL_FINE,
                      "findContract: calling onOptionContractCreate: %s",
                      fullSymbol);

            // Call all client option create handlers
            // in acknowledgement of new contract
            vector<MamdaOptionChainHandler*>::iterator i;

            for (i = mHandlers.begin(); i != mHandlers.end(); ++i)
            {
                (*i)->onOptionContractCreate (subscription, 
                                              mSelf, 
                                              msg,
                                              *contract, 
                                              *mChain);
            }
        }
        else // contract already exists
        {
            // We have requested a recap 
            if (contract->getRecapRequired())
            {
                // This message may or may not be the requested initial image
                // Until the recap timer expires we will try to glean the missing 
                // manditory fields from any message that comes along. 
                bool gotExpireDate =
                    (contract->gotExpireDate() || 
                     getExpireDate(msg, fullSymbol, expireDate));

                bool gotStrikePrice =
                    (contract->gotStrikePrice() || 
                     getStrikePrice(msg, fullSymbol, strikePrice));

                bool gotPutCall = 
                    (contract->gotPutCall() || 
                     getPutCall(msg, fullSymbol, putCall));

                bool gotAllPrimFields =
                    (gotExpireDate && gotStrikePrice && gotPutCall);

                bool gotAllMandFields = 
                    (gotAllPrimFields || 
                     ( (!mExpireDateManditory 
                        || (mExpireDateManditory && gotExpireDate))
                       && (!mStrikePriceManditory 
                           || (mStrikePriceManditory && gotStrikePrice))
                       && (!mPutCallManditory 
                           || (mPutCallManditory && gotPutCall)) ) );

                bool gotOpenInterest  = 
                    getOpenInterest (msg, fullSymbol, openInterest);

                bool gotExerciseStyle = 
                    getExerciseStyle(msg, fullSymbol, exerciseStyle);

                // update contract with any additional fields
                if (!contract->gotExpireDate() && gotExpireDate)
                    contract->setExpireDate (expireDate);

                if (!contract->gotStrikePrice() && gotStrikePrice)
                    contract->setStrikePrice (strikePrice);

                if (!contract->gotPutCall() && gotPutCall)
                    contract->setPutCall (putCall);

                if (!contract->gotOpenInterest() && gotOpenInterest)
                    contract->setOpenInterest (openInterest);

                if (!contract->gotExerciseStyle() && gotExerciseStyle)
                    contract->setExerciseStyle (exerciseStyle);

                if(subscription->checkDebugLevel(MAMA_LOG_LEVEL_FINE))
                {
                    mama_forceLog (MAMA_LOG_LEVEL_FINE,
                                   "MamdaOptionChainListener (%s.%s(%s)) findContract() - "
                                   "contract update - "
                                   "ExpireDate: %s StrikePrice: %f PutCall: %c"
                                   "OpenInterest: %d ExerciseStyle: %c\n",
                                   subscription->getSource (),
                                   subscription->getSymbol (),
                                   fullSymbol,	
                                   contract->getExpireDate().getAsString(),
                                   contract->getStrikePrice(),
                                   contract->getPutCall(),
                                   contract->getOpenInterest(),
                                   contract->getExerciseStyle());
                }

                if (gotAllMandFields)
                {	      
                    // Contract has a minimum of the manditory fields
                    // For all intensive purposes it is complete 
                    // chain should be made aware of new contract details
                    contract->setRecapRequired(!gotAllMandFields);
                    mChain->processNewContractDetails (fullSymbol, contract);

                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "findContract: incomplete contract now completed: %s\n"
                              "\t - now visible to client application", 
                              fullSymbol);

                    // call all client option create handlers
                    // in acknowledgement of new / complete contract
                    vector<MamdaOptionChainHandler*>::iterator i;
                    for (i = mHandlers.begin(); i != mHandlers.end(); ++i)
                    {
                        (*i)->onOptionContractCreate (subscription, 
                                                      mSelf, 
                                                      msg,
                                                      *contract, 
                                                      *mChain);
                    }
                }
            }
        }
        return *contract;
    }

    bool MamdaOptionChainListener::MamdaOptionChainListenerImpl::getExpireDate(
        const MamaMsg&        msg,
        const char*           fullSymbol,
        MamaDateTime&         expireDate)
    {
        bool rtnVal = true;
        mama_log (MAMA_LOG_LEVEL_FINE, 
	              "findContract: trying to get expiration date field: %s",
	              fullSymbol);

        if (!msg.tryDateTime (MamdaOptionFields::EXPIRATION_DATE, expireDate))
        {
            rtnVal = false;
	        mama_log (MAMA_LOG_LEVEL_FINE,
		              "findContract: CANNOT find expiration date in msg: %s",
		              fullSymbol);
        }

        return rtnVal;
    }

    bool MamdaOptionChainListener::MamdaOptionChainListenerImpl::getStrikePrice(
        const MamaMsg&        msg,
        const char*           fullSymbol,
        double&               strikePrice)
    {
        bool rtnVal = true;
        mama_log (MAMA_LOG_LEVEL_FINE, 
	              "findContract: trying to get strike price field: %s",
	              fullSymbol);

        if (!msg.tryF64 (MamdaOptionFields::STRIKE_PRICE, strikePrice))
        {
            rtnVal = false;
	        mama_log (MAMA_LOG_LEVEL_FINE,
		              "findContract: CANNOT find strike price in msg: %s",
		              fullSymbol);
        }
        return rtnVal;
    }

    bool MamdaOptionChainListener::MamdaOptionChainListenerImpl::getPutCall(
        const MamaMsg&       msg,
        const char*          fullSymbol,
        MamdaOptionPutCall&  putCall)
    {
        bool         rtnVal      = true;
        const char*  putCallStr  = "";
        int          putCallInt  = 0;

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "findContract: trying to get put call field: %s", 
                  fullSymbol);

        if (!msg.tryField (MamdaOptionFields::PUT_CALL, &mTmpField))
        {
            rtnVal = false;
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "findContract: CANNOT find put/call in msg: %s", 
                      fullSymbol);
        }
        else
        {
            switch(mTmpField.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    putCallInt = mTmpField.getU32();
                    switch(putCallInt)
                    {
                        case 1:
                            putCall = MAMDA_PUT_CALL_PUT;
                            break;
                        case 2:
                            putCall = MAMDA_PUT_CALL_CALL;
                            break;
                        case 99:
                            putCall = MAMDA_PUT_CALL_UNKNOWN;
                            break;
                        default:
                            putCall = MAMDA_PUT_CALL_UNKNOWN;
                            mama_log (MAMA_LOG_LEVEL_FINE,
                                      "Unhandled value %d for wPutCall.", 
                                      putCallInt);
                            break;
                    }
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    putCallStr = mTmpField.getString();
                    switch (putCallStr[0])
                    {
                        case '1':
                        case 'P':
                            putCall = MAMDA_PUT_CALL_PUT;
                            break;
                        case '2':
                        case 'C':
                            putCall = MAMDA_PUT_CALL_CALL;
                            break;
                        default:
                            putCall = MAMDA_PUT_CALL_UNKNOWN;
                            if ((strcmp(putCallStr,"99") != 0) && (strcmp(putCallStr,"Z") != 0))
                            {
                                mama_log (MAMA_LOG_LEVEL_FINE,
                                          "Unhandled value '%s' for wPutCall.", 
                                          putCallStr);
                            }
                            break;
                    }
                    break;
                default:
                    putCall = MAMDA_PUT_CALL_UNKNOWN;
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wPutCall.  "
                              "Expected string or integer.", 
                              mTmpField.getType());
                    break;
            }
        }
        return rtnVal;
    }

    bool MamdaOptionChainListener::MamdaOptionChainListenerImpl::getOpenInterest(
        const MamaMsg&            msg,
        const char*               fullSymbol,
        uint32_t&                 openInterest)
    {
        bool rtnVal = true;
        mama_log (MAMA_LOG_LEVEL_FINE, 
	              "findContract: trying to get open interest field: %s",
	              fullSymbol);

        if (!msg.tryU32 (MamdaOptionFields::OPEN_INTEREST, openInterest))
        {
            rtnVal = false;
	        mama_log (MAMA_LOG_LEVEL_FINEST, 
		              "Cannot find open interest in msg, %s", 
                      fullSymbol);
        }
        return rtnVal;
    }

    bool MamdaOptionChainListener::MamdaOptionChainListenerImpl::getExerciseStyle(
        const MamaMsg&            msg,
        const char*               fullSymbol,
        MamdaOptionExerciseStyle& exerciseStyle)
    {
        bool         rtnVal            = true;
        const char*  exerciseStyleStr  = "";
        int          exerciseStyleInt  = 0;

        mama_log (MAMA_LOG_LEVEL_FINE,
                  "findContract: trying to get exercise style field: %s",
                  fullSymbol);

        if (!msg.tryField (MamdaOptionFields::EXERCISE_STYLE, &mTmpField))
        {
            rtnVal = false;
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "Cannot find exercise style in msg, %s", 
                      fullSymbol);
        }
        else
        {
            switch(mTmpField.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    exerciseStyleInt = mTmpField.getU32();
                    switch(exerciseStyleInt)
                    {
                        case 1:
                            // American
                            exerciseStyle = MAMDA_EXERCISE_STYLE_AMERICAN;
                            break;
                        case 2:
                            // European
                            exerciseStyle = MAMDA_EXERCISE_STYLE_EUROPEAN;
                            break;
                        case 3:
                            // Capped
                            exerciseStyle = MAMDA_EXERCISE_STYLE_CAPPED;
                            break;
                        case 99:
                            exerciseStyle = MAMDA_EXERCISE_STYLE_UNKNOWN;
                            break;
                        default:
                            exerciseStyle = MAMDA_EXERCISE_STYLE_UNKNOWN;
                            mama_log (MAMA_LOG_LEVEL_FINE,
                                      "Unhandled value %d for wExerciseStyle.", 
                                      exerciseStyleInt);
                            break;
                    }
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    exerciseStyleStr = mTmpField.getString();
                    switch (exerciseStyleStr[0])
                    {
                        case '1':
                        case 'A':
                            // American
                            exerciseStyle = MAMDA_EXERCISE_STYLE_AMERICAN;
                            break;
                        case '2':
                        case 'E':
                            // European
                            exerciseStyle = MAMDA_EXERCISE_STYLE_EUROPEAN;
                            break;
                        case '3':
                        case 'C':
                            // Capped
                            exerciseStyle = MAMDA_EXERCISE_STYLE_CAPPED;
                            break;
                        default:
                            exerciseStyle = MAMDA_EXERCISE_STYLE_UNKNOWN;
                            if ((strcmp(exerciseStyleStr,"99") != 0) && (strcmp(exerciseStyleStr,"Z") != 0))
                            {
                                mama_log (MAMA_LOG_LEVEL_FINE,
                                          "Unhandled value '%s' for wExerciseStyle.", 
                                          exerciseStyleStr);
                            }
                            break;
                    }
                    break;
                default:
                    exerciseStyle = MAMDA_EXERCISE_STYLE_UNKNOWN;
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wExerciseStyle.  "
                              "Expected string or integer.", 
                              mTmpField.getType());
                    break;
            }
        }
        
        return rtnVal;
    }

    void MamdaOptionChainListener::
    MamdaOptionChainListenerImpl::getSymbolAndExchange(
        string               fullSymbol,
        string&              symbol,
        string&              exchange)
    {
        string::size_type dotPos = fullSymbol.find('.');
        if (dotPos != string::npos)
        { 
            exchange = fullSymbol.substr(dotPos+1,(fullSymbol.size()-dotPos));
        	symbol   = fullSymbol.substr(0,dotPos);
        }
        else /* The BBO may not have a suffix - this is the default */
        {
            symbol = fullSymbol;
        }
    }

} // namespace
