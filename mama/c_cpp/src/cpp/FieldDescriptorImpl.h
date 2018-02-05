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

#ifndef FIELD_DESCRIPTOR_IMPL_H__
#define FIELD_DESCRIPTOR_IMPL_H__
namespace Wombat
{
/******************************************************************************
 * MamaFieldDescriptor Implementation
 */
class FieldDescriptorImpl 
{
public:

    FieldDescriptorImpl (mamaFieldDescriptor field) : 
        mTrackModState (false),
        mField          (field),
        mClosure       (NULL),
        mPubName       (NULL),
        mIsLocalCopy   (false)
    {}

    FieldDescriptorImpl (mama_fid_t            fid,
                         mamaFieldType         type,
                         const char*           name)
        : mTrackModState (false)
        , mClosure       (NULL)
        , mPubName       (NULL)
        , mIsLocalCopy   (true)
    {
        mField = NULL;
        mamaFieldDescriptor_create (&mField, fid, type, name);
    }
    
    virtual ~FieldDescriptorImpl (void) 
    { 
        delete[] mPubName;
        if (mIsLocalCopy)
            mamaFieldDescriptor_destroy (mField);
    }
    
    uint16_t getFid (void) const;

    mamaFieldType  getType          (void) const;
    const char*    getName          (void) const;
    const char*    getTypeName      (void) const;
    void           setClosure       (void* closure);
    void*          getClosure       (void) const;
    void           setTrackModState (bool on);
    bool           getTrackModState (void) const;
    void           setPubName       (const char* name);
    const char*    getPubName       () const;
    bool  							mTrackModState;
    mamaFieldDescriptor 			mField;

private:
    void* 							mClosure;
    char* 							mPubName;
    bool  							mIsLocalCopy;
};
} // namespace Wombat
#endif // FIELD_DESCRIPTOR_IMPL_H__
