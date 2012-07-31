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
* Wrapper class for the native C publisher structure and related functions
*/
public class MamaPublisher
{

    static
    {
        initIDs();
    }

    /*A long value containing a pointer to the underlying C publisher structure*/
    private long    publisherPointer_i   =   0;

    public void create (MamaTransport transport, String topic)
    {
        _create(transport,topic);
    }

    public void send (MamaMsg msg)
    {
        checkIsCreated("send()");
        _send(msg);
    }

    public void sendWithThrottle (MamaMsg msg)
    {
        checkIsCreated("sendWithThrottle()");
        _sendWithThrottle(msg);
    }

	public void sendWithThrottle (MamaMsg msg, MamaThrottleCallback callback)
	{
		 checkIsCreated("sendWithThrottleWithCallback()");
		 _sendWithThrottleWithCallback(msg, callback);
	}

	 public void sendReplyToInbox (MamaMsg request, MamaMsg reply)
    {
        checkIsCreated("sendReplyToInbox()");
        _sendReplyToInbox(request,reply);
    }

    public void sendReplyToInboxWithThrottle (MamaMsg request, MamaMsg reply)
    {
        checkIsCreated("sendReplyToInboxWithThrottle()");
        _sendReplyToInboxWithThrottle(request,reply);
    }

    public void sendFromInbox (MamaInbox inbox, MamaMsg msg)
    {
        checkIsCreated("sendFromInbox()");
        _sendFromInbox(inbox,msg);
    }

    private native void _create (MamaTransport transport, String topic);

    private native void _send (MamaMsg msg);

    private native void _sendWithThrottle (MamaMsg msg);

	private native void _sendWithThrottleWithCallback(MamaMsg msg, MamaThrottleCallback callback);

	private native void _sendReplyToInbox (MamaMsg request, MamaMsg reply);

    private native void _sendReplyToInboxWithThrottle (MamaMsg request, MamaMsg reply);

    private native void _sendFromInbox (MamaInbox inbox, MamaMsg msg);
    
    private static native void initIDs();

    private void checkIsCreated(String invokingMethod)
    {
        if(0==publisherPointer_i)
        {
            throw new MamaException("Cannot call "+invokingMethod+
                                    "before invoking create()");
        }
    }

}/*End class*/
