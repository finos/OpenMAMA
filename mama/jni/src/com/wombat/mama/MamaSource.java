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
import java.util.HashMap;
import java.util.Map;
import java.util.Collections;
/**
 * A MAMA source maintains information about a data source, including
 * the quality of the data coming from that source.  It inherits
 * MamaSourceManager because a source can have sub-sources.
 */

public class MamaSource extends MamaSourceManager
{

    private  MamaSource       myParent;
    private  Map              mySubscriptionsMap;
    private  MamaSourceState  myState;
    /* Keep a reference to the transport here so we don't need
       to call down to C code */
    private  MamaTransport    myTransport;
    private  long             mamaSourcePointer_i = 0;
    static
    {
        initIDs();
    }

    /**
     * Creates a new mama source with default parameters
     */
    public MamaSource ()
    {
        super ();
        createMamaSource ();
    }
    
    public MamaSource (final String sourceId, MamaTransport transport,
                       final String symbolNamespace)
    {
        super ();
        createMamaSource ();
        this.setId (sourceId);
        this.setTransport (transport);
        this.setSymbolNamespace (symbolNamespace);
    }
    
    public native void setId (final String sourceId);

    public native void setMappedId (final String mappedId);
    
    public native void setDisplayId (final String displayId);
   
    public native void setQuality (short quality);
   
    public void setState (MamaSourceState state)
    {
        this.myState = state;
    }
    
    public void setParent (MamaSource parent)
    {
        this.myParent = parent;
    }

    public native void setSymbolNamespace (final String symbolNamespace);
    
    public void setTransportName (final String transportName)
    {
        myTransport.setName (transportName);
        this._setTransportName (transportName);
    }

    private native void _setTransportName (final String transportName);
    
    public void setTransport (MamaTransport transport)
    {
        this.myTransport = transport;
        this._setTransport (transport);
    }

    private native void _setTransport (MamaTransport transport);

    public native final String getId ();
    
    public native final String getMappedId ();
    
    public native final String getDisplayId ();
    
    public native short getQuality ();
    
    /**
     * Get the state of a mama source object
     * @return The state of the source object
     */
    public MamaSourceState getState ()
    {
        return myState;
    }
    /**
     * Get the stringified  state name of a mama source object
     * @return The stringified state of the source object
     */
    public String getStateAsString ()
    {
        if (myState != null)
        return myState.toString();

        return MamaSourceState.UKNOWN.toString();
    }    
    /**
     * Get the parent source of the mama source object
     * @return MamaSource parent object
     */
    public MamaSource getParent ()
    {
        return myParent;
    }

    public native final String getSymbolNamespace ();
    
    public MamaTransport getTransport ()
    {
        return myTransport;
    }
    
    /**
     * Add a subscriptions to the mamaSource
     * @param MamaSubscription object
     */
    public void addSubscription (final String key, 
                                 MamaSubscription subscription)
    {
        if (mySubscriptionsMap == null)
        {
            mySubscriptionsMap = Collections.synchronizedMap(
                new HashMap());
        }
        if (mySubscriptionsMap.containsKey (key))
        {
            throw new MamaException ("MamaSource: Duplicate found");
        }
        mySubscriptionsMap.put (key, subscription);
    }
    /**
     * find a MamaSubscription, null will be returned if not found
     * @param The String identifier key for the subscription
     * @return The mama subscription object
     */
    public MamaSubscription findSubscription (final String key)
    {
        if (null != mySubscriptionsMap)
        {
            return (MamaSubscription)mySubscriptionsMap.get (key);
        } 
        else if (null != myParent)
        {
            return myParent.findSubscription (key);
        }
        else
        {
            return null;
        }
    }

    public boolean isPartOf (MamaSource source)
    {
        return this == source || myParent != null && myParent.isPartOf (source);
    } 
    
    private native static void initIDs();
    
    private native void createMamaSource ();

    private native void destroyMamaSource ();

    public void destroy()
    {
        // Call the base class
        super.destroy();

        // Destroy the underlying object
        destroyMamaSource();
    }
}
/**End*/
