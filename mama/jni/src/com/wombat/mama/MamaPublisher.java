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

    /* A long value containing a pointer to the underlying C publisher structure */
    private long  publisherPointer_i = 0;

    /* Reusable MamaTransport object. */
    private MamaTransport mamaTransport_i = null;

	public void create (MamaTransport transport, String topic)
    {
        _create(transport, null, topic, null, null);
    }

	public void create (MamaTransport transport, MamaQueue queue, String topic, MamaPublisherCallback cb)
    {
        _create(transport, queue, topic, null, cb);
    }

	public void create (MamaTransport transport, String topic, String source)
    {
        _create(transport, null, topic, source, null);
    }

	public void create (MamaTransport transport, MamaQueue queue, String topic, String source, MamaPublisherCallback cb)
    {
        _create(transport, queue, topic, source, cb);
    }

	public native void destroy();

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

    public MamaTransport getTransport ()
    {
        /* Calls the native method first. This will
           reuse the reusable mamaTransport in MamaPublisher
           Only create the MamaTransport if we actually need to
        */
        if (mamaTransport_i == null)
        {
            mamaTransport_i = new MamaTransport();
        }

        mamaTransport_i.setPointerVal(0);

        _getTransport ();

        if (mamaTransport_i.getPointerVal() == 0)
        {
            return null;
        }

        return mamaTransport_i;
    }

	public native String getRoot();

	public native String getSource();

	public native String getSymbol();

	public native short getState();

	public native String stringForState(short state);

    private native void  _getTransport ();

    private native void _create (MamaTransport transport, MamaQueue queue, String topic, String source, MamaPublisherCallback cb);

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
