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

#ifndef MamdaOrderBookExceptionsH
#define MamdaOrderBookExceptionsH

#include <stdexcept>
#include <string>
#include <mamda/MamdaOptionalConfig.h>

using std::invalid_argument;
using std::string;

namespace Wombat
{

class MamdaOrderBookEntry;

/**
 * MamdaOrderBookException is generated when an inconsistent state is
 * detected in an order book.  Such a condition would usually indicate
 * a potential problem with the publisher's state of the order book or
 * undetected lost messages over the middleware.
 */

class MAMDAOPTExpDLL MamdaOrderBookException : public invalid_argument
{
public:
    MamdaOrderBookException (const string& msg)
        : invalid_argument (msg)
        {}
    virtual ~MamdaOrderBookException() throw () {}
};


/**
 * MamdaOrderBookDuplicateEntry is generated when an existing entry is
 * unexpectedly encountered when updating a MamdaOrderBook or
 * MamdaOrderBookEntryManager.
 */

class MAMDAOPTExpDLL MamdaOrderBookDuplicateEntry : public MamdaOrderBookException
{
public:
    MamdaOrderBookDuplicateEntry (MamdaOrderBookEntry*  origEntry,
                                  MamdaOrderBookEntry*  dupEntry)
        : MamdaOrderBookException ("duplicate entry ID")
        , mOrigEntry (origEntry)
        , mDupEntry  (dupEntry)
        {}
    MamdaOrderBookEntry*  getOrigEntry() { return mOrigEntry; }
    MamdaOrderBookEntry*  getDupEntry()  { return mDupEntry;  }

private:
    MamdaOrderBookEntry*  mOrigEntry;
    MamdaOrderBookEntry*  mDupEntry;
};


/**
 * MamdaOrderBookMissingEntry is generated when an expected entry is
 * not found when updating a MamdaOrderBook or MamdaOrderBookEntryManager.
 */

class MAMDAOPTExpDLL MamdaOrderBookMissingEntry : public MamdaOrderBookException
{
public:
    MamdaOrderBookMissingEntry (const string&  missingEntryId)
        : MamdaOrderBookException ("missing entry ID")
        , mMissingId (missingEntryId)
        {}
    virtual ~MamdaOrderBookMissingEntry() throw () {}
    const char*  getMissingEntryId() { return mMissingId.c_str(); }

private:
    string mMissingId;
};


/**
 * MamdaOrderBookInvalidEntry is generated when an entry is applied as
 * an update to or deletion from an order book but the internal
 * references to price level and/or order book do not exist.
 */

class MAMDAOPTExpDLL MamdaOrderBookInvalidEntry : public MamdaOrderBookException
{
public:
    MamdaOrderBookInvalidEntry (const MamdaOrderBookEntry*  entry,
                                const string&               msg)
        : MamdaOrderBookException (msg)
        , mEntry (entry)
        {}
    const MamdaOrderBookEntry*  getInvalidEntry() { return mEntry; }

private:
    const MamdaOrderBookEntry*  mEntry;
};


} // namespace

#endif // MamdaOrderBookExceptionsH
