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

import com.wombat.common.WombatException;
import java.util.Iterator;


public class MamaMsgIterator implements Iterator
{

    public native void iteratorCreate (MamaMsg msg, MamaDictionary dictionary);
    
    public native void iteratorAssociate (MamaMsg msg, MamaDictionary dictionary);
    
    public native void iteratorNext ();
    
    public native boolean iteratorHasNext ();

    private native void iteratorDestroy ();

    private long  iteratorPointer_i = 0;
    private MamaMsg myParent =  null;
    
    private MamaMsgField myMsgField_i = null;
        
    public MamaMsgIterator (MamaMsg parent, MamaDictionary dictionary)
    {
        if (myMsgField_i == null)
        {
            myMsgField_i = new MamaMsgField();
        }
        
        myParent = parent;
        iteratorCreate (myParent, dictionary);
    }
    
    public void reset (MamaMsg parent, MamaDictionary dictionary)
    {
        myParent = parent;
        iteratorAssociate (myParent, dictionary);
    }

    public Object next()
    {
        iteratorNext ();
        return myMsgField_i;
    }
    
    public boolean hasNext()
    {

        return (iteratorHasNext ());
    }
    
    public void remove()
    {
    }

    /* Not exposed publically, since there is no corresponding public
       create function.  The iterator will be destroyed when the message
       is destroyed. */
    void destroy ()
    {
        myMsgField_i.destroy();
        iteratorDestroy ();
    }
}
