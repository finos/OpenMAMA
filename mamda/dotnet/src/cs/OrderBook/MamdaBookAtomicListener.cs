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
using System.Collections;
using System.Text;
using System.Runtime.InteropServices;

namespace Wombat
{

    public class MamdaBookAtomicListener :
                   MamdaMsgListener,
                   MamdaBookAtomicLevel,
                   MamdaBookAtomicLevelEntry,
                   MamdaBookAtomicGap
    {
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

        public string getSymbol ()
        {
            return mSymbol;
        }

        public string getPartId ()
        {
            return mPartId;
        }

        public DateTime getSrcTime()
        {
            return mSrcTime;
        }

        public DateTime getActivityTime()
        {
            return mActivityTime;
        }

        public DateTime getLineTime()
        {
            return mLineTime;
        }

        public DateTime getSendTime()
        {
            return mSendTime;
        }

        public long getEventSeqNum()
        {
            return mEventSeqNum;
        }

        public DateTime getEventTime()
        {
            return mEventTime;
        }

        /*  Field State Accessors   */
        
        public MamdaFieldState   getSymbolFieldState()
        {
            return mSymbolFieldState;
        }

        public MamdaFieldState   getPartIdFieldState()
        {
            return mPartIdFieldState;
        }

        public MamdaFieldState   getSrcTimeFieldState()
        {
            return mSrcTimeFieldState;
        }

        public MamdaFieldState   getActivityTimeFieldState()
        {
            return mActivityTimeFieldState;
        }

        public MamdaFieldState   getLineTimeFieldState()
        {
            return mLineTimeFieldState;
        }

        public MamdaFieldState   getSendTimeFieldState()
        {
            return mSendTimeFieldState;
        }

        public MamdaFieldState getEventSeqNumFieldState()
        {
            return mEventSeqNumFieldState;
        }

        public MamdaFieldState   getEventTimeFieldState()
        {
            return mEventTimeFieldState;
        }

        /* End Field State Accessors    */
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
            return mPriceLevelSizeChange;
        }

        public char getPriceLevelAction ()
        {
            return (char) mPriceLevelAction;
        }

        public char getPriceLevelSide ()
        {
            return (char) mPriceLevelSide;
        }

        public DateTime getPriceLevelTime()
        {
            return mPriceLevelTime;
        }

        public double getPriceLevelNumEntries ()
        {
            return mPriceLevelNumEntries;
        }

        public long getPriceLevelActNumEntries ()
        {
            return mPriceLevelActNumEntries;
        }

        public char getPriceLevelEntryAction ()
        {
            return (char) mPriceLevelEntryAction;
        }

        public IntPtr getPriceLevelEntryIdIntPtr ()
        {
            return mPriceLevelEntryIdIntPtr;
        }

        public string getPriceLevelEntryId ()
        {
            return Marshal.PtrToStringAnsi(mPriceLevelEntryIdIntPtr);
        }

        public long getPriceLevelEntrySize ()
        {
            return mPriceLevelEntrySize;
        }

        public DateTime getPriceLevelEntryTime()
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

        public void onMsg (MamdaSubscription subscription,
                           MamaMsg           msg,
                           mamaMsgType       msgType)
        {	
			if(!MamdaOrderBookFields.isSet())
			{
				return;
			}

            try
            {
                switch (msgType)
                {
                case mamaMsgType.MAMA_MSG_TYPE_BOOK_CLEAR:
                    handleClear (subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_BOOK_INITIAL:
                case mamaMsgType.MAMA_MSG_TYPE_BOOK_RECAP:
                case mamaMsgType.MAMA_MSG_TYPE_BOOK_SNAPSHOT:
                    handleRecap (subscription, msg);
                    break;
                case mamaMsgType.MAMA_MSG_TYPE_BOOK_UPDATE:
                    handleUpdate (subscription, msg);
                    break;
                }
            }
            catch (MamaException)
            {
            }
        }


        private void handleClear (MamdaSubscription  subscription,
                                  MamaMsg            msg)
        {
            handleStandardFields (subscription, msg, false);
            invokeClearHandlers (subscription, msg);
        }

        private void handleRecap (MamdaSubscription  subscription,
                                  MamaMsg            msg)
        {
            handleStandardFields (subscription, msg, false);
            createDeltaFromMamaMsg (subscription, msg, true);
        }

        private void handleUpdate (MamdaSubscription  subscription,
                                   MamaMsg            msg)
        {
            handleStandardFields (subscription, msg, false);
            createDeltaFromMamaMsg (subscription, msg, false);
        }

        private void handleStandardFields (MamdaSubscription subscription,
                                           MamaMsg msg,
                                           bool checkSeqNum)
        {

            if (mSymbol == null)
				msg.tryString (MamdaCommonFields.SYMBOL, ref mSymbol);

			if (mPartId == null)
			{            
				if ((!msg.tryString (MamdaOrderBookFields.PART_ID, ref mPartId)) && (mSymbol != null))
		        {
		            // No explicit part ID in message, but maybe in symbol.
		            int lastDot = mSymbol.IndexOf (".");
		            if (lastDot != -1)
		            {
		                lastDot++;
		                int lastChar = mSymbol.Length;
		                if (lastDot != lastChar)
		                {
		                    mPartId = mSymbol.Substring (lastDot, (lastChar-lastDot));
		                }
		            }
		        }
			}

            msg.tryDateTime (MamdaOrderBookFields.SRC_TIME, ref mSrcTime);
            msg.tryDateTime (MamdaOrderBookFields.ACTIVITY_TIME, ref mActivityTime);
            msg.tryDateTime (MamdaOrderBookFields.LINE_TIME, ref mLineTime);
            msg.tryDateTime (MamaReservedFields.SendTime, ref mSendTime);

			mEventTime = msg.getDateTime (MamdaOrderBookFields.BOOK_TIME, mSrcTime);
            
            long seqNum = msg.getI64(MamaReservedFields.SeqNum);
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


        private void createDeltaFromMamaMsg (MamdaSubscription subscription,
                                             MamaMsg msg,
                                             bool isRecap)
        {
            if (mBookHandler != null)
            {
                mBookHandler.onBookAtomicBeginBook (subscription, this, isRecap);
            }

            // Note: the following test checks whether vector fields are part
            // of the data dictionary.  However, the vector fields may indeed
            // be part of the data dictionary, but the message may still
            // contain the non-vector style order book data.
            if (MamdaOrderBookFields.getHasVectorFields())
            {
                /* null is passed as default value otherwise
                 * getVectorMsg throws an exception if not found */
                MamaMsg[] msgLevels =
                    msg.getVectorMsg(MamdaOrderBookFields.PRICE_LEVELS, null);

                if (msgLevels != null)
                {        
                    createDeltaFromMamaMsgWithVectorFields (subscription,
                                                            msg,
                                                            msgLevels,
                                                            isRecap);
                }
                else
                {
                    createDeltaFromMamaMsgWithoutVectorFields (subscription, msg, isRecap);
                }
            }
            else
            {
                createDeltaFromMamaMsgWithoutVectorFields (subscription, msg, isRecap);
            }

            if (mBookHandler!=null)
            {
                mBookHandler.onBookAtomicEndBook (subscription, this);
            }
        }

        private void  createDeltaFromMamaMsgWithVectorFields( MamdaSubscription subscription,
                                                              MamaMsg msg,
                                                              MamaMsg[] msgLevels,
                                                              bool isRecap)
        {
            mPriceLevels = msgLevels.Length;

            for (int i = 0; i < msgLevels.Length; i++)
            {
                clearLevelFields();
                mPriceLevel = i+1;
                MamaMsg plMsg = msgLevels[i];
                getLevelInfoAndEntries(subscription, msg, plMsg, isRecap);
            }
        }


        private void  createDeltaFromMamaMsgWithoutVectorFields(
                                                MamdaSubscription subscription,
                                                MamaMsg msg,
                                                bool isRecap)
        {
            NullableInt numLevelFields_ = new NullableInt();

            int numLevelFieldInMsg = 1;

            if (msg.tryI32 (MamdaOrderBookFields.NUM_LEVELS, ref numLevelFields_))
            {
                numLevelFieldInMsg = numLevelFields_.Value;
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
                maxLevelFields = numLevelFieldInMsg;
            }
            mPriceLevels = maxLevelFields;

            for (int i = 1; i <= maxLevelFields; i++)
            {
                clearLevelFields();
                mPriceLevel = i;
                MamaMsg plMsg = null;

                if (MamdaOrderBookFields.PRICE_LEVEL.Length > 1)
                {
                    plMsg = msg.getMsg(MamdaOrderBookFields.PRICE_LEVEL[i], null);
                }
                
                if (plMsg == null)
                {
                    if (numLevelFieldInMsg == 1)
                    {
                        // Price level fields are probably be in the main message.
                        plMsg = msg;
                    }
                    else
                    {
                         throw new MamdaDataException (
                            "cannot find price level fields in MamaMsg");
                    }
                }
                getLevelInfoAndEntries(subscription, msg, plMsg, isRecap);
            }
        }


        private void getLevelInfoAndEntries(
                                MamdaSubscription subscription,
                                MamaMsg msg,
                                MamaMsg plMsg,
                                bool isRecap)
       {
			
            double priceLevelSizeChange = 0.0;
			int priceLevelActionInt = 0;
			int priceLevelSideInt = 0;
            MamaMsg entMsg = null;

            plMsg.tryPrice(MamdaOrderBookFields.PL_PRICE, ref mPriceLevelPrice);

            plMsg.tryF64(MamdaOrderBookFields.PL_SIZE, ref mPriceLevelSize);

            plMsg.tryF64(MamdaOrderBookFields.PL_NUM_ENTRIES, ref mPriceLevelNumEntries);

			if (plMsg.tryI32(MamdaOrderBookFields.PL_ACTION, ref priceLevelActionInt))
			{
				mPriceLevelAction = (sbyte) priceLevelActionInt;
			}

			if (plMsg.tryI32(MamdaOrderBookFields.PL_SIDE, ref priceLevelSideInt))
			{
				mPriceLevelSide = (sbyte) priceLevelSideInt;
			}
            
            // Optional order book fields:

			mPriceLevelTime = plMsg.getDateTime (MamdaOrderBookFields.PL_TIME, mEventTime);
            
            if ( plMsg.tryF64(MamdaOrderBookFields.PL_SIZE_CHANGE, ref priceLevelSizeChange) )
            {
                mPriceLevelSizeChange = (long) priceLevelSizeChange;
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

				/* We won't have PL_ENTRIES if FieldAttrsOrderBookWombatMsg
				* is not specified in the data dictionary */
				if (MamdaOrderBookFields.PL_ENTRIES != null)
				{
					/* null is passed as default value otherwise 
					* getVectorMsg throws an exception if not found */
					msgEntries = plMsg.getVectorMsg(MamdaOrderBookFields.PL_ENTRIES, null);
					if (msgEntries != null)
					{
						numEntriesInMsg = msgEntries.Length;
					}
				}

				if (numEntriesInMsg > 0)
				{
					mPriceLevelActNumEntries = numEntriesInMsg;

					for (int j = 0; j < numEntriesInMsg; j++)
					{
						entMsg = msgEntries[j];

						getEntriesInfo(entMsg);
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

                    // getNumEntryFields() defaults to 0 but we want to go into the
                    // below loop at least once for flattened messages scenario 
                    if (maxEntryFields == 0)
                    {
                        maxEntryFields = 1;
                    }

					int numEntryAttached = plMsg.getI32(MamdaOrderBookFields.PL_NUM_ATTACH, 1);
					if (numEntryAttached < maxEntryFields)
					{
						maxEntryFields = numEntryAttached;
					}
					mPriceLevelActNumEntries = maxEntryFields;
					for (int j = 1; j <= maxEntryFields; j++)
					{

                        if (MamdaOrderBookFields.PL_ENTRY.Length > 1)
                        {
						    entMsg = plMsg.getMsg(MamdaOrderBookFields.PL_ENTRY[j], null);
                        }
                        
						if ((entMsg == null) && (numEntryAttached == 1))
						{
							// Price level fields are probably be in the main message.
							entMsg = plMsg;
						}
						if (entMsg != null)
						{
							if (!getEntriesInfo(entMsg))
							{
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

        private bool getEntriesInfo(MamaMsg entMsg)
        {

            int priceLevelEntryActionInt = 0;
			ulong mPriceLevelEntrySizeUlong = 0;

			if (entMsg.tryI32(MamdaOrderBookFields.ENTRY_ACTION, ref priceLevelEntryActionInt))
			{
				mPriceLevelEntryAction = (sbyte) priceLevelEntryActionInt;
			}
                     
            entMsg.tryU64 (MamdaOrderBookFields.ENTRY_SIZE, ref mPriceLevelEntrySizeUlong);
			mPriceLevelEntrySize = (long) mPriceLevelEntrySizeUlong;
            
			mPriceLevelEntryTime = entMsg.getDateTime(MamdaOrderBookFields.ENTRY_TIME, mPriceLevelTime);
            
			if(entMsg.tryStringAnsi (MamdaOrderBookFields.ENTRY_ID, ref mPriceLevelEntryIdIntPtr))
			{
				return true;
			}
			else
			{
				return false;
			}
        }

        private void invokeGapHandlers (MamdaSubscription subscription,
                                        MamaMsg msg)
        {
            if (mBookHandler!=null)
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
            mPriceLevelPrice.setValue (0.0);
            mPriceLevelSize            = 0.0;
            mPriceLevelNumEntries      = 1;
            mPriceLevelAction          = (sbyte) MamdaOrderBookPriceLevel.Actions.Add;
            mPriceLevelSide            = (sbyte) MamdaOrderBookPriceLevel.Sides.Bid;
            mPriceLevelSizeChange      = 0;
            mPriceLevelActNumEntries   = 0;
            mPriceLevelTime			= DateTime.MinValue;
            clearLevelEntryFields();
        }


        private void clearLevelEntryFields()
        {
            mPriceLevelEntryAction     = (sbyte) MamdaOrderBookEntry.Actions.Delete;
            mPriceLevelEntrySize       = 0;
            mPriceLevelEntryId         = "";
            mPriceLevelEntryTime		= DateTime.MinValue;
        }


        private MamdaBookAtomicBookHandler        mBookHandler;
        private MamdaBookAtomicLevelHandler       mLevelHandler;
        private MamdaBookAtomicLevelEntryHandler  mLevelEntryHandler;

        private string			mSymbol                  = null;
        private string			mPartId                  = null;
        private DateTime		mSrcTime                 = DateTime.MinValue;
        private DateTime		mActivityTime            = DateTime.MinValue;
        private DateTime		mLineTime                = DateTime.MinValue;
        private DateTime		mSendTime                = DateTime.MinValue;
        private DateTime		mEventTime               = DateTime.MinValue;
        private long			mEventSeqNum             = 0;

        private long			mPriceLevels             = 0;
        private long			mPriceLevel              = 0;
        private MamaPrice		mPriceLevelPrice         = new MamaPrice ();
        private double			mPriceLevelSize          = 0.0;
        private long			mPriceLevelSizeChange    = 0;
        private sbyte			mPriceLevelAction        = (sbyte) MamdaOrderBookPriceLevel.Actions.Add; // char field
        private sbyte			mPriceLevelSide          = (sbyte) MamdaOrderBookPriceLevel.Sides.Bid; // char field
        private DateTime		mPriceLevelTime          = DateTime.MinValue;
        private double			mPriceLevelNumEntries    = 1.0;
        private long			mPriceLevelActNumEntries = 0;
        private sbyte			mPriceLevelEntryAction   = (sbyte) MamdaOrderBookEntry.Actions.Delete; // char field
        private string			mPriceLevelEntryId       = "";
        private IntPtr			mPriceLevelEntryIdIntPtr = IntPtr.Zero;
        private long			mPriceLevelEntrySize     = 0;
        private DateTime		mPriceLevelEntryTime     = DateTime.MinValue;

        // Additional fields for gaps:
        private long      mGapBegin;
        private long      mGapEnd;
        
        //Field State
        private MamdaFieldState          mSymbolFieldState                  = new MamdaFieldState();
        private MamdaFieldState          mPartIdFieldState                  = new MamdaFieldState();
        private MamdaFieldState          mSrcTimeFieldState                 = new MamdaFieldState();
        private MamdaFieldState          mActivityTimeFieldState            = new MamdaFieldState();
        private MamdaFieldState          mLineTimeFieldState                = new MamdaFieldState();
        private MamdaFieldState          mSendTimeFieldState                = new MamdaFieldState();
        private MamdaFieldState          mEventTimeFieldState               = new MamdaFieldState();
        private MamdaFieldState          mEventSeqNumFieldState             = new MamdaFieldState();

    }
}
