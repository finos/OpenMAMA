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

#ifndef MAMA_MSG_CPP_H__
#define MAMA_MSG_CPP_H__

#include "mama/mamacpp.h"
#include "mama/MamaMsgField.h"
#include "mama/types.h"

namespace Wombat 
{
    class MamaFieldDescriptor;
    class MamaDateTime;
    class MamaPrice;
    class MamaMsgFieldIterator;
    class MamaDictionary;
    class WombatMsg;
    class MamaMsg;

    /**
    * Iterator class for mamaMsg
    *
    * Once an iterator has been created it can be set to the beginning of a 
    * mamaMsg and used to iterate over the MamaMsgFields.
    *
    * An iterator can only be used with one message at a time, and only one 
    * iterator can be on one message at a time
    *
    * Only forward iterating is supported
    *
    * The dictionary to be used with the fields can be set in the iterator
    */
     
    class MAMACPPExpDLL MamaMsgIterator
    {
    public:
        MamaMsgIterator  (const MamaDictionary*   dictionary);
        MamaMsgIterator  ();
        MamaMsgIterator  (const MamaMsgIterator&  copy);
        ~MamaMsgIterator ();

        void SetDict     (const MamaDictionary*   dictionary);
        MamaMsgIterator& operator= (const MamaMsgIterator&);

        bool operator==(const MamaMsgIterator&) const;
        bool operator!=(const MamaMsgIterator&) const;

        MamaMsgField&    operator*();
        MamaMsgIterator& operator++();

    protected:
        friend class     MamaMsg;
        mamaMsgIterator  myImpl;
        MamaMsgField     mMsgField;
    };

    /**
     * MAMA message representation.
     *
     * Field identifiers must be greater than 0. A field identifier of 0 indicates
     * that there is no unique FID and multiple fields with the same name
     * may have FID == 0.
     * 
     * Field lookup proceeds in the following manner similar to TIBRV.
     * - If the fid supplied is non-zero, search for a field with the
     *   specified fid and return the field if it exists (the name is not
     *   validated).  Otherwise throw a STATUS_NOT_FOUND exception.
     * - If the fid supplied is 0, return the first field encountered with
     *   the supplied name or throw a STATUS_NOT_FOUND exception if no such 
     *   field exists.
     * 
     * Get methods for numeric values may result in loss of information
     * through either rounding or truncation when a larger data type is
     * accessed as a smaller one. The result may be the same as the result
     * of casting the larger value to the smaller. For example calling
     * <code>getShort</code> for an integer field with a value greater
     * than <code>Short.MAX_VALUE</code> might return
     * <code>Short.MIN_VALUE</code>. It is also valid to throw a
     * <code>ClassCastException</code> or other appropriate
     * <code>RuntimeException</code>.
     *
     * Since some message implementations may not natively support all data
     * types, the behaviour may vary substantially. In creating and accessing 
     * messages the API's
     * assume that the underlying values are stored in the smallest
     * possible fields, and accesses them accordingly. For this reason the
     * minimal requirement is that the methods for accessing and creating
     * fields support the full range of values appropriate for their
     * type. How they deal with larger values should be irrelevant.
     *
     * @author Michael Schonberg
     */
    class MAMACPPExpDLL MamaMsg
    {
    public: 
        ~MamaMsg ();

        MamaMsg (void);
        MamaMsg (const MamaMsg&  mm);
        MamaMsg (WombatMsg&      wm);

        /**
         * Create the actual underlying wire format message. This method will
         * create a an underlying message native to the middleware being used.
         * Tibrv: RV message
         * LBM:   Wombat Message
         * TCP:   Wombat Message
         */
        void create (void);

        /**
         * Create a mamaMsg.
         *
         * @param id The identifier of the payload to be used.
         */
        void createForPayload (const char id);

        /**
         * Create a mamaMsg.
         *
         * @param id The payload bridge to be used.
         */
        void createForPayloadBridge (mamaPayloadBridge payloadBridge);


       
        /**
         * The underlying wire format for the message will be Wombat Message
         * regardless of the middleware being used.
         */
        void createForWombatMsg (void);

        /**
        * Create a MamaMsg from the provided byte buffer. The application
        * is responsible for destroying the message. In all cases the buffer
        * is copied as the message is constructed.
        *
        * Any transport differences are detailed below.
        *
        * rv: The bufferLength parameter is not required.
        *
        *
        * Note: wombatmsg format is not currently supported on tibrv 
        * transports. At the moment a buffer containing the wire format for
        * each of these transports is expected to be passed to the function.
        *
        * @param buffer The byte array containing the wire format of the message
        * @param bufferLength The length, in bytes, of the supplied buffer
        *
        * @return mama_status The outcome of the operation
        */
        void createFromBuffer (
            const void*  buffer,
            size_t       bufferLength);

        /**
        * Create a mamaMsg from the provided byte buffer using either the native 
        * format for the bridge (e.g. TIB/RV messages for the TIB/RV bridge) or 
        * a wombatmsg.  The function can determine from the buffer whether it is a 
        * wombatmsg or the native format for the transport being used.  The application 
        * is responsible for destroying the message.
        *
        * @param buffer       The byte array containing the wire format of the message
        * @param bufferLength The length, in bytes, of the supplied buffer
        * @param bridge       The bridge to use
        */
        void createForBridgeFromBuffer (
            const void*  buffer,
            size_t       bufferLength,
            mamaBridge   bridge);

        /**
         * Copy the message from another, severing all links to the
         * original message
         */
        void copy (const MamaMsg&  rhs);

        void applyMsg (const MamaMsg&  msg);
        
        /**
         * Clear the message.  All fields are removed.
         */
        void clear (void);

        /**
         * Returns the total number of fields in the message. Sub-messages count as
         * a single field.
         *
         * @return The number of fields in the message.
         */
        size_t getNumFields (void) const;

        /**
        * Get the message size in bytes.
        * Supported with the following transports:
        * tibrv
        * LBT
        *
        * @return The number of bytes in the message
        */
        size_t getByteSize (void) const;
        
        /**
         * Get a boolean field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        bool getBoolean (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get a boolean field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        bool getBoolean (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get a char field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        char getChar (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get a char field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        char getChar (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get an I8 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_i8_t getI8 (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get a I8 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_i8_t getI8 (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get a U8 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_u8_t getU8 (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get a U8 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_u8_t getU8 (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get an I16 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_i16_t getI16 (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get an I16 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_i16_t getI16 (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get a U16 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_u16_t getU16 (
            const char*  name, 
            mama_fid_t   fid) const;
        
        /**
         * Get an U16 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_u16_t getU16 (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get an I32 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_i32_t getI32 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a I32 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_i32_t getI32 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a U32 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_u32_t getU32 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a U32 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_u32_t getU32 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a I64 field.
         *
         * @param name The name
         * @param fid  The field identifier.
         * @return The field value as a long.
         */
        mama_i64_t getI64 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a I64 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_i64_t getI64 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a U64 field.
         * @param name The name
         * @param fid  The field identifier
         * @return  The integer value.
         */
        mama_u64_t getU64 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a U64 field.
         * @param fieldDesc  The field descriptor
         * @return  The integer value.
         */
        mama_u64_t getU64 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a f32 field.
         *
         * @param name The name.
         * @param fid The field identifier.
         * @return The double value.
         */
        mama_f32_t getF32 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a f32 field.
         * @param fieldDesc  The field descriptor
         * @return  The double value.
         */
        mama_f32_t getF32 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a f64 field.
         *
         * @param name The name.
         * @param fid The field identifier.
         * @return The double value.
         */
        mama_f64_t getF64 (
            const char*  name, 
            mama_fid_t   fid) const;

        /**
         * Get a f64 field.
         * @param fieldDesc  The field descriptor
         * @return  The double value.
         */
        mama_f64_t getF64 (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a const char* field
         * @param name The field name.
         * @param fid The field identifier.
         * @return The string value. The return value points to the string inside
         * the mamaMsg object. This piece of memory is owned by the object and
         * does not need to be explicitly freed.
         */
        const char* getString (
            const char*       name, 
            mama_fid_t        fid) const;

        /**
         * Get a const char* field
         * @param fieldDesc  Pointer to the field descriptor
         * @return The string value. The return value points to the string inside
         * the mamaMsg object. This piece of memory is owned by the object and
         * does not need to be explicitly freed.
         */
        const char* getString (
            const MamaFieldDescriptor* fieldDesc) const;
        
        /**
         * Get an opaque field.
         * @param name The field name.
         * @param fid The field identifier.
         * @param[out] size The opaque size in bytes.
         * @return  the string value.
         */
        const void* getOpaque (
            const char*       name, 
            mama_fid_t        fid,
            size_t&           size) const;

        /**
         * Get a const char* field.
         * @param fieldDesc  The field descriptor
         * @param[out] size The opaque size in bytes.
         * @return  The double value.
         */
        const void* getOpaque (
            const MamaFieldDescriptor* fieldDesc,
            size_t&                    size) const;

        /**
         * Get a MamaDateTime field.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) the date/time.
         */
        void getDateTime (
            const char*       name,
            mama_fid_t        fid,
            MamaDateTime&     result) const;

        /**
         * Get a MamaDateTime field.
         * @param fieldDesc  The field descriptor
         * @param result     (out) the date/time.
         */
        void getDateTime (
            const MamaFieldDescriptor* fieldDesc,
            MamaDateTime&              result) const;

        /**
         * Get a MamaPrice field.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) the date/time.
         */
        void getPrice (
            const char*       name,
            mama_fid_t        fid,
            MamaPrice&        result) const;

        /**
         * Get a MamaPrice field.
         * @param fieldDesc  The field descriptor
         * @param result     (out) the date/time.
         */
        void getPrice (
            const MamaFieldDescriptor*  fieldDesc,
            MamaPrice&                  result) const;

        /**
         * Get a submessage field.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @return           The submessage.
         */
        const MamaMsg* getMsg (
            const char*       name,
            mama_fid_t        fid) const;

        /**
         * Get a submessage field.
         * @param fieldDesc  The field descriptor
         * @return           The vector.
         */
        const MamaMsg* getMsg (
            const MamaFieldDescriptor* fieldDesc) const;

        /**
         * Get a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const char* getVectorChar (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const char* getVectorChar (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of signed 8-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i8_t* getVectorI8 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of signed 8-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i8_t* getVectorI8 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of unsigned 8-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u8_t* getVectorU8 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of unsigned 8-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u8_t* getVectorU8 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of signed 16-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i16_t* getVectorI16 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of signed 16-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i16_t* getVectorI16 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of unsigned 16-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u16_t* getVectorU16 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of unsigned 16-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u16_t* getVectorU16 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of signed 32-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i32_t* getVectorI32 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of signed 32-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i32_t* getVectorI32 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of unsigned 32-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u32_t* getVectorU32 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of unsigned 32-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u32_t* getVectorU32 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of signed 64-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i64_t* getVectorI64 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of signed 64-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_i64_t* getVectorI64 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of unsigned 64-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u64_t* getVectorU64 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of unsigned 64-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_u64_t* getVectorU64 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of 32-bit floating point numbers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_f32_t* getVectorF32 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of 32-bit floating point numbers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_f32_t* getVectorF32 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of 64-bit floating point numbers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_f64_t* getVectorF64 (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of 64-bit floating point numbers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const mama_f64_t* getVectorF64 (
            const MamaFieldDescriptor*  fieldDesc,
            size_t&                     resultLen) const;

        /**
         * Get a vector of submessages field.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const MamaMsg** getVectorMsg (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of submessages.
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const MamaMsg** getVectorMsg (
            const MamaFieldDescriptor* fieldDesc,
            size_t&                    resultLen) const;
        
        /**
         * Get a vector of strings field. The vector is deleted with the message
         * and overwritten by subsequent calls to getVectorString.
         *
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const char** getVectorString (
            const char*       name,
            mama_fid_t        fid,
            size_t&           resultLen) const;

        /**
         * Get a vector of submessages. The vector is deleted with the message and
         * overwritten by subsequent calls to getVectorString.
         *
         * @param fieldDesc  The field descriptor
         * @param resultLen  (out) the size of the vector.
         * @return           The vector.
         */
        const char** getVectorString (
            const MamaFieldDescriptor* fieldDesc,
            size_t&                    resultLen) const;


        /**
         * Try to get a boolean field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryBoolean (
            const char*   name, 
            mama_fid_t    fid,
            bool&         result) const;

        /**
         * Try to get a boolean field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryBoolean (
            const MamaFieldDescriptor* field,
            bool&                      result) const;

        /**
         * Try to get a char field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryChar (
            const char*   name, 
            mama_fid_t    fid,
            char&         result) const;

        /**
         * Try to get a char field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryChar (
            const MamaFieldDescriptor* field,
            char&                      result) const;
        
        /**
         * Try to get an unsigned 8 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryI8 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i8_t&    result) const;

        /**
         * Try to get an unsigned 8 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryI8 (
            const MamaFieldDescriptor* field,
            mama_i8_t&                 result) const;
        
        /**
         * Try to get an unsigned 8 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryU8 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u8_t&    result) const;

        /**
         * Try to get an unsigned 8 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryU8 (
            const MamaFieldDescriptor* field,
            mama_u8_t&                 result) const;
        
        /**
         * Try to get a signed 16 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryI16 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i16_t&   result) const;

        /**
         * Try to get a signed 16 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryI16 (
            const MamaFieldDescriptor* field,
            mama_i16_t&                result) const;

        /**
         * Try to get an unsigned 16 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryU16 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u16_t&   result) const;

        /**
         * Try to get an unsigned 16 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryU16 (
            const MamaFieldDescriptor* field,
            mama_u16_t&                result) const;

        /**
         * Try to get a signed 32 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryI32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i32_t&   result) const;

        /**
         * Try to get a signed 32 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryI32 (
            const MamaFieldDescriptor* field,
            mama_i32_t&                result) const;

        /**
         * Try to get an unsigned 32 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryU32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u32_t&   result) const;

        /**
         * Try to get an unsigned 32 bit integer field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryU32 (
            const MamaFieldDescriptor* field,
            mama_u32_t&                result) const;

        /**
         * Try to get a signed 64 bit field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryI64 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i64_t&   result) const;

        /**
         * Try to get a signed 64 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryI64 (
            const MamaFieldDescriptor* field,
            mama_i64_t&                result) const;

        /**
         * Try to get an unsigned 64 bit field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryU64 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u64_t&   result) const;

        /**
         * Try to get an unsigned 64 bit field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryU64 (
            const MamaFieldDescriptor* field,
            mama_u64_t&                result) const;

        /**
         * Try to get a f32 field.
         *
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryF32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_f32_t&   result) const;

        /**
         * Try to get a f32 field.
         *
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryF32 (
            const MamaFieldDescriptor* field,
            mama_f32_t&                result) const;

        /**
         * Try to get a f64 field.
         *
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryF64 (
            const char*   name, 
            mama_fid_t    fid,
            mama_f64_t&   result) const;

        /**
         * Try to get a f64 field.
         *
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryF64 (
            const MamaFieldDescriptor* field,
            mama_f64_t&                result) const;

        /**
         * Try to get a string field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @return Whether the field was present.
         */
        bool tryString (
            const char*   name, 
            mama_fid_t    fid,
            const char*&  result) const;

        /**
         * Try to get a string field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @return Whether the field was present.
         */
        bool tryString (
            const MamaFieldDescriptor* field,
            const char*&               result) const;

        /**
         * Try to get a date/time field.
         * @param name   The field name
         * @param fid    The field identifier
         * @param result The result (not modified if field not present) const
         * @return Whether the field was present.
         */
        bool tryDateTime (
            const char*    name, 
            mama_fid_t     fid,
            MamaDateTime&  result) const;

        /**
         * Try to get a date/time field.
         * @param field  The field to try
         * @param result The result (not modified if field not present) const
         * @return Whether the field was present.
         */
        bool tryDateTime (
            const MamaFieldDescriptor*  field,
            MamaDateTime&               result) const;

        /**
         * Try to get a price field.
         * @param name   The field name
         * @param fid    The field identifier
         * @param result The result (not modified if field not present) const
         * @return Whether the field was present.
         */
        bool tryPrice (
            const char*    name, 
            mama_fid_t     fid,
            MamaPrice&     result) const;

        /**
         * Try to get a price field.
         * @param field  The field to try
         * @param result The result (not modified if field not present) const
         * @return Whether the field was present.
         */
        bool tryPrice (
            const MamaFieldDescriptor*  field,
            MamaPrice&                  result) const;

        /**
         * Try to get a submessage field.
         * @param name   The field name
         * @param fid    The field identifier
         * @param result The result
         * @return Whether the field was present.
         */
        bool tryMsg (
            const char*     name, 
            mama_fid_t      fid,
            const MamaMsg*& result) const;

        /**
         * Try to get a submessage field.
         * @param field  The field to try
         * @param result The result
         * @return Whether the field was present.
         */
        bool tryMsg (
            const MamaFieldDescriptor*  field,
            const MamaMsg*&             result) const;
            
        /**
         * Try to get a string field.
         * @param result The result (not modified if field not present) const
         * @param name   The field name
         * @param fid    The field identifier
         * @param size (out) The size of the opaque in bytes.
         * @return Whether the field was present.
         */
        bool tryOpaque (
            const char*   name, 
            mama_fid_t    fid,
            const void*&  result,
            size_t&       size) const;

        /**
         * Try to get a string field.
         * @param result The result (not modified if field not present) const
         * @param field  The field to try
         * @param size (out) The size of the opaque in bytes.
         * @return Whether the field was present.
         */
        bool tryOpaque (
            const MamaFieldDescriptor* field,
            const void*&               result,
            size_t&                    size) const;
        
        /**
         * Try to get a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorChar (
            const char*       name,
            mama_fid_t        fid,
            const char*&      result,
            size_t&           resultLen) const;

        /**
         * Try to get a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorChar (
            const MamaFieldDescriptor*  fieldDesc,
            const char*&                result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of signed 8-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI8 (
            const char*         name,
            mama_fid_t          fid,
            const mama_i8_t*&   result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of signed 8-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i8_t*&           result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of unsigned 8-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU8 (
            const char*         name,
            mama_fid_t          fid,
            const mama_u8_t*&   result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of unsigned 8-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u8_t*&           result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of signed 16-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI16 (
            const char*          name,
            mama_fid_t           fid,
            const mama_i16_t*&   result,
            size_t&              resultLen) const;

        /**
         * Try to get a vector of signed 16-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i16_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of unsigned 16-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU16 (
            const char*         name,
            mama_fid_t          fid,
            const mama_u16_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of unsigned 16-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u16_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of signed 32-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI32 (
            const char*         name,
            mama_fid_t          fid,
            const mama_i32_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of signed 32-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i32_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of unsigned 32-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU32 (
            const char*         name,
            mama_fid_t          fid,
            const mama_u32_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of unsigned 32-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u32_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of signed 64-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI64 (
            const char*         name,
            mama_fid_t          fid,
            const mama_i64_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of signed 64-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorI64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i64_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of unsigned 64-bit integers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU64 (
            const char*         name,
            mama_fid_t          fid,
            const mama_u64_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of unsigned 64-bit integers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorU64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u64_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of 32-bit floating point numbers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorF32 (
            const char*         name,
            mama_fid_t          fid,
            const mama_f32_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of 32-bit floating point numbers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorF32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f32_t*&          result,
            size_t&                     resultLen) const;

        /**
         * Try to get a vector of 64-bit floating point numbers.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorF64 (
            const char*         name,
            mama_fid_t          fid,
            const mama_f64_t*&  result,
            size_t&             resultLen) const;

        /**
         * Try to get a vector of 64-bit floating point numbers.
         * or opaque fields over a vector of characters.)
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorF64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f64_t*&          result,
            size_t&                     resultLen) const;


        /**
         * Try to get a vector of strings.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorString (
            const char*    name,
            mama_fid_t     fid,
            const char**&  result,
            size_t&        resultLen) const;

        /**
         * Try to get a vector of strings
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorString (
            const MamaFieldDescriptor*  fieldDesc,
            const char**&               result,
            size_t&                     resultLen) const;
        /**
         * Try to get a vector of submessages field.
         * @param name       The field name.
         * @param fid        The field identifier.
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorMsg (
            const char*       name,
            mama_fid_t        fid,
            const MamaMsg**&  result,
            size_t&           resultLen) const;

        /**
         * Try to get a vector of submessages.
         * @param fieldDesc  The field descriptor
         * @param result     (out) The vector.
         * @param resultLen  (out) The size of the vector.
         * @return Whether the field was present.
         */
        bool tryVectorMsg (
            const MamaFieldDescriptor*  fieldDesc,
            const MamaMsg**&            result,
            size_t&                     resultLen) const;

        /**
         * Add a new boolean field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addBoolean (
            const char*   name, 
            mama_fid_t    fid,
            bool          value);
        
        /**
         * Add a new boolean field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addBoolean (
            const MamaFieldDescriptor*  fieldDesc,
            bool                        value);

        /**
         * Add a new char field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addChar (
            const char*   name, 
            mama_fid_t    fid,
            char          value);

        /**
         * Add a new char field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addChar (
            const MamaFieldDescriptor*  fieldDesc,
            char                        value);

        /**
         * Add a new I8 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addI8 (
            const char*  name, 
            mama_fid_t   fid,
            mama_i8_t    value);

        /**
         * Add a new I8 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addI8 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_i8_t                   value);

        /**
         * Add a new I16 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addI16 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i16_t    value);

        /**
         * Add a new I16 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addI16 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_i16_t                  value);

        /**
         * Add a new I32 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addI32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i32_t    value);

        /**
         * Add a new I32 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addI32 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_i32_t                  value);

        /**
         * Add a new I64 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addI64 (
            const char*   name, 
            mama_fid_t    fid,
            mama_i64_t    value);

        /**
         * Add a new I64 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addI64 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_i64_t                  value);

        /**
         * Add a new byte (U8) const field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addU8 (
            const char*  name, 
            mama_fid_t   fid,
            mama_u8_t    value);

        /**
         * Add a new U8 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addU8 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_u8_t                   value);

        /**
         * Add a new short (U16) const field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addU16 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u16_t    value);

        /**
         * Add a new U16 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addU16 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_u16_t                  value);

        /**
         * Add a new int (U32) const field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addU32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u32_t    value);

        /**
         * Add a new U32 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addU32 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_u32_t                  value);

        /**
         * Add a new int (U64) const field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addU64 (
            const char*   name, 
            mama_fid_t    fid,
            mama_u64_t    value);

        /**
         * Add a new U64 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addU64 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_u64_t                  value);

        /**
         * Add a new F32 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addF32 (
            const char*   name, 
            mama_fid_t    fid,
            mama_f32_t    value);

        /**
         * Add a new F32 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addF32 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_f32_t                  value);

        /**
         * Add a new F64 field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addF64 (
            const char*  name, 
            mama_fid_t   fid,
            mama_f64_t   value);

        /**
         * Add a new F64 field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addF64 (
            const MamaFieldDescriptor*  fieldDesc,
            mama_f64_t                  value);

        /**
         * Add a const char*   field.
         *
         * @param name The name.
         * @param value The value.
         * @param fid  The field identifier.
         */
        void addString (
            const char*   name, 
            mama_fid_t    fid,
            const char*   value);

        /**
         * Add a new const char* field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addString (
            const MamaFieldDescriptor*  fieldDesc,
            const char*                 value);
        
        /**
         * Add a date/time field.
         *
         * @param name The field name.
         * @param fid  The field identifier.
         * @param value The value.
         */
        void addDateTime (
            const char*          name, 
            mama_fid_t           fid,
            const MamaDateTime&  value);

        /**
         * Add a new date/time field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addDateTime (
            const MamaFieldDescriptor*  fieldDesc,
            const MamaDateTime&         value);

        /**
         * Add a price field.
         *
         * @param name The field name.
         * @param fid  The field identifier.
         * @param value The value.
         */
        void addPrice (
            const char*       name, 
            mama_fid_t        fid,
            const MamaPrice&  value);

        /**
         * Add a new price field.
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         */
        void addPrice (
            const MamaFieldDescriptor*  fieldDesc,
            const MamaPrice&            value);

        /**
         * Add an opaque field
         *
         * @param name The name.
         * @param value The value.
         * @param fid The field identifier.
         * @param size the size of the opaque field, in bytes
         */
        void addOpaque (
            const char*  name, 
            mama_fid_t   fid,
            const void*  value,
            size_t       size);

        /**
         * Add an opaque field
         *
         * @param fieldDesc     The field descriptor
         * @param value The value.
         * @param size the size of the opaque field, in bytes
         */
        void addOpaque (
            const MamaFieldDescriptor*  fieldDesc,
            const void*                 value,
            size_t                      size);

        /**
         * Add a MamaMsg object
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param value         the value to add
         */
       void addMsg(
            const char*  name,
            mama_fid_t   fid,
            MamaMsg*     value);
       
        /**
         * Add a MamaMsg object
         * @param fieldDesc     The field descriptor
         * @param value         the value to add
         */
       void addMsg (
            const MamaFieldDescriptor*  fieldDesc,
            MamaMsg*                    value);

        /**
         * Add a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorChar (
            const char*  name,
            mama_fid_t   fid,
            const char   vectorValues[],
            size_t       vectorLen);

        /**
         * Add a vector of characters.  (Note: prefer using string fields
         * or opaque fields over a vector of characters.)
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorChar (
            const MamaFieldDescriptor*  fieldDesc,
            const char                  vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of signed 8-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI8 (
            const char*       name,
            mama_fid_t        fid,
            const mama_i8_t   vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of signed 8-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i8_t             vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of unsigned 8-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU8 (
            const char*      name,
            mama_fid_t       fid,
            const mama_u8_t  vectorValues[],
            size_t           vectorLen);

        /**
         * Add a vector of unsigned 8-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u8_t             vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of signed 16-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI16 (
            const char*       name,
            mama_fid_t        fid,
            const mama_i16_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of signed 16-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i16_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of unsigned 16-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU16 (
            const char*       name,
            mama_fid_t        fid,
            const mama_u16_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of unsigned 16-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u16_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of signed 32-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI32 (
            const char*       name,
            mama_fid_t        fid,
            const mama_i32_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of signed 32-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i32_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of unsigned 32-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU32 (
            const char*       name,
            mama_fid_t        fid,
            const mama_u32_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of unsigned 32-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u32_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of signed 64-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI64 (
            const char*       name,
            mama_fid_t        fid,
            const mama_i64_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of signed 64-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorI64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i64_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of unsigned 64-bit integers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU64 (
            const char*       name,
            mama_fid_t        fid,
            const mama_u64_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of unsigned 64-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorU64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u64_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of 32-bit floating point numbers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorF32 (
            const char*       name,
            mama_fid_t        fid,
            const mama_f32_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of unsigned 32-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorF32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f32_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of 64-bit floating point numbers.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorF64 (
            const char*       name,
            mama_fid_t        fid,
            const mama_f64_t  vectorValues[],
            size_t            vectorLen);

        /**
         * Add a vector of unsigned 64-bit integers.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorF64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f64_t            vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of strings.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorString (
            const char*  name,
            mama_fid_t   fid,
            const char*  vectorValues[],
            size_t       vectorLen);

        /**
         * Add a vector of strings.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
         */
        void addVectorString (
            const MamaFieldDescriptor*  fieldDesc,
            const char*                 vectorValues[],
            size_t                      vectorLen);

        /**
         * Add a vector of MamaMsg objects.
         * @param name          The field name.
         * @param fid           The field identifier.
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
          */
       void addVectorMsg (
            const char*  name,
            mama_fid_t   fid,
            MamaMsg*     vectorValues[],
            size_t       vectorLen);
       
        /**
         * Add a vector of MamaMsg objects.
         * @param fieldDesc     The field descriptor
         * @param vectorValues  The vector values.
         * @param vectorLen     The size of the vector.
          */
       void addVectorMsg (
            const MamaFieldDescriptor*  fieldDesc,
            MamaMsg*                    vectorValues[],
            size_t                      vectorLen);     
             
        
        /**
         * Update the value of an existing boolean field. If the field
         * does not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateBoolean (
            const char*  name, 
            mama_fid_t   fid,
            bool         value);

        /**
         * Update the value of an existing boolean field. If the field
         * does not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateBoolean (
            const MamaFieldDescriptor*  fieldDesc,
            bool                        value);

        /**
         * Update the value of an existing char field. If the field does not exist
         * it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateChar (
            const char*  name, 
            mama_fid_t   fid,
            const char   value);

        /**
         * Update the value of an existing char field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateChar (
            const MamaFieldDescriptor*  fieldDesc,
            const char                  value);

        /**
         * Update the value of an existing byte field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateI8 (
            const char*      name, 
            mama_fid_t       fid,
            const mama_i8_t  value);

        /**
         * Update the value of an existing byte field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateI8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i8_t             value);

        /**
         * Update the value of an existing U8 field. If the field does not
         * exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateU8 (
            const char*      name, 
            mama_fid_t       fid,
            const mama_u8_t  value);

        /**
         * Update the value of an existing U8 field. If the field does not
         * exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateU8 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u8_t             value);

        /**
         * Update the value of an existing short field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateI16 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_i16_t  value);

        /**
         * Update the value of an existing short field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateI16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i16_t            value);

        /**
         * Update the value of an existing U16 field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateU16 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_u16_t  value);

        /**
         * Update the value of an existing U16 field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateU16 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u16_t            value);

        /**
         * Update the value of an existing integer field. If the field
         * does not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateI32 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_i32_t  value);

        /**
         * Update the value of an existing integer field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateI32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i32_t            value);

        /**
         * Update the value of an existing U32 field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateU32 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_u32_t  value);

        /**
         * Update the value of an existing U32 field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateU32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u32_t            value);

        /**
         * Update the value of an existing long field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateI64 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_i64_t  value);

        /**
         * Update the value of an existing long field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateI64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_i64_t            value);

        /**
         * Update the value of an existing U64 field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateU64 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_u64_t  value);

        /**
         * Update the value of an existing U64 field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateU64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_u64_t            value);

        /**
         * Update the value of an existing F32 field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateF32 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_f32_t  value);
        
        /**
         * Update the value of an existing F32 field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateF32 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f32_t            value);

        /**
         * Update the value of an existing F64 field. If the field does not exist
         * it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateF64 (
            const char*       name, 
            mama_fid_t        fid,
            const mama_f64_t  value);

        /**
         * Update the value of an existing F64 field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateF64 (
            const MamaFieldDescriptor*  fieldDesc,
            const mama_f64_t            value);

        /**
         * Update the value of an existing const char* field. If the field
         * does not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         */
        void updateString (
            const char*  name, 
            mama_fid_t   fid,
            const char*  value);
        
        /**
         * Update the value of an existing string field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateString (
            const MamaFieldDescriptor*  fieldDesc,
            const char*                 value);

        /**
         * Update the value of an existing opaque field. If the field does
         * not exist it is added.
         *
         * @param name The name.
         * @param value The new value.
         * @param fid  The field identifier.
         * @param size The size of the opaque in bytes
         */
        void updateOpaque (
            const char*  name, 
            mama_fid_t   fid,
            const void*  value,
            size_t       size);

        /**
         * Update the value of an existing opaque field. If the field does
         * not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         * @param size          The size of the opaque in bytes
         */
        void updateOpaque (
            const MamaFieldDescriptor*  fieldDesc,
            const void*                 value,
            size_t                      size);

        /**
         * Update a date/time field.  If the field does not exist it is
         * added.
         *
         * @param name The field name.
         * @param fid  The field identifier.
         * @param value The value.
         */
        void updateDateTime (
            const char*          name, 
            mama_fid_t           fid,
            const MamaDateTime&  value);

        /**
         * Update the value of an existing date/time field. If the field
         * does not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updateDateTime (
            const MamaFieldDescriptor*  fieldDesc,
            const MamaDateTime&         value);

        /**
         * Update a price field.  If the field does not exist it is
         * added.
         *
         * @param name The field name.
         * @param fid  The field identifier.
         * @param value The value.
         */
        void updatePrice (
            const char*       name, 
            mama_fid_t        fid,
            const MamaPrice&  value);

        /**
         * Update the value of an existing price field. If the field
         * does not exist it is added.
         *
         * @param fieldDesc     The field descriptor
         * @param value         The new value.
         */
        void updatePrice (
            const MamaFieldDescriptor*  fieldDesc,
            const MamaPrice&            value);

        /**
         * Get the message type.
         */
        mamaMsgType getType (void) const;

        /**
         * Get a human readable type name.
         */
        const char* getMsgTypeName (void) const;

        /**
         * Get the msg status.
         */
        mamaMsgStatus getStatus (void) const;

        /**
         * Get human readable message status.
         */
        const char* getMsgStatusString (void) const;

        /**
         * Iterate over all the fields.
         */
        void iterateFields (MamaMsgFieldIterator&  iterator,
                            const MamaDictionary*  dictionary,
                            void*                  closure) const;

        /**
         * Return a const char * representation the message. The memory allocated by
         * this method gets freed upon destroying the message or invoking toString()
         * again.
         *
         * @return A string representation of the message.
         */
        const char* toString () const;

        /**
         * Obtain a string representation the field with the given fid.
         * @param name The field name.
         * @param fid The field identifier.
         * @param result  Buffer to put result.
         * @param maxResultLen  Maximum size of buffer to put result.
         */
        void getFieldAsString (
            const char*  name,
            mama_fid_t   fid, 
            char*        result,
            size_t       maxResultLen) const;

        /**
         * Obtain a string representation the field with the given fid.
         * @param fieldDesc  The field descriptor
         * @param result  Buffer to put result.
         * @param maxResultLen  Maximum size of buffer to put result.
         */
        void getFieldAsString (
            const MamaFieldDescriptor*  fieldDesc,
            char*                       result,
            size_t                      maxResultLen) const;

        /**
         * Obtain a the mamaMsgField with the given fid.
         * @param name The field name.
         * @param fid The field identifier.
         */
        MamaMsgField* getField (
            const char*  name,
            mama_fid_t   fid) const;
        
        /**
         * Obtain a the mamaMsgField with the given fid.
         * @param fieldDesc  The field descriptor
         */
        MamaMsgField*  getField (
            const MamaFieldDescriptor*  fieldDesc) const;
            
        /**
         * Test for the presence of the MamaMsgField with the given fid. 
         * This method does not return the field.
         * @param name The field name.
         * @param fid The field identifier.
         */
        bool tryField (
            const char*  name,
            mama_fid_t   fid) const;

        /**
         * Test for the presence of the MamaMsgField with the given field descriptor. 
         * This method does not return the field.
         * @param fieldDesc  The field descriptor
         */
        bool tryField (
            const MamaFieldDescriptor*  fieldDesc) const;

        /**
         * Try to obtain the MamaMsgField with the given fid.
         * @param name The field name.
         * @param fid The field identifier.
         * @param result The result (not modified if field not present)
         */
        bool tryField (
            const char*    name,
            mama_fid_t     fid, 
            MamaMsgField*  result) const;

        /**
         * Try to obtain the MamaMsgField with the given field descriptor.
         * @param fieldDesc  The field descriptor
         * @param result The result (not modified if field not present)
         */
        bool tryField (
            const MamaFieldDescriptor*  fieldDesc,
            MamaMsgField*               result) const;

        /**
         * Try to obtain a string representation the field with the given fid.
         * @param name The field name.
         * @param fid The field identifier.
         * @param result  Buffer to put result.
         * @param maxResultLen  Maximum size of buffer to put result.
         * @return Whether the field was present.
         */
        bool tryFieldAsString (
            const char*  name,
            mama_fid_t   fid, 
            char*        result,
            size_t       maxResultLen) const;

        /**
         * Try to obtain a string representation the field with the given fid.
         * @param fieldDesc  The field descriptor
         * @param result  Buffer to put result.
         * @param maxResultLen  Maximum size of buffer to put result.
         * @return Whether the field was present.
         */
        bool tryFieldAsString (
            const MamaFieldDescriptor*  fieldDesc,
            char*                       result,
            size_t                      maxResultLen) const;

        /**
         * Get the underlying message as an array of bytes. The
         * buffer still belongs to the underlying message so no
         * attempt should be made to modify it.
         *
         * @param buffer The byte array containing the buffer
         * @param bufferLength The length, in bytes of the returned buffer
         */
        void getByteBuffer (
            const void*&  buffer,
            size_t&       bufferLength) const;

        /**
         * Create a MamaMsg. This will create a message using the native
         * format for the bridge e.g. TIB/RV messages for the TIB/RV bridge.
         * For middlewares which only do not have a native message format,
         * a wombatmsg will be created.
         *
         * @param bridgeImpl the bridge to use
         */
        void createForBridge (
            mamaBridge  bridgeImpl); 
        
        /**
         * Normally the Mama API owns incoming mamaMsg objects and they go out of 
	     * scope and are freed when the message callback returns. Calling this 
	     * method from the message callback creates a new C++ wrapper for the 
	     * underlying message and transfers responsibility for calling destroy()
	     * to the caller. 
	     *
	     * The caller must also delete the pointer returned by 
	     * detach(). Note that calling "delete msg.detach()" invokes destroy() so
	     * calling destroy is not necessary if the application calls delete.
         */
        MamaMsg* detach (void);


        /**
         * Whether this message is the result of a request needing a response.
         */
        bool isFromInbox (void) const;
        
        mama_seqnum_t getSeqNum (void) const;

        /**
         * Return true if this message is definitely a duplicate message. This
         * condition will not occur with the current feed handlers.
         */
        bool getIsDefinitelyDuplicate (void) const;

        /**
         * Return true if this message is possibly a duplicate message. This may
         * occur in the event of a fault tolerant feed handler take over where the
         * feed handler replays messages to prevent gaps.
         */
        bool getIsPossiblyDuplicate (void) const;

        /**
         * Return true if the message is possibly delayed. This condition may be
         * true during a fault-tolerant takeover.
         */
        bool getIsPossiblyDelayed (void) const;
        
        /**
         * Return true if the message is delayed. This condition may be
         * true during a fault-tolerant takeover.
         */
        bool getIsDefinitelyDelayed (void) const;
        
        /**
         * Return true when the FH sends the message out of sequence.
         */
        bool getIsOutOfSequence (void) const;

        /**
         * Sets a new for an existing mamaMsg using the provided byte buffer. 
         * The application is responsible for destroying the message.
         *
         * @param buffer the new byte buffer  
         * @param size  size of buffer
         * @return status
        */
        bool setNewBuffer (
            void*        buffer,
            mama_size_t  size);

        /**
         * Return the native middleware message handle.  This is only
         * intended for Wombat internal use.
         */
        void* getNativeHandle (void) const;

        /** Create the message from an existing mamaMsg*/
        void createFromMsg (
            mamaMsg  msg,
            bool     destroyMsg = false) const;

        /**
         * Set the message to a different underlying C message.
         * Can be called multiple times on a single MamaMsg
         */
        void setMsg (
            mamaMsg  msg);

        /** 
         * Return const reference to underlying mamaMsg
         */ 
        const mamaMsg& getUnderlyingMsg (void) const;

        /**
         * Return the underlying mamaMsg (non const)
         */
        mamaMsg getUnderlyingMsg (void);


        /** Return the type of the payload message (wombat message or, if using
         *  a non-wombat message payload, RV or FAST message). 
         *
         * @return payloadType The payload type.
         */   
        mamaPayloadType getPayloadType (void);
        
       /**
         * Get the native message structure for the underlying message
         * @return nativeMsg The resulting native handle.
         */
        void*  getNativeMsg (void);
        
       /**
         * Sets a iterator to be used with existing mamaMsg.
         * The iterator is set to the first MamaMsgField of the mamaMsg
         *
         * @param theiterator iterator to be used  
         * @return first MamaMsgField
        */
        
        MamaMsgField& begin (
            MamaMsgIterator&  theIterator) const;  

        /**
         * Get a copy of the reply Handle
         */
        mamaMsgReply getReplyHandle (void) const;

        /**
         * Destroy a copied reply Handle
         */
        static void destroyReplyHandle (
            mamaMsgReply  replyHandle);

    private:
        mutable mamaMsg        mMsg;
        mutable bool           mDestroy;
        mutable MamaMsg**      mVectorMsg;
        mutable size_t         mVectorMsgSize;
        mutable size_t         mVectorMsgAllocSize;
        mutable mamaMsg*       mCvectorMsg;
        mutable size_t         mCvectorMsgAllocSize;
        mutable MamaMsg*       mTmpMsg;
        mutable const char*    mString;
        mutable MamaMsgField*  mMsgField;

        void setDestroyCMsg    (bool destroy);
        void cleanup           ();
        void growVector        (size_t newSize) const;
        void growCvector       (size_t newSize) const;
    };

} // namespace Wombat
#endif // MAMA_MSG_CPP_H__
