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

#ifndef MamdaOrderBookEntryManagerH
#define MamdaOrderBookEntryManagerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookExceptions.h>

namespace Wombat
{

    /**
     * MamdaOrderBookEntryManager is a class that provides a global order
     * book lookup service, matching entry IDs that are unique across a
     * set of order books.  Some data sources do not provide a symbol when
     * sending updated or deletions for order book entries.  The primary
     * purpose of this class is to return access to the actual order book
     * entry represented by a unique entry ID and, therefore, also
     * (indirectly, via methods available from the book entry object) the
     * the order book itself.
     */
    class MAMDAOPTExpDLL MamdaOrderBookEntryManager
    {
    public:
        /**
         * Default constructor.  Create an empty order book entry manager.
         */
        MamdaOrderBookEntryManager (mama_u32_t  approxCount);

        /**
         * Destructor.
         */
        ~MamdaOrderBookEntryManager ();

        /**
         * Clear all entries from the manager.
         */
        void clear ();

        /**
         * Clear all entries for a particular symbol from the manager.
         */
        void clear (const char*  symbol);

        /**
         * Add an entry to the manager.  This method may throw a
         * MamdaOrderBookDuplicateEntry exception.
         */
        void addEntry (MamdaOrderBookEntry*  entry);

        /**
         * Add an entry to the manager using a specific entry ID, which
         * may be different from the entry's entry ID.  This method may
         * throw a MamdaOrderBookDuplicateEntry exception.
         */
        void addEntry (MamdaOrderBookEntry*  entry,
                       const char*           entryId);

        /**
         * Find an entry in the manager.  If no entry matches the unique
         * entry ID and "mustExist" is true then a
         * MamdaOrderBookMissingEntry exception is thrown, otherwise it
         * returns NULL.
         */
        MamdaOrderBookEntry* findEntry (const char*  entryId,
                                        bool         mustExist);

        /**
         * Remove an entry from the manager.  This method does not
         * actually delete the MamdaOrderBookEntry object itself.
         */
        void removeEntry (const char*  entryId);

        /**
         * Remove an entry from the manager.  This method does not
         * actually delete the MamdaOrderBookEntry object itself.
         */
        void removeEntry (MamdaOrderBookEntry*  entry);

        /**
         * Dump (print) all of the entries in this manager to the output
         * stream.
         */
        void dump (std::ostream&  output) const;

    private:
        /**
         * No copy constructor.
         */
        MamdaOrderBookEntryManager (const MamdaOrderBookEntryManager&);

        /**
         * No assignment operator.
         */
        MamdaOrderBookEntryManager& operator= (const MamdaOrderBookEntryManager&);

        struct MamdaOrderBookEntryManagerImpl;
        MamdaOrderBookEntryManagerImpl& mImpl;
    };

} // namespace

#endif // MamdaOrderBookEntryManagerH
