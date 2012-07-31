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

#include <string.h>

#include "mama/mamacpp.h"
#include "mama/MamaFieldDescriptor.h"
#include "FieldDescriptorImpl.h"
#include "mamacppinternal.h"

namespace Wombat
{

MamaFieldDescriptor::MamaFieldDescriptor (mamaFieldDescriptor field)
    : mPimpl (new FieldDescriptorImpl(field))
{
}

MamaFieldDescriptor::MamaFieldDescriptor (mama_fid_t            fid,
                                          mamaFieldType         type,
                                          const char*           name)
    : mPimpl (new FieldDescriptorImpl(fid, type, name))
{
}

uint16_t MamaFieldDescriptor::getFid (void) const
{
    return mamaFieldDescriptor_getFid (mPimpl->mField);
}

mamaFieldType MamaFieldDescriptor::getType (void) const
{
    return mPimpl->getType ();
}

const char* MamaFieldDescriptor::getName (void) const
{
    return mamaFieldDescriptor_getName (mPimpl->mField);
}

const char* MamaFieldDescriptor::getTypeName (void) const
{
    return mPimpl->getTypeName ();
}

void MamaFieldDescriptor::setClosure (void* closure)
{
    mPimpl->setClosure (closure);
}

void* MamaFieldDescriptor::getClosure () const
{
    return mPimpl->getClosure ();
}

void MamaFieldDescriptor::setTrackModState (bool on)
{
    mPimpl->setTrackModState (on);
}

bool MamaFieldDescriptor::getTrackModState () const
{
    return mPimpl->mTrackModState;
}

void MamaFieldDescriptor::setPubName (const char* name)
{
    mPimpl->setPubName (name);
}

const char* MamaFieldDescriptor::getPubName () const
{
    return mPimpl->getPubName();
}

MamaFieldDescriptor::~MamaFieldDescriptor () 
{
    if (mPimpl)
    {
        delete mPimpl;
        mPimpl = NULL;
    }
}

/******************************************************************************
 * MamaFieldDescriptor Methods
 */

mamaFieldType FieldDescriptorImpl::getType (void) const
{
    return mamaFieldDescriptor_getType (mField);
}

const char* FieldDescriptorImpl::getName (void) const
{
    return mamaFieldDescriptor_getName (mField);
}

const char* FieldDescriptorImpl::getTypeName (void) const
{
    return mamaFieldDescriptor_getTypeName (mField);
}
    
uint16_t FieldDescriptorImpl::getFid (void) const
{
    return mamaFieldDescriptor_getFid (mField);
}

void FieldDescriptorImpl::setClosure (void* closure)
{
    mClosure = closure;
}

void* FieldDescriptorImpl::getClosure () const
{
    return mClosure;
}

void FieldDescriptorImpl::setTrackModState (bool on)
{
    mTrackModState = on;
}

bool FieldDescriptorImpl::getTrackModState () const
{
    return mTrackModState;
}

void FieldDescriptorImpl::setPubName (const char* name)
{
    if (!name || name[0] == '\0') return;
    delete[] mPubName;
    mPubName = new char[strlen(name)+1];
    strcpy (mPubName, name);
}

const char* FieldDescriptorImpl::getPubName () const
{
    return (mPubName ? mPubName : getName()); 
}


} // namespace Wombat
