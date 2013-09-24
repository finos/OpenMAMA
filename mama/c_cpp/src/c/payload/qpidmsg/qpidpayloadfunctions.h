/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#ifndef MAMA_PAYLOAD_QPIDMSG_QPIDMSGIMPL_H__
#define MAMA_PAYLOAD_QPIDMSG_QPIDMSGIMPL_H__

#include "bridge.h"
#include "payloadbridge.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                      Public interface prototypes                      =
  =========================================================================*/

/**
 * Called when loading/creating a payload bridge. Creates the mamaPayloadBridge
 * object. Uses the INITIALIZE_PAYLOAD_BRIDGE macro to assign pointers for
 * appropriate method calls to the bridge object, returning both the object and
 * the type via the method parameters. Can also make use of the objects closure
 * element to pass arbitrary data back if necessary.
 *
 * Requirement: Required
 *
 * @param result A pointer to the payload bridge object created by the call.
 * @param identifier A character indicating the type of payload bridge created.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_createImpl      (mamaPayloadBridge* result, char* identifier);

/**
 * Returns the type of payload from the current bridge, from the mamaPayloadType
 * enum in msg.h
 *
 * Requirement: Nice to have.
 *
 * @return mamaPayloadType indicating the type of payload.
 */
mamaPayloadType
qpidmsgPayload_getType         (void);

/**
 * Method call to create an instance of the middleware message payload, which is
 * specific to the payload bridge - this includes the creation of any underlying
 * object types, initialization of structures etc.
 *
 * Requirement: Required
 *
 * @param msg Pointer to the msgPayload object created by the method.
 */
mama_status
qpidmsgPayload_create           (msgPayload*         msg);

/**
 * Method call to create instance of the middleware payload bridge for template
 * based payloads.
 *
 * Requirement: Required for certain payload types only.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_createForTemplate (msgPayload*         msg,
                                  mamaPayloadBridge   bridge,
                                  mama_u32_t          templateId);

/**
 * Method to copy the from one msgPayload object into another. Assumes both the
 * msg and it's copy are of the same payload type.
 *
 * Requirement: Required
 *
 * @param msg Message payload to be copied.
 * @param copy Pointer to a message
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_copy             (const msgPayload    msg,
                                 msgPayload*         copy);

/**
 * Method to clear the contents from the passed message to allow it to be reused
 * by a later call. Should clear out anything (such as buffers) which may cause
 * difficulty later.
 *
 * Requirement: Required.
 *
 * @param msg Message payload to be cleared.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_clear            (msgPayload          msg);

/**
 * Method to destroy the msgPayload, freeing any memory which was allocated
 * during the corresponding create, and performing any other cleanup required
 * by the underlying payload.
 *
 * Requirement: Required.
 *
 * @param msg The message payload to be destroyed.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_destroy          (msgPayload          msg);

/**
 * Method allows setting a reference within the underlying message payload to
 * the mamaMsg within which it is contained.
 *
 * Requirement: Not required.
 *
 * @param msg The message payload object for which the parent is being set.
 * @param parent The parent mamaMsg which is being passed to the payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_setParent        (msgPayload          msg,
                                 const mamaMsg       parent);

/**
 * Method to return the size in bytes of the msgPayload object passed to it.
 *
 * Requirement: Not required.
 *
 * @param msg The msgPayload for which we want to determine the size.
 * @param size The size of the msgPayload (as a mama_size_t).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getByteSize      (msgPayload          msg,
                                 mama_size_t*        size);

/**
 * Method for returning the number of mama fields contained within a message
 * payload.
 *
 * Requirement: Required
 *
 * @param msg The msgPayload for which we want to determine the number of fields.
 * @param numFields The number of fields (as a mama_size_t).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getNumFields     (const msgPayload    msg,
                                 mama_size_t*        numFields);

/**
 * Method to determine the subject of a subject from a msgPayload. Used for self
 * describing messages. Not required by most middlewares.
 *
 * Requirement: Not required
 *
 * @param msg The msgPayload object for which we want to determine the subject.
 * @param subject The string for returning the subject of the msgPayload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getSendSubject   (const msgPayload    msg,
                                 const char**        subject);

/**
 * Method to convert a message payload to a string representation (for printing
 * in debug messages etc).
 *
 * Requirement: Required.
 *
 * @param msg The msgPayload which we wish to convert to a string.
 *
 * @return char* representation of the msgPayload
 */
const char*
qpidmsgPayload_toString         (const msgPayload    msg);

/**
 * Method which iterates the fields of the supplied msgPayload, and calls the
 * iterator callback method for each field encountered. The callback has the
 * parent mamaMsg, the current field, and the closure object passed as it's
 * arguments.
 *
 * Requirement: Required.
 *
 * @param msg The message payload to be iterated over.
 * @param parent The parent mamaMsg object which contains the payload
 * @param field The message field which is reused for each field of the message.
 * @param cb Pointer to the callback method which is triggered for each field.
 * @param closure Arbitrary data passed to the method which is then passed to the cb
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_iterateFields    (const msgPayload    msg,
                                 const mamaMsg       parent,
                                 mamaMsgField        field,
                                 mamaMsgIteratorCb   cb,
                                 void*               closure);

/**
 * Method to return a byte buffer version of the message payload.
 *
 * Note: Typically this is an alias for the getByteBuffer() method, though
 * additional implementation may be associated with it.
 *
 * Requirement: Required
 *
 * @param msg The msgPayload which is to be serialized.
 * @param buffer The buffer into which the serialized message is placed.
 * @param bufferLength The length in bytes of the serialized message in the buffer.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_serialize        (const msgPayload    msg,
                                 const void**        buffer,
                                 mama_size_t*        bufferLength);

/**
 * Method to add a byte buffer into a passed message payload.
 *
 * Note: Typically this is an alias for the setByteBuffer() method, though
 * additional implementation may be associated with it.
 *
 * Requirement: Required
 *
 * @param msg The msgPayload into which the buffer should be de-serialized.
 * @param buffer The byte buffer from which the message payload should be removed.
 * @param bufferLength The lenght of the byte buffer.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_unSerialize      (const msgPayload    msg,
                                 const void**        buffer,
                                 mama_size_t         bufferLength);

/**
 * Method to return a byte buffer version of the message payload.
 *
 * Requirement: Required
 *
 * @param msg The msgPayload which is to be serialized.
 * @param buffer The buffer into which the serialized message is placed.
 * @param bufferLength The length in bytes of the serialized message in the buffer.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getByteBuffer    (const msgPayload    msg,
                                 const void**        buffer,
                                 mama_size_t*        bufferLength);

/**
 * Method which takes a byte buffer, and adds it to a message payload.
 *
 * Note: The message payload object needs to have been created before
 * setByteBuffer() is called, so a NULL message payload should fail.
 *
 * Requirement: Required
 *
 * @param msg The msgPayload into which the buffer should be de-serialized.
 * @param bridge A mamaPayloadBridge object.
 * @param buffer The byte buffer from which the message payload should be removed.
 * @param bufferLength The lenght of the byte buffer.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_setByteBuffer    (const msgPayload    msg,
                                 mamaPayloadBridge   bridge,
                                 const void*         buffer,
                                 mama_size_t         bufferLength);


/**
 * Method for creating new msgPayload for an existing byte buffer containing a
 * previously serialized payload.
 *
 * Requirement: Required
 *
 * @param msg Pointer to the msgPayload in which the new message is to be returned.
 * @param bridge A mamaPayloadBridge object (can be used to determine
 * @param buffer
 * @param bufferLength
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_createFromByteBuffer (msgPayload*         msg,
                                     mamaPayloadBridge   bridge,
                                     const void*         buffer,
                                     mama_size_t         bufferLength);

/**
 * Method to take two msgPayload objects, and apply their contents from the src
 * to the dest payload. This involves iterating the fields of the src message
 * payload, and updating/creating the equivalent field in the dest message
 * payload.
 *
 * Note: Calls to update a field should create one which doesn't exist already,
 * so there should be no need to check for existance in the dest message payload
 *
 * Requirement: Required
 *
 * @param dest The destination message to which the src should be applied.
 * @param src The source message payload from which fields are to be copied.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_apply            (msgPayload          dest,
                                 const msgPayload    src);

/**
 * For payloads which make use of an underlying codec, this method should return
 * the underlying representation of the message.
 *
 * Requirement: Not required
 *
 * @param msg The message payload object from which to extract the native message.
 * @param nativeMsg The native message object extracted from the payload message
 *                  cast to a void**.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getNativeMsg     (const msgPayload    msg,
                                 void**              nativeMsg);

/**
 * Method to extract a field from a given message payload, and convert it into
 * a string representation.
 *
 * Requirement: Required
 *
 * @param msg The message payload object from which to extract the field.
 * @param name The name of the field which is to be extracted.
 * @param fid The fid of the string which is to be extracted.
 * @param buffer The char* buffer into which the name is placed.
 * @param len The length of the buffer, as a mama_size_t
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getFieldAsString (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 char*               buffer,
                                 mama_size_t         len);


/*=========================================================================
  =                   Payload Add Scalar Field Methods                    =
  =========================================================================*/

/**
 * The following methods add a field of the scalar type specified by the method
 * name. Payloads which have limited type support should attempt to convert into
 * one of their supported types.
 *
 * Requirement: Required
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param value The value to be added to the message payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_addBool          (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_bool_t         value);
mama_status
qpidmsgPayload_addChar          (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 char                value);
mama_status
qpidmsgPayload_addI8            (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i8_t           value);
mama_status
qpidmsgPayload_addU8            (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u8_t           value);
mama_status
qpidmsgPayload_addI16           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i16_t          value);
mama_status
qpidmsgPayload_addU16           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u16_t          value);
mama_status
qpidmsgPayload_addI32           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i32_t          value);
mama_status
qpidmsgPayload_addU32           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u32_t          value);
mama_status
qpidmsgPayload_addI64           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i64_t          value);
mama_status
qpidmsgPayload_addU64           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u64_t          value);
mama_status
qpidmsgPayload_addF32           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f32_t          value);
mama_status
qpidmsgPayload_addF64           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f64_t          value);
mama_status
qpidmsgPayload_addString        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char*         value);

/**
 * Note: The opaque type is used to pass byte buffers of data where possible.
 */
mama_status
qpidmsgPayload_addOpaque        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const void*         value,
                                 mama_size_t         size);

/**
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_addDateTime      (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaDateTime  value);

/**
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_addPrice         (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaPrice     value);

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgPayload_addMsg           (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 msgPayload          value);

/*=========================================================================
  =                   Payload Add Vector Field Methods                    =
  =========================================================================*/

/**
 * The following methods add vectors of the type specified by the method name
 * into the specified field. Payloads which have limited type support may
 * attempt to convert into one of their supported types.
 *
 * Note: Without support for vectors, OpenMAMA cannot be used for market data
 * subscriptions, though can be used for more basic messaging.
 *
 * Requirement: Nice to have.
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param value An arrary of values to be added to the message payload.
 * @param size The size of the array to be added to the message.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_addVectorBool    (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_bool_t   value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorChar    (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char          value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorI8      (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i8_t     value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorU8      (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u8_t     value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorI16     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i16_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorU16     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u16_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorI32     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i32_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorU32     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u32_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorI64     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i64_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorU64     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u64_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorF32     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f32_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorF64     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f64_t    value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_addVectorString  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char*         value[],
                                 mama_size_t         size);

/**
 * Note: The array of messages to be added are of type msgPayload, so each has
 * the same general structure as the message to which it is being added.
 */
mama_status
qpidmsgPayload_addVectorMsg     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaMsg       value[],
                                 mama_size_t         size);

/**
 * Note: addVectorDateTime is not supported by MAMA at present.
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_addVectorDateTime (msgPayload          msg,
                                  const char*         name,
                                  mama_fid_t          fid,
                                  const mamaDateTime  value[],
                                  mama_size_t         size);

/**
 * Note: addVectorPrice is not supported by MAMA at present.
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_addVectorPrice   (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaPrice     value[],
                                 mama_size_t         size);


/*=========================================================================
  =                   Payload Update Scalar Field Methods                 =
  =========================================================================*/
/**
 * The following methods update fields of the scalar type specified by the method
 * name. Payloads which have limited type support should attempt to convert into
 * one of their supported types.
 *
 * Requirement:
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param value The value to be added to the message payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_updateBool       (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_bool_t         value);
mama_status
qpidmsgPayload_updateChar       (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 char                value);
mama_status
qpidmsgPayload_updateI8         (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i8_t           value);
mama_status
qpidmsgPayload_updateU8         (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u8_t           value);
mama_status
qpidmsgPayload_updateI16        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i16_t          value);
mama_status
qpidmsgPayload_updateU16        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u16_t          value);
mama_status
qpidmsgPayload_updateI32        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i32_t          value);
mama_status
qpidmsgPayload_updateU32        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u32_t          value);
mama_status
qpidmsgPayload_updateI64        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i64_t          value);
mama_status
qpidmsgPayload_updateU64        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u64_t          value);
mama_status
qpidmsgPayload_updateF32        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f32_t          value);
mama_status
qpidmsgPayload_updateF64        (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f64_t          value);
mama_status
qpidmsgPayload_updateString     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char*         value);

/**
 * Note: The opaque type is used to pass byte buffers of data where possible.
 */
mama_status
qpidmsgPayload_updateOpaque     (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const void*         value,
                                 mama_size_t         size);

/**
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_updateDateTime   (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaDateTime  value);

/**
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_updatePrice      (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaPrice     value);

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgPayload_updateSubMsg     (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const msgPayload    subMsg);


/*=========================================================================
  =                 Payload Update Vector Field Methods                   =
  =========================================================================*/
/**
 * The following methods update vectors of the type specified by the method
 * name. Payloads which have limited type support should attempt to convert into
 * one of their supported types.
 *
 * Requirement: Nice to have
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param value The array of values to be added to the message payload.
 * @param size The size of the array to be added to the message payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgPayload_updateVectorMsg  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mamaMsg       value[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorString (msgPayload         msg,
                                   const char*        fname,
                                   mama_fid_t         fid,
                                   const char*        strList[],
                                   mama_size_t        size);
mama_status
qpidmsgPayload_updateVectorBool (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_bool_t   boolList[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorChar (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const char          charList[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorI8   (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_i8_t     i8List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorU8   (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_u8_t     u8List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorI16  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_i16_t    i16List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorU16  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_u16_t    u16List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorI32  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_i32_t    i32List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorU32  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_u32_t    u32List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorI64  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_i64_t    i64List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorU64  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_u64_t    u64List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorF32  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_f32_t    f32List[],
                                 mama_size_t         size);
mama_status
qpidmsgPayload_updateVectorF64  (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mama_f64_t    f64List[],
                                 mama_size_t         size);

/**
 * Note: updateVectorPrice() is currently not supported by MAMA, so
 * implementation is not required at present, though we may standardize the
 * approach to vectors in the future.
 *
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_updateVectorPrice (msgPayload          msg,
                                  const char*         fname,
                                  mama_fid_t          fid,
                                  const mamaPrice*    priceList[],
                                  mama_size_t         size);

/**
 * Note: updateVectorTime() is currently not supported by MAMA, so
 * implementation is not required at present, though we may standardize the
 * approach to vectors in the future.
 *
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_updateVectorTime (msgPayload          msg,
                                 const char*         fname,
                                 mama_fid_t          fid,
                                 const mamaDateTime  timeList[],
                                 mama_size_t         size);


/*=========================================================================
  =                   Payload Get Scalar Field Methods                    =
  =========================================================================*/

/**
 * The following methods get a field of the scalar type specified by the method
 * name. Payloads which have limited type support should attempt to convert from
 * one of their supported types into the standard MAMA types.
 *
 * Requirement: Required
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param result The object in which the value of the field should be returned.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getBool          (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_bool_t*        result);
mama_status
qpidmsgPayload_getChar          (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 char*               result);
mama_status
qpidmsgPayload_getI8            (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i8_t*          result);
mama_status
qpidmsgPayload_getU8            (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u8_t*          result);
mama_status
qpidmsgPayload_getI16           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i16_t*         result);
mama_status
qpidmsgPayload_getU16           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u16_t*         result);
mama_status
qpidmsgPayload_getI32           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i32_t*         result);
mama_status
qpidmsgPayload_getU32           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u32_t*         result);
mama_status
qpidmsgPayload_getI64           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_i64_t*         result);
mama_status
qpidmsgPayload_getU64           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_u64_t*         result);
mama_status
qpidmsgPayload_getF32           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f32_t*         result);
mama_status
qpidmsgPayload_getF64           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mama_f64_t*         result);
mama_status
qpidmsgPayload_getString        (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char**        result);

/**
 * Note: The opaque type is used to pass byte buffers of data where possible.
 */
mama_status
qpidmsgPayload_getOpaque        (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const void**        result,
                                 mama_size_t*        size);

/**
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_getDateTime      (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mamaDateTime        result);

/**
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_getPrice         (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 mamaPrice           result);

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgPayload_getMsg           (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 msgPayload*         result);


/*=========================================================================
  =                   Payload Get Vector Field Methods                    =
  =========================================================================*/
/**
 * The following methods get an array of the type specified by the method
 * name. Payloads which have limited type support should attempt to convert from
 * one of their supported types into the standard MAMA types.
 *
 * Note: Without vector support, payloads cannot be used for market data
 * subscriptions, though they may still be used for basic subscription types.
 *
 * Requirement: Required
 *
 * @param msg The message payload into which the field should be added.
 * @param name The name of the field to be added.
 * @param fid The field identifier (fid) for the field to be added.
 * @param result The array in which the values stored in the field should be returned.
 * @param size The size of the array returned.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getVectorBool    (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_bool_t** result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorChar    (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char**        result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorI8      (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i8_t**   result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorU8      (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u8_t**   result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorI16     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i16_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorU16     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u16_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorI32     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i32_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorU32     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u32_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorI64     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i64_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorU64     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u64_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorF32     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f32_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorF64     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f64_t**  result,
                                 mama_size_t*        size);
mama_status
qpidmsgPayload_getVectorString  (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char***       result,
                                 mama_size_t*        size);

/**
 * Note: getVectorDateTime() is not currently supported by MAMA, so implementation
 * within the payload is not required. However, we may standardize the approach
 * to vectors at a later date, in which case it may be needed.
 *
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 */
mama_status
qpidmsgPayload_getVectorDateTime (const msgPayload    msg,
                                  const char*         name,
                                  mama_fid_t          fid,
                                  const mamaDateTime* result,
                                  mama_size_t*        size);

/**
 * Note: getVectorPrice() is not currently supported by MAMA, so implementation
 * within the payload is not required. However, we may standardize the approach
 * to vectors at a later date, in which case it may be needed.
 *
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 */
mama_status
qpidmsgPayload_getVectorPrice   (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaPrice*    result,
                                 mama_size_t*        size);

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgPayload_getVectorMsg     (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const msgPayload**  result,
                                 mama_size_t*        size);


/*=========================================================================
  =                msgFieldPayload bridge function pointers               =
  =========================================================================*/

/**
 * Method to take a message payload, and a specified field name and fid, and
 * return a msgFieldPayload object. The msgFieldPayload is a payload specific
 * structure which is used to store the contents of the field being requested -
 * how this is stored is up to the specific payload, but it should at the least
 * support each available data type, as well as storing the fid, name and type
 * of the field, and a reference to the parent message if appropriate.
 *
 * Requirement: Required.
 *
 * @param msg The message payload from which the field is to be extracted.
 * @param name The name of the field to be extracted.
 * @param fid The field identifier of the field to be extracted.
 * @param result A pointer to the msgFieldPayload object returned by the method
 *              call.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayload_getField         (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 msgFieldPayload*    result);

/**
 * Method to handle the creation and allocation of a msgFieldPayload object,
 * including the creation of any additional objects required by it.
 *
 * Requirement: Required
 *
 * @param field Pointer to the msgFieldPayload object returned by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_create      (msgFieldPayload*        field);


/**
 * Method to handle the destruction of the msgFieldPayload object, including
 * freeing any memory associated with it, and cleaning up any other associated
 * objects.
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload object to be destroyed by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_destroy     (msgFieldPayload         field);

/**
 * Method to return the type of the field stored in the msgFieldPayload object,
 * returned as a mamaFieldType
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload object from which the type is required.
 * @param result mamaFieldType indicating the type of the field stored in the
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getType     (const msgFieldPayload   field,
                                 mamaFieldType*          result);

/**
 * Method to return the name of the field stored in the msgFieldPayload object.
 * Implementations may pull the name from a variety of sources, including the
 * contents of the msgFieldPayload, a passed mamaFieldDescriptor, or from a
 * passed dictionary object.
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload object from which the name is to be extracted.
 * @param dict A dictionary from which the field name can be extracted.
 * @param desc A field descriptor from which the name can be extracted.
 * @param result char* indicating the type of the field stored in the msgFieldPayload
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getName     (msgFieldPayload         field,
                                 mamaDictionary          dict,
                                 mamaFieldDescriptor     desc,
                                 const char**            result);

/**
 * Method to return the field identifier of the field stored in the
 * msgFieldPayload object. Implementations may pull the name from a variety of
 * sources, including the contents of the msgFieldPayload, a passed
 * mamaFieldDescriptor, or from a passed dictionary object.
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload object from which the fid is to be extracted.
 * @param dict A dictionary from which the field name can be extracted.
 * @param desc A field descriptor from which the name can be extracted.
 * @param result unint16_t indicating the type of the field stored in the msgFieldPayload
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getFid      (const msgFieldPayload   field,
                                 mamaDictionary          dict,
                                 mamaFieldDescriptor     desc,
                                 uint16_t*               result);

/**
 * Method to return the descriptor of the field stored in the msgFieldPayload
 * object, returned as a mamaFieldDescriptor. Generally this will be implemented
 * by looking up the required descriptor from the passed dictionary, using the
 * fid within the msgFieldPayload object.
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload object for which the descriptor is to be extracted.
 * @param result Pointer to mamaFieldDescriptor which is returned by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getDescriptor (const msgFieldPayload  field,
                                   mamaDictionary         dict,
                                   mamaFieldDescriptor*   result);


/*=========================================================================
  =               FieldPayload Update Scalar Field Methods                =
  =========================================================================*/

/**
 * The following methods update the value of the field contained in the
 * msgFieldPayload into a given message payload, for the scalar type specified
 * by the method name.
 *
 * Requirement: Required
 *
 * @param field The msgFieldPayload containing the details of the field to be updated.
 * @param msg The message payload into which the field should be added.
 * @param value The value to be added to the message payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_updateBool  (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_bool_t             value);
mama_status
qpidmsgFieldPayload_updateChar  (msgFieldPayload         field,
                                 msgPayload              msg,
                                 char                    value);
mama_status
qpidmsgFieldPayload_updateI8    (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i8_t               value);
mama_status
qpidmsgFieldPayload_updateU8    (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u8_t               value);
mama_status
qpidmsgFieldPayload_updateI16   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i16_t              value);
mama_status
qpidmsgFieldPayload_updateU16   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u16_t              value);
mama_status
qpidmsgFieldPayload_updateI32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i32_t              value);
mama_status
qpidmsgFieldPayload_updateU32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u32_t              value);
mama_status
qpidmsgFieldPayload_updateI64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i64_t              value);
mama_status
qpidmsgFieldPayload_updateU64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u64_t              value);
mama_status
qpidmsgFieldPayload_updateF32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_f32_t              value);
mama_status
qpidmsgFieldPayload_updateF64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_f64_t              value);
mama_status
qpidmsgFieldPayload_updateString (msgFieldPayload         field,
                                  msgPayload              msg,
                                  const char*             value);
mama_status
qpidmsgFieldPayload_updateDateTime (msgFieldPayload         field,
                                    msgPayload              msg,
                                    const mamaDateTime      value);
mama_status
qpidmsgFieldPayload_updatePrice (msgFieldPayload         field,
                                 msgPayload              msg,
                                 const mamaPrice         value);
mama_status
qpidmsgFieldPayload_updateSubMsg (msgFieldPayload         field,
                                  msgPayload              msg,
                                  const msgPayload        subMsg);


/*=========================================================================
  =                 FieldPayload Get Scalar Field Methods                 =
  =========================================================================*/

/**
 * The following methods get the value of a field of the scalar type specified
 * by the method name from the contents of the passed msgFieldPayload object.
 *
 * Requirement:
 *
 * @param field The msgFieldPayload object from which the value is to be extracted
 * @param result A pointer to the value returned by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getBool     (const msgFieldPayload   field,
                                 mama_bool_t*            result);
mama_status
qpidmsgFieldPayload_getChar     (const msgFieldPayload   field,
                                 char*                   result);
mama_status
qpidmsgFieldPayload_getI8       (const msgFieldPayload   field,
                                 mama_i8_t*              result);
mama_status
qpidmsgFieldPayload_getU8       (const msgFieldPayload   field,
                                 mama_u8_t*              result);
mama_status
qpidmsgFieldPayload_getI16      (const msgFieldPayload   field,
                                 mama_i16_t*             result);
mama_status
qpidmsgFieldPayload_getU16      (const msgFieldPayload   field,
                                 mama_u16_t*            result);
mama_status
qpidmsgFieldPayload_getI32      (const msgFieldPayload   field,
                                 mama_i32_t*             result);
mama_status
qpidmsgFieldPayload_getU32      (const msgFieldPayload   field,
                                 mama_u32_t*             result);
mama_status
qpidmsgFieldPayload_getI64      (const msgFieldPayload   field,
                                 mama_i64_t*             result);
mama_status
qpidmsgFieldPayload_getU64      (const msgFieldPayload   field,
                                 mama_u64_t*             result);
mama_status
qpidmsgFieldPayload_getF32      (const msgFieldPayload   field,
                                 mama_f32_t*             result);
mama_status
qpidmsgFieldPayload_getF64      (const msgFieldPayload   field,
                                 mama_f64_t*             result);
mama_status
qpidmsgFieldPayload_getString   (const msgFieldPayload   field,
                                 const char**            result);
mama_status
qpidmsgFieldPayload_getOpaque   (const msgFieldPayload   field,
                                 const void**            result,
                                 mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getDateTime (const msgFieldPayload   field,
                                 mamaDateTime            result);
mama_status
qpidmsgFieldPayload_getPrice    (const msgFieldPayload   field,
                                 mamaPrice               result);
mama_status
qpidmsgFieldPayload_getMsg      (const msgFieldPayload   field,
                                 msgPayload*             result);


/*=========================================================================
  =                 FieldPayload Get Vector Field Methods                 =
  =========================================================================*/

/**
 * The following methods get arrays of the type specified by the method name
 * from the passed msgFieldPayload object.
 *
 * Requirement:
 *
 * @param msg The message payload into which the field should be added.
 * @param result Pointer to the array of values returned by the method.
 * @param size The size of the returned array of values.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getVectorBool (const msgFieldPayload   field,
                                   const mama_bool_t**     result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorChar (const msgFieldPayload   field,
                                   const char**            result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorI8   (const msgFieldPayload   field,
                                   const mama_i8_t**       result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorU8   (const msgFieldPayload   field,
                                   const mama_u8_t**       result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorI16  (const msgFieldPayload   field,
                                   const mama_i16_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorU16  (const msgFieldPayload   field,
                                   const mama_u16_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorI32  (const msgFieldPayload   field,
                                   const mama_i32_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorU32  (const msgFieldPayload   field,
                                   const mama_u32_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorI64  (const msgFieldPayload   field,
                                   const mama_i64_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorU64  (const msgFieldPayload   field,
                                   const mama_u64_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorF32  (const msgFieldPayload   field,
                                   const mama_f32_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorF64  (const msgFieldPayload   field,
                                   const mama_f64_t**      result,
                                   mama_size_t*            size);
mama_status
qpidmsgFieldPayload_getVectorString (const msgFieldPayload   field,
                                     const char***           result,
                                     mama_size_t*            size);

/**
 * Note: getVectorDateTime() is not currently supported by MAMA, so implementation
 * within the payload is not required. However, we may standardize the approach
 * to vectors at a later date, in which case it may be needed.
 *
 * Note: mamaDateTime is a mama_u64_t value, which encodes both second and
 * millisecond values, along with precision and hints values.
 *
 * Note: This is not currently implemented and the prototype expects a
 * mamaDateTime* rather than the mamaDateTime** that you would expect. This
 * may change if this is ever implemented in the MAMA layer
 */
mama_status
qpidmsgFieldPayload_getVectorDateTime (const msgFieldPayload   field,
                                       const mamaDateTime*     result,
                                       mama_size_t*            size);

/**
 * Note: getVectorPrice() is not currently supported by MAMA, so implementation
 * within the payload is not required. However, we may standardize the approach
 * to vectors at a later date, in which case it may be needed.
 *
 * Note: mamaPrice is a flexible format (typedef'd to void*) which can contain
 * both a price value and a hints value. Code working with the price needs to
 * be able to handle both.
 *
 * Note: This is not currently implemented and the prototype expects a
 * mamaPrice* rather than the mamaPrice** that you would expect. This
 * may change if this is ever implemented in the MAMA layer
 */
mama_status
qpidmsgFieldPayload_getVectorPrice (const msgFieldPayload   field,
                                    const mamaPrice*        result,
                                    mama_size_t*            size);

/**
 * Note: The msg to be added is a msgPayload, so has the same general structure
 * as the message to which it is being added.
 */
mama_status
qpidmsgFieldPayload_getVectorMsg   (const msgFieldPayload   field,
                                    const msgPayload**      result,
                                    mama_size_t*            size);

/**
 * it appears to be inconsistent, and the passing of an alloc'd buffer is odd (for MAMA)
 *
 * Method to return the contents of a msgFieldPayload object as a string
 * representation.
 *
 * Note: This method only needs to return the value stored within the msgFieldPayload
 * - no other details are required in the output.
 *
 * Requirement: Required
 *
 * @param field The field for which the value is to be returned as a string.
 * @param msg The message payload for which the value is being returned.
 * @param buffer Pointer to a string which is populated by the method.
 * @param len The maximum size of the string returned by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayload_getAsString (const msgFieldPayload   field,
                                 const msgPayload        msg,
                                 char*                   buffer,
                                 mama_size_t             len);


/*=========================================================================
  =               msgPayloadIter bridge function pointers                 =
  =========================================================================*/

/**
 * Method to create an iterator for a given message payload. The msgPayloadIter
 * object returned is payload specific, but should allow access to the current
 * field pointed to by the iterator, and if required, provide a mechanism for
 * determining the current position within the msgPayload, and it's next field.
 *
 * @param iter A pointer to the iterator returned by the method.
 * @param msg The message payload which is to be iterated over.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadIter_create   (msgPayloadIter*         iter,
                             msgPayload              msg);

/**
 * Method to get the next field from a message as a msgFieldPayload object
 * using the selected iterator. Ideally payloads should be able to manage their
 * position using just the iterator, but the current field, and message payload
 * are both provided as additional arguments should they be required for context.
 *
 * @param iter The iterator for the current message.
 * @param field The current msgFieldPayload (providing additional context for
 *              the iterator if required).
 * @param msg The message payload which is being iterated over.
 *
 * @return msgFieldPayload containing the contents of the next field within the message.
 */
msgFieldPayload
qpidmsgPayloadIter_next     (msgPayloadIter          iter,
                             msgFieldPayload         field,
                             msgPayload              msg);

/**
 * Method to determine if a given iterator has a next field within its target
 * message payload, returning a mama_bool_t.
 *
 * @param iter The current iterator.
 * @param msg The message payload which is to be iterated over.
 *
 * @return mama_bool_t indicating if the message has a next field.
 */

mama_bool_t
qpidmsgPayloadIter_hasNext  (msgPayloadIter          iter,
                             msgPayload              msg);

/**
 * Method to get the first field from a message as a msgFieldPayload object
 * using the selected iterator. Ideally payloads should be able to manage their
 * position using just the iterator, but the current field, and message payload
 * are both provided as additional arguments should they be required for context.
 *
 * @param iter The iterator for the current message.
 * @param field The current msgFieldPayload (providing additional context for
 *              the iterator if required).
 * @param msg The message payload which is to be iterated over.
 *
 * @return msgFieldPayload containing the contents of the first field within
 *              the message.
 */
msgFieldPayload
qpidmsgPayloadIter_begin    (msgPayloadIter          iter,
                             msgFieldPayload         field,
                             msgPayload              msg);

/**
 * Method to get the last field from a message as a msgFieldPayload object
 * using the selected iterator. Ideally payloads should be able to manage their
 * position using just the iterator, but the current field, and message payload
 * are both provided as additional arguments should they be required for context.
 *
 * @param iter The iterator for the current message.
 * @param field The current msgFieldPayload (providing additional context for
 *              the iterator if required).
 * @param msg The message payload which is to be iterated over.
 *
 * @return msgFieldPayload containing the contents of the last field within
 *              the message.
 */
msgFieldPayload
qpidmsgPayloadIter_end      (msgPayloadIter          iter,
                             msgPayload              msg);

/**
 * Method to associate a given iterator with an alternative message payload.
 *
 * @param iter The iterator which is to be altered.
 * @param msg The message payload for which the iterator is to be associated.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */

mama_status
qpidmsgPayloadIter_associate (msgPayloadIter          iter,
                              msgPayload              msg);

/**
 * Method to destroy an iterator object, freeing any allocated memory and
 * cleaning up any other associated objects.
 *
 * @param iter The iterator to be destroyed by the method.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadIter_destroy   (msgPayloadIter          iter);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_PAYLOAD_QPIDMSG_QPIDMSGIMPL_H__ */
