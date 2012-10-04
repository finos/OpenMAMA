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

public class MamaFtMember
{
    static
    {
        initIDs();
    }

    private long ftMemberPointer_i      = 0;

    private long ftCallbackClosure_i    = 0;

    public MamaFtMember () 
    {
        create();
    }

    public native void destroy ();

    public native void activate ();

    public native void deactivate ();

    public native boolean isActive ();

    public native String getGroupName ();

    public native int getWeight ();

    public native double getHeartbeatInterval ();

    public native double getTimeoutInterval ();

    public native void setWeight (int weight);

    public native void setInstanceId (String id);

    public native void setupFtMember (short          ftType,
                                      MamaQueue      queue, 
                                      MamaFtCallback callback,
                                      MamaTransport  transport,
                                      String         groupName,
                                      int            weight,
                                      double         heartbeatInterval,
                                      double         timeoutInterval);

    private native void create ();

    private static native void initIDs ();

    protected void finalize ()
    {
        if (ftMemberPointer_i != 0)
        {
            destroy();
        }
    }
}
