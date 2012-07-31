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
import  java.util.concurrent.locks.*;

public class MamdaLock extends ReentrantReadWriteLock
{
    private final ReentrantReadWriteLock lock = new ReentrantReadWriteLock();
    private final Lock w = lock.writeLock();
    private final Lock r = lock.readLock();
  
    /**
     * Acquires the lock
     **/
      
    public final void acquireWriteLock()
    {
        w.lock();
    }
    
    public final void acquireReadLock()
    {
        r.lock();
    }
    
    /**
     * Releases the lock
     **/

    public final void releaseWriteLock() 
    {
        w.unlock();
    }
    
    public final void releaseReadLock() 
    {
        r.unlock();
    }
}
