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

/**
 * The <code>MamaDQPublisher</code> interface represents a subscription for a symbol.
 * It can also allow for the broadcast and point-to-point publishing of messages to clients.
 *
 * @see Mama
 */
public class MamaDQPublisher
{
	/*A long value containing a pointer to the underlying C dqpublisher structure*/
	private long dqPublisherPointer_i = 0;

    /* Object container to hold the user supplied cache */
    private Object myCache;

	static
	{
		initIDs();
	}

    /**
     * Constructor for MamaDQPublisher
     */
    public MamaDQPublisher()
    {
        /* nothing to do */
    }
    
    /**
     * Create a new MamaDQPublisher
     *
     * @param transport The transport for which to create the new publisher against
     * @param topic The topic associated with the new publisher
     */
	public void create (MamaTransport transport, String topic)
	{
		_create (transport, topic);
	}

    /**
     * Send a message (broadcast) from the MamaDQPublisher
     *
     * @param msg The MamaMsg that will be sent from this publisher
     */
	public void send (MamaMsg msg)
	{
		checkIsCreated("send");
		_send(msg);
	}

    /**
     * Send a message (point-to-point) from the MamaDQPublisher
     *
     * @param request The request message (from an inbox)
     * @param reply The reply message containing the relevant data
     */
	public void sendReply (MamaMsg request, MamaMsg reply)
	{
		checkIsCreated ("sendReply");
		_sendReply(request, reply);
	}

    /**
     * Destroy the MamaDQPublisher
     */
	public void destroy ()
	{
		checkIsCreated ("destroy");
		_destroy();
	}

    /**
     * Set the status for the MamaDQPublisher
     *
     * @param status The mamaMsgStatus to set for the publisher
     */
	public void setStatus (int status)
	{
		checkIsCreated ("setStatus");
		_setStatus (status);
	}

    /**
     * Set the sender ID for the MamaDQPublisher
     *
     * @param id The publisher sender id to set for the publisher
     */
	public void setSenderId (long id)
	{
		checkIsCreated ("setSenderId");
		_setSenderId (id);
	}

    /**
     * Set the message sequence number for the MamaDQPublisher
     *
     * @param num The sequence number to set for the publisher
     */
	public void setSeqNum (long num)
	{
		checkIsCreated ("setSeqNum");
		_setSeqNum (num);
	}

	private native void _create (MamaTransport transport, String topic);

	private native void _send (MamaMsg msg);

	private native void _sendReply (MamaMsg request, MamaMsg reply);

	private native void _destroy ();

	private native void _setStatus (int status);

	private native void _setSenderId (long id);

	private native void _setSeqNum (long num);

	private static native void initIDs ();

	private void checkIsCreated (String invokingMethod)
	{
		if (0 == dqPublisherPointer_i)
		{
			throw new MamaException("Cannot call " + invokingMethod +
					"before invoking create()");
		}
	}

    /**
     * Set a cache for the MamaDQPublisher
     *
     * @param cache The cache to save against the publisher
     */
    public void setCache (Object cache)
    {
        myCache = cache;
    }

    /**
     * Return the cache for the MamaDQPublisher
     *
     * @return Object The cache for the publisher
     */
    public Object getCache()
    {
        return myCache;
    }
}
