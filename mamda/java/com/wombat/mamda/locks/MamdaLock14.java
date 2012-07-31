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

package com.wombat.mamda.locks;

public class MamdaLock
{
    private int lockCount;
    private int waitingWriters;
    private final Object lockObj = new Object();

    public final void acquireReadLock()
    {
        synchronized(lockObj)
        {
            try
            {
                while ((lockCount == -1) || (waitingWriters != 0))
                {
                    lockObj.wait();
                }
            }
            catch(java.lang.InterruptedException e)
            {
                System.out.println(e);
            }
            ++lockCount;
        }
    }

    public final void acquireWriteLock()
    {
        synchronized(lockObj)
        {
            ++waitingWriters;
            try
            {
                while (lockCount != 0)
                {
                    lockObj.wait();
                }
            }
            catch(java.lang.InterruptedException e)
            {
                System.out.println(e);
            }
            --waitingWriters;
            lockCount = -1;
        }
    }

    public final void releaseWriteLock()
    {
        synchronized(lockObj)
        {
            if (0 == lockCount)
                return;
            if (-1 == lockCount)
                lockCount = 0;
            else
                --lockCount;

            lockObj.notifyAll();
        }
    }

    public final void releaseReadLock()
    {
        synchronized(lockObj)
        {
            if (0 == lockCount)
                return;
            if (-1 == lockCount)
                lockCount = 0;
            else
                --lockCount;

            lockObj.notifyAll();
        }
    }
}
