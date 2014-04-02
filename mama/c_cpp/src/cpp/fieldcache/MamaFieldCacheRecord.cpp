/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/fieldcache/MamaFieldCacheRecord.cpp#1 $
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

#include <mama/fieldcache/MamaFieldCacheRecord.h>
#include <mama/fieldcache/MamaFieldCacheField.h>
#include <mama/fieldcache/fieldcacherecord.h>

namespace Wombat
{

struct MamaFieldCacheRecord::MamaFieldCacheRecordImpl
{
    MamaFieldCacheRecordImpl()
        : mRecord(NULL)
    {
    }
    ~MamaFieldCacheRecordImpl()
    {
        clear();
    }
    
    void clear();

    mamaFieldCacheRecord mRecord;
    mutable MamaFieldCacheField mField;
};

void MamaFieldCacheRecord::MamaFieldCacheRecordImpl::clear()
{
    // clear reusable container members
    mField.getCValue() = NULL;
    mField.clear();
}


MamaFieldCacheRecord::MamaFieldCacheRecord()
    : mPimpl(new MamaFieldCacheRecordImpl)
{
}

MamaFieldCacheRecord::~MamaFieldCacheRecord()
{
    mamaFieldCacheRecord_destroy(mPimpl->mRecord);
    delete mPimpl;
}

void MamaFieldCacheRecord::create()
{
    if (mPimpl->mRecord)
        return; // Already created

    mamaFieldCacheRecord_create(&mPimpl->mRecord);
}

MamaFieldCacheField& MamaFieldCacheRecord::add(mama_fid_t fid,
                                               mamaFieldType type,
                                               const char* name)
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_add(mPimpl->mRecord, fid, type, name, &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

MamaFieldCacheField& MamaFieldCacheRecord::add(const MamaFieldDescriptor& descriptor)
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_add(mPimpl->mRecord,
                             descriptor.getFid(),
                             descriptor.getType(),
                             descriptor.getName(),
                             &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

const MamaFieldCacheField& MamaFieldCacheRecord::operator[](mama_size_t index) const
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_getField(mPimpl->mRecord, index, &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

MamaFieldCacheField& MamaFieldCacheRecord::operator[](mama_size_t index)
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_getField(mPimpl->mRecord, index, &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

const MamaFieldCacheField& MamaFieldCacheRecord::find(mama_fid_t fid,
                                                      const char* name) const
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_find(mPimpl->mRecord, fid, name, &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

MamaFieldCacheField& MamaFieldCacheRecord::find(mama_fid_t fid,
                                                const char* name)
{
    mamaFieldCacheField cfield = NULL;
    mamaFieldCacheRecord_find(mPimpl->mRecord, fid, name, &cfield);
    mPimpl->mField.getCValue() = cfield;
    return mPimpl->mField;
}

mama_size_t MamaFieldCacheRecord::getSize() const
{
    mama_size_t size = 0;
    mamaFieldCacheRecord_getSize(mPimpl->mRecord, &size);
    return size;
}

void MamaFieldCacheRecord::clear()
{
    mamaFieldCacheRecord_clear(mPimpl->mRecord);
    mPimpl->clear();
}

mamaFieldCacheRecord MamaFieldCacheRecord::getCValue()
{
    return mPimpl->mRecord;
}

const mamaFieldCacheRecord MamaFieldCacheRecord::getCValue() const
{
    return mPimpl->mRecord;
}

} // namespace Wombat
