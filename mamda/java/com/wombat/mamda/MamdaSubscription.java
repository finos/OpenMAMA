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

package com.wombat.mamda;

import com.wombat.mama.*;
import java.util.logging.Logger;
import java.util.Vector;

/**
 * A MamdaSubscription is used to register interest in a particular
 * symbol and source.  A MamaTransport is required to actually
 * activate the subscription.
 *
 * Multiple listeners can be added to the MamdaSubscription.  In this
 * way, an application can make use of more than one of the
 * specialized value added MAMDA listeners, such as MamdaTradeListener
 * and MamdaQuoteListener.
 */

public class MamdaSubscription
{
    private static String CLASS_NAME = MamdaSubscription.class.getName();
    private static Logger mLogger   = Logger.getLogger(CLASS_NAME);

    private String            mSymbol           = null;
    private String            mSource           = null;
    private MamaTransport     mTransport        = null;
    private MamaQueue         mQueue            = null;
    private Object            mClosure          = null;
    private boolean           mRequireInitial   = true;
    private double            mTimeout          = 10;
    private int               mRetries          = 3;
    private Vector            mMsgListeners     = new Vector();
    private Vector            mStaleListeners   = new Vector();
    private Vector            mErrorListeners   = new Vector();
    private MamaSubscription  mSubscription     = null;
    private MamaSubscriptionType mType          = MamaSubscriptionType.NORMAL;
    private short             mServiceLevel     = MamaServiceLevel.REAL_TIME;
    private short             mServiceLevelOpt  = 0;
    private MamaSource        mMamaSource       = new MamaSource ();
    private MamaMsg           mLatestMsg;
    private boolean           mValid            = true;

    /**
     * Default constructor. 
     * The subscription is not created until either create() or 
     * activate() is called.
     */
    public MamdaSubscription (){}

    /**
     * Create and activate a subscription.
     * Any properties for the subscription should be set prior to 
     * calling this method.
     */
    public void create (MamaTransport         transport,
                        MamaQueue             queue,
                        String                source,
                        String                symbol,
                        Object                closure)
    {
        setSource    (source);
        setSymbol    (symbol);
        setTransport (transport);
        setQueue     (queue);
        setClosure   (closure);
        activate     ();
    }
    
    /**
     * Set the data source name.  Do this before calling activate().
     */
    public void setSource (String  source)
    {
        mSource = source;
    }

    /**
     * Set the symbol.  Do this before calling activate().
     */
    public void setSymbol (String  symbol)
    {
        mSymbol = symbol;
    }

    /**
     * Set the MAMA transport.  Do this before calling activate().
     */
    public void setTransport (MamaTransport  transport)
    {
        mTransport = transport;
    }

    /**
     * Set the MAMA queue.  Do this before calling activate().
     */
    public void setQueue (MamaQueue  queue)
    {
        mQueue = queue;
    }

    /**
     * Set the subscrption type.  Do this before calling activate().
     * Default is <code>MamaSubscriptionType.NORMAL</code>
     */
    public void setType (MamaSubscriptionType  type)
    {
        mType = type;
    }

    /**
     * Set the MAMA service level.  Do this before calling activate().
     * Default value is <code>MamaServiceLevel.REAL_TIME</code> and
     * 0
     */
    public void setServiceLevel (short  serviceLevel,
                                 short  serviceLevelOpt)
    {
        mServiceLevel    = serviceLevel;
        mServiceLevelOpt = serviceLevelOpt;
    }

    /**
     * Set whether an initial value is required.  Do this before
     * calling activate(). Default is <code>true</code>
     */
    public void setRequireInitial (boolean  require)
    {
        mRequireInitial = require;
    }

    /**
     * Set the timeout (in seconds) for this subscription.  Do this
     * before calling activate(). Default is 10 seconds.
     */
    public void setTimeout (double  timeout)
    {
        mTimeout = timeout;
    }
    
    /**
     * Set the retries for this subscirption. Do this 
     * before calling activate. Default is 3
     */
    public void setRetries (int retries)
    {
        mRetries = retries;
    }
    
    /**
     * Set the closure.  Do this before calling activate().
     */
    public void setClosure (Object  closure)
    {
        mClosure = closure;
    }
     
    /**
     * Add a listener for regular messages.  
     */
    public void addMsgListener (MamdaMsgListener  listener)
    {
        if (mMsgListeners.contains(listener))
            return;

        mMsgListeners.addElement(listener);
    }

    public Vector getMsgListeners()
    {
        return mMsgListeners;
    }

    /**
     * Add a listener for changes in stale status.  
     */
    public void addStaleListener (MamdaStaleListener  listener)
    {
        if (mStaleListeners.contains(listener))
            return;

        mStaleListeners.addElement(listener);
    }

    /**
     * Add a listener for error events.  
     */
    public void addErrorListener (MamdaErrorListener  listener)
    {
        if (mErrorListeners.contains(listener))
            return;

        mErrorListeners.addElement(listener);
    }

    /**
     * Activate the subscription.  Until this method is invoked, no
     * updates will be received.
     */
    public synchronized void activate ()
    {
        /*Already activated if not null*/
        if (mSubscription != null) return;         
       
        mSubscription = new MamaSubscription ();

        mSubscription.setSubscriptionType (mType);
        mSubscription.setServiceLevel     (mServiceLevel,mServiceLevelOpt);
        mSubscription.setRequiresInitial  (mRequireInitial);
        mSubscription.setRetries          (mRetries); /* MAMA_DEFAULT_RETRIES, */
        mSubscription.setTimeout          (mTimeout);

        mMamaSource.setTransport          (mTransport);
        mMamaSource.setSymbolNamespace    (mSource);

        mSubscription.createSubscription (
            new MamdaSubscriptionCallback(this),
            mQueue,
            mMamaSource,
            mSymbol,
            null);

        mValid = true;        
    }

    /**
     * Deactivate the subscription.  No more updates will be received
     * for this subscription (unless activate() is invoked again).
     *
     * This function must be called from the same thread dispatching on the
     * associated event queue unless both the default queue and dispatch queue are
     * not actively dispatching.
     */
    public void deactivate ()
    {
        if( mSubscription != null )
        {
            mSubscription.destroy();
            mSubscription = null;
        }
        mValid = false;
    }  

    /**
     * Returns the underlying <code>MamaSubscriptionType</code> for the specified
     * Subscription.
     *
     */
    public MamaSubscriptionType getSubscriptionType ()
    {
        return mSubscription.getSubscriptionType();
    }

    /**
     * Get the service level for the specified Subscription.
     *
     */
    public short getServiceLevel ()
    {
        return mSubscription.getServiceLevel();
    }

    /**
     * Get the service level options for the specified subscription.
     *
     * @return serviceLevel 
     */
    public long getServiceLevelOpt ()
    {
        return mSubscription.getServiceLevelOptions();
    }

    /**
     * Whether an initial value is required for the specified Subscription.
     * This only applies to market data subscriptions and not to basic
     * subscriptions. 
     * Default value of true indicating that initial values are required.
     * @param requiresInitial [true|false] Whether to request an initial value or 
     * not. 
     */
    public void setRequiresInitial (boolean  requiresInitial)
    {
        mSubscription.setRequiresInitial(requiresInitial);
    }

    /**
     * Returns a value of true or false indicating whether this Subscription 
     * is interested in initial values.
     *
     * @return boolean 
     */
    public boolean getRequiresInitial ()
    {
        return mSubscription.getRequiresInitital();
    }

    /**
     * Set the application-specific data type.
     *
     * @param appDataType The application-specific data type.
     */
    public void setAppDataType (MamaMdDataType appDataType)
    {
        mSubscription.setAppDataType (appDataType);
    }

    /**
     * Retrieve the application-specific data type.
     *
     * @return appDataType .
     */
    public MamaMdDataType getAppDataType ()
    {            
        return mSubscription.getAppDataType();
    }

    /**
     * Whether a Subscription should attempt to recover from
     * sequence number gaps.
     *
     * @param  doesRecover indicates not to recover. true The Subscription
     * will attempt to recover via a recap request.
     */
    public void setRecoverGaps (boolean doesRecover)
    {
        if (mSubscription != null)
        {
            mSubscription.setRecoverGaps (doesRecover);
        }
    }

    /**
     * Whether the specified subscription will attempt to recover from
     * sequence number gaps.
     *
     * @return boolean  - false does not recoever true - does attempt to recover
     */
    public boolean getRecoverGaps ()
    {
        return mSubscription.getRecoverGaps();
    }

    /**
     * Set the number of messages to cache for each symbol before the initial
     * value arrives. This allows the Subscription to recover when the
     * initial value arrives late (after a subsequent trade or quote already
     * arrived).
     *
     * For group subscription, a separate cache is used for each group member.
     *
     * The default is 10.
     * @param cacheSize The size of the cache.
     */
    public void setPreInitialCacheSize (int cacheSize)
    {
        if (mSubscription != null)
        {
            mSubscription.setPreInitialCacheSize (cacheSize);
        }
    }

    /**
     * Return the initial value cache size
     *
     * @return cacheSize
     */
    public int getPreInitialCacheSize ()
    {
        return mSubscription.getPreInitialCacheSize();
    }

    /**
     * Return the MamaMsgQualifier
     * @return MamaMsgQualifier
     */
    public MamaMsgQualifier getMsgQualifier()
    {	
        return mSubscription.getMessageQualifierFilter();
    }

    /**
     * Force a recap request to be sent for this subscription.
     */
    public void requestRecap ()
    {
    }

    public int getRetries()
    {
        return mRetries;
    }
    
    public double getTimeout()
    {
        return mTimeout;
    }

    public String getSource()
    {
        return mSource;
    }
    
    public String getSymbol()
    {
        return mSymbol;
    }

    public MamaQueue getQueue()
    {
        return mQueue;
    }

    public MamaTransport getTransport()
    {    
        return mTransport;
    }

    /**
     * Get the additional object passed as the closure to the create()
     * method.
     */
    public Object getClosure()
    {
        return mClosure;
    }

    public long getSeqNum ()
    {
        if (mLatestMsg != null)
            return mLatestMsg.getSeqNum();
        return 0;
    }

    public MamaSubscription getMamaSubscription ()
    {
        return mSubscription;
    }

    private class MamdaSubscriptionCallback implements MamaSubscriptionCallback
    {
        MamdaSubscription  mSubscription = null;

        public MamdaSubscriptionCallback (MamdaSubscription  subscription)
        {
            mSubscription = subscription;
        }  

        public void onMsg (final MamaSubscription subscription,
                           final MamaMsg          msg)
        {
            short msgType   = MamaMsgType.typeForMsg     (msg);
            int   msgStatus = MamaMsgStatus.statusForMsg (msg);
            mLatestMsg      = msg;

            switch (msgType)
            {
                case MamaMsgType.TYPE_DELETE:
                case MamaMsgType.TYPE_EXPIRE:
                    subscription.destroy();
                    mLatestMsg = null;
                    return;
            }

            switch (msgStatus)
            {
                case MamaMsgStatus.STATUS_BAD_SYMBOL:
                case MamaMsgStatus.STATUS_EXPIRED:
                case MamaMsgStatus.STATUS_TIMEOUT:
                    subscription.destroy();
                    mLatestMsg = null;
                    return;
            }

            Vector listeners = mMsgListeners;

            int size = listeners.size();
            if (size == 0)
            {
                mLatestMsg = null;
                return;
            }

            for (int i = 0; i < size; ++i)
            {
                if(mSubscription == null)
                {
                    return;
                }
                if (mValid)
                {
                    MamdaMsgListener listener = (MamdaMsgListener)listeners.elementAt(i);
                    listener.onMsg (mSubscription, msg, msgType);
                }
            }
            mLatestMsg = null;
        }

        /**
         * Method invoked when listener creation is complete, and
         * before any calls to <code>onMsg</code>.
         *
         * @param subscription The listenerCreated.
         */
        public void onCreate (MamaSubscription subscription)
        {
            synchronized( MamdaSubscription.this )
            {
                MamdaSubscription.this.mSubscription = subscription;
            }
        }

        public void onError (MamaSubscription  subscription,
                             short             wombatStatus,
                             int               platformError,
                             String            subject, 
                             Exception         e)
        {
            Vector listeners = mErrorListeners;

            int size = listeners.size();
            if (size == 0)
            {
                return;
            }

            for (int i = 0; i < size; ++i)
            {
                /* NOTE: This assumes that the MamaMsgStatus values correspond
                 * dirrectly to the MamdaErrorCode values.
                 */
                MamdaErrorListener listener =
                    (MamdaErrorListener)listeners.elementAt(i);
                short errorCode = MamdaErrorCode.codeForMamaMsgStatus (wombatStatus);
                listener.onError (
                    mSubscription,
                    MamdaErrorSeverity.severityForErrorCode (errorCode),
                    errorCode,
                    MamdaErrorCode.stringForMamdaError (wombatStatus));
            }
        }

        public void onQuality (MamaSubscription subscription,
                               short            quality,
                               short            cause,
                               final Object     opaque)
        {
            Vector listeners = mStaleListeners;

            int size = listeners.size();
            if (size == 0)
            {
                return;
            }

            for (int i = 0; i < size; ++i)
            {
                MamdaStaleListener listener =
                    (MamdaStaleListener)listeners.elementAt(i);
                listener.onStale (mSubscription, quality);
            }
        }
        
        public void onRecapRequest (MamaSubscription subscription)            
        {
            // Do nothing
        }
        
        public void onGap (MamaSubscription subscription)            
        {
            // Do nothing
        }

        public void onDestroy (MamaSubscription subscription)            
        {
            // Do nothing
        }
    }
}
