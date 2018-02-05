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

#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookExceptions.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntryManager.h>
#include <mamda/MamdaOrderBook.h>
#include <cstring>
#include <assert.h>

using std::strcpy;
using std::strlen;

static bool sStrictChecking = false;

namespace Wombat
{

    MamdaOrderBookEntry::MamdaOrderBookEntry()
        : mId          (NULL)
        , mUniqueId    (NULL)
        , mSize        (0)
        , mPriceLevel  (NULL)
        , mManager     (NULL)
        , mSourceDeriv (NULL)
        , mClosure     (NULL)
        , mQuality     (MAMA_QUALITY_OK)
        , mAction      (MAMDA_BOOK_ACTION_ADD)
        , mStatus      (0)
        , mReason      (MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN)
    {
    }

    MamdaOrderBookEntry::MamdaOrderBookEntry(const MamdaOrderBookEntry& copy)
        : mId          (NULL)
        , mUniqueId    (NULL)
        , mSize        (copy.mSize)
        , mTime        (copy.mTime)
        , mPriceLevel  (NULL)
        , mManager     (NULL)
        , mSourceDeriv (copy.mSourceDeriv)
        , mClosure     (copy.mClosure)
        , mQuality     (copy.mQuality)
        , mAction      (copy.mAction)
        , mStatus      (0)
        , mReason      (copy.mReason)
    {
        setId (copy.mId);
        setUniqueId (copy.mUniqueId);
    }

    MamdaOrderBookEntry::MamdaOrderBookEntry(
        const char*                  entryId,
        mama_quantity_t              size,
        Action                       action,
        const MamaDateTime&          entryTime,
        const MamaSourceDerivative*  deriv)
        : mId          (NULL)
        , mUniqueId    (NULL)
        , mSize        (size)
        , mTime        (entryTime)
        , mPriceLevel  (NULL)
        , mManager     (NULL)
        , mSourceDeriv (deriv)
        , mClosure     (NULL)
        , mQuality     (MAMA_QUALITY_OK)
        , mAction      (action)
        , mStatus      (0)
        , mReason      (MamdaOrderBookTypes::MAMDA_BOOK_REASON_MODIFY)
    {
        setId (entryId);
    }

    MamdaOrderBookEntry::~MamdaOrderBookEntry()
    {
        clear();
    }

    MamdaOrderBookEntry& MamdaOrderBookEntry::operator= (
        const MamdaOrderBookEntry&  rhs)
    {
        if (this != &rhs)
        {
            copy(rhs);
        }
        return *this;
    }

    void MamdaOrderBookEntry::clear()
    {
        // First unhook ourselves from our manager using our "unique" ID:
        if (mManager)
        {
            mManager->removeEntry(getUniqueId());
        }

        // Now clear everything
        delete[] mId;
        delete[] mUniqueId;
        mId          = NULL;
        mUniqueId    = NULL;
        mSize        = 0;
        mTime.clear();
        mPriceLevel  = NULL;
        mManager     = NULL;
        mSourceDeriv = NULL;
        mClosure     = NULL;
        mQuality     = MAMA_QUALITY_OK;
        mAction      = MAMDA_BOOK_ACTION_ADD;
        mStatus      = 0;
        mReason      = MamdaOrderBookTypes::MAMDA_BOOK_REASON_UNKNOWN;
    }

    void MamdaOrderBookEntry::copy(const MamdaOrderBookEntry& copy)
    {
        setId (copy.mId);
        setId (copy.mUniqueId);
        mSize        = copy.mSize;
        mTime        = copy.mTime;
        mPriceLevel  = NULL;
        mManager     = NULL;
        mSourceDeriv = copy.mSourceDeriv;
        mClosure     = copy.mClosure;
        mQuality     = copy.mQuality;
        mAction      = copy.mAction;
        mStatus      = copy.mStatus;
        mReason      = copy.mReason;
    }

    void MamdaOrderBookEntry::setId (const char*  id)
    {
        delete[] mId;
        if (id)
        {
            mId = new char[strlen(id)+1];
            strcpy (mId, id);
        }
        else
        {
            mId = NULL;
        }
    }

    void MamdaOrderBookEntry::setUniqueId (const char*  id)
    {
        delete[] mUniqueId;
        if (id)
        {
            mUniqueId = new char[strlen(id)+1];
            strcpy (mUniqueId, id);
        }
        else
        {
            mUniqueId = NULL;
        }
    }

    void MamdaOrderBookEntry::setSize (mama_quantity_t  size)
    {
        mSize = size;
    }

    void MamdaOrderBookEntry::setAction (Action  action)
    {
        mAction = action;
    }

    void MamdaOrderBookEntry::setReason (MamdaOrderBookTypes::Reason  reason)
    {
        mReason = reason;
    }

    void MamdaOrderBookEntry::setTime (const MamaDateTime&  time)
    {
        mTime = time;
    }

    void MamdaOrderBookEntry::setStatus (mama_u16_t  status)
    {
        mStatus = status;
    }

    void MamdaOrderBookEntry::setDetails (const MamdaOrderBookEntry& copy)
    {
        mSize   = copy.mSize;
        mTime   = copy.mTime;
        mStatus = copy.mStatus;
    }

    const char* MamdaOrderBookEntry::getId () const
    {
        return mId;
    }

    const char* MamdaOrderBookEntry::getUniqueId () const
    {
        return mUniqueId ? mUniqueId : mId;
    }

    mama_quantity_t MamdaOrderBookEntry::getSize () const
    {
        return mSize;
    }

    MamdaOrderBookEntry::Action MamdaOrderBookEntry::getAction () const
    {
        return mAction;
    }

    MamdaOrderBookTypes::Reason MamdaOrderBookEntry::getReason () const
    {
        return mReason;
    }

    const MamaDateTime& MamdaOrderBookEntry::getTime () const
    {
        return mTime;
    }

    mama_u16_t MamdaOrderBookEntry::getStatus () const
    {
        return mStatus;
    }

    double  MamdaOrderBookEntry::getPrice () const
    {
        if (mPriceLevel)
            return mPriceLevel->getPrice();
        else
            throw MamdaOrderBookInvalidEntry (this,
                                              "MamdaOrderBookEntry::getPrice()");
    }

    MamaPrice  MamdaOrderBookEntry::getMamaPrice () const
    {
        if (mPriceLevel)
            return mPriceLevel->getMamaPrice();
        else
            throw MamdaOrderBookInvalidEntry (this,
                                              "MamdaOrderBookEntry::getMamaPrice()");
    }

    MamdaOrderBookPriceLevel::Side  MamdaOrderBookEntry::getSide () const
    {
        if (mPriceLevel)
            return mPriceLevel->getSide();
        else
            throw MamdaOrderBookInvalidEntry (this,
                                              "MamdaOrderBookEntry::getSide()");
    }

    mama_u32_t  MamdaOrderBookEntry::getPosition (mama_u32_t  maxPos) const
    {
        if (!mPriceLevel)
            throw MamdaOrderBookInvalidEntry (this,
                                "MamdaOrderBookEntry::getPosition() (no level)");
        MamdaOrderBook*  book = getOrderBook();
        assert (book != NULL);
        mama_u32_t  pos = 0;
        if (MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID == mPriceLevel->getSide())
        {
            MamdaOrderBook::bidIterator end = book->bidEnd();
            MamdaOrderBook::bidIterator i   = book->bidBegin();
            for (; i != end; ++i)
            {
                const MamdaOrderBookPriceLevel* level = *i;
                if (level != mPriceLevel)
                {
                    // Not the level containing this entry
                    if (level->getPrice() < mPriceLevel->getPrice())
                        throw MamdaOrderBookInvalidEntry (this,
                                 "MamdaOrderBookEntry::getPosition() (bid)");
                    pos += level->getNumEntries();
                }
                else
                {
                    // Count the entries before this entry within the level
                    MamdaOrderBookPriceLevel::iterator plEnd  = level->end();
                    MamdaOrderBookPriceLevel::iterator plIter = level->begin();
                    for (; plIter != plEnd; ++ plIter)
                    {
                        if (*plIter == this)
                            return pos;
                        pos++;
                    }
                }
                if ((maxPos > 0) && (pos > maxPos))
                    return maxPos;
            }
        }
        else
        {
            MamdaOrderBook::askIterator end = book->askEnd();
            MamdaOrderBook::askIterator i   = book->askBegin();
            for (; i != end; ++i)
            {
                const MamdaOrderBookPriceLevel* level = *i;
                if (level != mPriceLevel)
                {
                    // Not the level containing this entry
                    if (level->getPrice() > mPriceLevel->getPrice())
                        throw MamdaOrderBookInvalidEntry (this,
                                 "MamdaOrderBookEntry::getPosition() (ask)");
                    pos += level->getNumEntries();
                }
                else
                {
                    // Count the entries before this entry within the level
                    MamdaOrderBookPriceLevel::iterator plEnd  = level->end();
                    MamdaOrderBookPriceLevel::iterator plIter = level->begin();
                    for (; plIter != plEnd; ++ plIter)
                    {
                        if (*plIter == this)
                            return pos;
                        pos++;
                    }
                }
                if ((maxPos > 0) && (pos > maxPos))
                    return maxPos;
            }
        }
        throw MamdaOrderBookInvalidEntry (this,
            "MamdaOrderBookEntry::getPosition() (not found)");
    }

    bool MamdaOrderBookEntry::operator== (
        const MamdaOrderBookEntry& rhs) const
    {
        return equalId(rhs.mId) && (mSize == rhs.mSize) &&
            (mAction == rhs.mAction) && (mTime == rhs.mTime);
    }

    void MamdaOrderBookEntry::setPriceLevel (MamdaOrderBookPriceLevel*  level)
    {
        mPriceLevel = level;
    }

    MamdaOrderBookPriceLevel*  MamdaOrderBookEntry::getPriceLevel () const
    {
        return mPriceLevel;
    }

    MamdaOrderBook*  MamdaOrderBookEntry::getOrderBook () const
    {
        if (mPriceLevel)
            return mPriceLevel->getOrderBook();
        else
            return NULL;
    }

    void MamdaOrderBookEntry::setManager (MamdaOrderBookEntryManager*  manager)
    {
        mManager = manager;
    }

    MamdaOrderBookEntryManager*  MamdaOrderBookEntry::getManager () const
    {
        return mManager;
    }

    const char* MamdaOrderBookEntry::getSymbol() const
    {
        if (mPriceLevel)
            return mPriceLevel->getSymbol();
        else
            return NULL;
    }

    void  MamdaOrderBookEntry::setSourceDerivative (
        const MamaSourceDerivative*  deriv)
    {
        mSourceDeriv = deriv;
    }

    const MamaSourceDerivative*  MamdaOrderBookEntry::getSourceDerivative () const
    {
        return mSourceDeriv;
    }

    const MamaSource*  MamdaOrderBookEntry::getSource () const
    {
        return mSourceDeriv ? mSourceDeriv->getBaseSource() : NULL;
    }

    mamaSourceState  MamdaOrderBookEntry::getSourceState () const
    {
        return mSourceDeriv ? mSourceDeriv->getState() : MAMA_SOURCE_STATE_OK;
    }

    bool  MamdaOrderBookEntry::getCheckSourceState () const
    {
        MamdaOrderBook*  book = getOrderBook();
        return book ? book->getCheckSourceState() : false;
    }

    void  MamdaOrderBookEntry::setQuality (mamaQuality  quality)
    {
        mQuality = quality;
    }

    mamaQuality  MamdaOrderBookEntry::getQuality () const
    {
        if ((mQuality == MAMA_QUALITY_OK) && mSourceDeriv)
            return mSourceDeriv->getQuality();
        else
            return mQuality;
    }

    bool  MamdaOrderBookEntry::isVisible () const
    {
        return getSourceState() == MAMA_SOURCE_STATE_OK;
    }

    void MamdaOrderBookEntry::setClosure (void*  closure)
    {
        mClosure = closure;
    }

    void*  MamdaOrderBookEntry::getClosure () const
    {
        return mClosure;
    }


    void MamdaOrderBookEntry::assertEqual (
        const MamdaOrderBookEntry& rhs) const
    {
        const char* error = NULL;
        if (!equalId(rhs.mId))
            error = "entry IDs not equal";
        if (mSize != rhs.mSize)
            error = "entry size not equal";
        if (mAction != rhs.mAction)
            error = "entry action not equal";
        if (mTime != rhs.mTime)
            error = "entry time not equal";
        if (error)
        {
            char msg[1024];
            if (mPriceLevel)
                snprintf (msg, 1024,
                          "%s (Price:%g/%g, Side:%c/%c, ID:%s/%s, Size:%g/%g)",
                          error,
                          getPrice(),     rhs.getPrice(),
                          getSide(),      rhs.getSide(),
                          mId,           rhs.mId,
                          (double)mSize, (double)rhs.mSize);
            else
                snprintf (msg, 1024,
                          "%s (ID:%s/%s, Size:%g/%g)",
                          error,
                          mId,           rhs.mId,
                          (double)mSize, (double)rhs.mSize);
            throw MamdaOrderBookException(msg);
        }
    }

    bool MamdaOrderBookEntry::equalId (const char*  id) const
    {
        return mId && id && (strcmp(mId, id) == 0);
    }

    void MamdaOrderBookEntry::setStrictChecking (bool strict)
    {
        sStrictChecking = strict;
    }

}
