/* $Id: dictionary.h,v 1.26.24.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaDictionaryH__
#define MamaDictionaryH__


#if defined (__cplusplus)
extern "C" {
#endif

#include "mama/types.h"
#include "mama/fielddesc.h"


/**
 * The mamaDictionary class maps field identifiers (FIDs) to
 * human readable strings. Any incoming mamaMsg might contain FIDs but
 * no field names. The dictionary allows applications to determine the name
 * associated with a given FID. 
 */

/**
 * Typedefs for the dictionary callbacks.
 *
 *  - mamaDictionary_setErrorCallback ()
 *  - mamaDictionary_setCompleteCallback ()
 *  - mamaDictionary_setTimeoutCallback ()
 */
typedef void (MAMACALLTYPE *mamaDictionary_completeCallback)(mamaDictionary,void*);

typedef void (MAMACALLTYPE *mamaDictionary_timeoutCallback)(mamaDictionary,void*);

typedef void (MAMACALLTYPE *mamaDictionary_errorCallback)(mamaDictionary,const char*,void*);


/**
 * A structure containing the callbacks for dictionary creation.
 */
typedef struct mamaDictionaryCallbackSet
{
    mamaDictionary_completeCallback onComplete;
    mamaDictionary_timeoutCallback  onTimeout;
    mamaDictionary_errorCallback    onError;
} mamaDictionaryCallbackSet;

/**
 * Create a data dictionary from a subscription.
 *
 * @param dictionary A pointer for the dictionary being created.
 * @param queue      The mama queue.
 * @param dictionaryCallbacks   A mamaDictionaryCallbackSet with the callbacks
 *                              for completion, errors and timeouts.
 * @param source The mamaSource identifying the source of the dictionary.
 * @param timeout the timeout
 * @param retries number of retries
 * @param closure A user supplied value passed to the callbacks.
 *
 */
MAMAExpDLL
extern mama_status
mama_createDictionary (
    mamaDictionary*            dictionary,
    mamaQueue                  queue,
    mamaDictionaryCallbackSet  dictionaryCallbacks,
    mamaSource                 source,
    double                     timeout,
    int                        retries,
    void*                      closure);

/**
 * Create an empty mamaDictionary so that can be populated at
 * a later stage via a call to buildDictionaryFromMessage () or populated
 * manually via calls to addFieldDescriptor ()
 *
 * @param dictionary The address to where the dictionary will be written
 */
MAMAExpDLL
extern mama_status
mamaDictionary_create (
    mamaDictionary*  dictionary);


/**
 * Destroy this mamaDictionary object and free all its resources.
 *
 * @param dictionary The dictionary.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_destroy (
    mamaDictionary dictionary);

/**
 * Return the dictionary source feed name
 *
 * @param dictionary The dictionary.
 * @param result (out) points to the feed name
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFeedName (
    mamaDictionary dictionary,
    const char** result);

/**
 * Return the dictionary source feed host
 *
 * @param dictionary The dictionary.
 * @param result (out) points to the feed host
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFeedHost (
    mamaDictionary dictionary,
    const char** result);

/**
 * Return the mamaFieldDescriptor with the specified field FID. This method
 * is very efficient (constant time).
 *
 * @param dictionary The dictionary.
 * @param fid The field id.
 * @param result (out) points to the mamaFieldDescriptor (not a copy) 
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFieldDescriptorByFid (
    mamaDictionary        dictionary, 
    mamaFieldDescriptor*  result,
    mama_fid_t            fid);

/**
 * Return the field with the corresponding zero based index. This method
 * is O (N) with respect to the size of the dictionary.
 *
 * @param dictionary The dictionary.
 * @param index The zero-based index.
 * @param result The result.
 *
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFieldDescriptorByIndex (
    mamaDictionary        dictionary,
    mamaFieldDescriptor*  result,
    unsigned short        index);

/**
 * Return the descriptor of the field with the specified name. If there 
 * is more than one field with the same name, the one with the lowest 
 * field id is returned.
 *
 * @param dictionary The dictionary.
 * @param result the result * or NULL if no such field 
 * @param fname The name of the field to search for.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFieldDescriptorByName (
    mamaDictionary        dictionary, 
    mamaFieldDescriptor*  result,
    const char*           fname);

/**
 * Return an array of mamaFieldDescriptor which includes every field 
 * in the dictionary with the specified name. 
 * The caller is responsible for allocating descList with room enough
 * for all possible duplicate fields (use mamaDictionary_getSize () to 
 * be safe). 
 *
 * @param dictionary The dictionary.
 * @param fname (in) The name to search dictionary for.
 * @param descList (out) An array of mamaFieldDescriptor objects, which are
 * not copies and should not be destroyed by the caller.
 * @param size (out) The final number of entries in descList.  The value should 
 * be intialised to the size allocated to decList.  This will be modified to the
 * actual number found on return, or will return once this value has been found.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getFieldDescriptorByNameAll (
    mamaDictionary        dictionary, 
    const char*           fname,
    mamaFieldDescriptor*  descList,
    size_t*               size);

/**
 * Return the highest field identifier.
 *
 * @param dictionary The dictionary.
 * @param value A pointer that will contain highest FID.
 */
MAMAExpDLL
extern mama_status 
mamaDictionary_getMaxFid (
    mamaDictionary  dictionary,
    mama_fid_t*     value);

/**
 * Return the number of fields in the dictionary.
 *
 * @param dictionary The dictionary.
 * @param value   The number of entries in the dictionary.
 */
MAMAExpDLL
extern mama_status 
mamaDictionary_getSize (
    mamaDictionary  dictionary,
    size_t*         value);

/**
 * Return true if there are multiple fields with the same name.
 *
 * @param dictionary The dictionary.
 * @param value 1 if there are duplicates, 0 otherwise.
 */
MAMAExpDLL
extern mama_status 
mamaDictionary_hasDuplicates (
    mamaDictionary  dictionary,
    int*            value);

/**
 * Build a data dictionary from the specified message.
 *
 * @param dictionary The dictionary 
 * @param msg A mamaMsg representing the contents of a data dictionary.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_buildDictionaryFromMessage (
    mamaDictionary dictionary,
    const mamaMsg msg );

/**
 * Get the underlying message for the data dictionary.
 * 
 * A new message instance is created each time this function is called. It is
 * the responsibility for the caller to destroy the message when no longer
 * required.
 *
 * @param dictionary The Dictionary
 * @param msg The address of the mamaMsg where the result is to be written
 */
MAMAExpDLL
extern mama_status
mamaDictionary_getDictionaryMessage (
    mamaDictionary dictionary,
    mamaMsg* msg);

/**
 * Create a new field descriptor and add it to the dictionary.
 * New fields can be added to an existing dictionary obtained
 * from the MAMA infrastructure. This function can also be used
 * to manually populate a new data dictionary.
 *
 * @param dictionary The dictionary to which the field is to be added.
 * @param fid        The fid for the new field descriptor.
 * @param name       The name for the new field descriptor.
 * @param type       The type for the new field descriptor.
 * @param descriptor The newly created fieldDescriptor. NULL can be specified.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_createFieldDescriptor (
        mamaDictionary       dictionary,
        mama_fid_t           fid,
        const char*          name,
        mamaFieldType        type,
        mamaFieldDescriptor* descriptor);

/**
 * Tell the dictionary what the probable maximum fid in the data dictionary
 * may be. This is not necessary but will aid performance for manually
 * creating  a new dictionary or adding new fields to an existing dictionary.
 *
 * Calling this function ensures that there is capacity in the dictionary for
 * field descriptors with fids up to the max specified.
 *
 * Fields with fids greater than specified can be added to the dictionary
 * but this will incur the overhead of allocating more memory and copying
 * dictionary elements.
 *
 * @param dictionary The mamaDictionary.
 * @param maxFid     The probable maximum fid being added to the dictionary.
 */
MAMAExpDLL
extern mama_status
mamaDictionary_setMaxFid (
        mamaDictionary       dictionary,
        mama_size_t          maxFid);

/**
 * Write the data dictionary to a file.
 * The dictionary will be written in the form:
 * fid|fieldName|fieldType
 *
 * @param dictionary The dictionary to serialize.
 * @param fileName   The name of the file to serialize the dictionary to. This
 *      can be a fully qualified name, relative or a file on the
 *      \$WOMBAT_PATH
 */
MAMAExpDLL
extern mama_status
mamaDictionary_writeToFile (
        mamaDictionary       dictionary,
        const char*          fileName);

/**
 * Populate a dictionary from the contents of a file.
 * Can be used to add additional fields to an existing dictionary or
 * to populate a new dictionary.
 *
 * @param dictionary The dictionary to populate.
 * @param fileName   The file from which to populate the dictionary. This
 *      can be a fully qualified name, relative or a file on the
 *      \$WOMBAT_PATH
 */
MAMAExpDLL
extern mama_status
mamaDictionary_populateFromFile (
        mamaDictionary       dictionary,
        const char*          fileName);

#if defined (__cplusplus)
}
#endif

#endif /* MamaDictionaryH__ */
