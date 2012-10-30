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

using System;
using System.Threading;
using System.Collections;

namespace Wombat
{
	/// <summary>
	/// MamdaAuctionListener is a class that specializes in handling Auction
	/// updates.  Developers provide their own implementation of the
	/// MamdaAuctionHandler interface and will be delivered notifications for
	/// Auctions and Auction closing prices.  An obvious application for this
	/// MAMDA class is any kind of Auction tick capture application.
	/// </summary>
	/// <remarks>
	/// Note: The MamdaAuctionListener class caches Auction-related field
	/// values.  Among other reasons, caching of these fields makes it
	/// possible to provide complete Auction-related callbacks, even when the
	/// publisher (e.g., feed handler) is only publishing deltas containing
	/// modified fields.
	/// </remarks>
	public class MamdaAuctionListener : 
		MamdaMsgListener,
		MamdaAuctionRecap,
		MamdaAuctionUpdate
	{
		/// <summary>
		/// Create a specialized Auction listener.  This listener handles
		/// Auction updates, quote recaps, and Auction gap notifications.
		/// </summary>
		public MamdaAuctionListener()
		{
			//Initialize the cache
			clearCache(mAuctionCache);
       
			//Single iterator instance needed for all messages
			mFieldIterator = new FieldIterator (this);
		}

		private static void clearCache(MamdaAuctionCache cache)
		{
			cache.mSymbol        = null;
			cache.mPartId        = null;
			cache.mSrcTime       = DateTime.MinValue;
			cache.mActTime       = DateTime.MinValue;
			cache.mLineTime      = DateTime.MinValue;
			cache.mSendTime      = DateTime.MinValue;
            cache.mEventTime     = DateTime.MinValue;
            cache.mEventSeqNum   = 0;

			cache.mUncrossPrice.clear();
			cache.mUncrossVolume = 0;
			cache.mUncrossPriceIndStr = null;

            
            //Field State
            cache.mSymbolFieldState             = MamdaFieldState.NOT_INITIALISED;
            cache.mPartIdFieldState             = MamdaFieldState.NOT_INITIALISED;
            cache.mSrcTimeFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mActTimeFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mLineTimeFieldState           = MamdaFieldState.NOT_INITIALISED;
            cache.mSendTimeFieldState           = MamdaFieldState.NOT_INITIALISED;
            cache.mEventTimeFieldState          = MamdaFieldState.NOT_INITIALISED;
            cache.mEventSeqNumFieldState        = MamdaFieldState.NOT_INITIALISED;
            cache.mUncrossPriceFieldState       = MamdaFieldState.NOT_INITIALISED;
            cache.mUncrossVolumeFieldState      = MamdaFieldState.NOT_INITIALISED;
            cache.mUncrossPriceIndStrFieldState = MamdaFieldState.NOT_INITIALISED;
		}

		/// <summary>
		/// Add a specialized Auction handler.  Currently, only one
		/// handler can (and must) be registered.
		/// </summary>
		/// <param name="handler"></param>
		public void addHandler(MamdaAuctionHandler handler)
		{
			mHandlers.Add(handler);
		}

		public string getSymbol()
		{
			return mAuctionCache.mSymbol;
		}

		public string getPartId()
		{
			return mAuctionCache.mPartId;
		}

		public DateTime getSrcTime()
		{
			return mAuctionCache.mSrcTime;
		}

		public DateTime getActivityTime()
		{
			return mAuctionCache.mActTime;
		}

		public DateTime getLineTime()
		{
			return mAuctionCache.mLineTime;
		}

		public DateTime getSendTime()
		{
			return mAuctionCache.mSendTime;
		}
        
		public DateTime getEventTime()
		{
			return mAuctionCache.mEventTime;
		}

        public long getEventSeqNum()
		{
			return (long) mAuctionCache.mEventSeqNum;
		}
		public MamaPrice getUncrossPrice()
		{
			return mAuctionCache.mUncrossPrice;
		}

		public long getUncrossVolume()
		{
			return (long) mAuctionCache.mUncrossVolume;
		}

        public long getUncrossPriceInd()
        {
            return (long) MamdaUncrossPriceInd.mamdaAuctionPriceIndFromString (mAuctionCache.mUncrossPriceIndStr);
        }


        //Field State Accessors     
        
        public MamdaFieldState getSymbolFieldState()
        {
          return mAuctionCache.mSymbolFieldState;
        }

        public MamdaFieldState getPartIdFieldState()
        {
          return mAuctionCache.mPartIdFieldState;
        }

        public MamdaFieldState getSrcTimeFieldState()
        {
          return mAuctionCache.mSrcTimeFieldState;
        }

        public MamdaFieldState getActivityTimeFieldState()
        {
          return mAuctionCache.mActTimeFieldState;
        }


        public MamdaFieldState getLineTimeFieldState()
        {
          return mAuctionCache.mLineTimeFieldState;
        }

        public MamdaFieldState getSendTimeFieldState()
        {
          return mAuctionCache.mSendTimeFieldState;
        }
        
        public MamdaFieldState getEventTimeFieldState()
        {
          return mAuctionCache.mEventTimeFieldState;
        }
        
        public MamdaFieldState getEventSeqNumFieldState()
        {
          return mAuctionCache.mEventSeqNumFieldState;
        }

        public MamdaFieldState getUncrossPriceFieldState()
        {
          return mAuctionCache.mUncrossPriceFieldState;
        }

        public MamdaFieldState getUncrossVolumeFieldState()
        {
          return mAuctionCache.mUncrossVolumeFieldState;
        }

        public MamdaFieldState getUncrossPriceIndFieldState()
        {
          return mAuctionCache.mUncrossPriceIndStrFieldState;
        }
   
/*      End Field State Accessors       */
        
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
					if(!MamdaAuctionFields.isSet())
					{
						return;
					}

					if (mUpdaters == null)
					{
						AuctionUpdate[] updaters = createUpdaters();
						Thread.MemoryBarrier();
						mUpdaters = updaters;
					}
				}
			}

			// If msg is a Auction-related message, invoke the
			// appropriate callback:
			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
					handleAuctionMessage(subscription, msg, msgType);
					break;
                default:
					break;
			}
		}
        
        public string getFieldAsString(MamaMsgField field)
        {
            switch (field.getType())
            {
                case mamaFieldType.MAMA_FIELD_TYPE_I8:
                case mamaFieldType.MAMA_FIELD_TYPE_U8:
                case mamaFieldType.MAMA_FIELD_TYPE_I16:
                case mamaFieldType.MAMA_FIELD_TYPE_U16:
                case mamaFieldType.MAMA_FIELD_TYPE_I32:
                case mamaFieldType.MAMA_FIELD_TYPE_U32:
                    return field.getU32().ToString();
                case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                    return field.getString();
                default:
                    break;
            }
			
			return null;
        }        

		private void handleRecap(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
			foreach (MamdaAuctionHandler handler in mHandlers)
			{
				handler.onAuctionRecap(subscription, this, msg, this);
			}
		}

		private void handleUpdate(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{         
			foreach (MamdaAuctionHandler handler in mHandlers)
			{              
				handler.onAuctionUpdate (subscription, this, msg, this, this);
			}
		}

		private void handleAuctionMessage(
			MamdaSubscription  subscription,
			MamaMsg            msg,
            mamaMsgType        msgType)
		{          
            updateFieldStates();
            
            lock (this)
			{
				msg.iterateFields(mFieldIterator, null, null);
			}       

            switch(msgType)
            {
                case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
                    handleRecap(subscription, msg);
                    break;
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
					handleUpdate(subscription, msg);
					break;
                default:
					break; 
            
            
            }
		}
	    

		private static AuctionUpdate[] createUpdaters ()
		{
            mMaxFid = MamdaAuctionFields.getMaxFid();
			AuctionUpdate[] updaters = new AuctionUpdate[mMaxFid+1];

			addToUpdatersList(updaters, MamdaCommonFields.SYMBOL, new MamdaAuctionSymbol());
			addToUpdatersList(updaters, MamdaCommonFields.ISSUE_SYMBOL, new MamdaAuctionIssueSymbol());
			addToUpdatersList(updaters, MamdaCommonFields.PART_ID, new MamdaAuctionPartId());
			addToUpdatersList(updaters, MamdaCommonFields.SRC_TIME, new MamdaAuctionSrcTime());
			addToUpdatersList(updaters, MamdaCommonFields.ACTIVITY_TIME, new MamdaAuctionActivityTime());
			addToUpdatersList(updaters, MamdaCommonFields.LINE_TIME, new MamdaAuctionLineTime());
			addToUpdatersList(updaters, MamdaCommonFields.SEND_TIME, new MamdaAuctionSendTime());
			addToUpdatersList(updaters, MamdaAuctionFields.UNCROSS_PRICE, new MamdaAuctionUncrossPrice());
			addToUpdatersList(updaters, MamdaAuctionFields.UNCROSS_VOLUME, new MamdaAuctionUncrossVolume());
			addToUpdatersList(updaters, MamdaAuctionFields.UNCROSS_PRICE_IND, new MamdaAuctionUncrossPriceInd());
            
			return updaters;
		}

        private void updateFieldStates()
        {
            if(mAuctionCache.mSymbolFieldState             == MamdaFieldState.MODIFIED)
                mAuctionCache.mSymbolFieldState             = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mPartIdFieldState             == MamdaFieldState.MODIFIED)
                mAuctionCache.mPartIdFieldState             = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mSrcTimeFieldState            == MamdaFieldState.MODIFIED)
                mAuctionCache.mSrcTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mActTimeFieldState            == MamdaFieldState.MODIFIED)
                mAuctionCache.mActTimeFieldState            = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mLineTimeFieldState           == MamdaFieldState.MODIFIED)
                mAuctionCache.mLineTimeFieldState           = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mSendTimeFieldState           == MamdaFieldState.MODIFIED)
                mAuctionCache.mSendTimeFieldState           = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mUncrossPriceFieldState       == MamdaFieldState.MODIFIED)
                mAuctionCache.mUncrossPriceFieldState       = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mUncrossVolumeFieldState      == MamdaFieldState.MODIFIED)
                mAuctionCache.mUncrossVolumeFieldState      = MamdaFieldState.NOT_MODIFIED;
            if(mAuctionCache.mUncrossPriceIndStrFieldState    == MamdaFieldState.MODIFIED)
                mAuctionCache.mUncrossPriceIndStrFieldState    = MamdaFieldState.NOT_MODIFIED;
        }
        

		private static void addToUpdatersList(
			AuctionUpdate[]		updaters,
			MamaFieldDescriptor	fieldDesc,
			AuctionUpdate			updater)
		{
			if (fieldDesc == null) return;
			int fieldId = fieldDesc.getFid();
            if (fieldId <= mMaxFid)
            {
                updaters[fieldId] = updater;
            }
		}

		private interface AuctionUpdate
		{
			void onUpdate(MamdaAuctionListener listener, MamaMsgField field);
		}

		private class MamdaAuctionSymbol : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mSymbol = field.getString();
                listener.mAuctionCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaAuctionIssueSymbol : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mSymbol = field.getString();
                listener.mAuctionCache.mSymbolFieldState = MamdaFieldState.MODIFIED;
			}
		}
		private class MamdaAuctionPartId : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mPartId = field.getString();
                listener.mAuctionCache.mPartIdFieldState = MamdaFieldState.MODIFIED;
			}
		}
        
        private class MamdaAuctionSrcTime : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mSrcTime = field.getDateTime();
                listener.mAuctionCache.mSrcTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
        
        private class MamdaAuctionLineTime : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mLineTime = field.getDateTime();
                listener.mAuctionCache.mLineTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}
        
		private class MamdaAuctionActivityTime : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mActTime = field.getDateTime();
                listener.mAuctionCache.mActTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}     

		private class MamdaAuctionSendTime : AuctionUpdate
		{
			public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
			{
				listener.mAuctionCache.mSendTime = field.getDateTime();
                listener.mAuctionCache.mSendTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}        
        
        private class MamdaAuctionUncrossPrice : AuctionUpdate
        {
            public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
            {
                MamaPrice price;
                price = field.getPrice();
                if (listener.mAuctionCache.mUncrossPrice.getValue() != price.getValue())
                {
                    listener.mAuctionCache.mUncrossPrice.copy(price);
                    listener.mAuctionCache.mUncrossPriceFieldState = MamdaFieldState.MODIFIED;
                }
            }
        }
               
        private class MamdaAuctionUncrossVolume : AuctionUpdate
        {
            public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
            {
                double volume = field.getF64();
                if (listener.mAuctionCache.mUncrossVolume != volume)
                {
                    listener.mAuctionCache.mUncrossVolume = volume;
                    listener.mAuctionCache.mUncrossVolumeFieldState = MamdaFieldState.MODIFIED;
                }
            }
        }
        
        private class MamdaAuctionUncrossPriceInd : AuctionUpdate
        {
            public void onUpdate(MamdaAuctionListener listener, MamaMsgField field)
            {
                string indicator = listener.getFieldAsString(field);
                if (string.Compare(listener.mAuctionCache.mUncrossPriceIndStr, indicator) != 0)
                {
                    listener.mAuctionCache.mUncrossPriceIndStr = indicator;
                    listener.mAuctionCache.mUncrossPriceIndStrFieldState = MamdaFieldState.MODIFIED;
                }
            }
        }        
        
	    
		/*************************************************************
		*Private class used for Field Iteration
		************************************************************/
		private class FieldIterator : MamaMsgFieldIterator
		{
			private MamdaAuctionListener mListener;
	        
			public FieldIterator(MamdaAuctionListener listener)
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
                        AuctionUpdate updater = (AuctionUpdate)mUpdaters[fieldId];

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

		#region MamdaAuctionCache

		private class MamdaAuctionCache
		{
			public string          mSymbol;
			public string          mPartId;
			public DateTime        mSrcTime        = DateTime.MinValue;
			public DateTime        mActTime        = DateTime.MinValue;
			public DateTime        mLineTime       = DateTime.MinValue;
			public DateTime        mSendTime       = DateTime.MinValue;
   			public DateTime        mEventTime      = DateTime.MinValue;
            public long            mEventSeqNum;

            public MamaPrice       mUncrossPrice   = new MamaPrice();
			public double          mUncrossVolume;
			public string          mUncrossPriceIndStr;
            
            //Field State
            
            public MamdaFieldState       mSymbolFieldState              = new MamdaFieldState();
            public MamdaFieldState       mPartIdFieldState              = new MamdaFieldState();
            public MamdaFieldState       mSrcTimeFieldState             = new MamdaFieldState();
            public MamdaFieldState       mActTimeFieldState             = new MamdaFieldState();
            public MamdaFieldState       mLineTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mSendTimeFieldState            = new MamdaFieldState();
            public MamdaFieldState       mEventTimeFieldState           = new MamdaFieldState();
            public MamdaFieldState       mEventSeqNumFieldState         = new MamdaFieldState();
            
            public MamdaFieldState       mUncrossPriceFieldState        = new MamdaFieldState();
            public MamdaFieldState       mUncrossVolumeFieldState       = new MamdaFieldState();
            public MamdaFieldState       mUncrossPriceIndStrFieldState  = new MamdaFieldState();
		}

		#endregion MamdaAuctionCache

		#region State

		// A single static instance for all Listener instances
		// will be sufficient as each updater will take a reference to the cache
		// as an argument.
		private static AuctionUpdate[] mUpdaters = null;
		private static object mUpdatersGuard = new object();
        private static int mMaxFid = 0;

		private ArrayList mHandlers = new ArrayList();
        
        private readonly MamdaAuctionCache mAuctionCache = new MamdaAuctionCache();
        
		// Used for all field iteration processing
		private FieldIterator mFieldIterator = null;

		#endregion State

		#endregion Implementation details

	}
}

