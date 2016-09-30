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
using System.Threading;

namespace Wombat
{
	/// <summary>
	/// MamdaSecurityStatusListener is a class that specializes in handling
	/// security status updates.  Developers provide their own
	/// implementation of the MamdaSecurityStatusHandler interface and will
	/// be delivered notifications for security status updates.
	/// </summary>
	/// <remarks>
	/// Note: The MamdaSecurityStatusListener class caches some field
	/// values.  Among other reasons, caching of these fields makes it
	/// possible to provide complete certain callbacks, even when the
	/// publisher (e.g., feed handler) is only publishing deltas containing
	/// modified fields.
	/// </remarks>
	public class MamdaSecurityStatusListener : MamdaMsgListener, MamdaSecurityStatusRecap, MamdaSecurityStatusUpdate
	{
		/// <summary>
		/// Create a specialized trade listener.  This listener handles
		/// trade reports, trade recaps, trade errors/cancels, trade
		/// corrections, and trade gap notifications.
		/// </summary>
		public MamdaSecurityStatusListener()
		{
			clearCache(mSecurityStatusCache);
			if (mUpdaters == null)
			{
				lock (mUpdatersGuard)
				{
					if (mUpdaters == null)
					{
						SecurityStatusUpdate[] updaters = createUpdaters();
						Thread.MemoryBarrier();
						mUpdaters = updaters;
					}
				}
			}

			mFieldIterator = new FieldIterator(this);
		}

		/// <summary>
		/// Add a specialized trade handler.  Currently, only one
		/// handler can (and must) be registered.
		/// </summary>
		public void addHandler(MamdaSecurityStatusHandler handler)
		{
			mHandlers.Add(handler);
		}

		protected static void clearCache(MamdaSecurityStatusCache cache)
		{
			cache.mSrcTime = DateTime.MinValue;
			cache.mActTime = DateTime.MinValue;
			cache.mEventSeqNum = 0;
			cache.mEventTime = DateTime.MinValue;
			cache.mSecStatus = 0;
			cache.mSecStatusQual = 0;
			cache.mSecStatusStr = "Unknown";
			cache.mSecStatusQualStr = null;
			cache.mSecStatusOrigStr = null;
            cache.mShortSaleCircuitBreaker = ' ';
			cache.mReason = null;
            cache.mLuldTime = DateTime.MinValue;
            cache.mLuldIndicator = ' ';
            cache.mLuldHighLimit = new MamaPrice();
            cache.mLuldLowLimit = new MamaPrice();

            cache.mSrcTimeFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mActTimeFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mEventSeqNumFieldState             = MamdaFieldState.NOT_INITIALISED;
            cache.mEventTimeFieldState               = MamdaFieldState.NOT_INITIALISED;
            cache.mSecStatusFieldState               = MamdaFieldState.NOT_INITIALISED;
            cache.mSecStatusQualFieldState           = MamdaFieldState.NOT_INITIALISED;
            cache.mSecStatusStrFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mSecStatusQualStrFieldState        = MamdaFieldState.NOT_INITIALISED;
            cache.mSecStatusOrigStrFieldState        = MamdaFieldState.NOT_INITIALISED;
            cache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.NOT_INITIALISED;
            cache.mReasonFieldState                  = MamdaFieldState.NOT_INITIALISED;
            cache.mLuldTimeFieldState                 = MamdaFieldState.NOT_INITIALISED;
            cache.mLuldIndicatorFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mLuldHighLimitFieldState            = MamdaFieldState.NOT_INITIALISED;
            cache.mLuldLowLimitFieldState             = MamdaFieldState.NOT_INITIALISED;
		}

		public void populateRecap(MamdaConcreteSecurityStatusRecap recap)
		{
			lock (this)
			{
				recap.clear();
				recap.setSrcTime(mSecurityStatusCache.mSrcTime);
				recap.setActivityTime(mSecurityStatusCache.mActTime);
				recap.setSecurityStatus(mSecurityStatusCache.mSecStatus);
				recap.setSecurityStatusQualifier(mSecurityStatusCache.mSecStatusQual);
				recap.setSecurityStatusStr(mSecurityStatusCache.mSecStatusStr);
                recap.setShortSaleCircuitBreaker(mSecurityStatusCache.mShortSaleCircuitBreaker);
				recap.setSecurityStatusQualifierStr(mSecurityStatusCache.mSecStatusQualStr);
				recap.setReason(mSecurityStatusCache.mReason);
				recap.setLuldTime(mSecurityStatusCache.mLuldTime);
                recap.setLuldIndicator(mSecurityStatusCache.mLuldIndicator);
                recap.setLuldHighLimit(mSecurityStatusCache.mLuldHighLimit);
                recap.setLuldLowLimit(mSecurityStatusCache.mLuldLowLimit);
			}
		}

		public DateTime getSrcTime()
		{
			return mSecurityStatusCache.mSrcTime;
		}

		public DateTime getActivityTime()
		{
			return mSecurityStatusCache.mActTime;
		}

		public long getEventSeqNum()
		{
			return mSecurityStatusCache.mEventSeqNum;
		}

		public DateTime getEventTime()
		{
			return mSecurityStatusCache.mEventTime;
		}

		public long getSecurityStatus()
		{
			return (long)MamdaSecurityStatus.mamdaSecurityStatusFromString (mSecurityStatusCache.mSecStatusStr);
		}

		public long getSecurityStatusQualifier()
		{
			return (long)MamdaSecurityStatusQual.mamdaSecurityStatusQualFromString (mSecurityStatusCache.mSecStatusQualStr);
		}

		public MamdaSecurityStatus.mamdaSecurityStatus getSecurityStatusEnum()
		{
			return MamdaSecurityStatus.mamdaSecurityStatusFromString (mSecurityStatusCache.mSecStatusStr);
		}

		public MamdaSecurityStatusQual.mamdaSecurityStatusQual getSecurityStatusQualifierEnum()
		{
			return MamdaSecurityStatusQual.mamdaSecurityStatusQualFromString (mSecurityStatusCache.mSecStatusQualStr);
		}

		public string getSecurityStatusStr()
		{
			return mSecurityStatusCache.mSecStatusStr;
		}

        public char getShortSaleCircuitBreaker()
        {
            return mSecurityStatusCache.mShortSaleCircuitBreaker;
        }

		public string getSecurityStatusQualifierStr()
		{
			return mSecurityStatusCache.mSecStatusQualStr;
		}

		public string getSecurityStatusOrigStr()
		{
			return mSecurityStatusCache.mSecStatusQualStr;
		}

		public string getReason()
		{
			return mSecurityStatusCache.mReason;
		}

        public DateTime getLuldTime()
        {
            return mSecurityStatusCache.mLuldTime;
        }

        public char getLuldIndicator()
        {
            return mSecurityStatusCache.mLuldIndicator;
        }

        public MamaPrice getLuldHighLimit()
        {
            return mSecurityStatusCache.mLuldHighLimit;
        }

        public MamaPrice getLuldLowLimit()
        {
            return mSecurityStatusCache.mLuldLowLimit;
        }

        /*  Field State Accessors       */
        public MamdaFieldState getSrcTimeFieldState()
        {
          return mSecurityStatusCache.mSrcTimeFieldState;
        }

        public MamdaFieldState getActivityTimeFieldState()
        {
          return mSecurityStatusCache.mActTimeFieldState;
        }

        public MamdaFieldState getEventSeqNumFieldState()
        {
          return mSecurityStatusCache.mEventSeqNumFieldState;
        }

        public MamdaFieldState getEventTimeFieldState()
        {
          return mSecurityStatusCache.mEventTimeFieldState;
        }

        public MamdaFieldState getSecurityStatusFieldState()
        {
          return mSecurityStatusCache.mSecStatusStrFieldState;
        }

        public MamdaFieldState getSecurityStatusQualifierFieldState()
        {
          return mSecurityStatusCache.mSecStatusQualStrFieldState;
        }

        public MamdaFieldState getShortSaleCircuitBreakerFieldState()
        {
            return mSecurityStatusCache.mShortSaleCircuitBreakerFieldState;
        }

        public MamdaFieldState getSecurityStatusEnumFieldState()
        {
          return mSecurityStatusCache.mSecStatusStrFieldState;
        }

        public MamdaFieldState getSecurityStatusQualifierEnumFieldState()
        {
          return mSecurityStatusCache.mSecStatusQualStrFieldState;
        }

        public MamdaFieldState getSecurityStatusStrFieldState()
        {
          return mSecurityStatusCache.mSecStatusStrFieldState;
        }

        public MamdaFieldState getSecurityStatusQualifierStrFieldState()
        {
          return mSecurityStatusCache.mSecStatusQualStrFieldState;
        }

        public MamdaFieldState getSecurityStatusOrigStrFieldState()
        {
          return mSecurityStatusCache.mSecStatusQualStrFieldState;
        }

        public MamdaFieldState getReasonFieldState()
        {
          return mSecurityStatusCache.mReasonFieldState;
        }

        public MamdaFieldState getLuldTimeFieldState()
        {
          return mSecurityStatusCache.mLuldTimeFieldState;
        }

        public MamdaFieldState getLuldIndicatorFieldState()
        {
          return mSecurityStatusCache.mLuldIndicatorFieldState;
        }

        public MamdaFieldState getLuldHighLimitFieldState()
        {
          return mSecurityStatusCache.mLuldHighLimitFieldState;
        }

        public MamdaFieldState getLuldLowLimitFieldState()
        {
          return mSecurityStatusCache.mLuldLowLimitFieldState;
        }

		/// <summary>
		/// Implementation of MamdaListener interface.
		/// </summary>
		public void onMsg(
			MamdaSubscription subscription,
			MamaMsg msg,
			mamaMsgType msgType)
		{
			// If msg is a trade-related message, invoke the
			// appropriate callback:
            updateFieldStates();
			lock (this)
			{
				mUpdated = false;
				msg.iterateFields(mFieldIterator, null, null);
			}
			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
					handleRecap(subscription, msg);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_PREOPENING:
	            case mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS:
				case mamaMsgType.MAMA_MSG_TYPE_UPDATE:
				case mamaMsgType.MAMA_MSG_TYPE_QUOTE:
				case mamaMsgType.MAMA_MSG_TYPE_TRADE:
				case mamaMsgType.MAMA_MSG_TYPE_BOOK_UPDATE:
					handleSecurityStatus(subscription, msg);
					break;
			}
		}

		#region Implementation details

		private void handleRecap(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
			foreach (MamdaSecurityStatusHandler handler in mHandlers)
			{
				handler.onSecurityStatusRecap(subscription, this, msg, this);
			}
		}

		private void handleSecurityStatus(
			MamdaSubscription  subscription,
			MamaMsg            msg)
		{
			if (mUpdated)
			{
				foreach (MamdaSecurityStatusHandler handler in mHandlers)
				{
					handler.onSecurityStatusUpdate(subscription, this, msg, this, this);
				}
			}
		}

        private void updateFieldStates()
        {
            if (mSecurityStatusCache.mSrcTimeFieldState          == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSrcTimeFieldState          = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mActTimeFieldState          == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mActTimeFieldState          = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mEventSeqNumFieldState      == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mEventSeqNumFieldState      = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mEventTimeFieldState        == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mEventTimeFieldState        = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mSecStatusFieldState        == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSecStatusFieldState        = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mSecStatusQualFieldState    == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSecStatusQualFieldState    = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mSecStatusStrFieldState     == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSecStatusStrFieldState     = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mSecStatusQualStrFieldState == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSecStatusQualStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mSecStatusOrigStrFieldState == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mSecStatusOrigStrFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mShortSaleCircuitBreakerFieldState == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mReasonFieldState           == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mReasonFieldState           = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mLuldIndicatorFieldState     == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mLuldIndicatorFieldState     = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mLuldTimeFieldState          == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mLuldTimeFieldState          = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mLuldHighLimitFieldState     == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mLuldHighLimitFieldState     = MamdaFieldState.NOT_MODIFIED;
            if (mSecurityStatusCache.mLuldLowLimitFieldState      == MamdaFieldState.MODIFIED)
                mSecurityStatusCache.mLuldLowLimitFieldState      = MamdaFieldState.NOT_MODIFIED;
        }
		private static SecurityStatusUpdate[] createUpdaters()
		{
            mMaxFid = MamdaSecurityStatusFields.getMaxFid();
			SecurityStatusUpdate[] updaters = new SecurityStatusUpdate[mMaxFid+1];
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SRC_TIME, new SecurityStatusSrcTime());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.ACTIVITY_TIME, new SecurityStatusActivityTime());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SECURITY_STATUS, new SecurityStatusStr());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SECURITY_STATUS_QUAL, new SecurityStatusQualStr());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SECURITY_STATUS_ORIG, new SecurityStatusOrigStr());
            addUpdaterToList(updaters, MamdaSecurityStatusFields.SHORT_SALE_CIRCUIT_BREAKER, new ShortSaleCircuitBreaker());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SECURITY_STATUS_TIME, new SecurityStatusTime());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.SEQNUM, new SecurityStatusSeqNum());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.REASON, new SecurityStatusReason());

			addUpdaterToList(updaters, MamdaSecurityStatusFields.LULDINDICATOR, new SecurityStatusLuldIndicator());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.LULDTIME, new SecurityStatusLuldTime());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.LULDHIGHLIMIT, new SecurityStatusLuldHighLimit());
			addUpdaterToList(updaters, MamdaSecurityStatusFields.LULDLOWLIMIT, new SecurityStatusLuldLowLimit());
			return updaters;
		}

		private static void addUpdaterToList(
			SecurityStatusUpdate[] updaters,
			MamaFieldDescriptor fieldDesc,
			SecurityStatusUpdate updater)
		{
			if (fieldDesc == null) return;
			int fieldId = fieldDesc.getFid();
            if (fieldId <= mMaxFid)
            {
                updaters[fieldId] = updater;
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

		private interface SecurityStatusUpdate
		{
			void onUpdate(
				MamdaSecurityStatusListener listener,
				MamaMsgField                field);
		}

		private class SecurityStatusSrcTime : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				listener.mSecurityStatusCache.mSrcTime = field.getDateTime();
                listener.mSecurityStatusCache.mSrcTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class SecurityStatusActivityTime : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				listener.mSecurityStatusCache.mActTime = field.getDateTime();
                listener.mSecurityStatusCache.mActTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

        private class SecurityStatusQualStr : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                string statusQual = listener.getFieldAsString(field);
                if (string.Compare(listener.mSecurityStatusCache.mSecStatusQualStr, statusQual) != 0)
                {
                    listener.mSecurityStatusCache.mSecStatusQualStr = statusQual;
                    listener.mSecurityStatusCache.mSecStatusQualStrFieldState = MamdaFieldState.MODIFIED;
                    listener.mUpdated = true;
                }
            }
        }

		private class SecurityStatusTime : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				listener.mSecurityStatusCache.mEventTime = field.getDateTime();
                listener.mSecurityStatusCache.mEventTimeFieldState = MamdaFieldState.MODIFIED;
			}
		}

		private class SecurityStatusOrigStr : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				if (string.Compare(listener.mSecurityStatusCache.mSecStatusOrigStr, field.getString()) != 0)
				{
					listener.mSecurityStatusCache.mSecStatusOrigStr = field.getString();
                    listener.mSecurityStatusCache.mSecStatusOrigStrFieldState = MamdaFieldState.MODIFIED;
					listener.mUpdated = true;
				}
			}
		}

        private class ShortSaleCircuitBreaker : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                if ((field != null) && (listener.mSecurityStatusCache.mShortSaleCircuitBreaker != field.getChar()))
                {
                    listener.mSecurityStatusCache.mShortSaleCircuitBreaker = field.getChar();
                    listener.mSecurityStatusCache.mShortSaleCircuitBreakerFieldState = MamdaFieldState.MODIFIED;
                    listener.mUpdated = true;
                }
            }
        }

		private class SecurityStatusSeqNum : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				listener.mSecurityStatusCache.mEventSeqNum = field.getI64();
                listener.mSecurityStatusCache.mEventSeqNumFieldState = MamdaFieldState.MODIFIED;
			}
		}

        private class SecurityStatusStr : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                string status = listener.getFieldAsString(field);
                if (string.Compare(listener.mSecurityStatusCache.mSecStatusStr,	status) != 0)
                {
                    listener.mSecurityStatusCache.mSecStatusStr = status;
                    listener.mSecurityStatusCache.mSecStatusStrFieldState = MamdaFieldState.MODIFIED;
                    listener.mUpdated = true;
                }
            }
        }

		private class SecurityStatusReason : SecurityStatusUpdate
		{
			public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
			{
				listener.mSecurityStatusCache.mReason = listener.getFieldAsString(field);
                listener.mSecurityStatusCache.mReasonFieldState = MamdaFieldState.MODIFIED;
			}
		}

        private class SecurityStatusLuldTime : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                listener.mSecurityStatusCache.mLuldTime = field.getDateTime();
                listener.mSecurityStatusCache.mLuldTimeFieldState = MamdaFieldState.MODIFIED;
                listener.mUpdated = true;
            }
        }

        private class SecurityStatusLuldIndicator : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                if ((field != null) && (listener.mSecurityStatusCache.mLuldIndicator != field.getChar() || listener.mSecurityStatusCache.mLuldIndicatorFieldState == MamdaFieldState.NOT_INITIALISED))
                {
                    listener.mSecurityStatusCache.mLuldIndicator = field.getChar();
                    listener.mSecurityStatusCache.mLuldIndicatorFieldState = MamdaFieldState.MODIFIED;
                    listener.mUpdated = true;
                }
            }
        }

        private class SecurityStatusLuldHighLimit : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                listener.mSecurityStatusCache.mLuldHighLimit.copy(field.getPrice());
                listener.mSecurityStatusCache.mLuldHighLimitFieldState = MamdaFieldState.MODIFIED;
                listener.mUpdated = true;
            }
        }


        private class SecurityStatusLuldLowLimit : SecurityStatusUpdate
        {
            public void onUpdate(MamdaSecurityStatusListener listener, MamaMsgField field)
            {
                listener.mSecurityStatusCache.mLuldLowLimit.copy(field.getPrice());
                listener.mSecurityStatusCache.mLuldLowLimitFieldState = MamdaFieldState.MODIFIED;
                listener.mUpdated = true;
            }
        }

		private class FieldIterator : MamaMsgFieldIterator
		{
			public FieldIterator(MamdaSecurityStatusListener listener)
			{
				mListener = listener;
			}

			public void onField(
				MamaMsg msg,
				MamaMsgField field,
				object closure)
			{
				try
				{
					int fieldId = field.getFid();
                    if (fieldId <= mMaxFid)
                    {
                        SecurityStatusUpdate updater = (SecurityStatusUpdate)mUpdaters[fieldId];
                        if (updater != null)
                        {
                            updater.onUpdate(mListener, field);
                        }
                    }
				}
				catch (Exception ex )
				{
					throw new MamdaDataException(ex.Message);
				}
			}

			private MamdaSecurityStatusListener mListener;
		}

		protected class MamdaSecurityStatusCache
		{
            /*
            * NOTE: fields which are enums can be pubished as integers if feedhandler
            * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
            * publish the numerical value as a string. All enumerated fields must be handled
            * by getting the value based on the field type.
            */

			public DateTime	mSrcTime		= DateTime.MinValue;
			public DateTime	mActTime		= DateTime.MinValue;
			public long		mEventSeqNum;
			public DateTime	mEventTime		= DateTime.MinValue;
			public long		mSecStatus;
			public long		mSecStatusQual;
			public string	mSecStatusStr;
			public string	mSecStatusQualStr;
			public string	mSecStatusOrigStr;
            public char     mShortSaleCircuitBreaker;
			public string	mReason;
            public DateTime mLuldTime               = DateTime.MinValue;
            public char     mLuldIndicator;
			public MamaPrice    mLuldHighLimit      = new MamaPrice ();
			public MamaPrice    mLuldLowLimit       = new MamaPrice ();

            //Field States
            public MamdaFieldState  mSrcTimeFieldState          = new MamdaFieldState();
            public MamdaFieldState  mActTimeFieldState          = new MamdaFieldState();
            public MamdaFieldState  mEventSeqNumFieldState      = new MamdaFieldState();
            public MamdaFieldState  mEventTimeFieldState        = new MamdaFieldState();
            public MamdaFieldState  mSecStatusFieldState        = new MamdaFieldState();
            public MamdaFieldState  mSecStatusQualFieldState    = new MamdaFieldState();
            public MamdaFieldState  mSecStatusStrFieldState     = new MamdaFieldState();
            public MamdaFieldState  mSecStatusQualStrFieldState = new MamdaFieldState();
            public MamdaFieldState  mSecStatusOrigStrFieldState = new MamdaFieldState();
            public MamdaFieldState  mShortSaleCircuitBreakerFieldState = new MamdaFieldState();
            public MamdaFieldState  mReasonFieldState           = new MamdaFieldState();
            public MamdaFieldState  mLuldTimeFieldState         = new MamdaFieldState();
            public MamdaFieldState  mLuldIndicatorFieldState    = new MamdaFieldState();
            public MamdaFieldState  mLuldHighLimitFieldState    = new MamdaFieldState();
            public MamdaFieldState  mLuldLowLimitFieldState     = new MamdaFieldState();
		}

		#region State

		private static SecurityStatusUpdate [] mUpdaters;
		private readonly ArrayList mHandlers = new ArrayList();
		protected readonly MamdaSecurityStatusCache mSecurityStatusCache = new MamdaSecurityStatusCache();
        private static int mMaxFid = 0;

		// Used for all field iteration processing
		private FieldIterator mFieldIterator;

		private object mUpdatersGuard = new object();

		private bool mUpdated;

		#endregion State

		#endregion Implemenetation details
	}
}
