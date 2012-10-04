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
import java.util.*;

/**
 * A Mama source manager maintains information about a set of data sources,
 * including the quality of the data coming from sources.
 */

public class MamaSourceManager
{
    private MamaQuality mamaQuality;
    private Map         mySourceMap;
    /**
     * Constructor - creates a new MamaSourceManager object
     */
    public MamaSourceManager ()
    {
        /**thread safe hashmap*/
        mySourceMap = Collections.synchronizedMap(new HashMap());
    }
    
    /**
     * create a new mamaSource and add it to the manager.
     * @param name The string identifier for the mamaSource
     * @return MamaSource
     */
    private MamaSource createSource (final String sourceName)
    {
        /** check if the source already exists*/
        if (mySourceMap.containsKey(sourceName))
        {
            return (MamaSource)mySourceMap.get(sourceName);
        }
        /**if we don't have one create one and insert into
           the map*/
        MamaSource mamaSource = new MamaSource ();
        mamaSource.setId (sourceName);
        mySourceMap.put (sourceName, mamaSource);
        return mamaSource;
    }
    /**
     * Locates an existing mamaSource for a given name, if none
     * exists a new one is created and added to the source manager.
     * @param sourceName The string identifier for the mamaSource
     * @return MamaSource
     */
    public MamaSource findOrCreateSource (final String sourceName)
    {
        return createSource (sourceName);
    }
    
    /**
     * locates an existing source for a given name, if none is found
     * null is returned, The caller has to validate null value on 
     * return.
     * @param sourceName  The string identifier for the MamaSource
     * @return MamaSource
     */
    public  MamaSource findSource (final String sourceName)
    {
        MamaSource  temp = null;
        if (mySourceMap.containsKey (sourceName))
        {
            temp = (MamaSource)mySourceMap.get (sourceName);
            return temp;
        }
        return temp;
    }
    /**
     * Add an existing mama Source to the source manager.
     * The id of the source will be used instead of the name to 
     * identify the source within the source manager.
     * @param mamaSource an existing mama source
     */
    public void addSource (MamaSource mamaSource)
    {
        final String name = mamaSource.getId();
        addSource (mamaSource,name);
    }
    
    /**
     * Add an existing mama Source to the source manager.
     * The  name will used to uniquely identify the source 
     * within the source manager.
     * @param mamaSource an existing mama source
     * @param sourceName  The string identifier for the MamaSource
     */
    public void addSource (MamaSource mamaSource, 
                          final String sourceName)
    {
        if (mySourceMap.containsKey (sourceName))
        {
            throw new MamaException ("MamaSourceManager:addSource - " +
                                     "Duplicate source found !!!!");
        }
        mamaSource.setMappedId (sourceName);
        mySourceMap.put (sourceName,mamaSource);
    }
    
    /**
     * Return the size of the contents of source manager
     */
    public int getSize ()
    {
        return mySourceMap.size();
    }
    
    public Iterator sourceIterator ()
    {
        return new MamaSourceManagerIterator(this);
    }

    public void destroy()
    {
        if (mySourceMap != null)
            mySourceMap.clear();
    }

    private class MamaSourceManagerIterator implements Iterator
    {
        private MamaSourceManager mySourceManager = null;
        private Iterator myIterator = null;
        
        public MamaSourceManagerIterator (
            MamaSourceManager sourceManager)
        {
            if (sourceManager == null)
            {
                throw new NullPointerException ("MamaSourceManagerIterator : "+
                                                "Nullpointer exception " +
                                                " in construction");
            }
            mySourceManager = sourceManager;
            myIterator = mySourceManager.mySourceMap.
                entrySet().iterator();
        }

        public boolean hasNext ()
        {
            return myIterator.hasNext();
        }
        public void remove ()
        {
            myIterator.remove();
        }
        public Object next ()
        {
            /*what if I am interested in keys ?*/
            Map.Entry pairs = (Map.Entry)myIterator.next();
            MamaSource mamaSource =
                (MamaSource)pairs.getValue();
            return mamaSource; 
        }
    }
}
/**End*/
