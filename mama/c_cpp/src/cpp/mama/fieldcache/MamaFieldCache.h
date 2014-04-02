/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/cpp/mama/fieldcache/MamaFieldCache.h#1 $
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

#ifndef MAMA_FIELD_CACHE_CPP_H__
#define MAMA_FIELD_CACHE_CPP_H__

#include <mama/mamacpp.h>
#include <mama/fieldcache/MamaFieldCacheField.h>
#include <mama/fieldcache/fieldcache.h>

namespace Wombat 
{
    class MamaMsg;
    class MamaDictionary;
    class MamaFieldCacheRecord;
    
/**
 * The <code>MamaFieldCache</code> class is a collection of unordered
 * <code>MamaFieldCacheField</code> elements. This class provides methods for creating
 * and finding fields, applying changes to the fields in the cache and populating
 * a MamaMsg with the fields in cache.
 */
class MAMACPPExpDLL MamaFieldCache
{
public:
    /**
     * The <code>MamaFieldCacheIterator</code> class provides a way to iterate over
     * the fields contained in the cache.
     * Two iterators are provided: one const and one non-const.
     * To use it use the standard C++ notation <code>MamaFieldCache::iterator</code>.
     */
    class MAMACPPExpDLL MamaFieldCacheIterator
    {
    protected:
        MamaFieldCacheIterator(mamaFieldCache fieldCache);
    public:
        MamaFieldCacheIterator(const MamaFieldCacheIterator&);
        ~MamaFieldCacheIterator();

        MamaFieldCacheIterator& operator=(const MamaFieldCacheIterator& rhs);

        bool operator==(const MamaFieldCacheIterator& rhs) const;
        bool operator!=(const MamaFieldCacheIterator& rhs) const;

        MamaFieldCacheIterator& operator++();

    protected:
        mamaFieldCacheIterator mIterator;
        MamaFieldCacheField mField;
        
    private:
        friend class MamaFieldCache;
    };
    
    /**
     * Iterator type used to iterate over the fields contained in the cache.
     * This iterator allows modification of fields in the cache.
     */
    class MAMACPPExpDLL iterator : public MamaFieldCacheIterator
    {
    protected:
        iterator(mamaFieldCache fieldCache);

    public:
        typedef std::input_iterator_tag iterator_category;
        typedef MamaFieldCacheField value_type;
        typedef std::iterator_traits<MamaFieldCacheField> difference_type;
        typedef MamaFieldCacheField* pointer;
        typedef MamaFieldCacheField& reference;

        reference operator*();
        pointer operator->();

    private:
        friend class MamaFieldCache;
    };
    /**
     * Iterator type used to iterate over the fields contained in the cache.
     * This iterator does not allow modification of fields in the cache.
     */
    class MAMACPPExpDLL const_iterator : public MamaFieldCacheIterator
    {
    protected:
        const_iterator(mamaFieldCache fieldCache);
    
    public:
        const_iterator(const iterator& it);

        typedef std::input_iterator_tag iterator_category;
        typedef MamaFieldCacheField value_type;
        typedef std::iterator_traits<MamaFieldCacheField> difference_type;
        typedef const MamaFieldCacheField* pointer;
        typedef const MamaFieldCacheField& reference;

        reference operator*();
        pointer operator->();

    private:
        friend class MamaFieldCache;
    };

public:
    /**
     * Constructor.
     * Use <code>create</code> to create the actual cache.
     */
    MamaFieldCache();
    
    /**
     * Destructor.
     * This is called automatically when an object is destroyed. This method will
     * clean all the memory allocated.
     */
    virtual ~MamaFieldCache();

    /**
     * Create a MamaFieldCache.
     */
    virtual void create();

    /**
     * Clear the <code>MamaFieldCache</code>.
     * No fields will be present in the cache after this operation. The cache is
     * ready to be used, as long as create has been called once.
     */
    virtual void clear();
    
    /**
     * Return the number of fields in cache.
     * 
     * @return The number of fields in cache.
     */
    virtual mama_size_t getSize() const;

    /**
     * Set if field modifications are tracked when creating a delta message.
     * See method <code>getDeltaMessage</code>.
     * When modifications are not tracked, then calling <code>getDeltaMessage</code>
     * is the same as calling <code>getFullMessage</code>.
     * 
     * @param value Specify whether field modifications will be tracked.
     */
    virtual void setTrackModified(bool value);

    /**
     * Return the value of the track modifications flag.
     * 
     * @return Whether tracking modifications is enabled or not.
     */
    virtual bool getTrackModified() const;

    /**
     * Set if field names are used when adding fields to MamaMsg.
     * See methods <code>getDeltaMessage</code> and <code>getFullMessage</code>.
     * 
     * @param value Specify whether field names will be used.
     */
    virtual void setUseFieldNames(bool value);

    /**
     * Return the value of the use field names flag.
     * 
     * @return Whether field names are enabled or not.
     */
    virtual bool getUseFieldNames() const;

    /**
     * Set if the cache uses a lock when some methods are called for thread safety.
     * 
     * @param value Specify whether locking will be used.
     */
    virtual void setUseLock(bool value);

    /**
     * Return the value of the locking flag.
     * 
     * @return Whether the cache is using a lock or not.
     */
    virtual bool getUseLock() const;

    /**
     * Lock the cache.
     */
    virtual void lock();
    
    /**
     * Unlock the cache.
     */
    virtual void unlock();

    /**
     * Retrieve a field from the cache.
     * If the field is not found then NULL is returned.
     * 
     * @param fid Field id of the field to look up.
     * @param name Field name of the field to look up.
     * @return The field in the cache (if present)
     */
    virtual const MamaFieldCacheField* find(mama_fid_t fid,
                                            const char* name = NULL) const;
    
    /**
     * Retrieve a field from the cache.
     * If the field is not found then NULL is returned.
     * 
     * @param fid Field id of the field to look up.
     * @param name Field name of the field to look up.
     * @return The field in the cache (if present)
     */
    virtual MamaFieldCacheField* find(mama_fid_t fid,
                                      const char* name = NULL);

    /**
     * Set the specified field as modified even if the value has not changed.
     * @param field The field to set the modified flag to.
     */
    virtual void setModified(MamaFieldCacheField& field);
    
    /**
     * Update the cache content with the field given.
     * If the field is already present, it will be updated, otherwise it will be
     * added to the cache.
     * 
     * @param field The field to use for updating the cache.
     */
    virtual void apply(const MamaFieldCacheField& field);

    /**
     * Update the cache content with the data contained in the message.
     *
     * @param msg Reference to a <code>MamaMsg</code> containing the fields to
     *            update in the cache.
     * @param dict Reference to a <code>MamaDictionary</code> representing a data
     *             dictionary.
     */
    virtual void apply(const MamaMsg& msg, const MamaDictionary* dict = NULL);

    /**
     * Update the cache content with the data contained in the record.
     *
     * @param record Reference to a <code>MamaFieldCacheRecord</code> containing
     *               the fields to update in the cache.
     */
    virtual void apply(const MamaFieldCacheRecord& record);

    /**
     * Populate a MamaMsg with all the fields present in the cache.
     *
     * @param msg MamaMsg which will be populated with the fields from the cache.
     */
    virtual void getFullMessage(MamaMsg& msg);
    
    /**
     * Populate a MamaMsg with all the fields currently modified in the cache.
     * Note that this method also resets the modification state of the modified fields.
     * If trackModifications flag is set to false, then this method is the same as
     * <code>getFullMessage</code>.
     *
     * @param msg MamaMsg which will be populated with the modified fields from the cache.
     */
    virtual void getDeltaMessage(MamaMsg& msg);

    /**
     * Clear the list of all the modified fields and reset the modified flags
     * for all the modified fields in the cache.
     */
    virtual void clearModifiedFields();

    /**
     * Return a constant iterator to the first field in the cache and allows to
     * iterate over the fields in the cache.
     * 
     * @return An iterator to the first element in the cache.
     */
    virtual const_iterator begin() const;

    /**
     * Return an iterator to the first field in the cache and allows to iterate
     * over the fields in the cache.
     * 
     * @return An iterator to the first element in the cache.
     */
    virtual iterator begin();

    /**
     * Return a const iterator to an invalid element and allows to check if an
     * iterator has arrived to the end (no more elements to iterate).
     * The content of this iterator must not be accessed.
     * 
     * @return An iterator to the first element in the cache.
     */
    virtual const_iterator end() const;

    /**
     * Return an iterator to an invalid element and allows to check if an iterator
     * has arrived to the end (no more elements to iterate).
     * The content of this iterator must not be accessed.
     * 
     * @return An iterator to the first element in the cache.
     */
    virtual iterator end();

private:
    MamaFieldCache(const MamaFieldCache&);
    MamaFieldCache& operator=(const MamaFieldCache&);

private:
    struct MamaFieldCacheImpl;
    MamaFieldCacheImpl* mPimpl;
};

} // namespace Wombat

#endif // MAMA_FIELD_CACHE_CPP_H__
