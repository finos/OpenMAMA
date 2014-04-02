/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/mama/fieldcache/MamaFieldCacheField.h#1 $
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

#ifndef MAMA_FIELD_CACHE_FIELD_CPP_H__
#define MAMA_FIELD_CACHE_FIELD_CPP_H__

#include <mama/mamacpp.h>
#include <mama/fieldcache/fieldcachetypes.h>
#include <mama/fieldcache/fieldcachefield.h>
#include <cstring>

namespace Wombat 
{

    class MamaFieldDescriptor;

/**
 * The <code>MamaFieldCacheField</code> class is used to store the information on
 * the type of a field and its value(s). This class does not provide methods to set
 * and get the value (check the specific types of fields such as
 * <code>MamaFieldCacheFieldBool<code/>).
 */
class MAMACPPExpDLL MamaFieldCacheField
{
public:
    /**
     * Constructor.
     * Use <code>create</code> to create the actual field.
     */
    MamaFieldCacheField();
    /**
     * Destructor.
     * This is called automatically when an object is destroyed. This method will
     * clean all the memory allocated.
     */
    virtual ~MamaFieldCacheField();
    
    /**
     * Copy constructor.
     * 
     * @param 
     */
    MamaFieldCacheField(const MamaFieldCacheField& rhs);
    
    /**
     * Copy rhs into the current object.
     * 
     * @param rhs The source MamaFielCacheField.
     * @return The MamaFieldCacheField itself.
     */
    MamaFieldCacheField& operator=(const MamaFieldCacheField& rhs);

    /**
     * Return if the current field is equal to the specified field.
     * 
     * @param rhs The other field to check equality.
     * @return Whether the two fields are equal.
     */
    bool operator==(const MamaFieldCacheField& rhs) const;
    /**
     * Return if the current field is different from the specified field.
     * 
     * @param rhs The other field to check difference.
     * @return Whether the two fields are different.
     */
    bool operator!=(const MamaFieldCacheField& rhs) const;

    /**
     * Create a MamaFieldCacheField.
     * 
     * @param descriptor Descriptor of the field to create.
     */
    virtual void create(const MamaFieldDescriptor& descriptor);
    /**
     * Create a MamaFieldCacheField.
     * 
     * @param fid Field id of the field to create.
     * @param type Field type of the field to create.
     * @param name Name of the field to create.
     */
    virtual void create(mama_fid_t fid, mamaFieldType type, const char* name = NULL);
    
    /**
     * Clear the <code>MamaFieldCacheField</code>.
     * The field is ready to be used, as long as create has been called once.
     */
    virtual void clear();

    /**
     * Return the field id.
     * If the field is not valid, then 0 is returned.
     * 
     * @return The field id of the field.
     */
    virtual mama_fid_t getFid() const;
    
    /**
     * Return the field type.
     * If the field is not valid, then MAMA_FIELD_TYPE_UNKNOWN is returned.
     * 
     * @return The field type of the field.
     */
    virtual mamaFieldType getType() const;
    
    /**
     * Return the name of the field.
     * If the name is not set, then NULL is returned.
     * @return 
     */
    const char* getName() const;
    
    /**
     * Set the descriptor of the field.
     * 
     * @param descriptor Pointer to the descriptor of the field.
     */
    void setDescriptor(const MamaFieldDescriptor* descriptor);
    
    /**
     * Return the descriptor of the field.
     * If the descriptor is not set, NULL is returned.
     * 
     * @return Pointer to the descriptor of the field.
     */
    const MamaFieldDescriptor* getDescriptor() const;
    
    /**
     * Return whether the field has been modified or not.
     * 
     * @return Whether the field is modified.
     */
    virtual bool isModified() const;

    /**
     * Set the publish flag of the field.
     * If set to false, the field is never published.
     * 
     * @param value The publish flag.
     */
    virtual void setPublish(bool value = true);

    /**
     * Return the always publish flag of the field.
     * 
     * @return The publish flag.
     */
    virtual bool getPublish() const;

    /**
     * Set the checkModifed flag of a field.
     * If set to true, the field will be checked for modification before publishing.
     * 
     * @param value The checkModified flag.
     */
    virtual void setCheckModified(bool value = true);

    /**
     * Return the checkModified flag of the field.
     * 
     * @return The check modified flag.
     */
    virtual bool getCheckModified() const;

    /**
     * Return if the field is a vector (otherwise is a single-value field).
     * 
     * @return If the field is a vector field.
     */
    virtual bool isVector() const;

    mamaFieldCacheField& getCValue();
    const mamaFieldCacheField& getCValue() const;

private:
    struct MamaFieldCacheFieldImpl;
    MamaFieldCacheFieldImpl* mPimpl;
};


/**
 * Base type for fields
 */
class MAMACPPExpDLL MamaFieldCacheFieldBase
{
protected:
    MamaFieldCacheFieldBase() {}

public:
    virtual ~MamaFieldCacheFieldBase() {}

private: // Cannot copy MamaFieldCacheField objects
    MamaFieldCacheFieldBase(const MamaFieldCacheFieldBase& field);
    MamaFieldCacheFieldBase& operator=(const MamaFieldCacheFieldBase& field);
};

} // namespace Wombat

#endif // MAMA_FIELD_CACHE_FIELD_CPP_H__
