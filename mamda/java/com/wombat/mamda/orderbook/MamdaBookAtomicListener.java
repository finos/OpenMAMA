/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.*;
import com.wombat.mama.*;

import java.util.Vector;
import java.util.Iterator;
import java.util.*;
import java.util.logging.Logger;
import java.util.logging.Level;

public class MamdaBookAtomicListener implements MamdaMsgListener,
                                                MamdaBookAtomicLevel,
                                                MamdaBookAtomicLevelEntry,
                                                MamdaBookAtomicGap
{

    private static Logger mLogger =
                   Logger.getLogger("com.wombat.mamda.MamdaBookAtomicListener");


    MamdaBookAtomicBookHandler        mBookHandler;
    MamdaBookAtomicLevelHandler       mLevelHandler;
    MamdaBookAtomicLevelEntryHandler  mLevelEntryHandler;

    String             mSymbol                  = null;
    String             mPartId                  = null;
    MamaDateTime       mSrcTime                 = new MamaDateTime ();
    MamaDateTime       mActivityTime            = new MamaDateTime ();
    MamaDateTime       mLineTime                = new MamaDateTime ();
    MamaDateTime       mSendTime                = new MamaDateTime ();
    
    MamdaFieldState    mSymbolFieldState        = new MamdaFieldState();
    MamdaFieldState    mPartIdFieldState        = new MamdaFieldState();
    MamdaFieldState    mSrcTimeFieldState       = new MamdaFieldState ();
    MamdaFieldState    mActivityTimeFieldState  = new MamdaFieldState ();
    MamdaFieldState    mLineTimeFieldState      = new MamdaFieldState ();
    MamdaFieldState    mSendTimeFieldState      = new MamdaFieldState ();
    MamdaFieldState    mEventTimeFieldState     = new MamdaFieldState ();
    MamdaFieldState    mEventSeqNumFieldState   = new MamdaFieldState ();
    
    short              mMsgQual                 = MamaQuality.QUALITY_UNKNOWN;
    MamaDateTime       mEventTime               = new MamaDateTime ();
    long               mEventSeqNum             = 0;

    long               mPriceLevels             = 0;
    long               mPriceLevel              = 0;
    MamaPrice          mPriceLevelPrice         = new MamaPrice ();
    double             mPriceLevelSize          = 0.0;
    double             mPriceLevelSizeChange    = 0.0;
    long               mPriceLevelAction        = MamdaOrderBookPriceLevel.ACTION_ADD; //char
    long               mPriceLevelSide          = MamdaOrderBookPriceLevel.SIDE_BID; //char
    MamaDateTime       mPriceLevelTime          = new MamaDateTime ();
    double             mPriceLevelNumEntries    = 1.0;
    long               mPriceLevelActNumEntries = 0;
    long               mPriceLevelEntryAction   = MamdaOrderBookEntry.ACTION_DELETE; //char
    long               mPriceLevelEntryReason   = MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN; //char
    String             mPriceLevelEntryId       = "";
    long               mPriceLevelEntrySize     = 0;
    MamaDateTime       mPriceLevelEntryTime     = new MamaDateTime ();
    private Vector     mEntries                 = new Vector ();
    private MamaSourceDerivative       mSourceDeriv= null;

    char      mOrderType = MamdaOrderBookTypes.MAMDA_BOOK_LEVEL_LIMIT;

    // Additional fields for gaps
    long      mGapBegin;
    long      mGapEnd;
    
    boolean   mHasMarketOrders      = false;
	boolean   mProcessMarketOrders  = false;
	byte      mBookType			    = 0;
	MamdaOrderBook       mOrderBook = new MamdaOrderBook();
	MamdaOrderBookEntry  mEntry     = new MamdaOrderBookEntry();


    // Reuasble mama data objects
    MamaInteger reusableMamaInteger = new MamaInteger();
    MamaDouble  reusableMamaDouble  = new MamaDouble ();
    MamaFloat   reusableMamaFloat   = new MamaFloat ();
    MamaString  reusableMamaString  = new MamaString ();
    MamaLong    reusableMamaLong    = new MamaLong ();
    MamaShort   reusableMamaShort   = new MamaShort ();

    MamaMessage reusableMessage     = new MamaMessage();
    MamaArrayMsg reusableArrayMsg   = new MamaArrayMsg ();

    /**
     * Returns a <code>java.util.Iterator</code> for all entries within this
     * level.
     * Price Level entries are represented by the
     * <code>MamdaOrderBookEntry</code> class.
     *
     * @return Iterator The iterator for the price level entries
     */
    public Iterator entryIterator ()
    {
      return mEntries.iterator();
    }
    
    public void addBookHandler (MamdaBookAtomicBookHandler  handler)
    {
        mBookHandler = handler;
    }

    public void addLevelHandler (MamdaBookAtomicLevelHandler  handler)
    {
        mLevelHandler = handler;
    }

    public void addLevelEntryHandler (MamdaBookAtomicLevelEntryHandler  handler)
    {
        mLevelEntryHandler = handler;
    }

    public String getSymbol ()
    {
        return mSymbol;
    }

    public String getPartId ()
    {
        return mPartId;
    }

    public MamaDateTime getSrcTime()
    {
        return mSrcTime;
    }

    public MamaDateTime getActivityTime()
    {
        return mActivityTime;
    }

    public MamaDateTime getLineTime()
    {
        return mLineTime;
    }

    public MamaDateTime getSendTime()
    {
        return mSendTime;
    }

    public short getMsgQual ()
    {
        return mMsgQual;
    }

    public long getEventSeqNum()
    {
        return mEventSeqNum;
    }

    public MamaDateTime getEventTime()
    {
        return mEventTime;
    }

    public long getPriceLevelNumLevels ()
    {
        return mPriceLevels;
    }

    public long getPriceLevelNum ()
    {
        return mPriceLevel;
    }

    public double getPriceLevelPrice ()
    {
        return mPriceLevelPrice.getValue();
    }
    
    public MamaPrice getPriceLevelMamaPrice ()
    {
      return mPriceLevelPrice;
    }

    public double getPriceLevelSize ()
    {
        return mPriceLevelSize;
    }

    public long getPriceLevelSizeChange ()
    {
        // check for NaN
        if(Double.isNaN(mPriceLevelSizeChange))
        {
            mPriceLevelSizeChange = 0.0;
        }
        return (long) mPriceLevelSizeChange;
    }

    public char getPriceLevelAction ()
    {
        return (char)mPriceLevelAction;
    }

    public char getPriceLevelSide ()
    {
        return (char)mPriceLevelSide;
    }

    public MamaDateTime getPriceLevelTime()
    {
        return mPriceLevelTime;
    }

    public double getPriceLevelNumEntries ()
    {
        return mPriceLevelNumEntries;
    }

    public void setOrderType (char orderType)
    {
        mOrderType = orderType;
    }

    public char getOrderType ()
    {
        return mOrderType;
    }

    public boolean getHasMarketOrders ()
    {
        return mHasMarketOrders;
    }
    
    public void setProcessMarketOrders (boolean process)
    {
        mProcessMarketOrders = process;
    }

    public long getPriceLevelActNumEntries ()
    {
        return mPriceLevelActNumEntries;
    }

    public char getPriceLevelEntryAction ()
    {
        return (char)mPriceLevelEntryAction;
    }

    public char getPriceLevelEntryReason ()
    {
        return (char)mPriceLevelEntryReason;
    }

    public String getPriceLevelEntryId ()
    {
        return mPriceLevelEntryId;
    }

    public long getPriceLevelEntrySize ()
    {
        return mPriceLevelEntrySize;
    }

    public MamaDateTime getPriceLevelEntryTime()
    {
        return mPriceLevelEntryTime;
    }

    public long getBeginGapSeqNum()
    {
        return mGapBegin;
    }

    public long getEndGapSeqNum()
    {
        return mGapEnd;
    }

    public void setPriceLevel (long priceLevel)
    {
        mPriceLevel = priceLevel;
    }
    
    public boolean isVisible ()
    {
        return getSourceState() == MamaSourceState.OK;
    }


    // FieldState
    
    public short getSymbolFieldState ()
    {
        return mSymbolFieldState.getState();
    }

    public short getPartIdFieldState ()
    {
        return mPartIdFieldState.getState();
    }

    public short getSrcTimeFieldState()
    {
        return mSrcTimeFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
        return mActivityTimeFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return mLineTimeFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return mSendTimeFieldState.getState();
    }
    
    public short getEventTimeFieldState()
    {
        return mEventTimeFieldState.getState();
    }

    public short getEventSeqNumFieldState()
    {
        return mEventSeqNumFieldState.getState();
    }
    
    /**
     * Get whether this entry is "visible" in this book.  Visibilty is
     * controlled by the status of the MamaSourceDerivative for the entry.
     *
     * @return  Whether the entry is visible.
     */
    private MamaSourceState getSourceState ()
    {
        return mSourceDeriv != null ? mSourceDeriv.getState()
            : MamaSourceState.OK;
    }
    
    
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaBookAtomicBookListener: got message (type=" +
                             msgType + ")");

        if (!MamdaOrderBookFields.isSet())
        {
            mLogger.warning ("MamdaOrderBookFields::setDictionary() has not been called.");
            return;
        }

        try
        {
            switch (msgType)
            {
                case MamaMsgType.TYPE_BOOK_UPDATE:
                    handleUpdate (subscription, msg);
                    break;
			    case MamaMsgType.TYPE_BOOK_CLEAR:
                    handleClear (subscription, msg);
                    break;
                case MamaMsgType.TYPE_BOOK_INITIAL:
                case MamaMsgType.TYPE_BOOK_RECAP:
                case MamaMsgType.TYPE_BOOK_SNAPSHOT:
                    handleRecap (subscription, msg);
                    break;
            }
        }
        catch (MamaException e)
        {
            mLogger.info ( "MamdaBookAtomicListener: caught MamaStatus exception: " +
                      e.toString());
        }
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaBookAtomicListener: done with message");
    }


    private void handleClear (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        handleStandardFields (subscription, msg, false);
        invokeClearHandlers  (subscription, msg);
    }

    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        handleStandardFields   (subscription, msg, false);
        createDeltaFromMamaMsg (subscription, msg, true);
    }

    private void handleUpdate (MamdaSubscription  subscription,
                               MamaMsg            msg)
    {
        handleStandardFields   (subscription, msg, false);
        createDeltaFromMamaMsg (subscription, msg, false);
    }

    private void handleStandardFields (MamdaSubscription subscription,
                                       MamaMsg           msg,
                                       boolean           checkSeqNum)
    {
        if (msg.tryString (MamdaCommonFields.SYMBOL, reusableMamaString))
        {
            mSymbol = reusableMamaString.getValue();
            mSymbolFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
        	mSymbol = "";
            mSymbolFieldState.setState (MamdaFieldState.NOT_INITIALISED);
            
            if (mLogger.isLoggable (Level.FINEST))
                mLogger.finest ("MamdaAtomicBookListener::getSymbolAndPartId: cannot find symbol field in message");
        }

        if (!msg.tryString (MamdaCommonFields.PART_ID, reusableMamaString) && (mSymbol != null))
        {
            // No explicit part ID in message, but maybe in symbol.
            int lastDot = mSymbol.indexOf (".");
            if (lastDot != -1)
            {
                lastDot++;
                if (lastDot != mSymbol.length ())
                {
                    mPartId = mSymbol.substring (lastDot);
                    mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
                }
                else
                {
                	mPartId = "";
                    mPartIdFieldState.setState (MamdaFieldState.NOT_INITIALISED);
                    if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaAtomicBookListener::getSymbolAndPartId: cannot find part id field in message");                	
                }
            }
            else
            {
            	mPartId = "";
                mPartIdFieldState.setState (MamdaFieldState.NOT_INITIALISED);
                if (mLogger.isLoggable (Level.FINEST))
                    mLogger.finest ("MamdaAtomicBookListener::getSymbolAndPartId: cannot find part id field in message");            	
            }
        }
        else
        {
            mPartId = reusableMamaString.getValue();
            mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
        
        if (msg.tryDateTime (MamdaCommonFields.SRC_TIME, mSrcTime))
        {
        	mSrcTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
        	mSrcTime.clear();
        	mSrcTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);        	
        }

        if (msg.tryDateTime (MamdaCommonFields.ACTIVITY_TIME, mActivityTime))
        {
        	mActivityTimeFieldState.setState(MamdaFieldState.MODIFIED);
        }
        else
        {
        	mActivityTime.clear();
        	mActivityTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        	
        }

        if (msg.tryDateTime (MamdaCommonFields.LINE_TIME, mLineTime))
        {
        	mLineTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
        	mLineTime.clear();
        	mLineTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        }
        
        if (MamdaCommonFields.SEND_TIME!=null && 
        		msg.tryDateTime (MamdaCommonFields.SEND_TIME, mSendTime))
        {
        	mSendTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
        	mSendTime.clear();
        	mSendTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);        	
        }

        if (MamdaCommonFields.MSG_QUAL!=null &&
        		msg.tryU16 (MamdaCommonFields.MSG_QUAL, reusableMamaInteger))
        {
        	mMsgQual = (short) reusableMamaInteger.getValue();        	
        }
        
        if (msg.tryDateTime (MamdaOrderBookFields.BOOK_TIME, mEventTime))
        {
        	mEventTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
        else
        {
            mEventTime.copy(mSrcTime);
            mEventTimeFieldState.setState (mSrcTimeFieldState.getState());
        }

        long  seqNum = msg.getSeqNum();
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
    
    private void createDeltaFromPriceLevel (MamdaOrderBookPriceLevel priceLevel,
                                            MamdaSubscription        subscription,
                                            MamaMsg                  msg,
                                            boolean                  isRecap)
    {
        mPriceLevelPrice  = priceLevel.getPrice();
        mPriceLevelSide   = priceLevel.getSide();
        mPriceLevelAction = priceLevel.getAction();

        /*
         * With 2.16 PL_NUM_ENTRIES was sent and populated the NumEntries field and
         * the ActNumEntries field was populated with the actual number of sub messages
         * under the price level. Both populated for V5 as same since PL_NUM_ENTRIES
         * not sent
         */
        mPriceLevelNumEntries    = priceLevel.getNumEntries();
        mPriceLevelActNumEntries = (long) priceLevel.getNumEntries();

        /* 2.16 populates sizeChange with size on initial and this gives same behaviour */
        mPriceLevelSize       = priceLevel.getSize();
        mPriceLevelSizeChange = priceLevel.getSize();
        mPriceLevelTime       = priceLevel.getTime();

        if (mLevelHandler != null)
        {
            if (isRecap)
            {
                mLevelHandler.onBookAtomicLevelRecap (
                        subscription, this, msg, this);
            }
            else
            {
                mLevelHandler.onBookAtomicLevelDelta (
                        subscription, this, msg, this);
            }
        }

        if (mLevelEntryHandler != null)
        {
            Iterator entryIt = priceLevel.entryIterator();

            MamdaOrderBookEntry entry;
            MamaMsg             entryMsg;

            while (entryIt.hasNext())    		
            {
                    entry = (MamdaOrderBookEntry) entryIt.next();
                    clearLevelEntryFields();

                    mPriceLevelEntryAction = entry.getAction();
                    mPriceLevelEntryReason = entry.getReason();
                    mPriceLevelEntryId     = entry.getId();
                    mPriceLevelEntryTime   = entry.getTime();
                    mPriceLevelEntrySize   = (long) entry.getSize();

                    entryMsg = (MamaMsg) entry.getClosure();

                    if (entryMsg != null)
                    {
                    if (isRecap)
                    {
                        mLevelEntryHandler.onBookAtomicLevelEntryRecap (
                                subscription, this, entryMsg, this);
                    }
                    else
                    {
                        mLevelEntryHandler.onBookAtomicLevelEntryDelta (
                                subscription, this, entryMsg, this);
                    }
                }
                else
                {
                    mLogger.severe ("MamdaBookAtomicListener::createDeltaFromPriceLevel() : MamaMsg is null. symbol " 
                    		+ mSymbol +
                    		" price level "
                    		+ mPriceLevelPrice.getValue() + 
                    		" entry id  " 
                    		+ mPriceLevelEntryId);
                }
            }
	    }
    }
    
    private void createDeltaFromOrderBook (MamdaSubscription subscription,
                                           MamaMsg           msg,
                                           boolean           isRecap)
    {
        int i = 0;
        Iterator askIt = mOrderBook.askIterator();
        Iterator bidIt = mOrderBook.bidIterator();

        mPriceLevels = mOrderBook.getTotalNumLevels();

        while(askIt.hasNext())
        {
            clearLevelFields();
            mPriceLevel = ++i;

            createDeltaFromPriceLevel(
		            (MamdaOrderBookPriceLevel)askIt.next(), 
		            subscription, 
		            msg, 
		            isRecap);
        }  

        while(bidIt.hasNext())
        {
            clearLevelFields();
            mPriceLevel = ++i;

            createDeltaFromPriceLevel(
		            (MamdaOrderBookPriceLevel)bidIt.next(), 
		            subscription, 
		            msg, 
		            isRecap);
        }   
    }

    private void populateOrderBookFromEntryMsg (MamaMsg entryMsg)
    {
        MamaDateTime mTmpDateTime;
        clearLevelFields();
        clearLevelEntryFields();

        entryMsg.tryPrice(MamdaOrderBookFields.PL_PRICE, mPriceLevelPrice);
        mPriceLevelSide        = entryMsg.getI32      (MamdaOrderBookFields.PL_SIDE);
        mPriceLevelEntryId     = entryMsg.getString   (MamdaOrderBookFields.ENTRY_ID);
        mPriceLevelEntryAction = entryMsg.getI32      (MamdaOrderBookFields.ENTRY_ACTION);
        mPriceLevelEntrySize   = entryMsg.getU64      (MamdaOrderBookFields.ENTRY_SIZE);
        mTmpDateTime           = entryMsg.getDateTime (MamdaOrderBookFields.ENTRY_TIME);
        mPriceLevelEntryReason = entryMsg.getI32      (MamdaOrderBookFields.ENTRY_REASON);

        // We create object and then clearing the book will clean up
        // This takes shallow copy so we do not use mPriceLevelTime as it gets cleared    	
        mEntry = new MamdaOrderBookEntry(
		        mPriceLevelEntryId,
		        mPriceLevelEntrySize,
		        (char)mPriceLevelEntryAction,
		        mTmpDateTime,
		        (MamaSourceDerivative) null);

        mEntry.setReason((char)mPriceLevelEntryReason);
        mEntry.setClosure((Object)entryMsg);

        // Currently V5 does not send status
        mOrderBook.addEntry(
		        mEntry,
		        mPriceLevelPrice.getValue(),
		        (char) mPriceLevelSide,
		        mTmpDateTime,
		        (MamdaOrderBookBasicDelta) null);
    }

    private void createDeltaFromMamaMsg (MamdaSubscription subscription,
                                         MamaMsg msg,
                                         boolean isRecap)
    {
        long mNumLevels = 1;

        if (msg.tryU32 (MamdaOrderBookFields.NUM_LEVELS, reusableMamaLong))
        {
            mNumLevels = reusableMamaLong.getValue();
        }

        //single Mkt Order Update
        if ((!mProcessMarketOrders) && (!isRecap) && (mNumLevels == 0))
        {
            if (mLogger.isLoggable (Level.FINEST))
                mLogger.finest ("MamdaAtomicBookListener: Market Order Update deliberately not processed");
            return;
        }
        
        if (mBookHandler != null)
        {
            mBookHandler.onBookAtomicBeginBook (subscription, this, isRecap);
        }

        // Note: the following test checks whether vector fields are part
        // of the data dictionary.  However, the vector fields may indeed
        // be part of the data dictionary, but the message may still
        // contain the non-vector style order book data.

        mBookType = 0;

        if(msg.tryU8 (MamdaOrderBookFields.BOOK_TYPE, reusableMamaShort))
        {
        	mBookType = (byte)reusableMamaShort.getValue();
        }

        if (mBookType==1)
        {
            // this is an entry only, Flatbook which V5 sends as vector of PRICE_LEVELS
            clearLevelFields();
                        
            if (msg.tryArrayMsg (MamdaOrderBookFields.PRICE_LEVELS, reusableArrayMsg))
            {            	
            	if(reusableArrayMsg.getValue() != null)
            	{
            		if(isRecap)
            		{
            			/*
            			 * V5 entry book initials/recaps are unstructured book so we must
            			 * build up a book to fire callbacks appropriately
            			 */
            			mOrderBook.clear();

            			for (int i = 0; i < reusableArrayMsg.getValue().length; ++i)
            			{
            				populateOrderBookFromEntryMsg(reusableArrayMsg.getValue()[i]);
            			}

            			createDeltaFromOrderBook(subscription, msg, isRecap);
            		}
            		else
            		{
            			/*
            			 * V5 sends complex deltas of entries for single price
            			 * level (synthetic books in CTA/UTP)
            			 */
            			mPriceLevels = reusableArrayMsg.getValue().length;
            			mPriceLevel = 0;
            			for (int i = 0; i < reusableArrayMsg.getValue().length; ++i)
            			{
            		    	clearLevelFields();

            				mPriceLevel=i+1;

            				getInfoFromEntry(
            						subscription, 
            						msg, 
            						reusableArrayMsg.getValue()[i], 
            						isRecap);
            			}
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
            
            if (msg.tryArrayMsg (MamdaOrderBookFields.PRICE_LEVELS, reusableArrayMsg))
            {            	
            	if(reusableArrayMsg.getValue() != null)
            	{
            		mPriceLevels = reusableArrayMsg.getValue().length;
            		for (int i = 0; i < reusableArrayMsg.getValue().length; ++i)
            		{
            			clearLevelFields();
            			mPriceLevel = i+1;
            			getLevelInfoAndEntries(
            					subscription, 
            					msg, 
            					reusableArrayMsg.getValue()[i], 
            					isRecap);
            		}
            	}
            }
            else
            {
                mPriceLevels = 1;
                getLevelInfoAndEntries(subscription, msg, msg, isRecap);
            }
        }
        else if (MamdaOrderBookFields.getHasVectorFields())
        {
            if (mProcessMarketOrders)
            {
                //Process Market Orders
                MamaArrayMsg bidMsgArray = new MamaArrayMsg();
                MamaArrayMsg askMsgArray = new MamaArrayMsg();
                mHasMarketOrders = false;

                if (msg.tryArrayMsg (MamdaOrderBookFields.BID_MARKET_ORDERS, bidMsgArray))
                {
                    mHasMarketOrders = true;
                    mOrderType = MamdaOrderBookTypes.MAMDA_BOOK_LEVEL_MARKET;
                    if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaAtomicBookListener: processing Bid side Market Orders");

                    MamaMsg bidMsg;
                    bidMsg = bidMsgArray.getValue()[0];
                    createDeltaFromMamaMsgWithoutVectorFields (subscription, bidMsg, isRecap, true);
                }

                if (msg.tryArrayMsg (MamdaOrderBookFields.ASK_MARKET_ORDERS, askMsgArray))
                {
                    mHasMarketOrders = true;
                    mOrderType = MamdaOrderBookTypes.MAMDA_BOOK_LEVEL_MARKET;
                    if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaAtomicBookListener: processing Ask side Market Orders");

                    MamaMsg askMsg;
                    askMsg = askMsgArray.getValue()[0];
                    createDeltaFromMamaMsgWithoutVectorFields (subscription, askMsg, isRecap, true);
                }

                //single Mkt Order Update
                if ((mHasMarketOrders) && (!isRecap) && (mNumLevels == 0))
                {
                    if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaAtomicBookListener: simple Market Order update");

                    if (mBookHandler!=null)
                    {
                        mBookHandler.onBookAtomicEndBook (subscription, this);
                    }
                    return;
                }
            }
            //End process Mkt Orders

            if (msg.tryArrayMsg (MamdaOrderBookFields.PRICE_LEVELS, reusableArrayMsg))
            {
                if(reusableArrayMsg.getValue() != null)
                {
                    createDeltaFromMamaMsgWithVectorFields (subscription,
                                                            msg,
                                                            reusableArrayMsg.getValue(),
                                                            reusableArrayMsg.getValue().length,
                                                            isRecap);
                }
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

        if (mBookHandler != null)
        {
            mBookHandler.onBookAtomicEndBook (subscription, this);
        }
    }

    private void  createDeltaFromMamaMsgWithVectorFields (MamdaSubscription subscription,
                                                          MamaMsg           msg,
                                                          MamaMsg[]         msgLevels,
                                                          long              msgLevelsNum,
                                                          boolean           isRecap)
    {
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaBookAtomicListener: processing msg with vector fields.");

        mPriceLevels = msgLevelsNum;

        for (int i = 0; i < msgLevelsNum; i++)
        {
            clearLevelFields();
            mPriceLevel = i+1;
            MamaMsg plMsg = msgLevels[i];
            getLevelInfoAndEntries(subscription, msg, plMsg, isRecap);
        }
    }


    private void  createDeltaFromMamaMsgWithoutVectorFields (MamdaSubscription subscription,
                                                             MamaMsg           msg,
                                                             boolean           isRecap,
                                                             boolean           isMarketOrder)
    {
        if (mLogger.isLoggable (Level.FINEST))
            mLogger.finest ("MamdaBookAtomicListener: processing msg without vector fields.");

        long numLevelFieldInMsg = 1;

        if (msg.tryU32 (MamdaOrderBookFields.NUM_LEVELS, reusableMamaLong))
        {
            numLevelFieldInMsg = reusableMamaLong.getValue();
        }
        else
        {
            if (isMarketOrder == false)
            {
                // the NumLevels field should be present for non-vector book messages
                if (mLogger.isLoggable (Level.FINEST))
                    mLogger.finest ("MamdaBookAtomicListener: no NUM_LEVELS found!");
            }
        }

        int maxLevelFields = MamdaOrderBookFields.getNumLevelFields();

        // getNumLevelFields() defaults to 0 but we want to go into the
        // below loop at least once for flattened messages scenario
        if (maxLevelFields == 0)
        {
            maxLevelFields = 1;
        }

        if (numLevelFieldInMsg < maxLevelFields)
        {
            maxLevelFields =  (int) numLevelFieldInMsg;
        }

        mPriceLevels = maxLevelFields;

        for (int i = 1; i <= maxLevelFields; i++)
        {
            clearLevelFields();
            mPriceLevel = i;
            MamaMsg plMsg = null;

            if (MamdaOrderBookFields.PRICE_LEVEL.length > 1)
            {
                if (msg.tryMsg ((MamdaOrderBookFields.PRICE_LEVEL[i]), reusableMessage))
                {
                    plMsg = reusableMessage.getValue ();
                }
            }
            else
            {
                if (mLogger.isLoggable (Level.FINEST))
                    mLogger.finest ("MamdaBookAtomicListener: No RV wPriceLevel[X] fields in the dictionary");
            }

            if (plMsg == null)
            {
                if (numLevelFieldInMsg == 1)
                {
                    if (isMarketOrder == false)
                    {
                        // Price level fields are probably be in the main message.
                        if (mLogger.isLoggable (Level.FINEST))
                            mLogger.finest ("MamdaBookAtomicListener: expecting price level in main message");
                    }
                    plMsg = msg;
                }
                else
                {
                    if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaBookAtomicListener: cannot find price level fields in MamaMsg.");
                    break;
                }
            }
            getLevelInfoAndEntries (subscription, msg, plMsg, isRecap);
        }
    }


    private void getLevelInfoAndEntries (MamdaSubscription subscription,
                                         MamaMsg           msg,
                                         MamaMsg           plMsg,
                                         boolean           isRecap)
   {

        if ( !plMsg.tryPrice(MamdaOrderBookFields.PL_PRICE, mPriceLevelPrice) )
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level price present");
        }

        if ( plMsg.tryF64(MamdaOrderBookFields.PL_SIZE, reusableMamaDouble) )
        {
            mPriceLevelSize = reusableMamaDouble.getValue();
        }

        if ( plMsg.tryF32(MamdaOrderBookFields.PL_NUM_ENTRIES, reusableMamaFloat) )
        {
            mPriceLevelNumEntries = reusableMamaFloat.getValue();
        }

        if ( plMsg.tryI32(MamdaOrderBookFields.PL_ACTION, reusableMamaInteger))
        {
            mPriceLevelAction = reusableMamaInteger.getValue();
        }

        if ( plMsg.tryI32(MamdaOrderBookFields.PL_SIDE, reusableMamaInteger))
        {
            mPriceLevelSide = reusableMamaInteger.getValue();
        }
 
        // Optional order book fields
        if (!plMsg.tryDateTime(MamdaOrderBookFields.PL_TIME, mPriceLevelTime))
        {
            mPriceLevelTime.copy(mEventTime);
        }

        if ( plMsg.tryF64(MamdaOrderBookFields.PL_SIZE_CHANGE, reusableMamaDouble) )
        {
            mPriceLevelSizeChange = reusableMamaDouble.getValue();
        }

        // Call the Price Level Handler if set
        if (mLevelHandler!=null)
        {
            if (isRecap)
            {
                mLevelHandler.onBookAtomicLevelRecap (
                                subscription, this, msg, this);
            }
            else
            {
                mLevelHandler.onBookAtomicLevelDelta (
                                subscription, this, msg, this);
            }
        }

        // Handle entries.
        //
        // Note: the number of entries actually present may well
        // not add up to the PL_NUM_ENTRIES; it may be more than,
        // less than or equal to PL_NUM_ENTRIES.  For example, if
        // the delta is a price level update then PL_NUM_ENTRIES
        // indicates the total number of remaining entries whereas
        // the array of entries in the message will only contain
        // those that are being added/deleted/updated. Only if the
        // price level action is an add should the number of
        // entries match.
        //

        if (mLevelEntryHandler!=null)
        {
            // clear entry cache
            clearLevelEntryFields();

            // First try a single vector.
            int  numEntriesInMsg = 0;
            MamaMsg[] msgEntries = null;

            if (plMsg.tryArrayMsg (MamdaOrderBookFields.PL_ENTRIES, reusableArrayMsg))
            {
                if (reusableArrayMsg.getValue() != null)
                {
                    msgEntries = reusableArrayMsg.getValue();
                    numEntriesInMsg = msgEntries.length;
                }
            }

            if (numEntriesInMsg > 0)
            {
                mPriceLevelActNumEntries = numEntriesInMsg;

                for (int j = 0; j < numEntriesInMsg; j++)
                {
                    MamaMsg entMsg = msgEntries[j];

                    if (!getEntriesInfo (entMsg))
                    {
                        if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("entry not in level (or main) message");
                        return;
                    }

                    if (isRecap)
                    {
                        mLevelEntryHandler.onBookAtomicLevelEntryRecap (
                                    subscription, this, msg, this);
                    }
                    else
                    {
                        mLevelEntryHandler.onBookAtomicLevelEntryDelta (
                                    subscription, this, msg, this);
                    }
                }
            }
            else
            {
                // Second, try the list of entries.
                int maxEntryFields = MamdaOrderBookFields.getNumEntryFields ();
                int numEntryAttached = 1;

                // getNumEntryFields() defaults to 0 but we want to go into the
                // below loop at least once for flattened messages scenario
                if (maxEntryFields == 0)
                {
                    maxEntryFields = 1;
                }

                if ( plMsg.tryI32(MamdaOrderBookFields.PL_NUM_ATTACH, reusableMamaInteger))
                {
                    numEntryAttached = reusableMamaInteger.getValue();
                }

                if (numEntryAttached < maxEntryFields)
                {
                    maxEntryFields = numEntryAttached;
                }

                mPriceLevelActNumEntries = maxEntryFields;

                for (int j = 1; j <= maxEntryFields; j++)
                {
                    MamaMsg entMsg = null;

                    if (MamdaOrderBookFields.PL_ENTRY.length > 1)
                    {
                        if (plMsg.tryMsg ((MamdaOrderBookFields.PL_ENTRY[j]), reusableMessage))
                        {
                            entMsg = reusableMessage.getValue ();
                        }
                    }
                    else
                    {
                        if (mLogger.isLoggable (Level.FINEST))
                        mLogger.finest ("MamdaBookAtomicListener: No RV wPlEntry[X] fields in the dictionary");
                    }

                    if ((entMsg == null) && (numEntryAttached == 1))
                    {
                        // Price level fields are probably be in the main message.
                        if (mLogger.isLoggable (Level.FINEST))
                            mLogger.finest ("expecting entry in level (or main) message");
                        entMsg = plMsg;
                    }

                    if (entMsg != null)
                    {
                        if (!getEntriesInfo (entMsg))
                        {
                            if (mLogger.isLoggable (Level.FINEST))
                            mLogger.finest ("entry not in level (or main) message");
                            return;
                        }

                        if (isRecap)
                        {
                            mLevelEntryHandler.onBookAtomicLevelEntryRecap (
                                subscription, this, msg, this);
                        }
                        else
                        {
                            mLevelEntryHandler.onBookAtomicLevelEntryDelta (
                                subscription, this, msg, this);
                        }
                    }
                }
            }
        }
    }

    private boolean getEntriesInfo (MamaMsg entMsg)
    {
        if ( entMsg.tryI32 (MamdaOrderBookFields.ENTRY_ACTION, reusableMamaInteger))
        {
            mPriceLevelEntryAction = reusableMamaInteger.getValue();
        }

        if ( entMsg.tryI32 (MamdaOrderBookFields.ENTRY_REASON, reusableMamaInteger))
        {
            mPriceLevelEntryReason = reusableMamaInteger.getValue();
        }

        if ( entMsg.tryU64 (MamdaOrderBookFields.ENTRY_SIZE, reusableMamaLong))
        {
            mPriceLevelEntrySize = reusableMamaLong.getValue();
        }

        if (!entMsg.tryDateTime (MamdaOrderBookFields.ENTRY_TIME, mPriceLevelEntryTime))
        {
            mPriceLevelEntryTime.copy(mPriceLevelTime);
        }

        // C++ has essentially used this field to stop the entry callbacks being invoked
        if ( entMsg.tryString (MamdaOrderBookFields.ENTRY_ID, reusableMamaString))
        {
            mPriceLevelEntryId = reusableMamaString.getValue();
            return true;
        }
        else
        {
            return false;
        }
    }
    
    private void getInfoFromEntry (MamdaSubscription  subscription,
                                   MamaMsg      	  msg,
                                   MamaMsg            plMsg,
                                   boolean            isRecap)
    {
        clearLevelEntryFields();

        /* The number of entries at the price level is unknown in V5 */
        mPriceLevelNumEntries = 0;

        /* This is the actual number of entries for this price level in this message (plMsg) */
        mPriceLevelActNumEntries = 1;

        /* V5 does not send PriceLevel Action */
        mPriceLevelAction = MamdaOrderBookPriceLevel.ACTION_UNKNOWN;

        if (!plMsg.tryPrice (MamdaOrderBookFields.PL_PRICE, mPriceLevelPrice))
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level price present");
        }

        if (plMsg.tryI32 (MamdaOrderBookFields.PL_SIDE, reusableMamaInteger))
        {
            mPriceLevelSide = reusableMamaInteger.getValue();
        }
        else
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level side present");
        }

    	if (!plMsg.tryDateTime (MamdaOrderBookFields.ENTRY_TIME, mPriceLevelEntryTime))
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level entry time present");
        }

    	mPriceLevelTime = mPriceLevelEntryTime;

       	if (plMsg.tryI32 (MamdaOrderBookFields.ENTRY_ACTION, reusableMamaInteger))
        {
            mPriceLevelEntryAction = reusableMamaInteger.getValue();
        }
        else
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level entry action present");
        }

        if (plMsg.tryU64 (MamdaOrderBookFields.ENTRY_SIZE, reusableMamaLong))
        {
            mPriceLevelEntrySize = reusableMamaLong.getValue();
        }
        else
        {
            mLogger.finest ("MamdaBookAtomicListener: no price level entry size present");
        }

    	/*
    	 * Negate the entry size to get the price level size change
    	 * if the entry action is delete. Avoid branch using XOR
    	 */
    	mPriceLevelSizeChange = (mPriceLevelEntryAction == 'D') ? -mPriceLevelEntrySize : mPriceLevelEntrySize;

    	if (mLevelHandler != null)
    	{
            if (isRecap)
            {
                mLevelHandler.onBookAtomicLevelRecap (
                        subscription, this, plMsg, this);
            }
            else
            {
                mLevelHandler.onBookAtomicLevelDelta (
                        subscription, this, plMsg, this);
            }
    	}

    	if (mLevelEntryHandler != null)
    	{
            if (plMsg.tryI32 (MamdaOrderBookFields.ENTRY_REASON, reusableMamaInteger))
            {
                mPriceLevelEntryReason = reusableMamaInteger.getValue();
            }
            else
            {
                mLogger.finest ("MamdaBookAtomicListener: no price level entry reason present");
            }

            if (plMsg.tryString (MamdaOrderBookFields.ENTRY_ID, reusableMamaString))
            {
                mPriceLevelEntryId = reusableMamaString.getValue();
            }
            else
            {
                mLogger.finest ("MamdaBookAtomicListener: no price level entry ID present");
            }

            if (isRecap)
            {
                mLevelEntryHandler.onBookAtomicLevelEntryRecap (
                        subscription, this, plMsg, this);
            }
            else
            {
                mLevelEntryHandler.onBookAtomicLevelEntryDelta (
                        subscription, this, plMsg, this);
            }
    	}
    }


    private void invokeGapHandlers (MamdaSubscription subscription,
                                    MamaMsg           msg)
    {
        if (mBookHandler != null)
        {
            mBookHandler.onBookAtomicGap (subscription, this,
                        msg, this);
        }
    }

    private void invokeClearHandlers (MamdaSubscription subscription,
                                      MamaMsg           msg)
    {
        if (mBookHandler!=null)
        {
            mBookHandler.onBookAtomicClear (subscription, this, msg);
        }
    }

    private void clearLevelFields()
    {
        mPriceLevel                = 0;
        mPriceLevelPrice.clear();
        mPriceLevelSize            = 0.0;
        mPriceLevelNumEntries      = 1;
        mPriceLevelAction          = MamdaOrderBookPriceLevel.ACTION_ADD;
        mPriceLevelSide            = MamdaOrderBookPriceLevel.SIDE_BID;
        mPriceLevelSizeChange      = 0;
        mPriceLevelActNumEntries   = 0;
        mPriceLevelTime.clear();
        clearLevelEntryFields();
    }


    private void clearLevelEntryFields()
    {
        mPriceLevelEntryAction     = MamdaOrderBookEntry.ACTION_DELETE;
        mPriceLevelEntryReason     = MamdaOrderBookTypes.MAMDA_BOOK_REASON_UNKNOWN;
        mPriceLevelEntrySize       = 0;
        mPriceLevelEntryId         = "";
        mPriceLevelEntryTime.clear();
    }

}
