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

public class MamaDQPublisher
{
	/*A long value containing a pointer to the underlying C dqpublisher structure*/
	private long dqPublisherPointer_i = 0;

	static
	{
		initIDs();
	}

    public MamaDQPublisher()
    {
        /* nothing to do */
    }
    
	public void create (MamaTransport transport, String topic)
	{
		_create (transport, topic);
	}

	public void send (MamaMsg msg)
	{
		checkIsCreated("send");
		_send(msg);
	}

	public void sendReply (MamaMsg request, MamaMsg reply)
	{
		checkIsCreated ("sendReply");
		_sendReply(request, reply);
	}

	public void destroy ()
	{
		checkIsCreated ("destroy");
		_destroy();
	}

	public void setStatus (int status)
	{
		checkIsCreated ("setStatus");
		_setStatus (status);
	}

	public void setSenderId (long id)
	{
		checkIsCreated ("setSenderId");
		_setSenderId (id);
	}

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
}
