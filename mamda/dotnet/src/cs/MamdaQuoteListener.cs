/* $Id: MamdaQuoteListener.cs,v 1.20.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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

using System;
using System.Threading;
using System.Collections;

namespace Wombat
{
	/// <summary>
	/// MamdaQuoteListener is a class that specializes in handling quote
	/// updates.  Developers provide their own implementation of the
	/// MamdaQuoteHandler interface and will be delivered notifications for
	/// quotes and quote closing prices.  An obvious application for this
	/// MAMDA class is any kind of quote tick capture application.
	/// </summary>
	/// <remarks>
	/// Note: The MamdaQuoteListener class caches quote-related field
	/// values.  Among other reasons, caching of these fields makes it
	/// possible to provide complete quote-related callbacks, even when the
	/// publisher (e.g., feed handler) is only publishing deltas containing
	/// modified fields.
	/// </remarks>
	public class MamdaQuoteListener : 
		MamdaMsgListener,
		MamdaQuoteRecap,
		MamdaQuoteUpdate,
		MamdaQuoteGap,
		MamdaQuoteClosing
	{
		/// <summary>
		/// Create a specialized quote listener.  This listener handles
		/// quote updates, quote recaps, and quote gap notifications.
		/// </summary>
		public MamdaQuoteListener()
		{
			//Initialize the cache
			clearCache(mQuoteCache);
       
			//Single iterator instance needed for all messages
			mFieldIterator = new FieldIterator (this);
		}

		private static void clearCache(MamdaQuoteCache cache)
		{
			cache.mSymbol			= null;
			cache.mPartId			= null;
			cache.mSrcTime			= DateTime.MinValue;
			cache.mActTime			= DateTime.MinValue;
			cache.mLineTime			= DateTime.MinValue;
			cache.mSendTime			= DateTime.MinValue;
			cache.mPubId			= null;
			cache.mBidPrice			= new MamaPrice ();
			cache.mBidSize			= 0;
			cache.mBidDepth			= 0;
			cache.mBidPartId		= null;
			cache.mBidClosePrice	= new MamaPrice ();
			cache.mBidCloseDate		= DateTime.MinValue;
			cache.mBidPrevClosePrice = new MamaPrice ();
			cache.mBidPrevCloseDate = DateTime.MinValue;
			cache.mBidHigh			= new MamaPrice ();
			cache.mBidLow			= new MamaPrice ();
			cache.mAskPrice			= new MamaPrice ();
			cache.mAskSize			= 0;
			cache.mAskDepth			= 0;
			cache.mAskPartId		= null;
			cache.mAskClosePrice	= new MamaPrice ();
			cache.mAskCloseDate		= DateTime.MinValue;
			cache.mAskPrevClosePrice = new MamaPrice ();
			cache.mAskPrevCloseDate = DateTime.MinValue;
			cache.mAskHigh			= new MamaPrice ();
			cache.mAskLow			= new MamaPrice ();
			cache.mMidPrice			= new MamaPrice ();
			cache.mEventSeqNum		= 0;
			cache.mEventTime		= DateTime.MinValue;
            cache.mQuoteDate		= DateTime.MinValue;
			cache.mQuoteQualStr		= null;
			cache.mQuoteQualNative	= null;
			cache.mShortSaleBidTick = 'Z';
            cache.mShortSaleCircuitBreaker = ' ';
			cache.mTmpQuoteCount	= 0;
			cache.mQuoteCount		= 0; 
            cache.mConflateCount	= 1; 
			cache.mGotBidPrice		= false;
			cache.mGotAskPrice		= false;
			cache.mGotBidSize		= false;
			cache.mGotAskSize		= false;
			cache.mGotBidDepth		= false;
			cache.mGotAskDepth		= false;
			cache.mGotPartId		= false;
			cache.mGotBidPartId		= false;
			cache.mGotAskPartId		= false;
            cache.mGotQuoteCount	= false;
            
            cache.mAskTime          = DateTime.MinValue;
            cache.mBidTime          = DateTime.MinValue;
            cache.mAskIndicator     = null;
            cache.mBidIndicator     = null;
            cache.mAskUpdateCount   = 0;
            cache.mBidUpdateCount   = 0;
            cache.mAskYield         = 0;
            cache.mBidYield         = 0;
            
            //Field State
            cache.mSymbolFieldState				= new MamdaFieldState();
            cache.mPartIdFieldState				= new MamdaFieldState();
            cache.mSrcTimeFieldState			= new MamdaFieldState();
            cache.mActTimeFieldState			= new MamdaFieldState();
            cache.mLineTimeFieldState			= new MamdaFieldState();
            cache.mSendTimeFieldState			= new MamdaFieldState();
            cache.mPubIdFieldState				= new MamdaFieldState();
            cache.mBidPriceFieldState			= new MamdaFieldState ();
            cache.mBidSizeFieldState			= new MamdaFieldState();
            cache.mBidDepthFieldState			= new MamdaFieldState();
            cache.mBidPartIdFieldState			= new MamdaFieldState();
            cache.mBidClosePriceFieldState		= new MamdaFieldState ();
            cache.mBidCloseDateFieldState		= new MamdaFieldState();
            cache.mBidPrevClosePriceFieldState	= new MamdaFieldState ();
            cache.mBidPrevCloseDateFieldState	= new MamdaFieldState();
            cache.mBidHighFieldState			= new MamdaFieldState ();
            cache.mBidLowFieldState				= new MamdaFieldState ();
            cache.mAskPriceFieldState			= new MamdaFieldState ();
            cache.mAskSizeFieldState			= new MamdaFieldState();
            cache.mAskDepthFieldState			= new MamdaFieldState();
            cache.mAskPartIdFieldState			= new MamdaFieldState();
            cache.mAskClosePriceFieldState		= new MamdaFieldState ();
            cache.mAskCloseDateFieldState		= new MamdaFieldState();
            cache.mAskPrevClosePriceFieldState	= new MamdaFieldState ();
            cache.mAskPrevCloseDateFieldState	= new MamdaFieldState();
            cache.mAskHighFieldState			= new MamdaFieldState ();
            cache.mAskLowFieldState				= new MamdaFieldState ();
            cache.mEventSeqNumFieldState		= new MamdaFieldState();
            cache.mEventTimeFieldState			= new MamdaFieldState();
            cache.mQuoteDateFieldState			= new MamdaFieldState();
            cache.mQuoteQualStrFieldState		= new MamdaFieldState();
            cache.mQuoteQualNativeFieldState	= new MamdaFieldState();
            cache.mShortSaleBidTickFieldState	= new MamdaFieldState();
            cache.mShortSaleCircuitBreakerFieldState =new MamdaFieldState();
            cache.mTmpQuoteCountFieldState		= new MamdaFieldState();
            cache.mQuoteCountFieldState			= new MamdaFieldState(); 
            cache.mConflateCountFieldState		= new MamdaFieldState();
            cache.mAskTimeFieldState			= new MamdaFieldState();
            cache.mBidTimeFieldState			= new MamdaFieldState();
            cache.mAskIndicatorFieldState		= new MamdaFieldState();
            cache.mBidIndicatorFieldState		= new MamdaFieldState();
            cache.mAskUpdateCountFieldState		= new MamdaFieldState();
            cache.mBidUpdateCountFieldState		= new MamdaFieldState();
            cache.mAskYieldFieldState			= new MamdaFieldState();
            cache.mBidYieldFieldState			= new MamdaFieldState();
		}

		/// <summary>
		/// Add a specialized quote handler.  Currently, only one
		/// handler can (and must) be registered.
		/// </summary>
		/// <param name="handler"></param>
		public void addHandler(MamdaQuoteHandler handler)
		{
			mHandlers.Add(handler);
		}

		public string getSymbol()
		{
			return mQuoteCache.mSymbol;
		}

		public string getPartId()
		{
			return mQuoteCache.mPartId;
		}

		public DateTime getSrcTime()
		{
			return mQuoteCache.mSrcTime;
		}

		public DateTime getActivityTime()
		{
			return mQuoteCache.mActTime;
		}

		public DateTime getLineTime()
		{
			return mQuoteCache.mLineTime;
		}

		public DateTime getSendTime()
		{
			return mQuoteCache.mSendTime;
		}

		public string getPubId()
		{
			return mQuoteCache.mPubId;
		}

		public MamaPrice getBidPrice()
		{
			return mQuoteCache.mBidPrice;
		}

		public long getBidSize()
		{
			return (long) mQuoteCache.mBidSize;
		}

        public long getBidDepth()
        {
            return (long) mQuoteCache.mBidDepth;
        }

		public string getBidPartId()
		{
			return mQuoteCache.mBidPartId;
		}

		public MamaPrice getBidClosePrice()
		{
			return mQuoteCache.mBidClosePrice;
		}

		public DateTime getBidCloseDate()
		{
			return mQuoteCache.mBidCloseDate;
		}

		public MamaPrice getBidPrevClosePrice()
		{
			return mQuoteCache.mBidPrevClosePrice;
		}

		public DateTime getBidPrevCloseDate()
		{
			return mQuoteCache.mBidPrevCloseDate;
		}

		public MamaPrice getBidHigh()
		{
			return mQuoteCache.mBidHigh;
		}

		public MamaPrice getBidLow()
		{
			return mQuoteCache.mBidLow;
		}

		public MamaPrice getAskPrice()
		{
			return mQuoteCache.mAskPrice;
		}

		public long getAskSize()
		{
			return (long) mQuoteCache.mAskSize;
		}

        public long getAskDepth()
        {
            return (long) mQuoteCache.mAskDepth;
        }

		public string getAskPartId()
		{
			return mQuoteCache.mAskPartId;
		}

		public MamaPrice getAskClosePrice()
		{
			return mQuoteCache.mAskClosePrice;
		}

		public DateTime getAskCloseDate()
		{
			return mQuoteCache.mAskCloseDate;
		}

		public MamaPrice getAskPrevClosePrice()
		{
			return mQuoteCache.mAskPrevClosePrice;
		}

		public DateTime getAskPrevCloseDate()
		{
			return mQuoteCache.mAskPrevCloseDate;
		}

		public MamaPrice getAskHigh()
		{
			return mQuoteCache.mAskHigh;
		}

		public MamaPrice getAskLow()
		{
			return mQuoteCache.mAskLow;
		}

		public MamaPrice getQuoteMidPrice()
		{
			if ((mQuoteCache.mBidPrice.getValue() > 0.0) && 
				(mQuoteCache.mAskPrice.getValue() > 0.0))
			{
				mQuoteCache.mMidPrice.setValue((
				mQuoteCache.mAskPrice.getValue() + 
				mQuoteCache.mBidPrice.getValue()) / 2.0);
			}
			else
			{
				mQuoteCache.mMidPrice.clear();
			}
			return mQuoteCache.mMidPrice;
		}

		public string getQuoteQual()
		{
			return mQuoteCache.mQuoteQualStr;
		}

		public string getQuoteQualNative()
		{
			return mQuoteCache.mQuoteQualNative;
		}

		public char getShortSaleBidTick()
		{
			return mQuoteCache.mShortSaleBidTick;
		}

        public char getShortSaleCircuitBreaker()
        {
            return mQuoteCache.mShortSaleCircuitBreaker;
        }

		public long getQuoteCount()
		{
			return mQuoteCache.mQuoteCount;
		}

		public long getEventSeqNum()
		{
			return mQuoteCache.mEventSeqNum;
		}

		public DateTime getEventTime()
		{
			return mQuoteCache.mEventTime;
		}

        public DateTime getQuoteDate()
		{
			return mQuoteCache.mQuoteDate;
		}
        
		public long getBeginGapSeqNum()
		{
			return mGapBegin;
		}

		public long getEndGapSeqNum()
		{
			return mGapEnd;
		}

		public DateTime getAskTime () 
		{
			return mQuoteCache.mAskTime;
		}
	    
		public DateTime getBidTime () 
		{
			return mQuoteCache.mBidTime;
		}
	    
		public string getAskIndicator ()
		{
			return mQuoteCache.mAskIndicator;
		}
	    
		public string getBidIndicator ()
		{
			return mQuoteCache.mBidIndicator;
		}
	    
		public long getAskUpdateCount () 
		{
			return mQuoteCache.mAskUpdateCount;
		}
	    
		public long getBidUpdateCount () 
		{
			return mQuoteCache.mBidUpdateCount;
		}
		public double getAskYield () 
		{
			return mQuoteCache.mAskYield;
		}
	    
		public double getBidYield () 
		{
			return mQuoteCache.mBidYield;
		}
    
		public MamdaFieldState getSymbolFieldState()
		{
			return mQuoteCache.mSymbolFieldState;
		}

		public MamdaFieldState getPartIdFieldState()
		{
			return mQuoteCache.mPartIdFieldState;
		}

		public MamdaFieldState getSrcTimeFieldState()
		{
			return mQuoteCache.mSrcTimeFieldState;
		}

		public MamdaFieldState getActivityTimeFieldState()
		{
			return mQuoteCache.mActTimeFieldState;
		}

		public MamdaFieldState getLineTimeFieldState()
		{
			return mQuoteCache.mLineTimeFieldState;
		}

		public MamdaFieldState getSendTimeFieldState()
		{
			return mQuoteCache.mSendTimeFieldState;
		}

		public MamdaFieldState getPubIdFieldState()
		{
			return mQuoteCache.mPubIdFieldState;
		}

		public MamdaFieldState getBidPriceFieldState()
		{
			return mQuoteCache.mBidPriceFieldState;
		}

		public MamdaFieldState getBidSizeFieldState()
		{
			return (MamdaFieldState) mQuoteCache.mBidSizeFieldState;
		}

		public MamdaFieldState getBidDepthFieldState()
		{
			return (MamdaFieldState) mQuoteCache.mBidDepthFieldState;
		}

		public MamdaFieldState getBidPartIdFieldState()
		{
			return mQuoteCache.mBidPartIdFieldState;
		}

		public MamdaFieldState getBidClosePriceFieldState()
		{
			return mQuoteCache.mBidClosePriceFieldState;
		}

		public MamdaFieldState getBidCloseDateFieldState()
		{
			return mQuoteCache.mBidCloseDateFieldState;
		}

		public MamdaFieldState getBidPrevClosePriceFieldState()
		{
			return mQuoteCache.mBidPrevClosePriceFieldState;
		}

		public MamdaFieldState getBidPrevCloseDateFieldState()
		{
			return mQuoteCache.mBidPrevCloseDateFieldState;
		}

		public MamdaFieldState getBidHighFieldState()
		{
			return mQuoteCache.mBidHighFieldState;
		}

		public MamdaFieldState getBidLowFieldState()
		{
			return mQuoteCache.mBidLowFieldState;
		}

		public MamdaFieldState getAskPriceFieldState()
		{
			return mQuoteCache.mAskPriceFieldState;
		}

		public MamdaFieldState getAskSizeFieldState()
		{
			return (MamdaFieldState) mQuoteCache.mAskSizeFieldState;
		}

		public MamdaFieldState getAskDepthFieldState()
		{
			return (MamdaFieldState) mQuoteCache.mAskDepthFieldState;
		}

		public MamdaFieldState getAskPartIdFieldState()
		{
			return mQuoteCache.mAskPartIdFieldState;
		}

		public MamdaFieldState getAskClosePriceFieldState()
		{
			return mQuoteCache.mAskClosePriceFieldState;
		}

		public MamdaFieldState getAskCloseDateFieldState()
		{
			return mQuoteCache.mAskCloseDateFieldState;
		}

		public MamdaFieldState getAskPrevClosePriceFieldState()
		{
			return mQuoteCache.mAskPrevClosePriceFieldState;
		}

		public MamdaFieldState getAskPrevCloseDateFieldState()
		{
			return mQuoteCache.mAskPrevCloseDateFieldState;
		}

		public MamdaFieldState getAskHighFieldState()
		{
			return mQuoteCache.mAskHighFieldState;
		}

		public MamdaFieldState getAskLowFieldState()
		{
			return mQuoteCache.mAskLowFieldState;
		}

		public MamdaFieldState getQuoteQualFieldState()
		{
			return mQuoteCache.mQuoteQualStrFieldState;
		}

		public MamdaFieldState getQuoteQualNativeFieldState()
		{
			return mQuoteCache.mQuoteQualNativeFieldState;
		}

		public MamdaFieldState getShortSaleBidTickFieldState()
		{
			return mQuoteCache.mShortSaleBidTickFieldState;
		}

		public MamdaFieldState getShortSaleCircuitBreakerFieldState()
		{          
			return mQuoteCache.mShortSaleCircuitBreakerFieldState;
		}  

		public MamdaFieldState getQuoteCountFieldState()
		{
			return mQuoteCache.mQuoteCountFieldState;
		}

		public MamdaFieldState getEventSeqNumFieldState()
		{
			return mQuoteCache.mEventSeqNumFieldState;
		}

		public MamdaFieldState getEventTimeFieldState()
		{
			return mQuoteCache.mEventTimeFieldState;
		}

		public MamdaFieldState getQuoteDateFieldState()
		{
			return mQuoteCache.mQuoteDateFieldState;
		}
	        
		public MamdaFieldState getBeginGapSeqNumFieldState()
		{
			return mGapBeginFieldState;
		}

		public MamdaFieldState getEndGapSeqNumFieldState()
		{
			return mGapEndFieldState;
		}

		public MamdaFieldState getAskTimeFieldState() 
		{
			return mQuoteCache.mAskTimeFieldState;
		}
	    
		public MamdaFieldState getBidTimeFieldState() 
		{
			return mQuoteCache.mBidTimeFieldState;
		}
    
		public MamdaFieldState getAskIndicatorFieldState()
		{
			return mQuoteCache.mAskIndicatorFieldState;
		}
	    
		public MamdaFieldState getBidIndicatorFieldState()
		{
			return mQuoteCache.mBidIndicatorFieldState;
		}
	    
		public MamdaFieldState getAskUpdateCountFieldState() 
		{
			return mQuoteCache.mAskUpdateCountFieldState;
		}
	    
		public MamdaFieldState getBidUpdateCountFieldState() 
		{
			return mQuoteCache.mBidUpdateCountFieldState;
		}
		public MamdaFieldState getAskYieldFieldState() 
		{
			return mQuoteCache.mAskYieldFieldState;
		}
	    
		public MamdaFieldState getBidYieldFieldState() 
		{
			return mQuoteCache.mBidYieldFieldState;
		}
        
		// Implementation of MamdaListener interface.
		// NB! For internal use only.
		public void onMsg(
			MamdaSubscription subscription,
			MamaMsg           msg,
			mamaMsgType		  msgType)
		{			
			// Listeners may be created on multiple threads and we only
			// want a single list of updaters.
			if (mUpdaters == null)
			{
				lock (mUpdatersGuard)
				{
					if(!MamdaQuoteFields.isSet())
					{
						return;
					}

					if (mUpdaters == null)
					{
						QuoteUpdate[] updaters = createUpdaters();
						Thread.MemoryBarrier();
						mUpdaters = updaters;
					}
				}
			}

			// If msg is a quote-related message, invoke the
			// appropriate callback:
            
            //Clear conflate count
            mQuoteCache.mConflateCount  = 1; 
             
			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
				case mamaMsgType.MAMA_MSG_TYPE_PREOPENING:
				case mamaMsgType.MAMA_MSG_TYPE_SNAPSHOT:
					handleRecap(subscription, msg);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_QUOTE:
					handleQuote(subscription, msg);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
				case mamaMsgType.MAMA_MSG_TYPE_TRADE:
					handleUpdate(subscription, msg);
					break;
			}
		}


		public void populateRecap(MamdaConcreteQuoteRecap recap)
		{
			lock (this)
			{
				recap.setSymbol(mQuoteCache.mSymbol);
				recap.setPartId(mQuoteCache.mPartId);
				recap.setSrcTime(mQuoteCache.mSrcTime);
				recap.setActivityTime(mQuoteCache.mActTime);
				recap.setLineTime(mQuoteCache.mLineTime);
				recap.setSendTime(mQuoteCache.mSendTime);
				recap.setPubId(mQuoteCache.mPubId);
				recap.setBidPrice(mQuoteCache.mBidPrice);
				recap.setBidSize(mQuoteCache.mBidSize);
				recap.setBidDepth(mQuoteCache.mBidDepth);
				recap.setBidPartId(mQuoteCache.mBidPartId);
				recap.setBidClosePrice(mQuoteCache.mBidClosePrice);
				recap.setBidCloseDate(mQuoteCache.mBidCloseDate);
				recap.setBidPrevClosePrice(mQuoteCache.mBidPrevClosePrice);
				recap.setBidPrevCloseDate(mQuoteCache.mBidPrevCloseDate);
				recap.setBidHigh(mQuoteCache.mBidHigh);
				recap.setBidLow(mQuoteCache.mBidLow);
				recap.setAskPrice(mQuoteCache.mAskPrice);
				recap.setQuoteMidPrice(mQuoteCache.mMidPrice);
				recap.setAskSize(mQuoteCache.mAskSize);
				recap.setAskDepth(mQuoteCache.mAskDepth);
				recap.setAskPartId(mQuoteCache.mAskPartId);
				recap.setAskClosePrice(mQuoteCache.mAskClosePrice);
				recap.setAskCloseDate(mQuoteCache.mAskCloseDate);
				recap.setAskPrevClosePrice(mQuoteCache.mAskPrevClosePrice);
				recap.setAskPrevCloseDate(mQuoteCache.mAskPrevCloseDate);
				recap.setAskHigh(mQuoteCache.mAskHigh);
				recap.setAskLow(mQuoteCache.mAskLow);
				recap.setEventSeqNum(mQuoteCache.mEventSeqNum);
				recap.setEventTime(mQuoteCache.mEventTime);
                recap.setQuoteDate(mQuoteCache.mQuoteDate);
				recap.setQuoteQual(mQuoteCache.mQuoteQualStr);
				recap.setQuoteQualNative(mQuoteCache.mQuoteQualNative);
				recap.setQuoteCount(mQuoteCache.mQuoteCount);
				recap.setShortSaleBidTick(mQuoteCache.mShortSaleBidTick);
                recap.setShortSaleCircuitBreaker(getShortSaleCircuitBreaker());                
                
                recap.setAskTime (mQuoteCache.mAskTime);
                recap.setBidTime (mQuoteCache.mBidTime);
                recap.setAskIndicator (mQuoteCache.mAskIndicator);
                recap.setBidIndicator (mQuoteCache.mBidIndicator);
                recap.setAskUpdateCount (mQuoteCache.mAskUpdateCount);
                recap.setBidUpdateCount (mQuoteCache.mBidUpdateCount);
                recap.setAskYield (mQuoteCache.mAskYield);
                recap.setBidYield (mQuoteCache.mBidYield);
			}
		}

		private void handleRecap(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
            updateFieldStates();
            updateQuoteFields(msg);
        
            checkQuoteCount(subscription, msg, false);

			foreach (MamdaQuoteHandler handler in mHandlers)
			{
				handler.onQuoteRecap(subscription, this, msg, this);
			}
		}

		private void handleQuote(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{         
            updateFieldStates();
            updateQuoteFields(msg);          
			checkQuoteCount(subscription, msg, true);

            if (mIgnoreUpdate)
            {
                mIgnoreUpdate = false;
                return;
            }

			foreach (MamdaQuoteHandler handler in mHandlers)
			{              
				handler.onQuoteUpdate (subscription, this, msg, this, this);
			}
		}

		private void handleUpdate(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{          
            updateFieldStates();
            updateQuoteFields(msg);           
			if (mLastGenericMsgWasQuote)
			{
				checkQuoteCount(subscription, msg, true);

                if (mIgnoreUpdate)
                {
                    mIgnoreUpdate = false;
                    return;
                }

				foreach (MamdaQuoteHandler handler in mHandlers)
				{
					handler.onQuoteUpdate (subscription, this, msg, this, this);
				}
				mLastGenericMsgWasQuote = false;
			}
		}
	    
		private void checkQuoteCount(
			MamdaSubscription  subscription,
			MamaMsg            msg,
			bool               checkForGap)
		{        
            mIgnoreUpdate = false;
    
			if (checkForGap && mQuoteCache.mGotQuoteCount)
			{
				if ((mQuoteCache.mQuoteCount > 0) && 
                        (mQuoteCache.mTmpQuoteCount > (mQuoteCache.mQuoteCount + mQuoteCache.mConflateCount)))
				{
					mGapBegin = mQuoteCache.mQuoteCount + mQuoteCache.mConflateCount;
                    mGapBeginFieldState = MamdaFieldState.MODIFIED;
					mGapEnd   = mQuoteCache.mTmpQuoteCount - 1;
                    mGapEndFieldState = MamdaFieldState.MODIFIED;
					mQuoteCache.mQuoteCount = mQuoteCache.mTmpQuoteCount;
                    mQuoteCache.mQuoteCountFieldState = MamdaFieldState.MODIFIED;
					foreach (MamdaQuoteHandler handler in mHandlers)
					{
						handler.onQuoteGap(subscription, this, msg, this, this);
					}
				}
			}

			/* Check for duplicates.  Only check if quote count > 0, in case it isn't being sent down.*/
            if ( mQuoteCache.mGotQuoteCount && 
                 mQuoteCache.mQuoteCount > 0 && 
                 (mQuoteCache.mTmpQuoteCount == mQuoteCache.mQuoteCount) )
            {
                mIgnoreUpdate = true;
            }        

			mQuoteCache.mQuoteCount = mQuoteCache.mTmpQuoteCount;
            mQuoteCache.mQuoteCountFieldState = MamdaFieldState.MODIFIED;
		}

		private static QuoteUpdate[] createUpdaters ()
		{
            mMaxFid = MamdaQuoteFields.getMaxFid();
			QuoteUpdate[] updaters = new QuoteUpdate[mMaxFid+1];

			addToUpdatersList(updaters, MamdaQuoteFields.SYMBOL, new MamdaQuoteSymbol());
			addToUpdatersList(updaters, MamdaQuoteFields.ISSUE_SYMBOL, new MamdaQuoteIssueSymbol());
			addToUpdatersList(updaters, MamdaQuoteFields.PART_ID, new MamdaQuotePartId());
			addToUpdatersList(updaters, MamdaQuoteFields.SRC_TIME, new MamdaQuoteSrcTime());
			addToUpdatersList(updaters, MamdaQuoteFields.ACTIVITY_TIME, new MamdaQuoteActivityTime());
			addToUpdatersList(updaters, MamdaQuoteFields.LINE_TIME, new MamdaQuoteLineTime());
			addToUpdatersList(updaters, MamdaQuoteFields.SEND_TIME, new MamdaQuoteSendTime());
			addToUpdatersList(updaters, MamdaQuoteFields.PUB_ID, new MamdaQuotePubId());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_PRICE, new MamdaQuoteBidPrice());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_SIZE, new MamdaQuoteBidSize());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_PART_ID, new MamdaQuoteBidPartId());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_DEPTH, new MamdaQuoteBidDepth());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_CLOSE_PRICE, new MamdaQuoteBidClosePrice());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_CLOSE_DATE, new MamdaQuoteBidCloseDate());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_PREV_CLOSE_PRICE, new MamdaQuoteBidPrevClosePrice());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_PREV_CLOSE_DATE, new MamdaQuoteBidPrevCloseDate());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_HIGH, new MamdaQuoteBidHigh());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_LOW, new MamdaQuoteBidLow());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_PRICE, new MamdaQuoteAskPrice());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_SIZE, new MamdaQuoteAskSize());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_PART_ID, new MamdaQuoteAskPartId());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_DEPTH, new MamdaQuoteAskDepth());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_CLOSE_PRICE, new MamdaQuoteAskClosePrice());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_CLOSE_DATE, new MamdaQuoteAskCloseDate());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_PREV_CLOSE_PRICE, new MamdaQuoteAskPrevClosePrice());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_PREV_CLOSE_DATE, new MamdaQuoteAskPrevCloseDate());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_HIGH, new MamdaQuoteAskHigh());
			addToUpdatersList(updaters, MamdaQuoteFields.ASK_LOW, new MamdaQuoteAskLow());
			addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_SEQ_NUM, new MamdaQuoteSeqNum());
			addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_TIME, new MamdaQuoteTime());
            addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_DATE, new MamdaQuoteDate());
			addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_QUAL, new MamdaQuoteQual());
			addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_QUAL_NATIVE, new MamdaQuoteQualNative());
			addToUpdatersList(updaters, MamdaQuoteFields.QUOTE_COUNT, new MamdaQuoteCount());
            addToUpdatersList(updaters, MamdaQuoteFields.CONFLATE_COUNT, new MamdaConflateCount());
			addToUpdatersList(updaters, MamdaQuoteFields.SHORT_SALE_BID_TICK, new MamdaShortSaleBidTick());
            addToUpdatersList(updaters, MamdaQuoteFields.SHORT_SALE_CIRCUIT_BREAKER, new MamdaShortSaleCircuitBreaker());
			addToUpdatersList(updaters, MamdaQuoteFields.BID_TICK, new MamdaShortSaleBidTick());
            
            addToUpdatersList (updaters, MamdaQuoteFields.ASK_TIME, new MamdaAskTime ());
            addToUpdatersList (updaters, MamdaQuoteFields.BID_TIME, new MamdaBidTime ());
            addToUpdatersList (updaters, MamdaQuoteFields.ASK_INDICATOR, new MamdaAskIndicator ());
            addToUpdatersList (updaters, MamdaQuoteFields.BID_INDICATOR, new MamdaBidIndicator ());
            addToUpdatersList (updaters, MamdaQuoteFields.ASK_UPDATE_COUNT, new MamdaAskUpdateCount ());
            addToUpdatersList (updaters, MamdaQuoteFields.BID_UPDATE_COUNT, new MamdaBidUpdateCount ());
            addToUpdatersList (updaters, MamdaQuoteFields.ASK_YIELD, new MamdaAskYield ());
            addToUpdatersList (updaters, MamdaQuoteFields.BID_YIELD, new MamdaBidYield ());
			return updaters;
		}

        private void updateFieldStates()
        {
        if(mQuoteCache.mSymbolFieldState             == MamdaFieldState.MODIFIED)
        mQuoteCache.mSymbolFieldState             = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mPartIdFieldState             == MamdaFieldState.MODIFIED)
        mQuoteCache.mPartIdFieldState             = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mSrcTimeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mSrcTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mActTimeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mActTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mLineTimeFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mLineTimeFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mSendTimeFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mSendTimeFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mPubIdFieldState              == MamdaFieldState.MODIFIED)
        mQuoteCache.mPubIdFieldState              = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidPriceFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidPriceFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidSizeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidSizeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidDepthFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidDepthFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidPartIdFieldState          == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidPartIdFieldState          = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidClosePriceFieldState      == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidClosePriceFieldState      = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidCloseDateFieldState       == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidCloseDateFieldState       = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidPrevClosePriceFieldState  == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidPrevClosePriceFieldState  = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidPrevCloseDateFieldState   == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidPrevCloseDateFieldState   = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidHighFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidHighFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidLowFieldState             == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidLowFieldState             = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskPriceFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskPriceFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskSizeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskSizeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskDepthFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskDepthFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskPartIdFieldState          == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskPartIdFieldState          = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskClosePriceFieldState      == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskClosePriceFieldState      = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskCloseDateFieldState       == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskCloseDateFieldState       = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskPrevClosePriceFieldState  == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskPrevClosePriceFieldState  = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskPrevCloseDateFieldState   == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskPrevCloseDateFieldState   = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskHighFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskHighFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskLowFieldState             == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskLowFieldState             = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mEventSeqNumFieldState        == MamdaFieldState.MODIFIED)
        mQuoteCache.mEventSeqNumFieldState        = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mEventTimeFieldState          == MamdaFieldState.MODIFIED)
        mQuoteCache.mEventTimeFieldState          = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mQuoteDateFieldState          == MamdaFieldState.MODIFIED)
        mQuoteCache.mQuoteDateFieldState          = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mQuoteQualStrFieldState       == MamdaFieldState.MODIFIED)
        mQuoteCache.mQuoteQualStrFieldState       = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mQuoteQualNativeFieldState    == MamdaFieldState.MODIFIED)
        mQuoteCache.mQuoteQualNativeFieldState    = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mShortSaleBidTickFieldState   == MamdaFieldState.MODIFIED)
        mQuoteCache.mShortSaleBidTickFieldState   = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mShortSaleCircuitBreakerFieldState   == MamdaFieldState.MODIFIED)
        mQuoteCache.mShortSaleCircuitBreakerFieldState   = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mTmpQuoteCountFieldState      == MamdaFieldState.MODIFIED)
        mQuoteCache.mTmpQuoteCountFieldState      = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mQuoteCountFieldState         == MamdaFieldState.MODIFIED)
        mQuoteCache.mQuoteCountFieldState         = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mConflateCountFieldState      == MamdaFieldState.MODIFIED)
        mQuoteCache.mConflateCountFieldState      = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskTimeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidTimeFieldState            == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskIndicatorFieldState       == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskIndicatorFieldState       = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidIndicatorFieldState       == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidIndicatorFieldState       = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskUpdateCountFieldState     == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskUpdateCountFieldState     = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidUpdateCountFieldState     == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidUpdateCountFieldState     = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mAskYieldFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mAskYieldFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mQuoteCache.mBidYieldFieldState           == MamdaFieldState.MODIFIED)
        mQuoteCache.mBidYieldFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mGapBeginFieldState           == MamdaFieldState.MODIFIED)  
           mGapBeginFieldState           = MamdaFieldState.NOT_MODIFIED;
        if(mGapEndFieldState             == MamdaFieldState.MODIFIED)  
            mGapEndFieldState           = MamdaFieldState.NOT_MODIFIED;
        }

        private void updateQuoteFields(MamaMsg msg)
		{
			mLastGenericMsgWasQuote    = false;
			mQuoteCache.mGotBidPrice  = false;
			mQuoteCache.mGotAskPrice  = false;
			mQuoteCache.mGotBidSize   = false;
			mQuoteCache.mGotAskSize   = false;
			mQuoteCache.mGotBidDepth  = false;
			mQuoteCache.mGotAskDepth  = false;
			mQuoteCache.mGotPartId    = false;
			mQuoteCache.mGotBidPartId = false;
			mQuoteCache.mGotAskPartId = false;
            mQuoteCache.mGotQuoteCount = false;

			// Iterate over all of the fields in the message.
			lock (this)
			{
				msg.iterateFields(mFieldIterator, null, null);
			}

			// Check certain special fields.
			if (mQuoteCache.mGotBidSize   || mQuoteCache.mGotAskSize   ||
				mQuoteCache.mGotBidDepth  || mQuoteCache.mGotAskDepth  ||
				mQuoteCache.mGotBidPrice  || mQuoteCache.mGotAskPrice  ||
				mQuoteCache.mGotBidPartId || mQuoteCache.mGotAskPartId)
			{
				mLastGenericMsgWasQuote = true;
			}

			if (mQuoteCache.mGotPartId == false && mQuoteCache.mSymbol != null)
			{
				// No explicit part ID in message, but maybe in symbol.
				int lastDot = mQuoteCache.mSymbol.IndexOf(".");
				if (lastDot != -1)
				{
					lastDot++;
					int lastChar = mQuoteCache.mSymbol.Length;
					if (lastDot != lastChar)
					{
                        mQuoteCache.mPartId =
                            mQuoteCache.mSymbol.Substring(lastDot, lastChar - lastDot);
                        mQuoteCache.mGotPartId = true;
                        mQuoteCache.mPartIdFieldState = MamdaFieldState.MODIFIED; 
                    }
				}
                    
			}
		}

		private static void addToUpdatersList(
			QuoteUpdate[]		updaters,
			MamaFieldDescriptor	fieldDesc,
			QuoteUpdate			updater)
		{
			if (fieldDesc == null) return;
			int fieldId = fieldDesc.getFid();
            if (fieldId <= mMaxFid)
            {
                updaters[fieldId] = updater;
            }
		}

		private interface QuoteUpdate
		{
			void onUpdate(MamdaQuoteListener listener, MamaMsgField field);
		}

		private class MamdaQuoteSymbol : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mSymbol = field.getString();
                listener.mQuoteCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteIssueSymbol : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mSymbol = field.getString();
                listener.mQuoteCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuotePartId : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mPartId = field.getString();
                listener.mQuoteCache.mPartIdFieldState = MamdaFieldState.MODIFIED;
				listener.mQuoteCache.mGotPartId = true;
			}
		}
        private class MamdaQuoteBidPrice : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                MamaPrice bidPrice;
                bidPrice = field.getPrice();
                if (listener.mQuoteCache.mBidPrice.getValue() != bidPrice.getValue() ||
                    listener.mQuoteCache.mBidPriceFieldState == MamdaFieldState.NOT_INITIALISED)
                {
                    listener.mQuoteCache.mBidPrice.copy(bidPrice);
                    listener.mQuoteCache.mBidPriceFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotBidPrice = true;
                }
            }
        }
        private class MamdaQuoteBidSize : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                double bidSize = field.getF64();
                if (listener.mQuoteCache.mBidSize != bidSize)
                {
                    listener.mQuoteCache.mBidSize = bidSize;
                    listener.mQuoteCache.mBidSizeFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotBidSize = true;
                }
            }
        }
        private class MamdaQuoteBidPartId : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                string bidPartId = field.getString();
                if (listener.mQuoteCache.mBidPartId != bidPartId)
                {
                    listener.mQuoteCache.mBidPartId = bidPartId;
                    listener.mQuoteCache.mBidPartIdFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotBidPartId = true;
                }
            }
        }
        private class MamdaQuoteBidDepth : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                double bidDepth = field.getF64();
                if (listener.mQuoteCache.mBidDepth != bidDepth)
                {
                    listener.mQuoteCache.mBidDepth = bidDepth;
                    listener.mQuoteCache.mBidDepthFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotBidDepth = true;
                }
            }
        }
		private class MamdaQuoteBidClosePrice : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidClosePrice.copy (field.getPrice());
                listener.mQuoteCache.mBidClosePriceFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteBidCloseDate : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidCloseDate = field.getDateTime();
                listener.mQuoteCache.mBidCloseDateFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteBidPrevClosePrice : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidPrevClosePrice.copy(field.getPrice());
                listener.mQuoteCache.mBidPrevClosePriceFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteBidPrevCloseDate : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidPrevCloseDate = field.getDateTime();
                listener.mQuoteCache.mBidPrevCloseDateFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteBidHigh : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidHigh.copy(field.getPrice());
                listener.mQuoteCache.mBidHighFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteBidLow : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mBidLow.copy(field.getPrice());
                listener.mQuoteCache.mBidLowFieldState = MamdaFieldState.MODIFIED;
			}
		}
        private class MamdaQuoteAskPrice : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                MamaPrice askPrice;
                askPrice = field.getPrice();
                if (listener.mQuoteCache.mAskPrice.getValue() != askPrice.getValue())
                {
                    listener.mQuoteCache.mAskPrice.copy(askPrice);
                    listener.mQuoteCache.mAskPriceFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotAskPrice = true;
                }
            }
        }
        private class MamdaQuoteAskSize : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                double askSize = field.getF64();
                if (listener.mQuoteCache.mAskSize != askSize)
                {
                    listener.mQuoteCache.mAskSize = askSize;
                    listener.mQuoteCache.mAskSizeFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotAskSize = true;
                }
            }
        }
        private class MamdaQuoteAskPartId : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                string askPartId = field.getString();
                if (listener.mQuoteCache.mAskPartId != askPartId)
                {
                    listener.mQuoteCache.mAskPartId = askPartId;
                    listener.mQuoteCache.mAskPartIdFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotAskPartId = true;
                }
            }
        }
        private class MamdaQuoteAskDepth : QuoteUpdate
        {
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
            {
                double askDepth = field.getF64();
                if (listener.mQuoteCache.mAskDepth != askDepth)
                {
                    listener.mQuoteCache.mAskDepth = askDepth;
                    listener.mQuoteCache.mAskDepthFieldState = MamdaFieldState.MODIFIED;
                    listener.mQuoteCache.mGotAskDepth = true;
                }
            }
        }
		private class MamdaQuoteAskClosePrice : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskClosePrice.copy(field.getPrice());
                listener.mQuoteCache.mAskClosePriceFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteAskCloseDate : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskCloseDate = field.getDateTime();
                listener.mQuoteCache.mAskCloseDateFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteAskPrevClosePrice : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskPrevClosePrice.copy(field.getPrice());
                listener.mQuoteCache.mAskPrevClosePriceFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteAskPrevCloseDate : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskPrevCloseDate = field.getDateTime();
                listener.mQuoteCache.mAskPrevCloseDateFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteAskHigh : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskHigh.copy(field.getPrice());
                listener.mQuoteCache.mAskHighFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteAskLow : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mAskLow.copy(field.getPrice());
                listener.mQuoteCache.mAskLowFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteSrcTime : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mSrcTime = field.getDateTime();
                listener.mQuoteCache.mSrcTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteActivityTime : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mActTime = field.getDateTime();
                listener.mQuoteCache.mActTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteLineTime : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mLineTime = field.getDateTime();
                listener.mQuoteCache.mLineTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteSendTime : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mSendTime = field.getDateTime();
                listener.mQuoteCache.mSendTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuotePubId : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mPubId = field.getString();
                listener.mQuoteCache.mPubIdFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteSeqNum : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mEventSeqNum = field.getI64();
                listener.mQuoteCache.mEventSeqNumFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteTime : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mEventTime = field.getDateTime();
                listener.mQuoteCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
        private class MamdaQuoteDate : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mQuoteDate = field.getDateTime();
                listener.mQuoteCache.mQuoteDateFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteQual : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mQuoteQualStr = field.getString();
                listener.mQuoteCache.mQuoteQualStrFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaQuoteQualNative : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_STRING)
				{
					listener.mQuoteCache.mQuoteQualNative = field.getString();
                    listener.mQuoteCache.mQuoteQualNativeFieldState = MamdaFieldState.MODIFIED;
				}
				else if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_CHAR)
				{
					listener.mQuoteCache.mQuoteQualNative = field.getChar().ToString();
                    listener.mQuoteCache.mQuoteQualNativeFieldState = MamdaFieldState.MODIFIED;
				}
			}
		}
		private class MamdaQuoteCount : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mTmpQuoteCount = field.getI64();
                listener.mQuoteCache.mTmpQuoteCountFieldState = MamdaFieldState.MODIFIED;
                listener.mQuoteCache.mGotQuoteCount = true;
			}
		}
        private class MamdaConflateCount : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				listener.mQuoteCache.mConflateCount = field.getU32();
                listener.mQuoteCache.mConflateCountFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaShortSaleBidTick : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR :
						listener.mQuoteCache.mShortSaleBidTick = field.getChar();
                        listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING :
						if (field.getString() != String.Empty)
						{
							listener.mQuoteCache.mShortSaleBidTick = field.getString()[0];
                            listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						}
						else
						{
							listener.mQuoteCache.mShortSaleBidTick = ' ';
                            listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						}
						break;
					default : break;
				}
			}
		}

        private class MamdaShortSaleCircuitBreaker : QuoteUpdate
        {           
            public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)                 
            { 
                listener.mQuoteCache.mShortSaleCircuitBreaker = field.getChar(); 
                listener.mQuoteCache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.MODIFIED;  
            }
        }      

		private class MamdaBidTick : QuoteUpdate
		{
			public void onUpdate(MamdaQuoteListener listener, MamaMsgField field)
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR :
						listener.mQuoteCache.mShortSaleBidTick = field.getChar();
                        listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING :
						if (field.getString() != String.Empty)
						{
							listener.mQuoteCache.mShortSaleBidTick = field.getString()[0];
                            listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						}
						else
						{
							listener.mQuoteCache.mShortSaleBidTick = ' ';
                            listener.mQuoteCache.mShortSaleBidTickFieldState = MamdaFieldState.MODIFIED;
						}
						break;
					default : break;
				}
			}
		}
        
    private class MamdaAskTime : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mAskTime = field.getDateTime();
            listener.mQuoteCache.mAskTimeFieldState = MamdaFieldState.MODIFIED;
        }
    }
    
    private class MamdaBidTime : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mBidTime = field.getDateTime();
            listener.mQuoteCache.mBidTimeFieldState = MamdaFieldState.MODIFIED;
        }
    }
    
    private class MamdaAskIndicator : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_STRING)
            {
                listener.mQuoteCache.mAskIndicator = field.getString();
                listener.mQuoteCache.mAskIndicatorFieldState = MamdaFieldState.MODIFIED;
            }
            else if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_CHAR)
            {
                listener.mQuoteCache.mAskIndicator = field.getChar().ToString();
                listener.mQuoteCache.mAskIndicatorFieldState = MamdaFieldState.MODIFIED;
            }
        }
    }
    
    private class MamdaBidIndicator : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {

            if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_STRING)
            {
                listener.mQuoteCache.mBidIndicator = field.getString();
                listener.mQuoteCache.mBidIndicatorFieldState = MamdaFieldState.MODIFIED;
            }
            else if (field.getType() == mamaFieldType.MAMA_FIELD_TYPE_CHAR)
            {
                listener.mQuoteCache.mBidIndicator = field.getChar().ToString();
                listener.mQuoteCache.mBidIndicatorFieldState = MamdaFieldState.MODIFIED;
            }

        }
    }
    
    private class MamdaAskUpdateCount : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mAskUpdateCount =
                field.getU32 ();
            listener.mQuoteCache.mAskUpdateCountFieldState = MamdaFieldState.MODIFIED;
        }
    }
    
    private class MamdaBidUpdateCount : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mBidUpdateCount =
                field.getU32 ();
            listener.mQuoteCache.mBidUpdateCountFieldState = MamdaFieldState.MODIFIED;
        }
    }
    
    private class MamdaAskYield : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mAskYield =
                field.getF64 ();
            listener.mQuoteCache.mAskYieldFieldState = MamdaFieldState.MODIFIED;
        }
    }
    
    private class MamdaBidYield : QuoteUpdate
    {
        public void onUpdate (MamdaQuoteListener listener,
                              MamaMsgField        field)
        {
            listener.mQuoteCache.mBidYield =
                field.getF64 ();
            listener.mQuoteCache.mBidYieldFieldState = MamdaFieldState.MODIFIED;
        }
    }
	    
		/*************************************************************
		*Private class used for Field Iteration
		************************************************************/
		private class FieldIterator : MamaMsgFieldIterator
		{
			private MamdaQuoteListener mListener;
	        
			public FieldIterator(MamdaQuoteListener listener)
			{
				mListener = listener;
			}

			public void onField(MamaMsg        msg,
								MamaMsgField   field,
								object         closure)
			{
				try
				{
                    
					int fieldId = field.getFid();
                    if (fieldId <= mMaxFid)
                    {
                        QuoteUpdate updater = (QuoteUpdate)mUpdaters[fieldId];

                        if (updater != null)
                        {                          
                            updater.onUpdate(mListener, field);
                        }
                    }
				}
				catch (Exception ex )
				{
					throw new MamdaDataException(ex.Message, ex);
				}
			}

		}

		#region Implementation details

		#region MamdaQuoteCache

		private class MamdaQuoteCache
		{
			public string          mSymbol;
			public string          mPartId;
			public DateTime        mSrcTime        = DateTime.MinValue;
			public DateTime        mActTime        = DateTime.MinValue;
			public DateTime        mLineTime       = DateTime.MinValue;
			public DateTime        mSendTime       = DateTime.MinValue;
			public string          mPubId;
			public MamaPrice       mBidPrice;
			public double          mBidSize;
			public double          mBidDepth;
			public string          mBidPartId;
			public MamaPrice       mBidClosePrice;
			public DateTime        mBidCloseDate   = DateTime.MinValue;
			public MamaPrice       mBidPrevClosePrice;
			public DateTime        mBidPrevCloseDate = DateTime.MinValue;
			public MamaPrice       mBidHigh;
			public MamaPrice       mBidLow;
			public MamaPrice       mAskPrice;
			public double          mAskSize;
			public double          mAskDepth;
			public string          mAskPartId;
			public MamaPrice       mAskClosePrice;
			public DateTime        mAskCloseDate   = DateTime.MinValue;
			public MamaPrice       mAskPrevClosePrice;
			public DateTime        mAskPrevCloseDate = DateTime.MinValue;
			public MamaPrice       mAskHigh;
			public MamaPrice       mAskLow;
			public MamaPrice       mMidPrice;
			public long            mEventSeqNum;
			public DateTime        mEventTime      = DateTime.MinValue;
            public DateTime        mQuoteDate      = DateTime.MinValue;
			public string          mQuoteQualStr;
			public string          mQuoteQualNative;
			public char            mShortSaleBidTick = 'Z';
            public char            mShortSaleCircuitBreaker;
			public long            mTmpQuoteCount;
			public long            mQuoteCount; 
            public long            mConflateCount;
			public bool            mGotBidPrice;
			public bool            mGotAskPrice;
			public bool            mGotBidSize;
			public bool            mGotAskSize;
			public bool            mGotBidDepth;
			public bool            mGotAskDepth;
			public bool            mGotPartId;
			public bool            mGotBidPartId;
			public bool            mGotAskPartId;
            public bool            mGotQuoteCount;
            
            public DateTime        mAskTime;
            public DateTime        mBidTime;
            public string          mAskIndicator;
            public string          mBidIndicator;
            public long            mAskUpdateCount;
            public long            mBidUpdateCount;
            public double          mAskYield;
            public double          mBidYield;
            
            public MamdaFieldState       mSymbolFieldState             = new MamdaFieldState();
            public MamdaFieldState       mPartIdFieldState             = new MamdaFieldState();
            public MamdaFieldState       mSrcTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mActTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mLineTimeFieldState           = new MamdaFieldState();
            public MamdaFieldState       mSendTimeFieldState           = new MamdaFieldState();
            public MamdaFieldState       mPubIdFieldState              = new MamdaFieldState();
            public MamdaFieldState       mBidPriceFieldState           = new MamdaFieldState();
            public MamdaFieldState       mBidSizeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mBidDepthFieldState           = new MamdaFieldState();
            public MamdaFieldState       mBidPartIdFieldState          = new MamdaFieldState();
            public MamdaFieldState       mBidClosePriceFieldState      = new MamdaFieldState();
            public MamdaFieldState       mBidCloseDateFieldState       = new MamdaFieldState();
            public MamdaFieldState       mBidPrevClosePriceFieldState  = new MamdaFieldState();
            public MamdaFieldState       mBidPrevCloseDateFieldState   = new MamdaFieldState();
            public MamdaFieldState       mBidHighFieldState            = new MamdaFieldState();
            public MamdaFieldState       mBidLowFieldState             = new MamdaFieldState();
            public MamdaFieldState       mAskPriceFieldState           = new MamdaFieldState();
            public MamdaFieldState       mAskSizeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mAskDepthFieldState           = new MamdaFieldState();
            public MamdaFieldState       mAskPartIdFieldState          = new MamdaFieldState();
            public MamdaFieldState       mAskClosePriceFieldState      = new MamdaFieldState();
            public MamdaFieldState       mAskCloseDateFieldState       = new MamdaFieldState();
            public MamdaFieldState       mAskPrevClosePriceFieldState  = new MamdaFieldState();
            public MamdaFieldState       mAskPrevCloseDateFieldState   = new MamdaFieldState();
            public MamdaFieldState       mAskHighFieldState            = new MamdaFieldState();
            public MamdaFieldState       mAskLowFieldState             = new MamdaFieldState();
            public MamdaFieldState       mEventSeqNumFieldState        = new MamdaFieldState();
            public MamdaFieldState       mEventTimeFieldState          = new MamdaFieldState();
            public MamdaFieldState       mQuoteDateFieldState          = new MamdaFieldState();
            public MamdaFieldState       mQuoteQualStrFieldState       = new MamdaFieldState();
            public MamdaFieldState       mQuoteQualNativeFieldState    = new MamdaFieldState();
            public MamdaFieldState       mShortSaleBidTickFieldState   = new MamdaFieldState();
            public MamdaFieldState       mShortSaleCircuitBreakerFieldState = new MamdaFieldState(); 
            public MamdaFieldState       mTmpQuoteCountFieldState      = new MamdaFieldState();
            public MamdaFieldState       mQuoteCountFieldState         = new MamdaFieldState();
            public MamdaFieldState       mConflateCountFieldState      = new MamdaFieldState();
            public MamdaFieldState       mAskTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mBidTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mAskIndicatorFieldState       = new MamdaFieldState();
            public MamdaFieldState       mBidIndicatorFieldState       = new MamdaFieldState();
            public MamdaFieldState       mAskUpdateCountFieldState     = new MamdaFieldState();
            public MamdaFieldState       mBidUpdateCountFieldState     = new MamdaFieldState();
            public MamdaFieldState       mAskYieldFieldState           = new MamdaFieldState();
            public MamdaFieldState       mBidYieldFieldState           = new MamdaFieldState();
		}

		#endregion MamdaQuoteCache

		#region State

		// A single static instance for all Listener instances
		// will be sufficient as each updater will take a reference to the cache
		// as an argument.
		private static QuoteUpdate[] mUpdaters = null;
		private static object mUpdatersGuard = new object();
        private static int mMaxFid = 0;
        private bool mIgnoreUpdate = false;

		private ArrayList mHandlers = new ArrayList();
		// The following fields are used for caching the offical last
		// price and related fields.  These fields can be used by
		// applications for reference and will be passed for recaps.
		private readonly MamdaQuoteCache mQuoteCache = new MamdaQuoteCache();

		// Used for all field iteration processing
		private FieldIterator mFieldIterator = null;

		// Additional fields for gaps:
		private long                  mGapBegin = 0;
		private long                  mGapEnd   = 0;
        private MamdaFieldState       mGapBeginFieldState = new MamdaFieldState();
        private MamdaFieldState       mGapEndFieldState = new MamdaFieldState();
		// true if an generic update is infact a quote
		private bool                  mLastGenericMsgWasQuote = false;

		#endregion State

		#endregion Implementation details

	}
}

