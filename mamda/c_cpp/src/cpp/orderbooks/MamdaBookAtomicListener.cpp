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

#include <mamda/MamdaBookAtomicListener.h>
#include <mamda/MamdaBookAtomicBookHandler.h>
#include <mamda/MamdaBookAtomicLevelHandler.h>
#include <mamda/MamdaBookAtomicLevelEntryHandler.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaFieldState.h>
#include "../MamdaUtils.h"
#include <mama/mamacpp.h>
#include <iostream>
#include <string>
#include <wombat/wMath.h>

using std::string;

namespace Wombat
{

    class MamdaBookAtomicListener::MamdaBookAtomicListenerImpl
    {
    public:
        MamdaBookAtomicListenerImpl (MamdaBookAtomicListener& listener);

        ~MamdaBookAtomicListenerImpl();

        void handleClear            (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);

        void handleRecap            (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);

        void handleUpdate           (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg);

        void handleStandardFields   (MamdaSubscription*  subscription, 
                                     const MamaMsg&      msg,
                                     bool                checkSeqNum);

        void createDeltaFromMamaMsg (MamdaSubscription*  subscription,
                                     const MamaMsg&      msg,
                                     bool                isRecap);
     
        void createDeltaFromMamaMsgWithVectorFields (
                                     MamdaSubscription*  subscription,
                                     const MamaMsg&      msg,
                                     const MamaMsg**     msgLevels,
                                     size_t              msgLevelsNum,
                                     bool                isRecap);

        void createDeltaFromMamaMsgWithoutVectorFields (
                                     MamdaSubscription*  subscription,
                                     const MamaMsg&      msg,
                                     bool                isRecap,
                                     bool                isMarketOrder);

        void populateOrderBookFromEntryMsg (
                                     const MamaMsg*      entryMsg);

        void createDeltaFromPriceLevel (
                    const MamdaOrderBookPriceLevel* priceLevel,
                    MamdaSubscription*              subscription,
                    const MamaMsg&                  msg,
                    bool                            isRecap);

        void createDeltaFromOrderBook (
                    MamdaSubscription*              subscription,
                    const MamaMsg&                  msg,
                    bool                            isRecap);

        void getInfoFromEntry(
                    MamdaSubscription*  subscription,
                    const MamaMsg&      msg,
                    const MamaMsg&      plMsg,
                    bool                isRecap);

        void getLevelInfoAndEntries(
                    MamdaSubscription*  subscription,
                    const MamaMsg&      msg,
                    const MamaMsg&      plMsg,
                    bool                isRecap); 

        void invokeClearHandlers (MamdaSubscription*  subscription,
                                  const MamaMsg&      msg);
        void invokeGapHandlers   (MamdaSubscription*  subscription,
                                  const MamaMsg&      msg);   

        void clearLevelFields();
        void clearLevelEntryFields();


        MamdaBookAtomicListener&           mListener;
        MamdaBookAtomicBookHandler*        mBookHandler;
        MamdaBookAtomicLevelHandler*       mLevelHandler;
        MamdaBookAtomicLevelEntryHandler*  mLevelEntryHandler;

        // The following fields are used for caching the order book delta and
        // related fields.  These fields can be used by applications for 
        // reference and will be passed for recaps.

        string             mSymbol;        MamdaFieldState      mSymbolFieldState;
        string             mPartId;        MamdaFieldState      mPartIdFieldState;
        MamaDateTime       mSrcTime;       MamdaFieldState      mSrcTimeFieldState;
        MamaDateTime       mActTime;       MamdaFieldState      mActTimeFieldState;
        MamaDateTime       mLineTime;      MamdaFieldState      mLineTimeFieldState;
        MamaDateTime       mSendTime;      MamdaFieldState      mSendTimeFieldState;
        MamaMsgQual        mMsgQual;       MamdaFieldState      mMsgQualFieldState;
        MamaDateTime       mEventTime;     MamdaFieldState      mEventTimeFieldState;
        mama_seqnum_t      mEventSeqNum;   MamdaFieldState      mEventSeqNumFieldState;

        uint8_t 	       mBookType;

        mama_u32_t         mPriceLevels;
        mama_u32_t         mPriceLevel;
        MamaPrice          mPriceLevelPrice;
        mama_f64_t         mPriceLevelSize;
        mama_f64_t         mPriceLevelSizeChange;
        int32_t            mPriceLevelAction;      // char field 
        int32_t            mPriceLevelSide;        // char field
        MamaDateTime       mPriceLevelTime;
        mama_f32_t         mPriceLevelNumEntries; 
        mama_u32_t         mPriceLevelActNumEntries; 
        int32_t            mPriceLevelEntryAction; // char field
        int32_t            mPriceLevelEntryReason; // char field
        string             mPriceLevelEntryId;
        mama_u64_t         mPriceLevelEntrySize;
        MamaDateTime       mPriceLevelEntryTime;

        MamdaOrderBookTypes::OrderType     mOrderType;
        // Additional fields for gaps:
        mama_seqnum_t      mGapBegin;
        mama_seqnum_t      mGapEnd;
        
        bool               mHasMarketOrders;
        bool               mProcessMarketOrders;

        MamdaOrderBook     mOrderBook;
        MamdaOrderBookEntry* mEntry;
    };

    MamdaBookAtomicListener::MamdaBookAtomicListener ()
        : mImpl (*new MamdaBookAtomicListenerImpl(*this))
    {
    }

    MamdaBookAtomicListener::~MamdaBookAtomicListener ()
    {
        delete &mImpl;
    }

    void MamdaBookAtomicListener::
    addBookHandler (MamdaBookAtomicBookHandler* handler)
    {
        mImpl.mBookHandler = handler;
    }

    void MamdaBookAtomicListener::
    addLevelHandler (MamdaBookAtomicLevelHandler*  handler)
    {
        mImpl.mLevelHandler = handler;
    }

    void MamdaBookAtomicListener::
    addLevelEntryHandler (MamdaBookAtomicLevelEntryHandler*  handler)
    {
        mImpl.mLevelEntryHandler = handler;
    }

    const char* MamdaBookAtomicListener::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaBookAtomicListener::getPartId() const
    {
        return mImpl.mPartId.c_str();
    }

    const MamaDateTime& MamdaBookAtomicListener::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaBookAtomicListener::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaBookAtomicListener::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaBookAtomicListener::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const MamaMsgQual& MamdaBookAtomicListener::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    mama_seqnum_t MamdaBookAtomicListener::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime& MamdaBookAtomicListener::getEventTime() const
    {
        return mImpl.mEventTime;
    }

    mama_u32_t MamdaBookAtomicListener::getPriceLevelNumLevels () const
    {
        return mImpl.mPriceLevels;
    } 

    mama_u32_t MamdaBookAtomicListener::getPriceLevelNum () const
    {
        return mImpl.mPriceLevel;
    } 

    double MamdaBookAtomicListener::getPriceLevelPrice () const
    {
        return mImpl.mPriceLevelPrice.getValue();
    }

    MamaPrice&  MamdaBookAtomicListener::getPriceLevelMamaPrice () const
    {
      return mImpl.mPriceLevelPrice;
      
    }

    mama_f64_t MamdaBookAtomicListener::getPriceLevelSize () const
    {
        return mImpl.mPriceLevelSize;
    }

    mama_i64_t MamdaBookAtomicListener::getPriceLevelSizeChange () const
    {
        // this field may be sent as NAN e.g. HKSE
        if (wIsnan(mImpl.mPriceLevelSizeChange))
        {
             mImpl.mPriceLevelSizeChange = 0;
        }

        // cast as we don't want to break the interface
        return (mama_i64_t) mImpl.mPriceLevelSizeChange;
    }

    char MamdaBookAtomicListener::getPriceLevelAction () const
    {
        return mImpl.mPriceLevelAction;
    }

    char MamdaBookAtomicListener::getPriceLevelSide () const
    {
        return mImpl.mPriceLevelSide;
    }

    const MamaDateTime& MamdaBookAtomicListener::getPriceLevelTime() const
    {
        return mImpl.mPriceLevelTime;
    }

    mama_f32_t MamdaBookAtomicListener::getPriceLevelNumEntries () const
    {
        return mImpl.mPriceLevelNumEntries;
    }

    void MamdaBookAtomicListener::setOrderType (MamdaOrderBookTypes::OrderType orderType) const
    {
        mImpl.mOrderType = orderType;
    }

    MamdaOrderBookTypes::OrderType MamdaBookAtomicListener::getOrderType () const
    {
        return mImpl.mOrderType;
    }

    bool MamdaBookAtomicListener::getHasMarketOrders () const
    {
        return mImpl.mHasMarketOrders;
    }

    void MamdaBookAtomicListener::setProcessMarketOrders (bool process) const
    {
        mImpl.mProcessMarketOrders = process;
    }

    mama_u32_t MamdaBookAtomicListener::getPriceLevelActNumEntries () const
    {
        return mImpl.mPriceLevelActNumEntries;
    }

    char MamdaBookAtomicListener::getPriceLevelEntryAction () const
    {
        return mImpl.mPriceLevelEntryAction;
    }

    char MamdaBookAtomicListener::getPriceLevelEntryReason () const
    {
        return mImpl.mPriceLevelEntryReason;
    }

    const char* MamdaBookAtomicListener::getPriceLevelEntryId () const
    {
        return mImpl.mPriceLevelEntryId.c_str();
    }

    mama_u64_t MamdaBookAtomicListener::getPriceLevelEntrySize () const
    {
        return mImpl.mPriceLevelEntrySize;
    }

    const MamaDateTime& MamdaBookAtomicListener::getPriceLevelEntryTime() const
    {
        return mImpl.mPriceLevelEntryTime;
    }

    mama_seqnum_t MamdaBookAtomicListener::getBeginGapSeqNum() const
    {
        return mImpl.mGapBegin;
    }

    mama_seqnum_t MamdaBookAtomicListener::getEndGapSeqNum() const
    {
        return mImpl.mGapEnd;
    }

    MamdaFieldState MamdaBookAtomicListener::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState  MamdaBookAtomicListener::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getActivityTimeFieldState() const
    {
        return mImpl.mActTimeFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaBookAtomicListener::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }
    /* End FieldState Accessors */

    void MamdaBookAtomicListener::onMsg (MamdaSubscription* subscription,
                                         const MamaMsg&     msg,
                                         short              msgType)
    {
        //Check the dictionary is set correctly
        if (!MamdaOrderBookFields::isSet())
        {
             mama_log (MAMA_LOG_LEVEL_WARN,
                       "MamdaBookAtomicListener: MamdaOrderBookFields::setDictionary() "
                       "has not been called.");
             return;
        }

        // If msg is a orderBook-related message, invoke the
        // appropriate callback:
        try
        { 
            switch (msgType)
            {
            case MAMA_MSG_TYPE_BOOK_UPDATE:
                mImpl.handleUpdate (subscription, msg);
                break;
            case MAMA_MSG_TYPE_BOOK_CLEAR:
                mImpl.handleClear (subscription, msg);
                break;
            case MAMA_MSG_TYPE_BOOK_INITIAL:
            case MAMA_MSG_TYPE_BOOK_RECAP:
            case MAMA_MSG_TYPE_BOOK_SNAPSHOT:
                mImpl.handleRecap (subscription, msg);
                break;

            }
        }
        catch ( MamaStatus &e)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "Exception MamdaBookAtomicListener::onMsg() \n" \
                      "\t - %s", e.toString());
        }
    }

    MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::MamdaBookAtomicListenerImpl(
          MamdaBookAtomicListener& listener)
          : mListener                (listener)
          , mBookHandler             (NULL)
          , mLevelHandler            (NULL)
          , mLevelEntryHandler       (NULL)
          , mEventSeqNum             (0)
          , mPriceLevels             (0)
          , mPriceLevel              (0)
          , mPriceLevelPrice         (0.0)
          , mPriceLevelSize          (0.0)
          , mPriceLevelAction        (0)
          , mPriceLevelSide          (0)
          , mPriceLevelNumEntries    (0.0f)
          , mPriceLevelActNumEntries (0)
          , mPriceLevelEntryAction   (0)
          , mPriceLevelEntryReason   (0)
          , mPriceLevelEntrySize     (0)
          , mOrderType               (MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_LIMIT)
          , mGapBegin                (0)
          , mGapEnd                  (0)
          , mHasMarketOrders         (false)
          , mProcessMarketOrders     (false)
          , mEntry                   (NULL)
    {
    }

    MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::~MamdaBookAtomicListenerImpl()
    {}


    void MamdaBookAtomicListener::MamdaBookAtomicListenerImpl::handleClear (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        handleStandardFields (subscription, msg, false);
        invokeClearHandlers(subscription, msg);
    }

    void MamdaBookAtomicListener::MamdaBookAtomicListenerImpl::handleRecap (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        handleStandardFields (subscription, msg, false);
        createDeltaFromMamaMsg (subscription, msg, true);
    }

    void MamdaBookAtomicListener::MamdaBookAtomicListenerImpl::handleUpdate (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        handleStandardFields (subscription, msg, true);
        createDeltaFromMamaMsg  (subscription, msg, false);
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::handleStandardFields ( 
        MamdaSubscription*  subscription, 
        const MamaMsg&      msg,
        bool                checkSeqNum)
    {
        const char* symbol = NULL;
        const char* partId = NULL;
        uint16_t    msgQual = 0;

        /*
         * Not using MamdaUtils as does not use correct ordering
         */
        if (msg.tryString (MamdaCommonFields::SYMBOL, symbol))
        {
            mSymbol = symbol;
            mSymbolFieldState = Wombat::MODIFIED;
        }
        else
        {
            mSymbol = "";
            mSymbolFieldState = Wombat::NOT_INITIALISED;
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "getSymbolAndPartId: cannot find symbol field in message");
        }

        if (msg.tryString (MamdaCommonFields::PART_ID, partId))
        {
            mPartId = partId;
            mPartIdFieldState = Wombat::MODIFIED;
        }
        else
        {
            if (symbol != NULL)
            {
                const char* lastDot = strrchr (symbol, '.');
                if (lastDot!= NULL)
                {
                    if (lastDot+1 != '\0')
                    {
                        partId = lastDot+1;
                        mPartId = partId;
                        mPartIdFieldState = Wombat::MODIFIED;
                    }
                    else
                    {
                        mPartId = "";
                        mPartIdFieldState = Wombat::NOT_INITIALISED;
                        mama_log (MAMA_LOG_LEVEL_FINEST,
                                  "getSymbolAndPartId: cannot find part id field in message");
                    }
                }
                else
                {
                    mPartId = "";
                    mPartIdFieldState = Wombat::NOT_INITIALISED;
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "getSymbolAndPartId: cannot find part id field in message");
                }
            }
            else
            {
                mPartId = "";
                mPartIdFieldState = Wombat::NOT_INITIALISED;
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "getSymbolAndPartId: cannot find symbol or part id fields in message");
            }
        }

        mSrcTime.clear();
        mActTime.clear();
        mLineTime.clear();
        mSendTime.clear();

        getTimeStamps   (msg, mSrcTime, mActTime, mLineTime, mSendTime);

        mSrcTimeFieldState = mSrcTime.empty()   ?  Wombat::NOT_INITIALISED : Wombat::MODIFIED;
        mActTimeFieldState = mActTime.empty()   ?  Wombat::NOT_INITIALISED : Wombat::MODIFIED;
        mLineTimeFieldState = mLineTime.empty() ?  Wombat::NOT_INITIALISED : Wombat::MODIFIED;
        mSendTimeFieldState = mSendTime.empty() ?  Wombat::NOT_INITIALISED : Wombat::MODIFIED;

        if (MamdaCommonFields::MSG_QUAL)
        {
            mMsgQual.clear();
            if (msg.tryU16 (MamdaCommonFields::MSG_QUAL, msgQual))
            {
                mMsgQual.setValue(msgQual);
            }
        }
        
        if (!msg.tryDateTime (MamdaOrderBookFields::BOOK_TIME, mEventTime))
        {
            mEventTime=mSrcTime;
            mEventTimeFieldState = mSrcTimeFieldState;
        }
        else
        {
            mEventTimeFieldState = Wombat::MODIFIED;
        }

        mama_seqnum_t  seqNum = msg.getSeqNum();
        if (checkSeqNum && (seqNum != (mEventSeqNum + 1)))
        {
            mGapBegin    = mEventSeqNum + 1;
            mGapEnd      = seqNum - 1;
            mEventSeqNum = seqNum;
            invokeGapHandlers (subscription, msg);
        }
        else
        {
            mEventSeqNum = seqNum;
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::populateOrderBookFromEntryMsg (
        const MamaMsg* entryMsg)
    {
        clearLevelFields();
        clearLevelEntryFields();

        entryMsg->tryPrice                            (MamdaOrderBookFields::PL_PRICE, mPriceLevelPrice);
        mPriceLevelSide        = entryMsg->getI32     (MamdaOrderBookFields::PL_SIDE);
        mPriceLevelEntryId     = entryMsg->getString  (MamdaOrderBookFields::ENTRY_ID);
        mPriceLevelEntryAction = entryMsg->getI32     (MamdaOrderBookFields::ENTRY_ACTION);
        mPriceLevelEntrySize   = entryMsg->getU64     (MamdaOrderBookFields::ENTRY_SIZE);
        entryMsg->getDateTime                         (MamdaOrderBookFields::ENTRY_TIME, mPriceLevelEntryTime);
        mPriceLevelEntryReason = entryMsg->getI32     (MamdaOrderBookFields::ENTRY_REASON);

        //We create object and then clearing the book will clean up
        mEntry = new MamdaOrderBookEntry(
                mPriceLevelEntryId.c_str(),
                mPriceLevelEntrySize,
                (MamdaOrderBookEntry::Action)mPriceLevelEntryAction,
                mPriceLevelEntryTime,
                (const MamaSourceDerivative*) NULL);

        mEntry->setReason((MamdaOrderBookTypes::Reason)mPriceLevelEntryReason);
        mEntry->setClosure((void*)entryMsg);

        //Currently V5 does not send status
        mOrderBook.addEntry(
                mEntry,
                mPriceLevelPrice,
                (MamdaOrderBookPriceLevel::Side) mPriceLevelSide,
                mPriceLevelEntryTime,
                (MamdaOrderBookBasicDelta*) NULL);
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::createDeltaFromPriceLevel (
            const MamdaOrderBookPriceLevel* priceLevel,
            MamdaSubscription* subscription,
            const MamaMsg& msg,
            bool isRecap)
    {
        mPriceLevelPrice = priceLevel->getPrice();
        mPriceLevelSide  = priceLevel->getSide();
        mPriceLevelAction= priceLevel->getAction();

        /*
         * With 2.16 PL_NUM_ENTRIES was sent and populated the NumEntries field and
         * the ActNumEntries field was populated with the actual number of sub messages
         * under the price level. Both populated for V5 as same since PL_NUM_ENTRIES
         * not sent
         */
        mPriceLevelNumEntries    = priceLevel->getNumEntries();
        mPriceLevelActNumEntries = priceLevel->getNumEntries();

        /* 2.16 populates sizeChange with size on initial and this gives same behaviour */
        mPriceLevelSize       = priceLevel->getSize();
        mPriceLevelSizeChange = priceLevel->getSize();
        mPriceLevelTime       = priceLevel->getTime();

        if (mLevelHandler)
        {
            if (isRecap)
            {
                mLevelHandler->onBookAtomicLevelRecap (
                        subscription, mListener, msg, mListener);
            }
            else
            {
                mLevelHandler->onBookAtomicLevelDelta (
                        subscription, mListener, msg, mListener);
            }
        }

        if (mLevelEntryHandler)
        {
            MamdaOrderBookPriceLevel::iterator entryIt, entryEndIt;
            const MamdaOrderBookEntry* entry;
            const MamaMsg* entryMsg;

            for(entryIt = priceLevel->begin(), entryEndIt = priceLevel->end();
                    entryIt != entryEndIt;
                    ++entryIt)
            {
                entry = *entryIt;
                clearLevelEntryFields();

                mPriceLevelEntryAction = entry->getAction();

                mPriceLevelEntryReason = entry->getReason();

                mPriceLevelEntryId = entry->getId();

                mPriceLevelEntrySize = mama_u64_t(entry->getSize());

                mPriceLevelEntryTime = entry->getTime();

                entryMsg = (const MamaMsg*) entry->getClosure();

                if(entryMsg != NULL)
                {
                    if (isRecap)
                    {
                        mLevelEntryHandler->onBookAtomicLevelEntryRecap (
                                subscription, mListener, *entryMsg, mListener);
                    }
                    else
                    {
                        mLevelEntryHandler->onBookAtomicLevelEntryDelta (
                                subscription, mListener, *entryMsg, mListener);
                    }
                }
                else
                {
                    mama_log (MAMA_LOG_LEVEL_ERROR,
                              "MamdaBookAtomicListener::createDeltaFromPriceLevel() : MamaMsg is NULL." 
                              " symbol [%s] price level [%f] entry id  [%s].",
                            mSymbol.c_str(),
                            mPriceLevelPrice.getValue(),
                            mPriceLevelEntryId.c_str());
                }
            }
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::createDeltaFromOrderBook (
            MamdaSubscription* subscription,
            const MamaMsg& msg,
            bool isRecap)
    {
        size_t i=0;
        MamdaOrderBook::askIterator askIt, askEndIt;
        MamdaOrderBook::bidIterator bidIt, bidEndIt;
        const MamdaOrderBookPriceLevel* priceLevel;

        mPriceLevels = mOrderBook.getTotalNumLevels();

        for(askIt = mOrderBook.askBegin(), askEndIt = mOrderBook.askEnd(), i=0;
                askIt != askEndIt;
                ++askIt, ++i)
        {
            priceLevel = *askIt;
            clearLevelFields();

            mPriceLevel=i+1;
            createDeltaFromPriceLevel(priceLevel, subscription, msg, isRecap);
        }

        for(bidIt = mOrderBook.bidBegin(), bidEndIt = mOrderBook.bidEnd();
                bidIt != bidEndIt;
                ++bidIt, ++i)
        {
            priceLevel = *bidIt;
            clearLevelFields();

            mPriceLevel=i+1;
            createDeltaFromPriceLevel(priceLevel, subscription, msg, isRecap);
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::createDeltaFromMamaMsg (
        MamdaSubscription* subscription,
        const MamaMsg& msg,
        bool isRecap)
    {
        uint32_t mNumLevels = 1;
        msg.tryU32 (MamdaOrderBookFields::NUM_LEVELS, mNumLevels);
        if ((!mProcessMarketOrders) && (!isRecap) && (mNumLevels == 0))
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaAtomicBookListener: Market Order Update deliberately not processed");
                return;
            }
        
	    if (mBookHandler)
        {
          mBookHandler->onBookAtomicBeginBook (
                 subscription, mListener, isRecap);
        }

        // Note: the following test checks whether vector fields are part
        // of the data dictionary.  However, the vector fields may indeed
        // be part of the data dictionary, but the message may still
        // contain the non-vector style order book data.
        mBookType =0;
        msg.tryU8 (MamdaOrderBookFields::BOOK_TYPE, mBookType);

        if (mBookType==1)
        {
            // this is an entry only, Flatbook which V5 sends as vector of PRICE_LEVELS
            clearLevelFields();
            size_t msgEntriesNum       = 0;
            bool  present              = false;
            const MamaMsg** msgEntries = NULL;

            present = msg.tryVectorMsg (MamdaOrderBookFields::PRICE_LEVELS,
                    msgEntries, msgEntriesNum);

            if (present)
            {
                if(isRecap)
                {
                    /*
                     * V5 entry book initials/recaps are unstructured book so we must
                     * build up a book to fire callbacks appropriately
                     */
                    mOrderBook.clear();
                    const MamaMsg* entryMsg;

                    for (size_t i = 0; i < msgEntriesNum; i++)
                    {
                        populateOrderBookFromEntryMsg (msgEntries[i]);
                    }

                    createDeltaFromOrderBook (subscription, msg, isRecap);
                }
                else
                {
                    /*
                     * V5 sends complex deltas of entries for single price
                     * level (synthetic books in CTA/UTP)
                     */
                    mPriceLevels = msgEntriesNum;
                    mPriceLevel = 0;
                    for (size_t i = 0; i < msgEntriesNum; ++i)
                    {
                        const MamaMsg* plMsg = msgEntries[i];

                        clearLevelFields();
                        mPriceLevel= i + 1;

                        getInfoFromEntry (subscription, msg, *plMsg, isRecap);
                    }
                }
            }
            else
            {
                /*
                 * We have got a flat update
                 */
                clearLevelFields();
                mPriceLevels = 1;
                mPriceLevel = 1;
                getInfoFromEntry (subscription, msg, msg, isRecap);
            }
        }
        else if (mBookType==2)
        {
            // this is a price level only book
            clearLevelFields();
            size_t msgLevelsNum = 0;
            bool  present;
            const MamaMsg** msgLevels = NULL;
            present = msg.tryVectorMsg (MamdaOrderBookFields::PRICE_LEVELS,
                                        msgLevels, msgLevelsNum);
            
            if (present)
            {
                mPriceLevels = msgLevelsNum;
                for (size_t i = 0; i < msgLevelsNum; i++)
                {
                    clearLevelFields();
                    mPriceLevel = i+1;
                    const MamaMsg* plMsg = msgLevels[i];
                    getLevelInfoAndEntries (subscription, msg, *plMsg, isRecap);
                }
            }
            else
            {
                mPriceLevels = 1;
                getLevelInfoAndEntries (subscription, msg, msg, isRecap);
            }
        }
        else if (MamdaOrderBookFields::getHasVectorFields())
        {
            //Process Market Orders
            if (mProcessMarketOrders)
            {
                MamaMsgField bidField;
                MamaMsgField askField;
                mHasMarketOrders = false;
                
                if (msg.tryField (MamdaOrderBookFields::BID_MARKET_ORDERS, &bidField))
                {
                    mHasMarketOrders = true;
                    mOrderType = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_MARKET;
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "MamdaAtomicBookListener: processing Bid side Market Orders");
                
                    const MamaMsg** bidMsgv;
                    mama_size_t   bidSize;
                    bidField.getVectorMsg (bidMsgv, bidSize);
                    createDeltaFromMamaMsgWithoutVectorFields (subscription, *bidMsgv[0], isRecap, true);
                }
                
                if (msg.tryField (MamdaOrderBookFields::ASK_MARKET_ORDERS, &askField))
                {
                    mHasMarketOrders = true;
                    mOrderType = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_MARKET;
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "MamdaAtomicBookListener: processing Ask side Market Orders");
                
                    const MamaMsg** askMsgv;
                    mama_size_t   askSize;
                    askField.getVectorMsg (askMsgv, askSize);
                    createDeltaFromMamaMsgWithoutVectorFields (subscription, *askMsgv[0], isRecap, true);
                }

                // Single Mkt Order update
                if ((mHasMarketOrders) && (!isRecap) && (mNumLevels == 0))
                {
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "MamdaAtomicBookListener: simple Market Order update");
                    if (mBookHandler)
                    {
                        mBookHandler->onBookAtomicEndBook (subscription, mListener);
                    }
                    return;
                }
            }

            size_t msgLevelsNum = 0;
            bool  present;
            const MamaMsg** msgLevels = NULL;
            present = msg.tryVectorMsg (MamdaOrderBookFields::PRICE_LEVELS,
                                        msgLevels, msgLevelsNum);
            if (present)
            {
                createDeltaFromMamaMsgWithVectorFields (subscription,
                                                        msg,
                                                        msgLevels,
                                                        msgLevelsNum,
                                                        isRecap);
            }
            else
            {
                createDeltaFromMamaMsgWithoutVectorFields (subscription, msg, isRecap, false);
            }

        }
        else
        {
            createDeltaFromMamaMsgWithoutVectorFields (subscription, msg, isRecap, false);
        }

        if (mBookHandler)
        {
            mBookHandler->onBookAtomicEndBook (subscription, mListener);
        } 
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::createDeltaFromMamaMsgWithVectorFields (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        const MamaMsg**     msgLevels,
        size_t              msgLevelsNum,
        bool                isRecap)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaBookAtomicListener: processing msg with vector fields");

        mPriceLevels = msgLevelsNum;
        for (size_t i = 0; i < msgLevelsNum; i++)
        {
            clearLevelFields();
            mPriceLevel = i+1;
            const MamaMsg* plMsg = msgLevels[i];

            getLevelInfoAndEntries(subscription, msg, *plMsg, isRecap);
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::createDeltaFromMamaMsgWithoutVectorFields (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        bool                isRecap,
        bool                isMarketOrder)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MamdaBookAtomicListener: processing msg without vector fields.");

        uint32_t numLevelFieldInMsg = 1;
        if (!msg.tryU32 (MamdaOrderBookFields::NUM_LEVELS, numLevelFieldInMsg) && (!isMarketOrder))
        {
            // the NumLevels field should be present for non-vector book messages
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no NUM_LEVELS found!");
        }

        // PRICE_LEVEL_LENGTH may be 0 but we want to go into the
        // below loop at least once for flattened messages scenario
        int16_t maxLevelFields = MamdaOrderBookFields::PRICE_LEVEL_LENGTH
                                    ? MamdaOrderBookFields::PRICE_LEVEL_LENGTH : 1;
        if (numLevelFieldInMsg < maxLevelFields)
        {
            maxLevelFields = numLevelFieldInMsg;
        }
        mPriceLevels = maxLevelFields;

        for (int16_t i = 1; i <= maxLevelFields; i++)
        {
            clearLevelFields();
            mPriceLevel = i;
            const MamaMsg* plMsg = NULL;

            // The PRICE_LEVEL array may not exist
            if (MamdaOrderBookFields::PRICE_LEVEL)
            {
                if (msg.tryField (MamdaOrderBookFields::PRICE_LEVEL[i]))
                {
                    plMsg = msg.getMsg (MamdaOrderBookFields::PRICE_LEVEL[i]);
                }
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaAtomicBookListener: no RV price level fields in the dictionary");
            }

            if ((plMsg == NULL))
            {
                if (numLevelFieldInMsg == 1)
                {
                    
                    if (!isMarketOrder)
                    {
                    /* Price level fields are probably be in the main message. */
                    // mama_log (MAMA_LOG_LEVEL_FINEST,
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "MamdaBookAtomicListener: "
                              "expecting price level in main message...");
                    }
                    plMsg = &msg;
                }
                else
                {
                    // mama_log (MAMA_LOG_LEVEL_FINEST, 
                    mama_log (MAMA_LOG_LEVEL_NORMAL, 
                              "MamdaBookAtomicListener: "
                              "cannot find price level fields in MamaMsg...");
                    break;
                }
            }
            getLevelInfoAndEntries (subscription, msg, *plMsg, isRecap);
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::getInfoFromEntry(
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        const MamaMsg&      plMsg,
        bool                isRecap)
    {
        bool negate = false;

        clearLevelEntryFields();

        /* The number of entries at the price level is unknown in V5 */
        mPriceLevelNumEntries = 0;

        /* This is the actual number of entries for this price level in this message (plMsg) */
        mPriceLevelActNumEntries = 1;

        /* V5 does not send PriceLevel Action */
        mPriceLevelAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN;

        if (!plMsg.tryPrice(MamdaOrderBookFields::PL_PRICE, mPriceLevelPrice))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level price present");
        }

        if (!plMsg.tryI32(MamdaOrderBookFields::PL_SIDE, mPriceLevelSide))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level side present");
        }

        if (!plMsg.tryDateTime(MamdaOrderBookFields::ENTRY_TIME, mPriceLevelEntryTime))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level entry time present");
        }

        mPriceLevelTime=mPriceLevelEntryTime;

        if (!plMsg.tryI32 (MamdaOrderBookFields::ENTRY_ACTION, mPriceLevelEntryAction))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level entry action present");
        }

        if (!plMsg.tryU64 (MamdaOrderBookFields::ENTRY_SIZE, mPriceLevelEntrySize))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level entry size present");
        }

        /*
         * Negate the entry size to get the price level size change
         * if the entry action is delete. Avoid branch using XOR
         */
        negate = (mPriceLevelEntryAction == 'D');
        mPriceLevelSizeChange = (((mama_i64_t)mPriceLevelEntrySize) ^ -negate ) + negate;

        if (mLevelHandler)
        {
            if (isRecap)
            {
                mLevelHandler->onBookAtomicLevelRecap (
                        subscription, mListener, plMsg, mListener);
            }
            else
            {
                mLevelHandler->onBookAtomicLevelDelta (
                         subscription, mListener, plMsg, mListener);
            }
        }
        if (mLevelEntryHandler)
        {
            if(!plMsg.tryI32 (MamdaOrderBookFields::ENTRY_REASON, mPriceLevelEntryReason))
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaBookAtomicListener: no price level entry reason present");
            }
            const char* tempChar;
            if(plMsg.tryString (MamdaOrderBookFields::ENTRY_ID, tempChar))
            {
                mPriceLevelEntryId = tempChar;
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "MamdaBookAtomicListener: no price level entry ID present");
            }

            if (isRecap)
            {
                mLevelEntryHandler->onBookAtomicLevelEntryRecap (
                        subscription, mListener, plMsg, mListener);
            }
            else
            {
                mLevelEntryHandler->onBookAtomicLevelEntryDelta (
                        subscription, mListener, plMsg, mListener);
            }
        }
    }


    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::getLevelInfoAndEntries(
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        const MamaMsg&      plMsg, 
        bool                isRecap)
    { 
        if(!plMsg.tryPrice(MamdaOrderBookFields::PL_PRICE, mPriceLevelPrice))
        {
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "MamdaBookAtomicListener: no price level price present");
        }

        plMsg.tryF64(MamdaOrderBookFields::PL_SIZE,mPriceLevelSize);

        plMsg.tryF32(MamdaOrderBookFields::PL_NUM_ENTRIES,mPriceLevelNumEntries);

        plMsg.tryI32(MamdaOrderBookFields::PL_ACTION,mPriceLevelAction);

        plMsg.tryI32(MamdaOrderBookFields::PL_SIDE,mPriceLevelSide);

        if (!plMsg.tryDateTime(MamdaOrderBookFields::PL_TIME, mPriceLevelTime))
        {
            mPriceLevelTime = mEventTime;
        }
        
        
        plMsg.tryF64(MamdaOrderBookFields::PL_SIZE_CHANGE, mPriceLevelSizeChange);
        
        // Call the Price Level Handler if set
        if (mLevelHandler)
        {
            if (isRecap)
            {
                mLevelHandler->onBookAtomicLevelRecap (
                    subscription, mListener, msg, mListener);
            }
            else
            {
                mLevelHandler->onBookAtomicLevelDelta (
                    subscription, mListener, msg, mListener);
            }
        }    

        /* Handle entries. 
         * 
         * Note: the number of entries actually present may well
         * not add up to the PL_NUM_ENTRIES; it may be more than,
         * less than or equal to PL_NUM_ENTRIES.  For example, if
         * the delta is a price level update then PL_NUM_ENTRIES
         * indicates the total number of remaining entries whereas
         * the array of entries in the message will only contain
         * those that are being added/deleted/updated. Only if the
         * price level action is an add should the number of
         * entries match.
         */
        if (mLevelEntryHandler && mBookType!=2)
        {
            // clear entry cache
            clearLevelEntryFields();
            if (mBookType==1)
            {
                plMsg.tryI32 (MamdaOrderBookFields::ENTRY_ACTION,
                        mPriceLevelEntryAction);

                plMsg.tryI32 (MamdaOrderBookFields::ENTRY_REASON,
                        mPriceLevelEntryReason);

                mPriceLevelEntryId
                = plMsg.getString (MamdaOrderBookFields::ENTRY_ID);

                plMsg.tryU64 (MamdaOrderBookFields::ENTRY_SIZE,
                        mPriceLevelEntrySize);

                if (!plMsg.tryDateTime(MamdaOrderBookFields::ENTRY_TIME,
                        mPriceLevelEntryTime))
                {
                    mPriceLevelEntryTime=mPriceLevelTime;
                }

                if (isRecap)
                {
                    mLevelEntryHandler->onBookAtomicLevelEntryRecap (
                            subscription, mListener, msg, mListener);
                }
                else
                {
                    mLevelEntryHandler->onBookAtomicLevelEntryDelta (
                            subscription, mListener, msg, mListener);
                }
            }
            else
            {
                /* First try a single vector. */
                size_t  numEntriesInMsg = 0;
                const MamaMsg** msgEntries = NULL;
                plMsg.tryVectorMsg (MamdaOrderBookFields::PL_ENTRIES,
                        msgEntries,
                        numEntriesInMsg);
                if (numEntriesInMsg > 0)
                {
                    mPriceLevelActNumEntries = numEntriesInMsg;
                    for (size_t j = 0; j < numEntriesInMsg; j++)
                    {
                        const MamaMsg* entMsg = msgEntries[j];
        
                        entMsg->tryI32 (MamdaOrderBookFields::ENTRY_ACTION,
                                        mPriceLevelEntryAction);
        
                        entMsg->tryI32 (MamdaOrderBookFields::ENTRY_REASON,
                                        mPriceLevelEntryReason);
        
                        mPriceLevelEntryId 
                        = entMsg->getString (MamdaOrderBookFields::ENTRY_ID);
            
                        entMsg->tryU64 (MamdaOrderBookFields::ENTRY_SIZE,
                                        mPriceLevelEntrySize);
            
                        if (!entMsg->tryDateTime(MamdaOrderBookFields::ENTRY_TIME,
                                                mPriceLevelEntryTime))
                        {
                            mPriceLevelEntryTime=mPriceLevelTime;
                        }
                
                        if (isRecap)
                        {
                            mLevelEntryHandler->onBookAtomicLevelEntryRecap (
                                    subscription, mListener, msg, mListener);
                        }
                        else
                        {
                            mLevelEntryHandler->onBookAtomicLevelEntryDelta (
                                subscription, mListener, msg, mListener);
                        }
                    }
                }
                else
                {
                    /* Second, try the list of entries. */
        
                    // PRICE_ENTRY_LENGTH may be 0 but we want to go into the
                    // below loop at least once for flattened messages scenario
                    int16_t maxEntryFields = MamdaOrderBookFields::PL_ENTRY_LENGTH
                                                ? MamdaOrderBookFields::PL_ENTRY_LENGTH : 1;
                    int16_t numEntryAttached = 1;
                    plMsg.tryI16(MamdaOrderBookFields::PL_NUM_ATTACH,
                            numEntryAttached);
                    
                    if (numEntryAttached < maxEntryFields)
                    {
                        maxEntryFields = numEntryAttached;
                    }
                    mPriceLevelActNumEntries = maxEntryFields;
                    for (int16_t j = 1; j <= maxEntryFields; j++)
                    {
                        const MamaMsg* entMsg = NULL;
        
                        // The PL_ENTRY array may not exist
                        if(MamdaOrderBookFields::PL_ENTRY)
                        {                
                            plMsg.tryMsg(MamdaOrderBookFields::PL_ENTRY[j], entMsg);
                        }
                        else
                        {
                            mama_log (MAMA_LOG_LEVEL_FINEST,
                            "MamdaAtomicBookListener: no RV pl entry fields in the dictionary");
                        }
        
                        if ((entMsg == NULL) && (numEntryAttached == 1))
                        {
                            /* Price level fields are probably be in the
                            * main message. */
                            mama_log (MAMA_LOG_LEVEL_FINEST,
                                      "MamdaOrderBookListener: "
                                      "expecting entry in level (or main) message...");
                            entMsg = &plMsg;
                        }
                        if (entMsg != NULL)
                        {
                            entMsg->tryI32 (MamdaOrderBookFields::ENTRY_ACTION,
                                            mPriceLevelEntryAction);
        
                            entMsg->tryI32 (MamdaOrderBookFields::ENTRY_REASON,
                                            mPriceLevelEntryReason);
                
                            mPriceLevelEntryId
                            = entMsg->getString (MamdaOrderBookFields::ENTRY_ID);
                            
                            entMsg->tryU64 (MamdaOrderBookFields::ENTRY_SIZE,
                                            mPriceLevelEntrySize);
                            
                            if (!entMsg->tryDateTime(MamdaOrderBookFields::ENTRY_TIME,
                                                    mPriceLevelEntryTime))
                            {
                                mPriceLevelEntryTime=mPriceLevelTime;
                            }
                            
                            if (isRecap)
                            {
                                mLevelEntryHandler->onBookAtomicLevelEntryRecap (
                                    subscription, mListener, msg, mListener);
                            }
                            else
                            {
                                mLevelEntryHandler->onBookAtomicLevelEntryDelta (
                                    subscription, mListener, msg, mListener);
                            }
                        }
                    }
                }
            }
        }  
    }


    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::invokeGapHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mBookHandler)
        {
            mBookHandler->onBookAtomicGap (subscription, mListener, 
                        msg, mListener);
        }
    }

    void MamdaBookAtomicListener::
    MamdaBookAtomicListenerImpl::invokeClearHandlers (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg)
    {
        if (mBookHandler)
        {
            mBookHandler->onBookAtomicClear (subscription, mListener, msg);
        }
    }

    void MamdaBookAtomicListener::MamdaBookAtomicListenerImpl::clearLevelFields()
    {
        mPriceLevel                = 0;
        mPriceLevelPrice.clear(); 
        mPriceLevelSize            = 0.0;
        mPriceLevelNumEntries      = 1;
        mPriceLevelAction          = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        mPriceLevelSide            = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
        mPriceLevelSizeChange      = 0;
        mPriceLevelActNumEntries   = 0; 
        mPriceLevelTime.clear();
        mOrderType                 = MamdaOrderBookTypes::MAMDA_BOOK_LEVEL_LIMIT;
        clearLevelEntryFields();
    }

    void 
    MamdaBookAtomicListener::MamdaBookAtomicListenerImpl::clearLevelEntryFields()
    {

        mPriceLevelEntryAction     = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE;
        mPriceLevelEntryReason     = MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN;
        mPriceLevelEntrySize       = 0;
        mPriceLevelEntryId         = "";
        mPriceLevelEntryTime.clear();
    }

} // namespace
