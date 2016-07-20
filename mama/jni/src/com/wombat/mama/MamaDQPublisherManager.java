/* $Id:
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

import java.util.ArrayList;
import java.util.Iterator;

/**
 * The <code>MamaDQPublisherManager</code> interface represents a collection of subscriptions.
 * It has the ability to provide all the aspects of a Mama Publisher including initials,
 * recaps, and data quality.
 *
 * @see Mama
 */
public class MamaDQPublisherManager
{
	/*A long value containing a pointer to the underlying C publisher manager structure*/
	private long dqpublisherManagerPointer_i = 0;
    
    private static final ArrayList myTopics = new ArrayList();
    
	static
	{
		initIDs();
	}

    /**
     * Create in instance of the MamaDQPublisherManager
     *
     * @param transport The transport to create the publishermanager on
     * @param queue The queue to create the publisher manager on
     * @param callback The associated callback to use
     * @param source The source namepaceto create the publisher manager with
     */
    public void create (MamaTransport transport, 
                        MamaQueue queue,
		                MamaDQPublisherManagerCallback callback, 
                        String source)
	{
		_create (transport, queue, callback, source, "_MD");
	}
    
    /**
     * Create in instance of the MamaDQPublisherManager
     *
     * @param transport The transport to create the publishermanager on
     * @param queue The queue to create the publisher manager on
     * @param callback The associated callback to use
     * @param source The source name to create the publisher manager with
     * @param root The root of the source namespace
     */
	public void create (MamaTransport transport, 
                        MamaQueue queue,
		                MamaDQPublisherManagerCallback callback, 
                        String source, 
                        String root)
	{
		_create (transport, queue, callback, source, root);
	}

    /**
     * Destroy the instance of the MamaDQPublisherManager
     */
	public void destroy()
	{
		checkIsCreated("destroy");
		_destroy();	
	}

    /**
     * Add a new Publisher to the MamaDQPublisherManager
     *
     * @param symbol The symbol associated with the new publisher
     * @param publisher The new publisher instance
     * @param cache The callback associated with the new publisher
     */
	// Probably needs to be more defensive in terms of dealing with nulls
	public void addPublisher (String symbol, 
                              MamaDQPublisher publisher,
		                      MamaSubscriptionCallback cache)
	{
		checkIsCreated("addPublisher");
		_addPublisher(symbol, publisher, cache);
	}

    
    /**
     * Remove a Publisher from the MamaDQPublisherManager
     *
     * @param symbol The symbol for which the associated publisher will be removed
     */
    public MamaDQPublisher removePublisher (String symbol)
    {
        checkIsCreated("removePublisher");
        return _removePublisher(symbol);
    }
    
    /**
     * Remove a Publisher from the MamaDQPublisherManager
     *
     * @param symbol The symbol for which the associated publisher will be destroyed and free'd
     */
    public void destroyPublisher (String symbol)
    {
        checkIsCreated("destroyPublisher");
        _destroyPublisher(symbol);
    }
    
    /**
     * Create a new Publisher and associate it with the MamaDQPublisherManager
     *
     * @param symbol The symbol for which the new publisher will be created
     * @param cache The callback that will be associated with the new publisher
     *
     * @return MamaDQPublisher The newly created MamaDQPublisher
     */
    // Probably needs to be more defensive in terms of dealing with nulls
	public MamaDQPublisher createPublisher (String symbol, Object cache)
	{
		checkIsCreated("createPublisher");
        
        MamaDQPublisher myPub = new MamaDQPublisher();
        MamaPublishTopic myTopic = new MamaPublishTopic(symbol, myPub, cache);
        _createPublisher (symbol, myPub, myTopic);
        myPub.setCache(cache);
        myTopics.add (myTopic);
        return myPub;
	}

    /**
     * Set the status for the MamaDQPublisherManager
     *
     * @param status The mamaMsgStatus to set for the publisher manager
     */
	public void setStatus (int status)
	{
		checkIsCreated("setStatus");
		_setStatus(status);
	}

    /**
     * Set the message sequence number for the MamaDQPublisherManager
     *
     * @param seqNum The sequence number to set for the publisher manager
     */
	public void setSeqNum (long seqNum)
	{
		checkIsCreated("setSeqNum");
		_setSeqNum(seqNum);
	}

    /**
     * Set the sender ID for the MamaDQPublisherManager
     *
     * @param id The publisher sender id to set for the publisher manager
     */
    public void setSenderId (long id)
	{
		checkIsCreated("setSenderId");
		_setSenderId(id);
	}
    
    /**
     * Send out a synchronization request from the MamaDQPublisherManager
     *
     * @param numMsg The number of topics to include per sync response message
     * @param delay The Interval (in seconds) for the responder to wait before sending the first response
     * @param duration The Interval (in seconds) over which to send the responses
     */
	public void sendSyncRequest (int numMsg, double delay, double duration)
	{
		checkIsCreated("sendSyncRequest");
		_sendSyncRequest(numMsg, delay, duration);
	}

    /**
     * Publish a NO_SUBSCRIBERS message for the given symbol
     *
     * @param symbol The symbol to publish a NO_SUBSCRIBERS message against
     */
	public void sendNoSubscribers (String symbol)
	{
		checkIsCreated("sendSyncRequest");
		_sendNoSubscribers(symbol);
	}
    
	private native void _create (MamaTransport transport, 
                                 MamaQueue queue,
		                         MamaDQPublisherManagerCallback callback, 
                                 String source, 
                                 String root);

	private native void _destroy ();

	private native void _addPublisher (String symbol, 
                                       MamaDQPublisher publisher, 
                                       Object cache);
    
    private native MamaDQPublisher _removePublisher (String symbol);
    
    private native void _destroyPublisher (String symbol);

	private native MamaDQPublisher _createPublisher (String symbol, MamaDQPublisher pub, Object cache);

	private native void _setStatus (int status);

	private native void _setSeqNum (long seqNum);

    private native void _setSenderId (long id);
    
	private native void _sendSyncRequest (int numMsg, double delay, double duration);

    private native void _sendNoSubscribers (String symbol);
    
	private static native void initIDs ();

	private void checkIsCreated (String invokingMethod)
	{
		if (0 == dqpublisherManagerPointer_i)
		{
			throw new MamaException("Cannot call " + invokingMethod +
					"before invoking create()");
		}
	}

    /**
     * This class is used to contain information on each publisher within the MamaDQPublisherManager
     */
	public static class MamaPublishTopic
	{
		private final MamaDQPublisher dqPublisher;
		private final String symbol;
        private final Object cache;
        
        /**
         * Constructor for MamaPublishTopic
         *
         * @param symbol The symbol
         * @param dqPublisher The publisher
         * @param cache The cache
         */
		public MamaPublishTopic (String          symbol, 
                                 MamaDQPublisher dqPublisher,
								 Object          cache)
		{
			this.dqPublisher = dqPublisher;
			this.cache = cache;
			this.symbol = symbol;
		}

        /**
         * Return the publisher for the MamaPublishTopic
         *
         * @return MamaDQPublisher The publisher
         */
		public MamaDQPublisher getPublisher()
		{
			return dqPublisher;	
		}

        /**
         * Return the cache for the MamaPublishTopic
         *
         * @return Object The cache
         */
		public Object getCache()
		{
			return cache;
		}

        /**
         * Return the symbol for the MamaPublishTopic
         *
         * @return String The symbol
         */
		public String getSymbol()
		{
			return symbol;
		}
	}

    /**
     * Retrieve the cache associated with the given topic.
     *
     * @param symbol The symbol for the which you want to get the cache
     *
     * @return The cache associated with the symbol/publisher
     */
    public static Object getCache(String symbol)
    {
        Iterator it = myTopics.iterator();
        while(it.hasNext())
        {
            MamaPublishTopic myTopic = (MamaPublishTopic) it.next();
            if (myTopic.getSymbol().equals(symbol))
                return myTopic.getCache();
        }
        return null;
    }

    /**
     * Accessor for the internal ArrayList that contains all the current publishers
     *
     * @param symbol The symbol for the which you want to get the publisher
     *
     * @return The MamaDQPublisher associated with the given symbol
     */
    public static MamaDQPublisher getPublisher(String symbol)
    {
        Iterator it = myTopics.iterator();
        while(it.hasNext())
        {
            MamaPublishTopic myTopic = (MamaPublishTopic) it.next();
            if (myTopic.getSymbol().equals(symbol))
                return myTopic.getPublisher();
        }
        return null;
    }

}
