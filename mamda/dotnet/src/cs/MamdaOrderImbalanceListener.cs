/* $Id: MamdaOrderImbalanceListener.cs,v 1.8.30.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
using System.Collections;
using System.Threading;

namespace Wombat
{
	/// <summary>
	/// A MamdaOrderImbalanceListener is class that specializes in handling
	/// imbalance order updates. An imbalance order occurs when too many 
	/// orders of a particular type - either buy, sell or limit - for listed
	/// securities and not enough of the other, matching orders are received by 
	/// an exchange.
	/// Developers provide their own implementation of the
	/// MamdaOrderImbalanceHandler interface and will be delivered notifications
	/// for imbalance updates.  
	/// </summary>
	public class MamdaOrderImbalanceListener :
		MamdaMsgListener,
		MamdaBasicEvent,
		MamdaBasicRecap,
		MamdaOrderImbalanceRecap
	{
		public MamdaOrderImbalanceListener()
		{
			clearCache (mOrderImbalanceCache);       			

			mFieldIterator = new FieldIterator(this);
		}

		/// <summary>
		/// Registers handler interested in receiving update 
		/// and recap events.
		/// </summary>
		/// <param name="handler">the handler to be registered.</param>
		public void addHandler(MamdaOrderImbalanceHandler handler)
		{
			mHandlers.Add(handler);
		}
    
		/// <summary>
		/// Returns the high indication price of the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getHighIndicationPrice() 
		{
			return mOrderImbalanceCache.mHighIndicationPrice;
		}

		/// <summary>
		/// Returns the low indication price of the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getLowIndicationPrice()
		{
			return mOrderImbalanceCache.mLowIndicationPrice;
		}

		/// <summary>
		/// Returns the indication price for the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getImbalancePrice()
		{
			return mOrderImbalanceCache.mIndicationPrice;
		}

		/// <summary>
		/// Returns the buy volume of the imbalance order
		/// </summary>
		/// <returns></returns>
		public  long getBuyVolume ()
		{
			return mOrderImbalanceCache.mBuyVolume;
		}

		/// <summary>
		/// Returns the sell volume of the imbalance order
		/// </summary>
		/// <returns></returns>
		public  long getSellVolume ()
		{
			return mOrderImbalanceCache.mSellVolume;
		}

		/// <summary>
		/// Returns the match volume of the imbalance order
		/// </summary>
		/// <returns></returns>
		public  long getMatchVolume ()
		{
			return mOrderImbalanceCache.mMatchVolume;
		}

		/// <summary>
		/// Returns the order imbalance state
		/// </summary>
		/// <returns></returns>
		public  String getImbalanceState ()
		{
			return mOrderImbalanceCache.mSecurityStatusQual;
		} 

		/// <summary>
		/// Returns the match price for the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getMatchPrice ()
		{
			return mOrderImbalanceCache.mInsideMatchPrice;
		}

		/// <summary>
		/// Returns the far clearing price for the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getFarClearingPrice ()
		{
			return mOrderImbalanceCache.mFarClearingPrice;
		}

		/// <summary>
		/// Returns the near clearing price for the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  MamaPrice getNearClearingPrice ()
		{
			return mOrderImbalanceCache.mNearClearingPrice;
		}

		/// <summary>
		/// Returns the no clearing price for the imbalance order.
		/// </summary>
		/// <returns></returns>
		public  char getNoClearingPrice ()
		{
			return mOrderImbalanceCache.mNoClearingPrice;
		}
    
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public  char getPriceVarInd ()
		{
			return mOrderImbalanceCache.mPriceVarInd;
		}

		/// <summary>
		/// Returns the imbalance cross type
		/// </summary>
		/// <returns></returns>
		public  char getCrossType ()
		{
			return mOrderImbalanceCache.mCrossType;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getEventTime"/>
		/// </summary>
		/// <returns></returns>
		public  DateTime getEventTime ()
		{
			return mOrderImbalanceCache.mEventTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getEventSeqNum"/>
		/// </summary>
		/// <returns></returns>
		public long getEventSeqNum ()
		{
			return mOrderImbalanceCache.mEventSeqNum;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getActivityTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getActivityTime ()
		{
			return mOrderImbalanceCache.mActTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getSrcTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getSrcTime ()
		{
			return mOrderImbalanceCache.mSrcTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getLineTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getLineTime ()
		{
			return mOrderImbalanceCache.mLineTime;
		}

		/// <summary>
		/// <see cref="MamdaBasicEvent.getSendTime"/>
		/// </summary>
		/// <returns></returns>
		public DateTime getSendTime ()
		{
			return mOrderImbalanceCache.mSendTime;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public int getMsgType ()
		{
			return mOrderImbalanceCache.mMsgType;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public String getIssueSymbol ()
		{
			return mOrderImbalanceCache.mIssueSymbol;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public String getSymbol ()
		{
			return mOrderImbalanceCache.mSymbol;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public String getPartId ()
		{
			return mOrderImbalanceCache.mPartId;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public int getSeqNum ()
		{
			return mOrderImbalanceCache.mSeqNum;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public String getSecurityStatusOrig ()
		{
			return mOrderImbalanceCache.mSecurityStatusOrig;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public DateTime getSecurityStatusTime ()
		{
			return mOrderImbalanceCache.mSecurityStatusTime;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public DateTime getAuctionTime ()
		{
			return mOrderImbalanceCache.mAuctionTime;
		}

/*      FieldState Accessors        */
        
        /// <summary>
        /// Returns the high indication price field state
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getHighIndicationPriceFieldState() 
        {
			return mOrderImbalanceCache.mHighIndicationPriceFieldState;
        }

        /// <summary>
        /// Returns the low indication price field state
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getLowIndicationPriceFieldState()
        {
			return mOrderImbalanceCache.mLowIndicationPriceFieldState;
        }

        /// <summary>
        /// Returns the indication price field state
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getImbalancePriceFieldState()
        {
			return mOrderImbalanceCache.mIndicationPriceFieldState;
        }

        /// <summary>
        /// Returns the buy volume field state of the imbalance order 
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getBuyVolumeFieldState()
        {
			return mOrderImbalanceCache.mBuyVolumeFieldState;
        }

        /// <summary>
        /// Returns the sell volume field state of the imbalance order
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getSellVolumeFieldState()
        {
			return mOrderImbalanceCache.mSellVolumeFieldState;
        }

        /// <summary>
        /// Returns the match volume field state of the imbalance order
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getMatchVolumeFieldState()
        {
			return mOrderImbalanceCache.mMatchVolumeFieldState;
        }

        /// <summary>
        /// Returns the order imbalance state field state
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getImbalanceStateFieldState()
        {
			return mOrderImbalanceCache.mSecurityStatusQualFieldState;
        } 

        /// <summary>
        /// Returns the match price field state for the imbalance order.
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getMatchPriceFieldState()
        {
			return mOrderImbalanceCache.mInsideMatchPriceFieldState;
        }

        /// <summary>
        /// Returns the far clearing price field state for the imbalance order.
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getFarClearingPriceFieldState()
        {
			return mOrderImbalanceCache.mFarClearingPriceFieldState;
        }

        /// <summary>
        /// Returns the near clearing price field state for the imbalance order.
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getNearClearingPriceFieldState()
        {
			return mOrderImbalanceCache.mNearClearingPriceFieldState;
        }

        /// <summary>
        /// Returns the no clearing price field state for the imbalance order.
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getNoClearingPriceFieldState()
        {
			return mOrderImbalanceCache.mNoClearingPriceFieldState;
        }
    
        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getPriceVarIndFieldState()
        {
			return mOrderImbalanceCache.mPriceVarIndFieldState;
        }

        /// <summary>
        /// Returns the imbalance cross type field state
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getCrossTypeFieldState()
        {
			return mOrderImbalanceCache.mCrossTypeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public  MamdaFieldState getEventTimeFieldState()
        {
			return mOrderImbalanceCache.mEventTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getEventSeqNumFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getEventSeqNumFieldState()
        {
			return mOrderImbalanceCache.mEventSeqNumFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getActivityTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getActivityTimeFieldState()
        {
			return mOrderImbalanceCache.mActTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getSrcTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSrcTimeFieldState()
        {
			return mOrderImbalanceCache.mSrcTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getLineTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getLineTimeFieldState()
        {
			return mOrderImbalanceCache.mLineTimeFieldState;
        }

        /// <summary>
        /// <see cref="MamdaBasicEvent.getSendTimeFieldState"/>
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSendTimeFieldState()
        {
			return mOrderImbalanceCache.mSendTimeFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getMsgTypeFieldState()
        {
			return mOrderImbalanceCache.mMsgTypeFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getIssueSymbolFieldState()
        {
			return mOrderImbalanceCache.mIssueSymbolFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSymbolFieldState()
        {
			return mOrderImbalanceCache.mSymbolFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getPartIdFieldState()
        {
			return mOrderImbalanceCache.mPartIdFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSeqNumFieldState()
        {
			return mOrderImbalanceCache.mSeqNumFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSecurityStatusOrigFieldState()
        {
			return mOrderImbalanceCache.mSecurityStatusOrigFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getSecurityStatusTimeFieldState()
        {
			return mOrderImbalanceCache.mSecurityStatusTimeFieldState;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        public MamdaFieldState getAuctionTimeFieldState()
        {
			return mOrderImbalanceCache.mAuctionTimeFieldState;
        }

/*      End FieldState Accessors        */

		/// <summary>
		/// Resets the recap object to cached values.
		/// </summary>
		/// <param name="recap"></param>
		public void  populateRecap(MamdaConcreteOrderImbalanceRecap recap)
		{
			lock (this)
			{
				recap.clear();
				recap.setHighIndicationPrice (mOrderImbalanceCache.mHighIndicationPrice);
				recap.setLowIndicationPrice  (mOrderImbalanceCache.mLowIndicationPrice);
				recap.setImbalancePrice      (mOrderImbalanceCache.mIndicationPrice);
				recap.setBuyVolume           (mOrderImbalanceCache.mBuyVolume);
				recap.setSellVolume          (mOrderImbalanceCache.mSellVolume);
				recap.setMatchVolume         (mOrderImbalanceCache.mMatchVolume);
				recap.setImbalanceState      (mOrderImbalanceCache.mSecurityStatusQual);
				recap.setMatchPrice          (mOrderImbalanceCache.mInsideMatchPrice);
				recap.setFarClearingPrice    (mOrderImbalanceCache.mFarClearingPrice);
				recap.setNearClearingPrice   (mOrderImbalanceCache.mNearClearingPrice);
				recap.setNoClearingPrice     (mOrderImbalanceCache.mNoClearingPrice);
				recap.setPriceVarInd         (mOrderImbalanceCache.mPriceVarInd);
				recap.setCrossType           (mOrderImbalanceCache.mCrossType);
				recap.setEventTime           (mOrderImbalanceCache.mEventTime);
				recap.setEventSeqNum         (mOrderImbalanceCache.mEventSeqNum);
				recap.setMsgType             (mOrderImbalanceCache.mMsgType);
				recap.setIssueSymbol         (mOrderImbalanceCache.mIssueSymbol);
				recap.setPartId              (mOrderImbalanceCache.mPartId);
				recap.setSeqNum              (mOrderImbalanceCache.mSeqNum);
				recap.setSecurityStatusOrig  (mOrderImbalanceCache.mSecurityStatusOrig);
				recap.setSecurityStatusTime  (mOrderImbalanceCache.mSecurityStatusTime);
				recap.setAuctionTime         (mOrderImbalanceCache.mAuctionTime);
				recap.setSrcTime             (mOrderImbalanceCache.mSrcTime);
				recap.setActivityTime        (mOrderImbalanceCache.mActTime);
				recap.setSymbol				 (mOrderImbalanceCache.mSymbol);
				recap.setLineTime			 (mOrderImbalanceCache.mLineTime);
				recap.setSendTime			 (mOrderImbalanceCache.mSendTime);
			}
		}

		/// <summary>
		/// Returns the imbalance side of the imbalance order. 
		/// This is determined by the Buy and Sell volumes.
		/// </summary>
		/// <returns></returns>
		public MamdaOrderImbalanceSide getImbalanceSide ()
		{
			if (mOrderImbalanceCache.mBuyVolume > 
				mOrderImbalanceCache.mSellVolume)
			{
				mSide = MamdaOrderImbalanceSide.BID_SIDE;
			}
			else
			{
				mSide = MamdaOrderImbalanceSide.ASK_SIDE;
			}
			return mSide;
		}
   
		/// <summary>
		/// Returns the imbalance volume, which is either the sell or buy volume
		/// </summary>
		/// <returns></returns>
		public long getImbalanceVolume ()
		{
			if (MamdaOrderImbalanceSide.BID_SIDE == getImbalanceSide())
			{
				return (getBuyVolume () - getSellVolume());
			}  
			return (getSellVolume () - getBuyVolume());
		}

		/// <summary>
		/// Implementation of MamdaListener interface.
		/// </summary>
		public void onMsg(
			MamdaSubscription	subscription,
			MamaMsg				msg,
			mamaMsgType			msgType)
		{
			if (mUpdaters == null)
			{
				lock (mUpdatersGuard)
				{
					if(!MamdaOrderImbalanceFields.isSet())
					{
						return;
					}

					if (mUpdaters == null)
					{
						ImbalanceOrderUpdate[] updaters = createUpdaters();
						Thread.MemoryBarrier();
						mUpdaters = updaters;
					}
				}
			}

			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
                    updateFieldStates();
					updateOrderImbalanceFields(subscription, msg, msgType);
					break;
				default:
					break;
			}
		}

		#region Implementation details

		private void handleRecap(
			MamdaSubscription	subscription,
			MamaMsg				msg)
		{
			foreach (MamdaOrderImbalanceHandler handler in mHandlers)
			{
				handler.onOrderImbalanceRecap(subscription, this, msg);
			}
		}
	   
		private void handleOrderImbalance(
			MamdaSubscription	subscription,
			MamaMsg				msg)
		{
			foreach (MamdaOrderImbalanceHandler handler in mHandlers)
			{
				handler.onOrderImbalance(subscription, this, msg);
			}
		}
	    
		private void handleNoOrderImbalance(
			MamdaSubscription	subscription,
			MamaMsg				msg)
		{
			foreach (MamdaOrderImbalanceHandler handler in mHandlers)
			{
				handler.onNoOrderImbalance(subscription, this, msg);
			}
		}

        private void updateFieldStates()
        {
        if (mOrderImbalanceCache.mHighIndicationPriceFieldState == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mHighIndicationPriceFieldState = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mLowIndicationPriceFieldState  == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mLowIndicationPriceFieldState  = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mIndicationPriceFieldState     == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mIndicationPriceFieldState     = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mBuyVolumeFieldState           == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mBuyVolumeFieldState           = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSellVolumeFieldState          == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSellVolumeFieldState          = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mMatchVolumeFieldState         == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mMatchVolumeFieldState         = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSecurityStatusQualFieldState  == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSecurityStatusQualFieldState  = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mInsideMatchPriceFieldState    == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mInsideMatchPriceFieldState    = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mFarClearingPriceFieldState    == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mFarClearingPriceFieldState    = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mNearClearingPriceFieldState   == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mNearClearingPriceFieldState   = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mNoClearingPriceFieldState     == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mNoClearingPriceFieldState     = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mPriceVarIndFieldState         == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mPriceVarIndFieldState         = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mCrossTypeFieldState           == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mCrossTypeFieldState           = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mEventTimeFieldState           == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mEventTimeFieldState           = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mEventSeqNumFieldState         == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mEventSeqNumFieldState         = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSrcTimeFieldState             == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSrcTimeFieldState             = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mActTimeFieldState             == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mActTimeFieldState             = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mMsgTypeFieldState             == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mMsgTypeFieldState             = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mIssueSymbolFieldState         == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mIssueSymbolFieldState         = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mPartIdFieldState              == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mPartIdFieldState              = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSymbolFieldState              == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSymbolFieldState              = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSeqNumFieldState              == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSeqNumFieldState              = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSecurityStatusOrigFieldState  == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSecurityStatusOrigFieldState  = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSecurityStatusTimeFieldState  == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSecurityStatusTimeFieldState  = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mAuctionTimeFieldState         == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mAuctionTimeFieldState         = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mLineTimeFieldState            == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mLineTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        if (mOrderImbalanceCache.mSendTimeFieldState            == MamdaFieldState.MODIFIED)  
            mOrderImbalanceCache.mSendTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
        }
        
        private void updateOrderImbalanceFields(
			MamdaSubscription	subscription,
			MamaMsg				msg,
			mamaMsgType			msgType)
		{
			string securityStatus = null;
			lock (this)
			{
				MamaMsgField msgField = null;
	            
				/*The wSecStatusQual will not always be in the message 
				so you need to account for this by checking for it.
				*/
				try
				{
					msgField = msg.getField(MamdaOrderImbalanceFields.SECURITY_STATUS_QUAL, null);
				}
				catch (MamdaDataException ex)
				{
					throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_PLATFORM, ex.Message);
				}
				if (msgField == null) //does not exist
					return;

				securityStatus = msgField.getString();

				if (isImbalanceType(securityStatus))
				{
					clearCache(mOrderImbalanceCache);
				}
				msg.iterateFields(mFieldIterator, null,null);
			}

			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
					handleRecap(subscription, msg);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
				{
					int value = 0;
					if (securityStatus != null)
					{
						value = MamdaOrderImbalanceType.stringToValue(securityStatus);
						if (MamdaOrderImbalanceType.isMamdaImbalanceOrder(value))
						{
							handleOrderImbalance(subscription,msg); 
						}
						else
						{
							handleNoOrderImbalance(subscription, msg); 
						}
					}
				}
				break;
			default:
				break;
			}
	        
		}
	    
		private static bool isImbalanceType(string securityStatus)
		{
			int value = MamdaOrderImbalanceType.stringToValue(securityStatus);
			return MamdaOrderImbalanceType.isMamdaOrderImbalanceType(value);
		}

		private static ImbalanceOrderUpdate[] createUpdaters()
		{
            mMaxFid = MamdaOrderImbalanceFields.getMaxFid();
			ImbalanceOrderUpdate[] updaters = new ImbalanceOrderUpdate[mMaxFid+1];

			addUpdaterToList(updaters, MamdaOrderImbalanceFields.HIGH_INDICATION_PRICE, new ImbalanceHighIndicationPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.LOW_INDICATION_PRICE, new ImbalanceLowIndicationPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.INDICATION_PRICE, new ImbalanceIndicationPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.BUY_VOLUME, new ImbalanceBuyVolume());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SELL_VOLUME, new ImbalanceSellVolume());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.MATCH_VOLUME, new ImbalanceMatchVolume());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SECURITY_STATUS_QUAL, new ImbalanceSecurityStatusQual());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.INSIDE_MATCH_PRICE, new ImbalanceInsideMatchPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.FAR_CLEARING_PRICE, new ImbalanceFarClearingPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.NEAR_CLEARING_PRICE, new ImbalanceNearClearingPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.NO_CLEARING_PRICE, new ImbalanceNoClearingPrice());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.PRICE_VAR_IND, new ImbalancePriceVarInd());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.CROSS_TYPE, new ImbalanceCrossType());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.MSG_TYPE, new ImbalanceMsgType());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.ISSUE_SYMBOL, new ImbalanceIssueSymbol());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.PART_ID, new ImbalancePartId());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SEQ_NUM, new ImbalanceSeqNum());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SECURITY_STATUS_ORIG, new ImbalanceSecurityStatusOrig());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SECURITY_STATUS_TIME, new ImbalanceSecurityStatusTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.AUCTION_TIME, new ImbalanceAuctionTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SRC_TIME, new ImbalanceSrcTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.ACTIVITY_TIME, new ImbalanceActivityTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.LINE_TIME, new ImbalanceLineTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SEND_TIME, new ImbalanceSendTime());
			addUpdaterToList(updaters, MamdaOrderImbalanceFields.SYMBOL, new ImbalanceSymbol());

			return updaters;
		}

		private static void addUpdaterToList(
			ImbalanceOrderUpdate[] updaters,
			MamaFieldDescriptor fieldDesc,
			ImbalanceOrderUpdate updater)
		{
			if (fieldDesc == null) return;
			int fieldId = fieldDesc.getFid();
            if (fieldId <= mMaxFid)
            {
                updaters[fieldId] = updater;
            }
		}

		// Private class used for Field Iteration
		private class FieldIterator : MamaMsgFieldIterator
		{
			public FieldIterator(MamdaOrderImbalanceListener listener)
			{
				mListener = listener;
			}

			public void onField(
				MamaMsg        msg,
				MamaMsgField   field,
				object         closure)
			{
				try
				{
					int fieldId = field.getFid();
                    if (fieldId <= mMaxFid)
                    {
                        ImbalanceOrderUpdate updater = (ImbalanceOrderUpdate)mUpdaters[fieldId];
                        if (updater != null)
                        {
                            updater.onUpdate(mListener, field);
                        }
                    }
				}
				catch (Exception ex )
				{
					throw new MamaException(MamaStatus.mamaStatus.MAMA_STATUS_PLATFORM, ex.Message);
				}
			}

			private MamdaOrderImbalanceListener mListener;
		}

		private interface ImbalanceOrderUpdate
		{
			void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field);
		}

		private class ImbalanceHighIndicationPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mHighIndicationPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mHighIndicationPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceLowIndicationPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mLowIndicationPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mLowIndicationPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceIndicationPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mIndicationPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mIndicationPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceBuyVolume : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mBuyVolume = field.getI64 ();
                listener.mOrderImbalanceCache.mBuyVolumeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSellVolume : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSellVolume = field.getI64 ();
                listener.mOrderImbalanceCache.mSellVolumeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceMatchVolume : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mMatchVolume = field.getI64 ();
                listener.mOrderImbalanceCache.mMatchVolumeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSecurityStatusQual : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSecurityStatusQual = field.getString ();
                listener.mOrderImbalanceCache.mSecurityStatusQualFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceInsideMatchPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mInsideMatchPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mInsideMatchPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceFarClearingPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mFarClearingPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mFarClearingPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceNearClearingPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mNearClearingPrice.copy(field.getPrice ());
                listener.mOrderImbalanceCache.mNearClearingPriceFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceNoClearingPrice : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR :
						listener.mOrderImbalanceCache.mNoClearingPrice = field.getChar ();
                        listener.mOrderImbalanceCache.mNoClearingPriceFieldState = MamdaFieldState.MODIFIED;
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING :
						if (field.getString() != String.Empty)
						{
							listener.mOrderImbalanceCache.mNoClearingPrice = field.getString()[0];
                            listener.mOrderImbalanceCache.mNoClearingPriceFieldState = MamdaFieldState.MODIFIED;
						}
						else
						{
							listener.mOrderImbalanceCache.mNoClearingPrice = ' ';
                            listener.mOrderImbalanceCache.mNoClearingPriceFieldState = MamdaFieldState.MODIFIED;
						}
						break;
					default : break;
				}
			}
		}

		private class ImbalancePriceVarInd : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR :
						listener.mOrderImbalanceCache.mPriceVarInd = field.getChar ();
                        listener.mOrderImbalanceCache.mPriceVarIndFieldState = MamdaFieldState.MODIFIED;
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING :
						if (field.getString() != String.Empty)
						{
							listener.mOrderImbalanceCache.mPriceVarInd = field.getString ()[0];
                            listener.mOrderImbalanceCache.mPriceVarIndFieldState = MamdaFieldState.MODIFIED;
						}
						else
						{
							listener.mOrderImbalanceCache.mPriceVarInd = ' ';
                            listener.mOrderImbalanceCache.mPriceVarIndFieldState = MamdaFieldState.MODIFIED;
						}
						break;
					default : break;
				}
			}
		}

		private class ImbalanceCrossType : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR :
						listener.mOrderImbalanceCache.mCrossType = field.getChar ();
                        listener.mOrderImbalanceCache.mCrossTypeFieldState = MamdaFieldState.MODIFIED;
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING :
						if (field.getString() != String.Empty)
						{
							listener.mOrderImbalanceCache.mCrossType = field.getString()[0];
                            listener.mOrderImbalanceCache.mCrossTypeFieldState = MamdaFieldState.MODIFIED;
						}
						else
						{
							listener.mOrderImbalanceCache.mCrossType = ' ';
                            listener.mOrderImbalanceCache.mCrossTypeFieldState = MamdaFieldState.MODIFIED;
						}
						break;
					default : break;
				}
			}
		}

		private class ImabalanceEventTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mEventTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceEventSeqNum : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mEventSeqNum = field.getI32 ();
                listener.mOrderImbalanceCache.mEventSeqNumFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSrcTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSrcTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mSrcTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceActivityTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mActTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mActTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceLineTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mLineTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mLineTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSendTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSendTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mSendTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceMsgType : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mMsgType = field.getI32 ();
                listener.mOrderImbalanceCache.mMsgTypeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceIssueSymbol : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mIssueSymbol = field.getString ();
                listener.mOrderImbalanceCache.mIssueSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSymbol : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSymbol = field.getString ();
                listener.mOrderImbalanceCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalancePartId : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mPartId = field.getString ();
                listener.mOrderImbalanceCache.mPartIdFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSeqNum : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSeqNum = field.getI32 ();
                listener.mOrderImbalanceCache.mSeqNumFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSecurityStatusOrig : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSecurityStatusOrig = field.getString ();
                listener.mOrderImbalanceCache.mSecurityStatusOrigFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceSecurityStatusTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mSecurityStatusTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mSecurityStatusTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class ImbalanceAuctionTime : ImbalanceOrderUpdate
		{
			public void onUpdate(MamdaOrderImbalanceListener listener, MamaMsgField field)
			{
				listener.mOrderImbalanceCache.mAuctionTime = field.getDateTime ();
                listener.mOrderImbalanceCache.mAuctionTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		//The following fields are used for caching the offical prices
		//and related fields. These fields can be used by application
		// for reference and will be passed for recaps.
		protected class MamdaOrderImbalanceCache
		{
			public MamaPrice    mHighIndicationPrice = new MamaPrice();
			public MamaPrice    mLowIndicationPrice  = new MamaPrice();
			public MamaPrice    mIndicationPrice     = new MamaPrice();
			public long         mBuyVolume;
			public long         mSellVolume;
			public long         mMatchVolume;
			public String       mSecurityStatusQual;
			public MamaPrice    mInsideMatchPrice    = new MamaPrice();
			public MamaPrice    mFarClearingPrice    = new MamaPrice();
			public MamaPrice    mNearClearingPrice   = new MamaPrice();
			public char         mNoClearingPrice;
			public char         mPriceVarInd;
			public char         mCrossType;
			public DateTime     mEventTime           = DateTime.MinValue;
			public long         mEventSeqNum;
			public DateTime     mSrcTime             = DateTime.MinValue;
			public DateTime     mActTime             = DateTime.MinValue;

			public int          mMsgType;
			public String       mIssueSymbol         = null;
			public String       mPartId              = null;
			public String		mSymbol	        	 = null;
			public int          mSeqNum;
			public String       mSecurityStatusOrig  = null;
			public DateTime     mSecurityStatusTime  = DateTime.MinValue;
			public DateTime     mAuctionTime         = DateTime.MinValue;
			public DateTime		mLineTime			 = DateTime.MinValue;
			public DateTime		mSendTime			 = DateTime.MinValue;
            
            //FieldState
            public MamdaFieldState    mHighIndicationPriceFieldState = new MamdaFieldState();
            public MamdaFieldState    mLowIndicationPriceFieldState  = new MamdaFieldState();
            public MamdaFieldState    mIndicationPriceFieldState     = new MamdaFieldState();
            public MamdaFieldState    mBuyVolumeFieldState           = new MamdaFieldState();
            public MamdaFieldState    mSellVolumeFieldState          = new MamdaFieldState();
            public MamdaFieldState    mMatchVolumeFieldState         = new MamdaFieldState();
            public MamdaFieldState    mSecurityStatusQualFieldState  = new MamdaFieldState();
            public MamdaFieldState    mInsideMatchPriceFieldState    = new MamdaFieldState();
            public MamdaFieldState    mFarClearingPriceFieldState    = new MamdaFieldState();
            public MamdaFieldState    mNearClearingPriceFieldState   = new MamdaFieldState();
            public MamdaFieldState    mNoClearingPriceFieldState     = new MamdaFieldState();
            public MamdaFieldState    mPriceVarIndFieldState         = new MamdaFieldState();
            public MamdaFieldState    mCrossTypeFieldState           = new MamdaFieldState();
            public MamdaFieldState    mEventTimeFieldState           = new MamdaFieldState();
            public MamdaFieldState    mEventSeqNumFieldState         = new MamdaFieldState();
            public MamdaFieldState    mSrcTimeFieldState             = new MamdaFieldState();
            public MamdaFieldState    mActTimeFieldState             = new MamdaFieldState();

            public MamdaFieldState    mMsgTypeFieldState             = new MamdaFieldState();
            public MamdaFieldState    mIssueSymbolFieldState         = new MamdaFieldState();
            public MamdaFieldState    mPartIdFieldState              = new MamdaFieldState();
            public MamdaFieldState    mSymbolFieldState              = new MamdaFieldState();
            public MamdaFieldState    mSeqNumFieldState              = new MamdaFieldState();
            public MamdaFieldState    mSecurityStatusOrigFieldState  = new MamdaFieldState();
            public MamdaFieldState    mSecurityStatusTimeFieldState  = new MamdaFieldState();
            public MamdaFieldState    mAuctionTimeFieldState         = new MamdaFieldState();
            public MamdaFieldState    mLineTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState    mSendTimeFieldState            = new MamdaFieldState();
		}

		/// <summary>
		/// Clears all cached data by reseting to default values.
		/// </summary>
		/// <param name="cache">The imbalance order cache to clear.</param>
		protected static void clearCache (MamdaOrderImbalanceCache cache)
		{
			cache.mHighIndicationPrice.clear();
			cache.mLowIndicationPrice.clear();
			cache.mIndicationPrice.clear();
			cache.mBuyVolume			= 0;
			cache.mSellVolume			= 0;
			cache.mMatchVolume			= 0;
			cache.mSecurityStatusQual	= null;
			cache.mInsideMatchPrice.clear();
			cache.mFarClearingPrice.clear();
			cache.mNearClearingPrice.clear();
			cache.mNoClearingPrice		= '\0';
			cache.mPriceVarInd			= '\0';
			cache.mCrossType			= '\0';
			cache.mEventTime			= DateTime.MinValue;
			cache.mEventSeqNum			= 0;
			cache.mSrcTime				= DateTime.MinValue;
			cache.mActTime				= DateTime.MinValue;
			cache.mSendTime				= DateTime.MinValue;
			cache.mLineTime				= DateTime.MinValue;

			cache.mMsgType				= 0;
			cache.mIssueSymbol			= null;
			cache.mSymbol				= null;
			cache.mPartId				= null;
			cache.mSeqNum				= 0;
			cache.mSecurityStatusOrig	= null;
			cache.mSecurityStatusTime	= DateTime.MinValue;
			cache.mAuctionTime			= DateTime.MinValue;
            
            cache.mHighIndicationPriceFieldState    = MamdaFieldState.NOT_INITIALISED;
            cache.mLowIndicationPriceFieldState     = MamdaFieldState.NOT_INITIALISED;
            cache.mIndicationPriceFieldState        = MamdaFieldState.NOT_INITIALISED;
            cache.mBuyVolumeFieldState              = MamdaFieldState.NOT_INITIALISED;
            cache.mSellVolumeFieldState             = MamdaFieldState.NOT_INITIALISED;
            cache.mMatchVolumeFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mSecurityStatusQualFieldState     = MamdaFieldState.NOT_INITIALISED;
            cache.mInsideMatchPriceFieldState       = MamdaFieldState.NOT_INITIALISED;
            cache.mFarClearingPriceFieldState       = MamdaFieldState.NOT_INITIALISED;
            cache.mNearClearingPriceFieldState      = MamdaFieldState.NOT_INITIALISED;
            cache.mNoClearingPriceFieldState        = MamdaFieldState.NOT_INITIALISED;
            cache.mPriceVarIndFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mCrossTypeFieldState              = MamdaFieldState.NOT_INITIALISED;
            cache.mEventTimeFieldState              = MamdaFieldState.NOT_INITIALISED;
            cache.mEventSeqNumFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mSrcTimeFieldState                = MamdaFieldState.NOT_INITIALISED;
            cache.mActTimeFieldState                = MamdaFieldState.NOT_INITIALISED;
            cache.mSendTimeFieldState               = MamdaFieldState.NOT_INITIALISED;
            cache.mLineTimeFieldState               = MamdaFieldState.NOT_INITIALISED;

            cache.mMsgTypeFieldState                = MamdaFieldState.NOT_INITIALISED;
            cache.mIssueSymbolFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mSymbolFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mPartIdFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mSeqNumFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mSecurityStatusOrigFieldState     = MamdaFieldState.NOT_INITIALISED;
            cache.mSecurityStatusTimeFieldState     = MamdaFieldState.NOT_INITIALISED;
            cache.mAuctionTimeFieldState            = MamdaFieldState.NOT_INITIALISED;
		}
            
		#region State
            
		private ArrayList mHandlers = new ArrayList();    
		private MamdaOrderImbalanceSide mSide = MamdaOrderImbalanceSide.ASK_SIDE;
            
		private static ImbalanceOrderUpdate [] mUpdaters = null;
		private static object mUpdatersGuard = new object();
        private static int mMaxFid = 0;
            
		protected readonly MamdaOrderImbalanceCache mOrderImbalanceCache =
			new MamdaOrderImbalanceCache ();
            
		// Used for all field iteration processing
		private FieldIterator mFieldIterator = null;
            
		#endregion State
            
		#endregion Implementation details
            
	}          
}           
            
