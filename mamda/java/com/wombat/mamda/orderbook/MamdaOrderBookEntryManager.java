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

package com.wombat.mamda.orderbook;

import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Iterator;

public class MamdaOrderBookEntryManager
{
    private HashMap mEntries = null;

    /**
     * Default constructor.  Create an empty order book entry manager.
     * @param approxCount Approximate number of entries.
     */
    public MamdaOrderBookEntryManager (int  approxCount)
    {
        mEntries = new HashMap (approxCount);
    }

    /**
     * Clear all entries from the manager.
     */
    public void clear ()
    {
        mEntries.clear ();
    }

    /**
     * Clear all entries for a particular symbol from the manager.
     * @param symbol the symbol.
     */
    public void clear (String  symbol)
    {
        Iterator i = mEntries.entrySet ().iterator ();
        while (i.hasNext ())
        {
            String key            = (String) i.next ();
            MamdaOrderBookEntry e = (MamdaOrderBookEntry) mEntries.get (key);

            if (symbol.equals (e.getSymbol ()))
            {
                mEntries.remove (key);
            }
        }
    }

    /**
     * Add an entry to the manager.  This method may throw a
     * MamdaOrderBookDuplicateEntry exception.
     * @param entry the entry to add.
     */
    public void addEntry (MamdaOrderBookEntry  entry)
    {
        addEntry (entry, entry.getId ());
    }

    /**
     * Add an entry to the manager using a specific entry ID, which
     * may be different from the entry's entry ID.  This method may
     * throw a MamdaOrderBookDuplicateEntry exception.
     * @param entry the entry to add.
     * @param entryId the entry id to use.
     */
    public void addEntry (MamdaOrderBookEntry  entry,
                          String               entryId)
    {
        MamdaOrderBookEntry existEntry =
            (MamdaOrderBookEntry) mEntries.get (entryId);

        if (existEntry != null)
        {
            throw new MamdaOrderBookDuplicateEntryException (existEntry, entry);
        }
        mEntries.put (entryId, entry);
    }

    /**
     * Find an entry in the manager.  If no entry matches the unique
     * entry ID and "mustExist" is true then a
     * MamdaOrderBookMissingEntryException exception is thrown, otherwise it
     * returns NULL.
     * @return the entry if found.
     * @param entryId The entry id to search for.
     * @param mustExist true if the entry must exist.
     */
    public MamdaOrderBookEntry findEntry (String       entryId,
                                          boolean      mustExist)
    {
        MamdaOrderBookEntry result =
            (MamdaOrderBookEntry) mEntries.get (entryId);
        if (mustExist && result == null)
            throw new MamdaOrderBookMissingEntryException (entryId);
        else
            return result;
    }

    /**
     * Remove an entry from the manager.  This method does not
     * actually delete the MamdaOrderBookEntry object itself.
     * @param entryId the id for the entry to be removed.
     */
    public void removeEntry (String  entryId)
    {
        mEntries.remove (entryId);
    }

    /**
     * Remove an entry from the manager.
     * @param entry the entry to remove.
     */
    void removeEntry (MamdaOrderBookEntry entry)
    {
        if (entry != null)
        {
            removeEntry (entry.getId ());
        }
    }

    /**
     * Dump (print) all of the entries in this manager to the output
     * stream.
     * @param output The <code>OutputStream</code>
     */
    void dump (OutputStream output)
    {
        PrintWriter out = new PrintWriter (output, true);
        out.println ("MamdaOrderBookEntryManager: dump:");
        Iterator i = mEntries.keySet ().iterator ();
        while (i.hasNext ())
        {
            String              key   = (String) i.next ();
            MamdaOrderBookEntry entry = (MamdaOrderBookEntry)mEntries.get(key);
            out.print   (key + ":");
            out.print   ("symbol="  + entry.getSymbol());
            out.print   (", price=" + entry.getPrice());
            out.print   (", side="  + entry.getSide());
            out.print   (", id="    + entry.getId());
            out.println (", size="  + entry.getSize());
        }
    }

    /**
     * Dump (print) all of the entries in this manager to System.out.
     */
    void dump ()
    {
        dump (System.out);
    }

}
