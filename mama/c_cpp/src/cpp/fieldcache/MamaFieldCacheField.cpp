/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/fieldcache/MamaFieldCacheField.cpp#1 $
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

#include <mama/fieldcache/MamaFieldCacheField.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <FieldDescriptorImpl.h>

namespace Wombat
{

struct MamaFieldCacheField::MamaFieldCacheFieldImpl
{
    MamaFieldCacheFieldImpl();
    ~MamaFieldCacheFieldImpl();
    
    void clear();
    
    const mamaFieldDescriptor* getFieldDescriptorC() const;

    mamaFieldCacheField mField;
    MamaFieldDescriptor mDescriptor;
};

MamaFieldCacheField::MamaFieldCacheFieldImpl::MamaFieldCacheFieldImpl()
    : mField(NULL)
    , mDescriptor(NULL)
{
}

MamaFieldCacheField::MamaFieldCacheFieldImpl::~MamaFieldCacheFieldImpl()
{
    clear();
}

void MamaFieldCacheField::MamaFieldCacheFieldImpl::clear()
{
    // clear reusable container members
    mDescriptor.mPimpl->mField = NULL;
}

const mamaFieldDescriptor*
MamaFieldCacheField::MamaFieldCacheFieldImpl::getFieldDescriptorC() const
{
    return &mDescriptor.mPimpl->mField;
}


MamaFieldCacheField::MamaFieldCacheField()
    : mPimpl(new MamaFieldCacheFieldImpl)
{
}

MamaFieldCacheField::~MamaFieldCacheField()
{
    mamaFieldCacheField_destroy(mPimpl->mField);
    delete mPimpl;
}

MamaFieldCacheField::MamaFieldCacheField(const MamaFieldCacheField& rhs)
    : mPimpl(new MamaFieldCacheFieldImpl)
{
    create(rhs.getFid(), rhs.getType(), rhs.getName());
    mamaFieldCacheField_copy(rhs.mPimpl->mField, mPimpl->mField);
}

MamaFieldCacheField& MamaFieldCacheField::operator=(const MamaFieldCacheField& rhs)
{
    if (this != &rhs)
    {
        mamaFieldCacheField_copy(rhs.mPimpl->mField, mPimpl->mField);
    }
    return *this;
}

bool MamaFieldCacheField::operator==(const MamaFieldCacheField& rhs) const
{
    return mPimpl->mField == rhs.mPimpl->mField;
}

bool MamaFieldCacheField::operator!=(const MamaFieldCacheField& rhs) const
{
    return mPimpl->mField != rhs.mPimpl->mField;
}

void MamaFieldCacheField::create(const MamaFieldDescriptor& descriptor)
{
    if (mPimpl->mField)
        return; // Already created

    mamaFieldCacheField_create(&mPimpl->mField,
            descriptor.getFid(),
            descriptor.getType(),
            descriptor.getName());
}

void MamaFieldCacheField::create(
        mama_fid_t fid,
        mamaFieldType type,
        const char* name)
{
    if (mPimpl->mField)
        return; // Already created

    mamaFieldCacheField_create(&mPimpl->mField, fid, type, name);
}

void MamaFieldCacheField::clear()
{
    mPimpl->clear();
}

mama_fid_t MamaFieldCacheField::getFid() const
{
    mama_fid_t value = 0;
    mamaFieldCacheField_getFid(mPimpl->mField, &value);
    return value;
}
    
mamaFieldType MamaFieldCacheField::getType() const
{
    mamaFieldType value = MAMA_FIELD_TYPE_UNKNOWN;
    mamaFieldCacheField_getType(mPimpl->mField, &value);
    return value;
}

const char* MamaFieldCacheField::getName() const
{
    const char* cname = NULL;
    mamaFieldCacheField_getName(mPimpl->mField, &cname);
    return cname;
}

void MamaFieldCacheField::setDescriptor(const MamaFieldDescriptor* descriptor)
{
    mamaFieldCacheField_setDescriptor(
            mPimpl->mField,
            descriptor ? descriptor->mPimpl->mField : NULL);
}

const MamaFieldDescriptor* MamaFieldCacheField::getDescriptor() const
{
    const mamaFieldDescriptor descriptor = NULL;
    mamaFieldCacheField_getDescriptor(mPimpl->mField, &descriptor);
    mPimpl->mDescriptor.mPimpl->mField = descriptor;
    return descriptor ? &mPimpl->mDescriptor : NULL;
}

bool MamaFieldCacheField::isModified() const
{
    mama_bool_t value = 0;
    mamaFieldCacheField_isModified(mPimpl->mField, &value);
    return value;
}

void MamaFieldCacheField::setPublish(bool value)
{
    mamaFieldCacheField_setPublish(mPimpl->mField, value);
}
    
bool MamaFieldCacheField::getPublish() const
{
    mama_bool_t value = 0;
    mamaFieldCacheField_getPublish(mPimpl->mField, &value);
    return value;
}

void MamaFieldCacheField::setCheckModified(bool value)
{
    mamaFieldCacheField_setCheckModified(mPimpl->mField, value);
}
    
bool MamaFieldCacheField::getCheckModified() const
{
    mama_bool_t value = 0;
    mamaFieldCacheField_getCheckModified(mPimpl->mField, &value);
    return value;
}

bool MamaFieldCacheField::isVector() const
{
    switch (getType())
    {
        case MAMA_FIELD_TYPE_VECTOR_I8:
        case MAMA_FIELD_TYPE_VECTOR_U8:
        case MAMA_FIELD_TYPE_VECTOR_I16:
        case MAMA_FIELD_TYPE_VECTOR_U16:
        case MAMA_FIELD_TYPE_VECTOR_I32:
        case MAMA_FIELD_TYPE_VECTOR_U32:
        case MAMA_FIELD_TYPE_VECTOR_I64:
        case MAMA_FIELD_TYPE_VECTOR_U64:
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        case MAMA_FIELD_TYPE_VECTOR_TIME:
            return true;
        default:
            break;
    }
    return false;
}

mamaFieldCacheField& MamaFieldCacheField::getCValue()
{
    return mPimpl->mField;
}

const mamaFieldCacheField& MamaFieldCacheField::getCValue() const
{
    return mPimpl->mField;
}

} // namespace Wombat
