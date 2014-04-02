/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/fieldcache/MamaFieldCache.cpp#1 $
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

#include <mama/fieldcache/MamaFieldCache.h>
#include <mama/fieldcache/MamaFieldCacheRecord.h>
#include <mama/fieldcache/MamaFieldCacheField.h>
#include <mama/MamaMsg.h>
#include <mama/MamaDictionary.h>
#include <mama/fieldcache/fieldcacheiterator.h>

namespace Wombat
{

struct MamaFieldCache::MamaFieldCacheImpl
{
    MamaFieldCacheImpl()
        : mCache(NULL)
    {
    }
    ~MamaFieldCacheImpl()
    {
        clear();
    }
    
    void clear();

    mamaFieldCache mCache;
    mutable MamaFieldCacheField mField;
};

void MamaFieldCache::MamaFieldCacheImpl::clear()
{
    // clear reusable container members
    mField.getCValue() = NULL;
    mField.clear();
}


MamaFieldCache::MamaFieldCache()
    : mPimpl(new MamaFieldCacheImpl)
{
}

MamaFieldCache::~MamaFieldCache()
{
    mamaFieldCache_destroy(mPimpl->mCache);
    delete mPimpl;
}

void MamaFieldCache::create()
{
    if (mPimpl->mCache)
        return; // Already created
    mamaFieldCache_create(&mPimpl->mCache);
}

void MamaFieldCache::clear()
{
    mamaFieldCache_clear(mPimpl->mCache);
    mPimpl->clear();
}

mama_size_t MamaFieldCache::getSize() const
{
    mama_size_t size = 0;
    mamaFieldCache_getSize(mPimpl->mCache, &size);
    return size;
}
    
void MamaFieldCache::setTrackModified(bool value)
{
    mamaFieldCache_setTrackModified(mPimpl->mCache, (mama_bool_t)value);
}

bool MamaFieldCache::getTrackModified() const
{
    mama_bool_t value = 0;
    mamaFieldCache_getTrackModified(mPimpl->mCache, &value);
    return value;
}

void MamaFieldCache::setUseFieldNames(bool value)
{
    mamaFieldCache_setUseFieldNames(mPimpl->mCache, (mama_bool_t)value);
}

bool MamaFieldCache::getUseFieldNames() const
{
    mama_bool_t value = 0;
    mamaFieldCache_getUseFieldNames(mPimpl->mCache, &value);
    return value;
}

void MamaFieldCache::setUseLock(bool value)
{
    mamaFieldCache_setUseLock(mPimpl->mCache, (mama_bool_t)value);
}

bool MamaFieldCache::getUseLock() const
{
    mama_bool_t value = 0;
    mamaFieldCache_getUseLock(mPimpl->mCache, &value);
    return value;
}

void MamaFieldCache::lock()
{
    mamaFieldCache_lock(mPimpl->mCache);
}
    
void MamaFieldCache::unlock()
{
    mamaFieldCache_unlock(mPimpl->mCache);
}

const MamaFieldCacheField* MamaFieldCache::find(mama_fid_t fid,
                                                const char* name) const
{
    mamaFieldCacheField field = NULL;
    mamaFieldCache_find(mPimpl->mCache, fid, name, &field);
    if (!field)
    {
        return NULL;
    }
    mPimpl->mField.getCValue() = field;
    return &mPimpl->mField;
}

MamaFieldCacheField* MamaFieldCache::find(mama_fid_t fid,
                                          const char* name)
{
    mamaFieldCacheField field = NULL;
    mamaFieldCache_find(mPimpl->mCache, fid, name, &field);
    if (!field)
    {
        return NULL;
    }
    mPimpl->mField.getCValue() = field;
    return &mPimpl->mField;
}

void MamaFieldCache::setModified(MamaFieldCacheField& field)
{
    mamaFieldCache_setModified(mPimpl->mCache, field.getCValue());
}

void MamaFieldCache::apply(const MamaFieldCacheField& field)
{
    mamaFieldCache_applyField(mPimpl->mCache, field.getCValue());
}

void MamaFieldCache::apply(const MamaMsg& msg, const MamaDictionary* dict)
{
    mamaFieldCache_applyMessage(mPimpl->mCache,
            msg.getUnderlyingMsg(),
            dict ? dict->getDictC() : NULL);
}

void MamaFieldCache::apply(const MamaFieldCacheRecord& record)
{
    mamaFieldCache_applyRecord(mPimpl->mCache, record.getCValue());
}

void MamaFieldCache::getFullMessage(MamaMsg& msg)
{
    mamaFieldCache_getFullMessage(mPimpl->mCache, msg.getUnderlyingMsg());
}

void MamaFieldCache::getDeltaMessage(MamaMsg& msg)
{
    mamaFieldCache_getDeltaMessage(mPimpl->mCache, msg.getUnderlyingMsg());
}

void MamaFieldCache::clearModifiedFields()
{
    mamaFieldCache_clearModifiedFields(mPimpl->mCache);
}

MamaFieldCache::iterator MamaFieldCache::begin()
{
    return iterator(mPimpl->mCache);
}

MamaFieldCache::const_iterator MamaFieldCache::begin() const
{
    return const_iterator(mPimpl->mCache);
}

MamaFieldCache::iterator MamaFieldCache::end()
{
    return iterator(NULL);
}

MamaFieldCache::const_iterator MamaFieldCache::end() const
{
    return const_iterator(NULL);
}


MamaFieldCache::MamaFieldCacheIterator::MamaFieldCacheIterator(mamaFieldCache fieldCache)
    : mIterator(NULL)
{
    mamaFieldCacheIterator_create(&mIterator, fieldCache);
    if (mamaFieldCacheIterator_hasNext(mIterator))
    {
        ++(*this); // Set to the begin value
    }
}

MamaFieldCache::MamaFieldCacheIterator::MamaFieldCacheIterator(const MamaFieldCacheIterator& rhs)
    : mIterator(rhs.mIterator)
{
    const_cast<MamaFieldCacheIterator&>(rhs).mIterator = NULL;
    mField.getCValue() = rhs.mField.getCValue();
}

MamaFieldCache::MamaFieldCacheIterator::~MamaFieldCacheIterator ()
{
    mamaFieldCacheIterator_destroy(mIterator);
    // This is only a container - it is not responsible for destroying the real C field
    mField.getCValue() = NULL;
}

MamaFieldCache::MamaFieldCacheIterator&
MamaFieldCache::MamaFieldCacheIterator::operator=(const MamaFieldCacheIterator& rhs)
{
    if (this != &rhs)
    {
        if (mIterator)
        {
            mamaFieldCacheIterator_destroy(mIterator);
        }
        mIterator = rhs.mIterator;
        const_cast<MamaFieldCacheIterator&>(rhs).mIterator = NULL;
        mField.getCValue() = rhs.mField.getCValue();
    }
    return *this;
}

bool MamaFieldCache::MamaFieldCacheIterator::operator==(const MamaFieldCacheIterator& rhs) const
{
    return mField == rhs.mField;
}
bool MamaFieldCache::MamaFieldCacheIterator::operator!=(const MamaFieldCacheIterator& rhs) const
{
    return mField != rhs.mField;
}

MamaFieldCache::MamaFieldCacheIterator&
MamaFieldCache::MamaFieldCacheIterator::operator++()
{
    const_cast<MamaFieldCacheField&>(mField).getCValue() = mamaFieldCacheIterator_next(mIterator);
    return *this;
}


MamaFieldCache::iterator::iterator(mamaFieldCache fieldCache)
    : MamaFieldCacheIterator (fieldCache)
{
}

MamaFieldCache::iterator::reference MamaFieldCache::iterator::operator*()
{
    return mField;
}

MamaFieldCache::iterator::pointer MamaFieldCache::iterator::operator->()
{
    return &mField;
}

MamaFieldCache::const_iterator::const_iterator(mamaFieldCache fieldCache)
    : MamaFieldCacheIterator (fieldCache)
{
}
    
MamaFieldCache::const_iterator::const_iterator(const MamaFieldCache::iterator& it)
    : MamaFieldCacheIterator(it)
{
}

MamaFieldCache::const_iterator::reference MamaFieldCache::const_iterator::operator*()
{
    return mField;
}

MamaFieldCache::const_iterator::pointer MamaFieldCache::const_iterator::operator->()
{
    return &mField;
}

} // namespace Wombat
