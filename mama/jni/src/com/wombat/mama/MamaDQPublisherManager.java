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

public class MamaDQPublisherManager
{
	/*A long value containing a pointer to the underlying C publisher manager structure*/
	private long dqpublisherManagerPointer_i = 0;
    
    private static final ArrayList myTopics = new ArrayList();
    
	static
	{
		initIDs();
	}

    public void create (MamaTransport transport, 
                        MamaQueue queue,
		                MamaDQPublisherManagerCallback callback, 
                        String source)
	{
		_create (transport, queue, callback, source, "_MD");
	}
    
	public void create (MamaTransport transport, 
                        MamaQueue queue,
		                MamaDQPublisherManagerCallback callback, 
                        String source, 
                        String root)
	{
		_create (transport, queue, callback, source, root);
	}

	public void destroy()
	{
		checkIsCreated("destroy");
		_destroy();	
	}

	// Probably needs to be more defensive in terms of dealing with nulls
	public void addPublisher (String symbol, 
                              MamaDQPublisher publisher,
		                      MamaSubscriptionCallback cache)
	{
		checkIsCreated("addPublisher");
		_addPublisher(symbol, publisher, cache);
	}

    
    public MamaDQPublisher removePublisher (String symbol)
    {
        checkIsCreated("addPublisher");
        return _removePublisher(symbol);
    }
    
    public void destroyPublisher (String symbol)
    {
        checkIsCreated("addPublisher");
        _destroyPublisher(symbol);
    }
    
    // Probably needs to be more defensive in terms of dealing with nulls
	public MamaDQPublisher createPublisher (String symbol, Object cache)
	{
		checkIsCreated("createPublisher");
        
        MamaDQPublisher myPub = new MamaDQPublisher();
        MamaPublishTopic myTopic = new MamaPublishTopic(symbol, myPub, cache);
        _createPublisher (symbol, myPub, myTopic);
        myTopics.add (myTopic);
        return myPub;
	}

	public void setStatus (int status)
	{
		checkIsCreated("setStatus");
		_setStatus(status);
	}

	public void setSeqNum (long seqNum)
	{
		checkIsCreated("setSeqNum");
		_setSeqNum(seqNum);
	}

    public void setSenderId (long id)
	{
		checkIsCreated("setSenderId");
		_setSenderId(id);
	}
    
	public void sendSyncRequest (int numMsg, double delay, double duration)
	{
		checkIsCreated("sendSyncRequest");
		_sendSyncRequest(numMsg, delay, duration);
	}

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

	public static class MamaPublishTopic
	{
		private final MamaDQPublisher dqPublisher;
		private final String symbol;
        private final Object cache;
        
		public MamaPublishTopic (String          symbol, 
                                 MamaDQPublisher dqPublisher,
								 Object          cache)
		{
			this.dqPublisher = dqPublisher;
			this.cache = cache;
			this.symbol = symbol;
		}

		public MamaDQPublisher getPublisher()
		{
			return dqPublisher;	
		}

		public Object getCache()
		{
			return cache;
		}

		public String getSymbol()
		{
			return symbol;
		}
	}
}
