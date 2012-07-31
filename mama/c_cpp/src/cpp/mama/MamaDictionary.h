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

#ifndef MAMA_DICTIONARY_CPP_H__
#define MAMA_DICTIONARY_CPP_H__

#include <mama/mamacpp.h>
#include <mama/MamaFieldDescriptor.h>

namespace Wombat 
{
class DictionaryImpl;

/**
 * The <code>MamaDictionary</code> class maps field identifiers (FIDs)
 * to human readable strings. Incoming <code>MamaMsg</code>s may
 * contain FIDs but no field names. The dictionary allows applications
 * to determine the name associated with a given FID. On some
 * platforms, the inbound messages may have names, but not
 * fids in which case the dictionary can map names to fids.
 *
 */
class MAMACPPExpDLL MamaDictionary
{
public:
    virtual ~MamaDictionary ();

    MamaDictionary (void);

    /**
     * Create a dictionary subscription. The caller supplied
     * <code>DictionaryCallback.onComplete</code> will be invoked
     * after the dictionary is fully constructed.
     * 
     * If there is an error creating the dictionary Mama invokes the
     * <code>onError</code> callback, and the returned dictionary is
     * not valid.  In the event of a timeout, Mama invokes the
     * <code>onTimeout</code> callback. This method uses the default
     * timeout and retry values
     * (<code>SubscriptionBridge.DEFAULT_TIMEOUT</code> and
     * <code>SubscriptionBridge.DEFAULT_RETRIES</code>.
     *
     * @param queue The mama queue.
     * @param callback The dictionary callback.
     * @param source The dictionary source. Depends upon feed handler
     * configuration. See feed handler documentation for details
     * @param timeout The timeout in seconds.
     * @param retries The number of times to retry before failing.
     * @param closure The caller supplied closure.
     *
     *
     */
    virtual void create (
        MamaQueue*              queue,
        MamaDictionaryCallback* callback,
        MamaSource*             source,
        int                     retries = MAMA_DEFAULT_RETRIES,
        double                  timeout = MAMA_DEFAULT_TIMEOUT,
        void*                   closure = NULL);

    /**
     * Return the dictionary source feed name
     *
     * @param dictionary The dictionary.
     * @return the feed name
     */
    virtual const char* getFeedName();

    /**
     * Return the dictionary source feed host
     *
     * @param dictionary The dictionary.
     * @return the feed host
     */
    virtual const char* getFeedHost();

    /**
     * Return the field with the specified field FID. This method is
     * very efficient.
     *
     * @param fid The field id.
     * @return The field.
     *
     */
    virtual MamaFieldDescriptor* getFieldByFid (mama_fid_t fid);

    /**
     * Return the field with the specified field FID. This method is
     * very efficient.
     *
     * @param fid The field id.
     * @return The field.
     *
     */
    virtual const MamaFieldDescriptor* getFieldByFid (mama_fid_t fid) const;

    /**
     * Return the field with the corresponding zero based index. This
     * method is O (N) with respect to the size of the dictionary.
     *
     * @param index The index.
     * @return The field.
     */
    virtual MamaFieldDescriptor* getFieldByIndex (size_t index);

    /**
     * Return the field with the corresponding zero based index. This
     * method is O (N) with respect to the size of the dictionary.
     *
     * @param index The index.
     * @return The field.
     */
    virtual const MamaFieldDescriptor* getFieldByIndex (size_t index) const;

    /**
     * Return the field with the specified name. If there is more than
     * one field with the same name, the one with the lowest field id
     * is returned.
     *
     * @param name The name of the field.
     * @return The field with the specified name or null if there is no such
     * field.
     */
    virtual MamaFieldDescriptor* getFieldByName (const char* name); 

    /**
     * Return the field with the specified name. If there is more than
     * one field with the same name, the one with the lowest field id
     * is returned.
     *
     * @param name The name of the field.
     * @return The field with the specified name or null if there is no such
     * field.
     */
    virtual const MamaFieldDescriptor* getFieldByName (const char* name) const;

    /**
     * Return the highest field identifier.
     * @return  The highest FID.
     */
    virtual mama_fid_t getMaxFid (void) const;

    /**
     * Return the number of fields in the dictionary.
     *
     * @return   The number of entries in the dictionary.
     */
    virtual size_t getSize (void) const;

    /**
     * Return true if there are multiple fields with the same name.
     *
     * @return  true if there are duplicates.
     */
    virtual bool hasDuplicates (void) const;

    /**
     * Return the callback.
     *
     * @see MamaDictionaryCallback
     * @return The callback
     */
    virtual MamaDictionaryCallback* getCallback (void) const;

    /**
     * Set the callback to receive notifications when creation is
     * complete or an error occurs.
     *
     * @param callback The callback.
     */
    virtual void setCallback (MamaDictionaryCallback* callback);

    /**
     * Return the underlying C mamaDictionary.
     */
    virtual mamaDictionary getDictC ();

    /**
     * Return the underlying C mamaDictionary.
     */
    virtual const mamaDictionary getDictC () const;

    /**
     * Return the closure for the dictionary.
     *
     * @return the closure.
     */
    virtual void* getClosure (void) const;

    /**
     * Returns a MamaMsg representing the data dictionary. This message can be
     * published or used to create a new MamaDictionary object.
     * A new MamaMsg is created for each invocation of the method. It is the
     * responsibility of the caller to delete the message when no longer
     * needed.
     *
     * @return Pointer to a new MamaMsg for the dictionary.
     */
    virtual MamaMsg* getDictionaryMessage() const;

    /**
     * Recreate a data dictionary from the MamaMsg supplied.
     * The MamaMsg is copied and can therefore be deleted after
     * the method has returned.
     *
     * @param msg Reference to a MamaMsg representing a data dictionary.
     */
    virtual void buildDictionaryFromMessage(MamaMsg& msg);

    /**
     * Add a new field descriptor to a dictionary. New fields can be added
     * to an existing dictionary obtained from the MAMA infrastructure. This
     * function can also be used to manually build a data dictionary.
     *
     * @param fid        The fid for the new field descriptor.
     * @param name       The name for the new field descriptor.
     * @param type       The type for the new field descriptor.
     */
    virtual MamaFieldDescriptor* createFieldDescriptor (
            mama_fid_t             fid,
            const char*            name,
            mamaFieldType          type);

    /**
     * Tell the dictionary what the probable maximum fid in the data dictionary
     * may be. This is not necessary but will aid performance for manually
     * creating  a new dictionary or adding new fields to an existing dictionary.
     *
     * Calling this function ensures that there is capacity in the dictionary for
     * field descriptors with fids up to the maximum specified.
     *
     * Fields with fids greater than specified can be added to the dictionary
     * but this will incur the overhead of allocating more memory and copying
     * dictionary elements.
     *
     * @param maxFid     The probable maximum fid being added to the dictionary.
     */
    virtual void setMaxFid (
            mama_size_t          maxFid);


    /**
     * Write the data dictionary to a file.
     * The dictionary will be written in the form:
     * fid|fieldName|fieldType
     *
     * @param fileName   The name of the file to serialize the dictionary to. This
     *      can be a fully qualified name, relative or a file on the
     *      \$WOMBAT_PATH
     */
    virtual void writeToFile (const char* fileName);

    /**
     * Populate a dictionary from the contents of a file.
     * Can be used to add additional fields to an existing dictionary or
     * to populate a new dictionary.
     *
     * @param fileName   The file from which to populate the dictionary. This
     *      can be a fully qualified name, relative or a file on the
     *      \$WOMBAT_PATH
     */
    virtual void populateFromFile (const char* fileName);
    
    DictionaryImpl* mPimpl;
};

} // namespace Wombat
#endif // MAMA_DICTIONARY_CPP_H__
