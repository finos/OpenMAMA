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

#ifndef MAMA_MSG_FIELD_CPP_H__
#define MAMA_MSG_FIELD_CPP_H__

#include <mama/mamacpp.h>

#include <vector>
#include <functional>


using namespace std;

namespace Wombat 
{

    class MamaMsgFieldImpl;
    class MamaFieldDescriptor;
    class MamaDateTime;
    class MamaPrice;
    class MamaMsg;

    /**
     * MamaMsg field representation.
     */
    class MAMACPPExpDLL MamaMsgField
    {
    public:
        ~MamaMsgField ();

        MamaMsgField  (void);

        MamaMsgField  (
            mamaMsgField  field);

        /**
         * Clear the field.
         */
        void clear ();

        /**
         * Set this field to a different MAMA C API field.
         */
        void set (
            mamaMsgField  field);

        const MamaFieldDescriptor* getDescriptor () const;

        /**
         * Return the field identifier.
         *
         * @return The fid.
         */
        mama_fid_t getFid () const;
        
        /**
         * Return the field name.
         *
         * @return The name.
         */
        const char* getName () const;

        /**
         * Return the field type.
         *
         * @return The type.
         */
        mamaFieldType getType () const;
        
        /**
         * Return the field type name. The type name is a human readable 
         * representation of the type.
         *
         * @return The type name.
         */
        const char* getTypeName () const;

        /**
         * Get as a boolean field.
         * @return  The boolean value.
         */
        mama_bool_t getBool () const;

        /**
         * Get as a character field.
         * @return  The character value.
         */
        char getChar () const;

        /**
         * Get as a I8 field.
         * @return  The integer value.
         */
        mama_i8_t getI8 () const;

        /**
         * Get as a U8 field.
         * @return  The integer value.
         */
        mama_u8_t getU8 () const;

        /**
         * Get as a I16 field.
         * @return  The integer value.
         */
        mama_i16_t getI16 () const;

        /**
         * Get as a U16 field.
         * @return  The integer value.
         */
        mama_u16_t getU16 () const;

        /**
         * Get as a I32 field.
         * @return  The integer value.
         */
        mama_i32_t getI32 () const;

        /**
         * Get as a U32 field.
         * @return  The integer value.
         */
        mama_u32_t getU32 () const;

        /**
         * Get as a I64 field.
         * @return The field value as a long.
         */
        mama_i64_t getI64 () const;

        /**
         * Get as a U64 field.
         * @return The field value as a long.
         */
        mama_u64_t getU64 () const;

        /**
         * Get as a f32 field.
         * @return The field value as a 32 bit floating point number.
         */
        mama_f32_t getF32 () const;

        /**
         * Get as a f64 field.
         * @return The field value as a 64 bit floating point number.
         */
        mama_f64_t getF64 () const;

        /**
         * Get as a const char* field.
         * @return  the string value.
         */
        const char* getString () const;

        /**
         * Get as a const void* field
         * @return The opaque value.
         */
        const void* getOpaque (
            mama_size_t&  size) const;

        /**
         * Get as a MamaDateTime field
         * @param result The date/time value.
         */
        void getDateTime (
            MamaDateTime&  result) const;

        /**
         * Get as a MamaPrice field
         * @param result The price value.
         */
        void getPrice (
            MamaPrice&  result) const;

        /**
         * Get as a MamaMsg field
         * @param result The msg value.
         */
        void getMsg (
            MamaMsg&  result) const;

        /**
         * Get a vector of characters.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorChar (
            const char*&  result,
            mama_size_t&  resultLen) const;

        /**
         * Get a vector of signed 8-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorI8 (
            const mama_i8_t*&  result,
            mama_size_t&       resultLen) const;

        /**
         * Get a vector of unsigned 8-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorU8 (
            const mama_u8_t*&  result,
            mama_size_t&       resultLen) const;

        /**
         * Get a vector of signed 16-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorI16 (
            const mama_i16_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of unsigned 16-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorU16 (
            const mama_u16_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of signed 32-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorI32 (
            const mama_i32_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of unsigned 32-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorU32 (
            const mama_u32_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of signed 64-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorI64 (
            const mama_i64_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of unsigned 64-bit integers.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorU64 (
            const mama_u64_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of 32-bit floats.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorF32 (
            const mama_f32_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of 64-bit floats.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorF64 (
            const mama_f64_t*&  result,
            mama_size_t&        resultLen) const;

        /**
         * Get a vector of strings.
         * @param result     (out) the vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorString (
            const char**&  result,
            mama_size_t&   resultLen) const;

        /**
         * Get a vector of submessages field.  Note: The vector is only
         * valid for the lifetime of the field object which, when
         * iterating over fields in a message is only as long as the
         * individual callback itself.  Use getVectorMsgDetached if it is
         * necessary to keep the vector of messages longer than the
         * lifetime of the field.
         * @param result     (out) vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorMsg (
            const MamaMsg**&  result,
            mama_size_t&      resultLen) const;

        /**
         * Get a vector of submessages field.  Deallocating the memory
         * allocated for array and it members will become the
         * responsibility of the caller.
         * @param result     (out) vector.
         * @param resultLen  (out) the size of the vector.
         */
        void getVectorMsgDetached (
            const MamaMsg**&  result,
            mama_size_t&      resultLen) const;

        /**
         * Return a string representation the field with the given fid.
         * @param result  Buffer to put result.
         * @param maxResultLen  Maximum size of buffer to put result.
         */
        void getAsString (
            char*        result,
            mama_size_t  maxResultLen) const;

        /**
         * Update the specified field with a new bool value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateBool (
            mama_bool_t  value); 

        /**
         * Update the specified field with a new char value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateChar (
            char  value); 

        /**
         * Update the specified field with a new i8 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateI8 (
            mama_i8_t  value);
        
        /**
         * Update the specified field with a new u8 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateU8 (
            mama_u8_t  value);

        /**
         * Update the specified field with a new i16 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateI16 (
            mama_i16_t  value);
        
        /**
         * Update the specified field with a new u16 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateU16 (
            mama_u16_t  value);

        /**
         * Update the specified field with a new i32 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateI32 (
            mama_i32_t  value);

        /**
         * Update the specified field with a new u32 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateU32 (
            mama_u32_t  value);
        
        /**
         * Update the specified field with a new i64 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateI64 (
            mama_i64_t  value);
        
        /**
         * Update the specified field with a new u64 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateU64 (
            mama_u64_t  value);
        
        /**
         * Update the specified field with a new f32 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateF32 (
            mama_f32_t  value);
        
        /**
         * Update the specified field with a new f64 value.
         *
         * @param value The new value for the field.
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateF64 (
            mama_f64_t  value);
        
        /**
         * Update the specified field with a new date/time value.
         *
         * @param value The new value for the field (mamaDateTime object).
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updateDateTime (
            const mamaDateTime  value);

        /**
         * Update the specified field with a new date/time value.
         *
         * @param value The new value for the field (MamaDateTime object).
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */

        void updateDateTime (
            const MamaDateTime  value);
        
        /**
         * Update the specified field with a new price value.
         *
         * @param value The new value for the field (mamaPrice object).
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updatePrice (
            const mamaPrice  value);

        /**
         * Update the specified field with a new price value.
         *
         * @param value The new value for the field (MamaPrice object).
         *
         * @throws MamaStatus exception with the following possible status values.
         *         MAMA_STATUS_WRONG_FIELD_TYPE The existing field type does not match
         *         the type of the update method called.
         *         MAMA_STATUS_NULL_ARG The field passed to the C function is NULL.
         *         MAMA_STATUS_INVALID_ARG The underlying bridge field is NULL.
         */
        void updatePrice (
            const MamaPrice  value);

        bool operator==(const MamaMsgField&) const;
        bool operator!=(const MamaMsgField&) const;
        
    private:
        mamaMsgField                  mField;
        mutable MamaFieldDescriptor*  mFieldDesc;
        mutable const MamaMsg**       mLastVectorMsg;
        mutable mama_size_t           mLastVectorMsgLen;

        void destroyLastVectorMsg () const;
    };

} // namespace Wombat
#endif // MAMA_MSG_FIELD_CPP_H__
