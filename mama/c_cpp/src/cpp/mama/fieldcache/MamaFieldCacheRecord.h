/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/mama/fieldcache/MamaFieldCacheRecord.h#1 $
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

#ifndef MAMA_FIELD_CACHE_RECORD_CPP_H__
#define MAMA_FIELD_CACHE_RECORD_CPP_H__

#include <mama/mamacpp.h>
#include <mama/fieldcache/fieldcachetypes.h>
#include <string>

namespace Wombat 
{

    class MamaFieldCacheField;

/**
 * The <code>MamaFieldCacheRecord</code> class is a collection of unordered
 * <code>MamaFieldCacheField</code> elements.
 */
class MAMACPPExpDLL MamaFieldCacheRecord
{
public:
    /**
     * Constructor.
     * Use <code>create</code> to create the actual cache.
     */
    MamaFieldCacheRecord();
    /**
     * Destructor.
     * This is called automatically when an object is destroyed. This method will
     * clean all the memory allocated.
     */
    virtual ~MamaFieldCacheRecord();
    
    /**
     * Creates a <code>MamaFieldCacheRecord</code>.
     */
    virtual void create();
    
    /**
     * Clear the <code>MamaFieldCacheRecord</code>.
     * No fields will be present in the record after this operation. The record is
     * ready to be used, as long as create has been called once.
     */
    virtual void clear();
    
    /**
     * Add and creates a <code>MamaFieldCacheField</code> to the record.
     * The new field is then returned.
     * 
     * @param fid Field id of the field to create.
     * @param type Field type of the field to create.
     * @param name Name of the field to create.
     * @return The new field created.
     */
    virtual MamaFieldCacheField& add(mama_fid_t fid,
                                     mamaFieldType type,
                                     const char* name = NULL);
    
    /**
     * Add and creates a <code>MamaFieldCacheField</code> to the record.
     * The new field is then returned.
     * 
     * @param descriptor Descriptor of the field to create.
     * @return The new field created.
     */
    virtual MamaFieldCacheField& add(const MamaFieldDescriptor& descriptor);

    
    virtual const MamaFieldCacheField& operator[](mama_size_t index) const;
    virtual MamaFieldCacheField& operator[](mama_size_t index);
    
    /**
     * Find a <code>MamaFieldCacheField</code> in the record.
     * 
     * @param fid Field id of the field to look up.
     * @param name Field name of the field to look up.
     * @return The field in the record.
     */
    virtual const MamaFieldCacheField& find(mama_fid_t fid,
                                            const char* name = NULL) const;
    
    /**
     * Find a <code>MamaFieldCacheField</code> in the record.
     * 
     * @param fid Field id of the field to look up.
     * @param name Field name of the field to look up.
     * @return The field in the record.
     */
    virtual MamaFieldCacheField& find(mama_fid_t fid, const char* name = NULL);
    /**
     * Return the number of fields present in the record.
     * 
     * @return Size of the record.
     */
    virtual mama_size_t getSize() const;
    
    virtual mamaFieldCacheRecord getCValue();
    virtual const mamaFieldCacheRecord getCValue() const;
    
private:
    MamaFieldCacheRecord(const MamaFieldCacheRecord&);
    MamaFieldCacheRecord& operator=(const MamaFieldCacheRecord&);

private:
    struct MamaFieldCacheRecordImpl;
    MamaFieldCacheRecordImpl* mPimpl;
};

} // namespace Wombat

#endif
