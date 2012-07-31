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

package com.wombat.mama;

/*
* A wrapper class for the underlying C transport structure
*/
public class MamaTransport
{
    /* ****************************************************** */
    /* Nested Classes. */
    /* ****************************************************** */

    /* This class implements a transport listener which will be set up
     * whenever the transport is allocated and used to route events
     * back to any client listener installed.
     */
    private class InternalTransportListener implements MamaTransportListener
    {
        /* ****************************************************** */
        /* Private Member Variables. */
        /* ****************************************************** */

        // The client listener
        private MamaTransportListener mClientListener;

        /* ****************************************************** */
        /* Construction and Finalization. */
        /* ****************************************************** */

        /**
         * Constructor initialises all member variables.
         */
        public void InternalTransportListener()
        {
            // Ensure that the listeners are null
            mClientListener = null;
        }
        
        /* ****************************************************** */
        /* Interface Implementations. */
        /* ****************************************************** */

        public void onDisconnect(short cause, final Object platformInfo)
        {
            if (mClientListener != null)
            {
                mClientListener.onDisconnect(cause, platformInfo);
            }
        }
        
        public void onReconnect(short cause, final Object platformInfo)
        {
            if (mClientListener != null)
            {
                mClientListener.onReconnect(cause, platformInfo);
            }
        }
        
        public void onQuality(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onQuality(cause, platformInfo);
            }
        }

        public void onConnect(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onConnect(cause, platformInfo);
            }
        }

        public void onAccept(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onAccept(cause, platformInfo);
            }
        }

        public void onAcceptReconnect(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onAcceptReconnect(cause, platformInfo);
            }
        }

        public void onPublisherDisconnect(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onPublisherDisconnect(cause, platformInfo);
            }
        }

        public void onNamingServiceConnect(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onNamingServiceConnect(cause, platformInfo);
            }
        }

        public void onNamingServiceDisconnect(short cause, final Object platformInfo)
        {
            if(mClientListener != null)
            {
                mClientListener.onNamingServiceDisconnect(cause, platformInfo);
            }
        }

        /* ****************************************************** */
        /* Public Functions. */
        /* ****************************************************** */

        /**
         * This function will set the client listener which will then
         * receive events from the transport.
         *
         * @param[in] clientListener The client listener.
         */
        public void setClientListener(MamaTransportListener clientListener)
        {
            /* Save argument in the member variable. */
            mClientListener = clientListener;
        }
    }

    /* This class implements a transport topic listener which will be set up
     * whenever the transport is allocated and used to route events
     * back to any client listener installed.
     */
    private class InternalTopicListener extends MamaTransportTopicListener
    {
        /* ****************************************************** */
        /* Private Member Variables. */
        /* ****************************************************** */

        // The client topic listener
        private MamaTransportTopicListener mClientTopicListener;

        /* ****************************************************** */
        /* Construction and Finalization. */
        /* ****************************************************** */

        /**
         * Constructor initialises all member variables.
         */
        public void InternalTopicListener()
        {
            // Ensure that the listeners are null
            mClientTopicListener = null;
        }

        /* ****************************************************** */
        /* Interface Implementations. */
        /* ****************************************************** */

        public void onTopicSubscribe(short cause, final Object platformInfo)
        {
            /* Call the client topic listener if it has been provided. */
            if(null != mClientTopicListener)
            {
                mClientTopicListener.onTopicSubscribe(cause, platformInfo);
            }
        }

        public void onTopicUnsubscribe(short cause, final Object platformInfo)
        {
            /* Call the client topic listener if it has been provided. */
            if(null != mClientTopicListener)
            {
                mClientTopicListener.onTopicUnsubscribe(cause, platformInfo);
            }
        }

        /* ****************************************************** */
        /* Public Functions. */
        /* ****************************************************** */

        /**
         * This function will set the client topic listener which will then
         * receive events from the transport.
         *
         * @param[in] clientTopicListener The client topic listener.
         */
        public void setClientTopicListener(MamaTransportTopicListener clientTopicListener)
        {
            /* Save argument in the member variable. */
            mClientTopicListener = clientTopicListener;
        }
    }

    /* ****************************************************** */
    /* Static Functions. */
    /* ****************************************************** */

    static
    {
        initIDs();
    }

    /* ****************************************************** */
    /* Private Member Variables. */
    /* ****************************************************** */

    /* This value contains a pointer to a utility structure passed as closure
     * to the transport. It is  created in the underlying JNI C layer but must
     * be stored here for each transport.
     */
    private long closurePointer_i   =   0;

    /* The re-usable connection object is passed back up into the transport
     * and topic listeners by the JNI C layer. It is created in Java to avoid
     * bugs in the JVM.
     */
    private MamaConnection mConnection;

    /* The Mama bridge. */
    private MamaBridge myBridge;
    
    /* The re-usable transport listener. */
    InternalTransportListener mListener;

    /* The re-usable transport topic listener. */
    private InternalTopicListener mTopicListener;

    /* This value contains a pointer to the underlying mamaTransport. This object
     * is created in the underlying JNI C layer but must be stored here for each
     * transport.
     */
    private long transportPointer_i =   0;

    /* ****************************************************** */
    /* Construction and Finalization. */
    /* ****************************************************** */

    /**
     * The constructor initialises all member variables and creates the
     * re-usable objects including the transport listeners.
     */
    public MamaTransport()
    {
        /* Create the re-usable connection object using the default constructor
         * so an underlying native object won't be created.
         */
        mConnection = new MamaConnection();

        // Allocate the transport
        allocateTransport(mConnection);
    }

    /* ****************************************************** */
    /* Private Functions. */
    /* ****************************************************** */

    /**
     * This function will allocate the underlying transport.
     *
     * @param[in] connection The re-usable connection object that will be passed
     *                       back up into listener callbacks.
     */
    private native void allocateTransport(MamaConnection connection);

    /**
     * This function will register for the native transport listener.
     * This is done only once the first time that the client registers, from
     * then on the listeners will be added and removed in the Java layer.
     * Unregistration at the C layer will only occur whenever the transport
     * is destroyed.
     *
     * @param[in] listenerEx The listener to add.
     */
    private native void nativeAddListener(MamaTransportListener listener);

    /**
     * This function will register for the native transport topic.
     * This is done only once the first time that the client registers, from
     * then on the listeners will be added and removed in the Java layer.
     * Unregistration at the C layer will only occur whenever the transport
     * is destroyed.
     *
     * @param[in] listenerEx The listener to add.
     */
    private native void nativeAddTopicListener(MamaTransportTopicListener topicListener);

    /* ****************************************************** */
    /* Public Functions. */
    /* ****************************************************** */

    /**
     * This function will set the client listener, note that only one
     * listener or extended listener is supported at any one time.
     *
     * @param[in] transportListener The new listener, null can be passed to
     *                              remove the current listener.
     */
    public void addTransportListener(MamaTransportListener transportListener)
    {
        // If this internal listener hasn't been created then do so now.
        if(mListener == null)
        {
            // Allocate the listener
            mListener = new InternalTransportListener();

            // Set it in the native layer
            nativeAddListener(mListener);
        }

        /* Set the client's listener inside the re-usable object. */
        mListener.setClientListener(transportListener);
    }

    /**
     * This function will set the client topic listener, note that only one
     * topic listener is supported at any one time.
     *
     * @param[in] transportTopicListener The new listener, null can be passed
     *                                   to remove the current listener.
     */
    public void addTransportTopicListener(MamaTransportTopicListener transportTopicListener)
    {
        // If this internal listener hasn't been created then do so now.
        if(null == mTopicListener)
        {
            // Allocate the listener
            mTopicListener = new InternalTopicListener();

            // Set it in the native layer
            nativeAddTopicListener(mTopicListener);
        }

        /* Set the client's listener inside the re-usable object. */
        mTopicListener.setClientTopicListener(transportTopicListener);
    }

    /**
     * This function will remove the current transport listener.
     * It will actually remove both the basic and extended listeners.
     *
     * @param[in] transportListener The transport listener. Note that this
     *                              parameter is currently redundant.
     */
    public void removeTransportListener(MamaTransportListener transportListener)
    {
        // Note that the listener will only be created if it has been set once
        if(mListener != null)
        {
            /* Set both listeners to null. */
            mListener.setClientListener(null);
        }
    }

    /**
     * This function will remove the current topic listener.
     */
    public void removeTransportTopicListener()
    {
        // Note that the listener will only be created if it has been set once
        if(null != mTopicListener)
        {
            /* Set the current topic listener to null. */
            mTopicListener.setClientTopicListener(null);
        }
    }

    public long getPointerVal()
    {
        return transportPointer_i;
    }

    public void create (MamaBridge bridge)
    {
        myBridge = bridge;
        _create (bridge);
    }
    public native void _create (MamaBridge bridge);


    public void create (String name, MamaBridge bridge)
    {
        myBridge = bridge;
        _create (name,bridge);
    }

    public native void _create (String name, MamaBridge bridge);

    /* Destroy the transport. */
    public native void destroy();

    /* Return the outbound throttle rate in messages/second. */
    private native double getOutboundThrottle (int throttleInstance);

    /* Return the outbound throttle rate in messages/second. */
    public double getOutboundThrottle (MamaThrottleInstance throttleInstance)
    {
        final String METHOD_NAME = "getOutboundThrottle(): ";

        if (null==throttleInstance) throw new
                        MamaException(METHOD_NAME + "NULL Throttle instance");

        return getOutboundThrottle (throttleInstance.getValue());
    }

    /*We need this overloaded version to satisify the pure Java samples*/
    public double getOutboundThrottle ()
    {
        return getOutboundThrottle (MamaThrottleInstance.INITIAL_THROTTLE);
    }

    /* Set the throttle rate for outbound message. */
    private native void setOutboundThrottle (int throttleInstance, double outboundThrottle);

    /* Set the throttle rate for outbound message. */
    public void setOutboundThrottle (MamaThrottleInstance throttleInstance,
                                     double outboundThrottle)
    {
        final String METHOD_NAME = "setOutboundThrottle(): ";

        if (null==throttleInstance) throw new MamaException(
                METHOD_NAME + "NULL Throttle instance");

        setOutboundThrottle (throttleInstance.getValue(), outboundThrottle);
    }

    /*We need this overloaded version to satisify the pure Java samples*/
    public void setOutboundThrottle (double outboundThrottle)
    {
         setOutboundThrottle (MamaThrottleInstance.INITIAL_THROTTLE,
                              outboundThrottle);
    }

    public native void setDescription (String description);

    public native String getDescription ();

    /*
        Set whether to invoke the quality callback for all subscriptions
        whenever a data quality event occurs (the default), or whether to
        invoke the quality callback only when data subsequently arrives for a
        subscription.
    */
    public native void setInvokeQualityForAllSubscs (
                        boolean invokeQualityForAllSubscs);

    /*
        Get whether the transport has been set to invoke the quality callback
        for all subscriptions whenever a data quality event occurs.
    */
    public native boolean getInvokeQualityForAllSubscs();

    /*
        Get the quality of the transport.
    */
    public native short getQuality();

    /*Used to cache ids for callback methods/fields*/
    private static native void initIDs();

    public MamaBridge getBridge()
    {
        return myBridge;
    }

    /*Request conflation*/
    public native void requestConflation();

    /*Request end conflation*/
    public native void requestEndConflation();

    /*Get transport name*/
    public native String getName();

    /*The String name for the transport*/
    public native void setName(String name);

}/*end class*/
