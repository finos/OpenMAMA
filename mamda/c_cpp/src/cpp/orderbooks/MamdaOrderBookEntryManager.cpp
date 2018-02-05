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

#include <mamda/MamdaOrderBookEntryManager.h>
#include <wombat/wtable.h>
#include <iostream>

namespace Wombat
{

    struct MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl
    {
        MamdaOrderBookEntryManagerImpl (mama_u32_t  approxCount);
        ~MamdaOrderBookEntryManagerImpl ();

        void clear();
        void clear (const char* symbol);

        void addEntry (MamdaOrderBookEntry*  entry, 
                       const char*           entryId);

        MamdaOrderBookEntry* findEntry (const char*  entryId,
                                        bool         mustExist);

        void removeEntry (const char*  entryId);

        void dump (std::ostream&  output) const;

    private:
        wtable_t     mEntries;
    };

    MamdaOrderBookEntryManager::MamdaOrderBookEntryManager (mama_u32_t approxCount)
        : mImpl (*new MamdaOrderBookEntryManagerImpl(approxCount))
    {
    }

    MamdaOrderBookEntryManager::~MamdaOrderBookEntryManager()
    {
        delete &mImpl;
    }

    void MamdaOrderBookEntryManager::clear()
    {
        mImpl.clear();
    }

    void MamdaOrderBookEntryManager::clear (const char*  symbol)
    {
        mImpl.clear (symbol);
    }

    void MamdaOrderBookEntryManager::addEntry (
        MamdaOrderBookEntry*  entry)
    {
        if (entry)
        {
            mImpl.addEntry (entry, entry->getUniqueId());
        }
    }

    void MamdaOrderBookEntryManager::addEntry (
        MamdaOrderBookEntry*  entry,
        const char*           entryId)
    {
        if (entry)
        {
            mImpl.addEntry (entry, entryId);
        }
    }

    MamdaOrderBookEntry* MamdaOrderBookEntryManager::findEntry (
        const char*  entryId,
        bool         mustExist)
    {
        return mImpl.findEntry (entryId, mustExist);
    }

    void MamdaOrderBookEntryManager::removeEntry (
        const char*  entryId)
    {
        mImpl.removeEntry (entryId);
    }

    void MamdaOrderBookEntryManager::removeEntry (
        MamdaOrderBookEntry*  entry)
    {
        if (entry)
            removeEntry (entry->getUniqueId());
    }

    void MamdaOrderBookEntryManager::dump (
        std::ostream&  s) const
    {
        mImpl.dump(s);
    }


    MamdaOrderBookEntryManager::
    MamdaOrderBookEntryManagerImpl::MamdaOrderBookEntryManagerImpl(
        mama_u32_t  approxCount)
    {
        mEntries = wtable_create ("MamdaOrderBookEntryManager", approxCount);
    }

    MamdaOrderBookEntryManager::
    MamdaOrderBookEntryManagerImpl::~MamdaOrderBookEntryManagerImpl()
    {
        clear();
        if (mEntries) wtable_destroy (mEntries);
    }

    void MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::clear()
    {
        wtable_clear (mEntries);
    }

    static void clearSymbolCheck (
        wtable_t     table,
        void*        data,
        const char*  key, 
        void*        closure)
    {
        if (MamdaOrderBookEntry* entry = (MamdaOrderBookEntry*)data)
        {
            const char* entrySymbol = entry->getSymbol();
            if (entrySymbol)
            {
                if (strcmp(entrySymbol, (const char*)closure) == 0)
                    wtable_remove (table, key);
            }
        }
    }

    void MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::clear(
        const char*  symbol)
    {
        wtable_for_each (mEntries, clearSymbolCheck, (void*)symbol);
    }

    void MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::addEntry (
        MamdaOrderBookEntry*  entry,
        const char*           entryId)
    {
        MamdaOrderBookEntry*  existEntry = 
            (MamdaOrderBookEntry*) wtable_lookup (mEntries, entryId);
        if (existEntry)
            throw MamdaOrderBookDuplicateEntry (existEntry, entry);
        wtable_insert (mEntries, entryId, entry);
    }

    MamdaOrderBookEntry*
    MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::findEntry (
        const char*  entryId,
        bool         mustExist)
    {
        MamdaOrderBookEntry* result =
            (MamdaOrderBookEntry*) wtable_lookup (mEntries, entryId);
        if (mustExist && !result)
            throw MamdaOrderBookMissingEntry (entryId);
        else
            return result;
    }

    void MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::removeEntry (
        const char*  entryId)
    {
        if (entryId)
        {
            wtable_remove (mEntries, entryId);
        }
    }

    static void dumpEntries (
        wtable_t  table, void* data, const char* key, void* closure)
    {
        MamdaOrderBookEntry* entry = (MamdaOrderBookEntry*)data;
        std::ostream& s = *((std::ostream*)closure);
        s << key << ": ";

        if (entry)
        {
            s << "symbol="  << entry->getSymbol()
              << ", price=" << entry->getPrice()
              << ", side="  << (char)entry->getSide()
              << ", id="    << entry->getId()
              << ", size="  << entry->getSize();
        }
        else
        {
          s << "NULL!";
        }
        s << std::endl;
    }

    void MamdaOrderBookEntryManager::MamdaOrderBookEntryManagerImpl::dump(
        std::ostream&  s) const
    {
        s << "MamdaOrderBookEntryManager: dump:\n";
        wtable_for_each(mEntries, dumpEntries, (void*)&s);
    }

}
