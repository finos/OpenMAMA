/* $Id: MamdaSubscription.cs,v 1.7.2.7 2012/08/24 16:12:11 clintonmcdowell Exp $
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

namespace Wombat
{
	/// <summary>
	/// A MamdaSubscription is used to register interest in a particular
	/// symbol and source.  A MamaTransport is required to actually
	/// activate the subscription.
	/// 
	/// Multiple listeners can be added to the MamdaSubscription.  In this
	/// way, an application can make use of more than one of the
	/// specialized value added MAMDA listeners, such as MamdaTradeListener
	/// and MamdaQuoteListener.
	/// </summary>
	public class MamdaSubscription : MamdaResourceContainer
	{
		/// <summary>
		/// Default constructor. 
		/// The subscription is not created until either create() or
		/// activate() is called.
		/// </summary>
		public MamdaSubscription() : base()
		{
		}

		/// <summary>
		/// Create and activate a subscription.
		/// Any properties for the subscription should be set prior to
		/// calling this method.
		/// </summary>
		/// <param name="transport"></param>
		/// <param name="queue"></param>
		/// <param name="source"></param>
		/// <param name="symbol"></param>
		/// <param name="closure"></param>
		public void create(
			MamaTransport         transport,
			MamaQueue             queue,
			string                source,
			string                symbol,
			object                closure)
		{
			setSource(source);
			setSymbol(symbol);
			setTransport(transport);
			setQueue(queue);
			setClosure(closure);
			activate();
		}

		/// <summary>
		/// Set the data source name.  Do this before calling activate().
		/// </summary>
		/// <param name="source"></param>
		public void setSource(string source)
		{
			mSource = source;
		}

		/// <summary>
		/// Set the symbol.  Do this before calling activate().
		/// </summary>
		/// <param name="symbol"></param>
		public void setSymbol(string symbol)
		{
			mSymbol = symbol;
		}

		/// <summary>
		/// Set the MAMA transport.  Do this before calling activate().
		/// </summary>
		/// <param name="transport"></param>
		public void setTransport(MamaTransport transport)
		{
			mTransport = transport;
		}

		/// <summary>
		/// Set the MAMA queue.  Do this before calling activate().
		/// </summary>
		/// <param name="queue"></param>
		public void setQueue(MamaQueue  queue)
		{
			mQueue = queue;
		}

		/// <summary>
		/// Set the subscrption type.  Do this before calling activate()
		/// Default is <code>mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL</code>
		/// </summary>
		/// <param name="type"></param>
		public void setType(mamaSubscriptionType type)
		{
			mType = type;
		}

		/// <summary>
		/// Set the MAMA service level.  In the future, certain service
		/// levels may have an optional argument (e.g., an interval for
		/// conflated data).  Do this before calling activate().
		/// Default value is <code>MamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME</code> and
		/// 0
		/// </summary>
		/// <param name="serviceLevel"></param>
		/// <param name="serviceLevelOpt"></param>
		public void setServiceLevel(
			mamaServiceLevel serviceLevel,
			int serviceLevelOpt)
		{
			mServiceLevel    = serviceLevel;
			mServiceLevelOpt = serviceLevelOpt;
		}

		/// <summary>
		/// Set whether an initial value is required.  Do this before
		/// calling activate(). Default is <code>true</code>
		/// </summary>
		/// <param name="require"></param>
		public void setRequireInitial(bool require)
		{
			mRequireInitial = require;
		}

		/// <summary>
		/// Set the timeout (in seconds) for this subscription.  Do this
		/// before calling activate(). Default is 10 seconds.
		/// </summary>
		/// <param name="timeout"></param>
		public void setTimeout(double timeout)
		{
			mTimeout = timeout;
		}

		/// <summary>
		/// Set the retries for this subscription.  Do this
		/// before calling activate(). Default is 10 seconds.
		/// </summary>
		/// <param name="timeout"></param>
		public void setRetries(int retries)
		{
			mRetries = retries;
		}
		
		/// <summary>
		/// Set the closure.  Do this before calling activate().
		/// </summary>
		/// <param name="closure"></param>
		public void setClosure (Object  closure)
		{
			mClosure = closure;
		}

		/// <summary>
		/// Add a listener for regular messages.
		/// </summary>
		/// <param name="listener"></param>
		public void addMsgListener(MamdaMsgListener listener)
		{
			if (!mMsgListeners.Contains(listener))
			{
				mMsgListeners.Add(listener);
			}
		}

        /// <summary>
        /// get a listener for regular messages.
        /// </summary>
        /// <param name="listener"></param>
        public ArrayList getMsgListeners()
        {
          return mMsgListeners;
        }

		/// <summary>
		/// Add a listener for changes in stale status.  
		/// </summary>
		/// <param name="listener"></param>
		public void addStaleListener(MamdaStaleListener listener)
		{
			if (!mStaleListeners.Contains(listener))
			{
				mStaleListeners.Add(listener);
			}
		}

		/// <summary>
		/// Add a listener for error events.  
		/// </summary>
		/// <param name="listener"></param>
		public void addErrorListener(MamdaErrorListener listener)
		{
			if (!mErrorListeners.Contains(listener))
			{
				mErrorListeners.Add(listener);
			}
		}

		/// <summary>
		/// Activate the subscription.  Until this method is invoked, no
		/// updates will be received.
		/// </summary>
		public void activate()
		{
			if (mSubscription != null) return;

			mSubscription = new MamaSubscription();

			mSubscription.setSubscriptionType(mType);
			mSubscription.setServiceLevel(mServiceLevel, mServiceLevelOpt);
			mSubscription.setRequiresInitial(mRequireInitial);
			mSubscription.setRetries(mRetries);
			mSubscription.setTimeout(mTimeout);

			mMamaSource.transport = mTransport;
			mMamaSource.symbolNamespace = mSource;

			mSubscription.create(
				mQueue,
				new MamdaSubscriptionCallback(this),
				mMamaSource,
				mSymbol,
				null);
		}

		/// <summary>
		/// Deactivate the subscription.  No more updates will be received
		/// for this subscription (unless activate() is invoked again).
		///
		/// This function must be called from the same thread dispatching on the
		/// associated event queue unless both the default queue and dispatch queue are
		/// not actively dispatching.
		/// </summary>
		public void deactivate ()
		{
			if (mSubscription != null)
			{
				mSubscription.destroyEx();
			}
		}

		/// <summary>
		/// Force a recap request to be sent for this subscription.
		/// </summary>
		/// <remarks>
		/// Note: there is still a possibility that the feed handler will
		/// reject such recap requests (based on configurable feature(s) of
		/// a feed handler).
		/// </remarks>
		public void requestRecap ()
		{
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public string getSource()
		{
			return mSource;
		}
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public double getTimout()
		{
			return mTimeout;
		}
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public int getRetries()
		{
			return mRetries;
		}
		
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public string getSymbol()
		{
			return mSymbol;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public MamaQueue getQueue()
		{
			return mQueue;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		public MamaTransport getTransport()
		{
			return mTransport;
		}

		/// <summary>
		/// Get the additional object passed as the closure to the create()
		/// method.
		/// </summary>
		/// <returns></returns>
		public Object getClosure()
		{
			return mClosure;
		}

        public MamaSubscription getMamaSubscription()
        {
            return mSubscription;
        }
       
		#region Mamda subscription callback

		private class MamdaSubscriptionCallback : MamaSubscriptionCallback
		{
			private MamdaSubscription mSubscription = null;

			public MamdaSubscriptionCallback(MamdaSubscription subscription)
			{
				mSubscription = subscription;
			}  

			public void onMsg(
				MamaSubscription subscription,
				MamaMsg msg)
			{
				mamaMsgType msgType = msg.typeForMsg();
				mamaMsgStatus msgStatus = msg.getStatus();

				switch (msgType)
				{
					case mamaMsgType.MAMA_MSG_TYPE_DELETE:
					case mamaMsgType.MAMA_MSG_TYPE_EXPIRE:
						subscription.deallocate();
						return;
				}

				switch (msgStatus)
				{
					case mamaMsgStatus.MAMA_MSG_STATUS_BAD_SYMBOL:
					case mamaMsgStatus.MAMA_MSG_STATUS_EXPIRED:
					case mamaMsgStatus.MAMA_MSG_STATUS_TIMEOUT:
						subscription.deallocate();
						return;
				}

				for (int i=0;i<mSubscription.mMsgListeners.Count;i++)
				{
					MamdaMsgListener listener = (MamdaMsgListener) mSubscription.mMsgListeners[i];
					listener.onMsg(mSubscription, msg, msgType);
				}
			}

			/// <summary>
			/// Method invoked when listener creation is complete, and
			/// before any calls to <code>onMsg</code>.
			/// </summary>
			/// <param name="subscription">The listenerCreated.</param>
			public void onCreate(MamaSubscription subscription)
			{
				lock (mSubscription)
				{
					mSubscription.mSubscription = subscription;
				}
			}

            public void onError(
                MamaSubscription subscription,
                MamaStatus.mamaStatus status,
                string subject)
            {
                MamdaErrorSeverity  severity = MamdaErrorSeverity.MAMDA_SEVERITY_OK;
                MamdaErrorCode      code     = MamdaErrorCode.MAMDA_NO_ERROR;
                    
                switch (status)
                {
                    case MamaStatus.mamaStatus.MAMA_STATUS_BAD_SYMBOL:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_BAD_SYMBOL;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_TIMEOUT:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_TIMEOUT;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_NOT_ENTITLED:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_NOT_ENTITLED;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_NOT_FOUND;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_NO_SUBSCRIBERS:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_NO_SUBSCRIBERS;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_EXPIRED:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_EXPIRED;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_NOT_PERMISSIONED:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_NOT_PERMISSIONED;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_BANDWIDTH_EXCEEDED:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_BANDWIDTH_EXCEEDED;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_DELETE:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_DELETE;
                        break;
                    case MamaStatus.mamaStatus.MAMA_STATUS_PLATFORM:
                    default:
                        severity = MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
                        code     = MamdaErrorCode.MAMDA_ERROR_PLATFORM_STATUS;
                        break;
                }

                for (int i=0;i<mSubscription.mErrorListeners.Count;i++)
                {
                    MamdaErrorListener listener = (MamdaErrorListener) mSubscription.mErrorListeners[i];
                    listener.onError(
                        mSubscription,
                        severity,
                        code,
                        MamdaErrorCodes.stringForMamdaError(code));
                }
            }

			public void onQuality(
				MamaSubscription subscription,
				mamaQuality quality,
				string symbol)
			{
				for (int i=0;i<mSubscription.mStaleListeners.Count;i++)
				{
					MamdaStaleListener listener = (MamdaStaleListener) mSubscription.mStaleListeners[i];
					listener.onStale(mSubscription, quality);
				}
			}

			public void onGap (
				MamaSubscription  subscription)
			{
			}

			public void onRecapRequest
				(MamaSubscription subscription)
			{
			}
			
			public void onDestroy (
				MamaSubscription subscription)
			{
				subscription.deallocate ();
				mSubscription = null;
			}
		}

		#endregion Mamda subscription callback

		#region Implementation details

		private string					mSymbol					= null;
		private string					mSource					= null;
		private MamaTransport			mTransport				= null;
		private MamaQueue				mQueue					= null;
		private object					mClosure				= null;
		private bool					mRequireInitial			= true;
		private double					mTimeout				= 10;
		private int 					mRetries				= 3;
		internal ArrayList				mMsgListeners			= new ArrayList();
		internal ArrayList				mStaleListeners			= new ArrayList();
		internal ArrayList				mErrorListeners			= new ArrayList();
		private MamaSubscription		mSubscription			= null;
		private mamaSubscriptionType	mType					= mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL;
		private mamaServiceLevel		mServiceLevel			= mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME;
		private int 					mServiceLevelOpt		= 0;
		private MamaSource				mMamaSource				= new MamaSource();

		#endregion Implementation details
	}
}
