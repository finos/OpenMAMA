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

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mama/mama.h>
#include <mama/price.h>
#include <proton/message.h>
#include <proton/error.h>

#include "payloadbridge.h"
#include "msgfieldimpl.h"

#include "qpidcommon.h"
#include "qpidpayloadfunctions.h"
#include "payload.h"
#include "field.h"

/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define ADD_SCALAR(TYPE)                                                       \
do                                                                             \
{                                                                              \
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*)msg;                       \
                                                                               \
    if (NULL == impl)                                                          \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    if (NULL == name && 0 == fid)                                              \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);           \
                                                                               \
    /* Each field is a separate list of name | FID | value */                  \
    pn_data_put_list   (impl->mBody);                                          \
    pn_data_enter      (impl->mBody);                                          \
                                                                               \
    /* strlen +1 to encode NULL terminator */                                  \
    if (NULL == name)                                                          \
    {                                                                          \
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));                  \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        pn_data_put_string (impl->mBody,                                       \
                            pn_bytes (strlen(name)+1, (char*) name));          \
    }                                                                          \
    pn_data_put_ushort (impl->mBody, fid);                                     \
    pn_data_put##TYPE  (impl->mBody, value);                                   \
                                                                               \
    /* Exit list */                                                            \
    pn_data_exit       (impl->mBody);                                          \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \


#define UPDATE_SCALAR(TYPE,SUFFIX)                                             \
do                                                                             \
{                                                                              \
    qpidmsgPayloadImpl* impl   = (qpidmsgPayloadImpl*) msg;                    \
    mama_status         status = MAMA_STATUS_OK;                               \
                                                                               \
    if (NULL == impl)                                                          \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    /* Find field */                                                           \
    status = qpidmsgPayloadImpl_findField (impl, name, fid);                   \
                                                                               \
    /* Create if this does not exist */                                        \
    if (MAMA_STATUS_NOT_FOUND == status)                                       \
    {                                                                          \
        return qpidmsgPayload_add##SUFFIX (msg, name, fid, value);             \
    }                                                                          \
    else if (MAMA_STATUS_OK != status)                                         \
    {                                                                          \
        /* Revert to the previous iterator state if applicable */              \
        qpidmsgPayloadImpl_resetToIteratorState (impl);                        \
        return status;                                                         \
    }                                                                          \
                                                                               \
    /* Store value */                                                          \
    pn_data_put##TYPE (impl->mBody, value);                                    \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \


#define GET_SCALAR(TYPE,MAMATYPE)                                              \
do                                                                             \
{                                                                              \
    qpidmsgPayloadImpl* impl   = (qpidmsgPayloadImpl*)msg;                     \
    mama_status         status = MAMA_STATUS_OK;                               \
    pn_atom_t           atom;                                                  \
                                                                               \
    if (NULL == impl || NULL == result)                                        \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    if (NULL == name && 0 == fid)                                              \
    {                                                                          \
        *result = 0;                                                           \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    /* Find field */                                                           \
    status = qpidmsgPayloadImpl_findField (impl, name, fid);                   \
    if (MAMA_STATUS_OK != status)                                              \
    {                                                                          \
        return status;                                                         \
    }                                                                          \
                                                                               \
    /* Move onto value and extract */                                          \
    pn_data_next (impl->mBody);                                                \
                                                                               \
    /* Atom casting is required here for type flexibility required */          \
    atom = pn_data_get_atom(impl->mBody);                                      \
                                                                               \
    /* Cast this atom to the required MAMA type */                             \
    GET_ATOM_AS_MAMA_TYPE (atom, MAMATYPE, *result);                           \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \


#define UPDATE_VECTOR(TYPE,CTYPE,SUFFIX)                                       \
do                                                                             \
{                                                                              \
    qpidmsgPayloadImpl* impl   = (qpidmsgPayloadImpl*) msg;                    \
    mama_status         status = MAMA_STATUS_OK;                               \
    mama_size_t         i      = 0;                                            \
                                                                               \
    if (NULL == impl || NULL == value || (NULL == name && 0 == fid))           \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    /* Find field - takes us directly to the content */                        \
    status = qpidmsgPayloadImpl_findField (impl, name, fid);                   \
                                                                               \
    /* Create if this does not exist */                                        \
    if (MAMA_STATUS_NOT_FOUND == status)                                       \
    {                                                                          \
        return qpidmsgPayload_addVector##SUFFIX (msg, name, fid, value, size); \
    }                                                                          \
    else if (MAMA_STATUS_OK != status)                                         \
    {                                                                          \
        /* Revert to the previous iterator state if applicable */              \
        qpidmsgPayloadImpl_resetToIteratorState (impl);                        \
        return status;                                                         \
    }                                                                          \
                                                                               \
    /* Store value */                                                          \
    pn_data_put_array (impl->mBody, 0, PN##CTYPE);                             \
    pn_data_enter     (impl->mBody);                                           \
                                                                               \
    for (; i != size; i++)                                                     \
    {                                                                          \
        /* add value to array element*/                                        \
        pn_data_put##TYPE (impl->mBody, value[i]);                             \
    }                                                                          \
                                                                               \
    /* exit array */                                                           \
    pn_data_exit (impl->mBody);                                                \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \


#define ADD_VECTOR(TYPE,CTYPE)                                                 \
do                                                                             \
{                                                                              \
    mama_size_t         i    = 0;                                              \
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;                      \
                                                                               \
    if (NULL == impl || NULL == value || (NULL == name && 0 == fid))           \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);           \
                                                                               \
    /* Each field is a separate list of name | FID | ARRAY_OF_VALUES */        \
    pn_data_put_list   (impl->mBody);                                          \
    pn_data_enter      (impl->mBody);                                          \
                                                                               \
    /* strlen +1 to encode NULL terminator */                                  \
    if(NULL == name)                                                           \
    {                                                                          \
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));                  \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        pn_data_put_string (impl->mBody,                                       \
                            pn_bytes (strlen(name)+1, (char*) name));          \
    }                                                                          \
    pn_data_put_ushort (impl->mBody, fid);                                     \
                                                                               \
    /*                                                                         \
     * Add array to list (0 argument below is indicative of the array not      \
     * being described)                                                        \
     */                                                                        \
    pn_data_put_array (impl->mBody,0,PN##CTYPE);                               \
                                                                               \
    /* enter array */                                                          \
    pn_data_enter (impl->mBody);                                               \
                                                                               \
    for (; i != size; i++)                                                     \
    {                                                                          \
        /* add value to array element*/                                        \
        pn_data_put##TYPE (impl->mBody, value[i]);                             \
    }                                                                          \
                                                                               \
    /* exit array */                                                           \
    pn_data_exit (impl->mBody);                                                \
                                                                               \
    /* exit field */                                                           \
    pn_data_exit (impl->mBody);                                                \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \


#define GET_VECTOR(TYPE,MAMATYPE)                                              \
do                                                                             \
{                                                                              \
    MAMATYPE*           temp   = NULL;                                         \
    mama_size_t         i      = 0;                                            \
    qpidmsgPayloadImpl* impl   = (qpidmsgPayloadImpl*) msg;                    \
    mama_status         status = MAMA_STATUS_OK;                               \
                                                                               \
    if (NULL == impl || NULL == result || NULL == size ||                      \
            (NULL == name && 0 == fid))                                        \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    /* Find field */                                                           \
    status = qpidmsgPayloadImpl_findField (impl, name, fid);                   \
                                                                               \
    if (MAMA_STATUS_OK != status)                                              \
    {                                                                          \
        return status;                                                         \
    }                                                                          \
                                                                               \
    /* Move onto value[] */                                                    \
    pn_data_next (impl->mBody);                                                \
                                                                               \
    /* get size of array */                                                    \
    *size  = pn_data_get_array (impl->mBody);                                  \
                                                                               \
    /* allocate space for resulting array*/                                    \
    qpidmsgPayloadImpl_allocateBufferMemory (&(impl->mBuffer),                 \
                                             &(impl->mBufferSize),             \
                                             *size * sizeof (MAMATYPE));       \
    temp = (MAMATYPE*) impl->mBuffer;                                          \
                                                                               \
    /* enter array */                                                          \
    pn_data_enter (impl->mBody);                                               \
                                                                               \
    for (; i != *size; ++i)                                                    \
    {                                                                          \
        /* go to next list element */                                          \
        pn_data_next (impl->mBody);                                            \
                                                                               \
        /* get value of list element and store in result array*/               \
        temp[i] = (MAMATYPE) pn_data_get##TYPE (impl->mBody);                  \
    }                                                                          \
    /* exit array */                                                           \
    pn_data_exit (impl->mBody);                                                \
                                                                               \
    /* exit field */                                                           \
    pn_data_exit (impl->mBody);                                                \
                                                                               \
    *result = temp;                                                            \
                                                                               \
    /* Revert to the previous iterator state if applicable */                  \
    qpidmsgPayloadImpl_resetToIteratorState (impl);                            \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)                                                                     \

#define UPDATE_SCALAR_FIELD(SUFFIX,MAMATYPE)                                   \
do                                                                             \
{                                                                              \
    MAMATYPE result;                                                           \
    qpidmsgFieldPayload_get##SUFFIX (field, &result);                          \
    qpidmsgPayload_update##SUFFIX   (dest, name, fid, result);                 \
} while (0)                                                                    \

#define UPDATE_VECTOR_FIELD(SUFFIX,MAMATYPE)                                   \
do                                                                             \
{                                                                              \
    const MAMATYPE * result = NULL;                                            \
    mama_size_t size = 0;                                                      \
    qpidmsgFieldPayload_getVector##SUFFIX (field, &result, &size);             \
    qpidmsgPayload_updateVector##SUFFIX   (dest, name, fid, result, size);     \
} while (0)                                                                    \

#define ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(SUFFIX,MAMATYPE)                     \
do                                                                             \
{                                                                              \
    MAMATYPE result;                                                           \
    qpidmsgFieldPayload_get##SUFFIX   (field, &result);                        \
    return qpidmsgPayload_add##SUFFIX (msg, name, fid, result);                \
} while (0)

#define ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(SUFFIX,MAMATYPE)                     \
do                                                                             \
{                                                                              \
    const MAMATYPE* result  = NULL;                                            \
    mama_size_t     size    = 0;                                               \
    qpidmsgFieldPayload_getVector##SUFFIX   (field, &result, &size);           \
    return qpidmsgPayload_addVector##SUFFIX (msg, name, fid, result, size);    \
} while (0)


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * When creating a payload message from a "byte buffer" (the pn_message_t
 * pointer), it is not desirable to create a proton message as in the payload's
 * create function because this message will be unnecessarily created, then
 * wither freed and deleted or else overwritten and leaked. This function
 * exists to allow the caller to create everything within a qpid message
 * payload implementation except the underlying proton message pointer.
 *
 * @param msg     The qpidmsgPayloadImpl to populate with the newly created
 *                implementation
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_createImplementationOnly (msgPayload*              msg);

/**
 * Some function calls expect a MAMA message whereas others expect the payload.
 * This function exists to translate a payload message into an encapsulating
 * mamaMsg where this is required.
 *
 * @param msg     The qpidmsgPayloadImpl to populate with the newly created
 *                implementation
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_payloadToMamaMsg         (qpidmsgPayloadImpl*      impl,
                                             mamaMsg*                 target);

/**
 * Due to the fact that MAMA fields are actually a series of underlying atoms,
 * walking over these elements actually moves the pn_data_t pointer which the
 * iterator depends on. This function allows the caller to put pn_data_t back
 * to where it was when it last left the iterator so that the function's
 * accessor methods have no impact on the current iteration state.
 *
 * @param impl    The qpidmsgPayloadImpl containing the pn_data_t to revert
 *                back to the last iterator position.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_resetToIteratorState     (qpidmsgPayloadImpl*      impl);

/**
 * This function will add the field specified to the msgPayload provided.
 *
 * @param msg     The payload to add the field to.
 * @param value   The field to add to the payload.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_addFieldToPayload        (msgPayload               msg,
                                             qpidmsgFieldPayloadImpl* value);

/**
 * This function will read the current underlying buffer from the current
 * position and attempt to translate that into a payload message.
 *
 * @param msg     The buffer to examine for payload content.
 * @param value   The payload to populate with the results.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_getMessageFromBuffer     (pn_data_t*               buffer,
                                             qpidmsgPayloadImpl*      target);

/**
 * This function accepts a proton data type and returns a MAMA equivalent type
 * for that data within its supported vectors. It is quite possible for a data
 * type which is legal as a scalar not to be supported as a MAMA vector. Where
 * that is the case, this function will return MAMA_FIELD_TYPE_UNKNOWN.
 *
 * @param type    The proton type to map to a MAMA type for vectors.
 *
 * @return mamaFieldType containing the MAMA equivalent field type.
 */
static mamaFieldType
qpidmsgPayloadImpl_arrayToMamaType          (pn_type_t                type);

/**
 * This is a MAMA iterator callback function used when iterating over a MAMA
 * message for the purpose of adding its contents to a qpidmsgPayloadImpl.
 *
 * @param msg     The MAMA message being iterated over
 * @param field   The current MAMA field being examined.
 * @param closure In this instance, the closure contains the qpidmsgPayloadImpl
 *                which this field should then be added to.
 */
static void MAMACALLTYPE
qpidmsgPayloadImpl_addMsgFieldCb            (const mamaMsg            msg,
                                             const mamaMsgField       field,
                                             void*                    closure);

/**
 * This function is for adding the *contents* of a MAMA message to the payload
 * provided (i.e. it does *not* add FID or name details so it can be used in
 * both qpidmsgPayload_addMsg and qpidmsgPayload_addVectorMsg, the later of
 * which does not have its own FID or field name associated with the message
 * provided).
 *
 * @param msg     The qpidmsgPayloadImpl which the MAMA message contents are to
 *                be added to.
 * @param value   The MAMA message containing the contents which will need to be
 *                copied over to the qpidmsgPayloadImpl.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_addBareMsg               (msgPayload               msg,
                                             mamaMsg                  value);

/**
 * For update or get by name / fid functions, this function will search through
 * the proton message body to find a field matching the credentials supplied.
 *
 * @param impl    The qpidmsgPayloadImpl to search
 * @param name    The field name to look for
 * @param fid     The field identifier to look for
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidmsgPayloadImpl_findField                (qpidmsgPayloadImpl*      impl,
                                             const char*              name,
                                             mama_fid_t               fid);


/*=========================================================================
  =                   Public interface functions                          =
  =========================================================================*/

mama_status
qpidmsgPayload_createImpl (mamaPayloadBridge* result, char* identifier)
{
    mamaPayloadBridgeImpl* impl = NULL;

    impl = (mamaPayloadBridgeImpl*) calloc (1, sizeof (mamaPayloadBridgeImpl));

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidmsgPayload_createImpl(): "
                  "Could not allocate memory for payload impl.");
        return MAMA_STATUS_NOMEM;
    }

    /* Use closure to store global data for payload bridge if required */
    impl->mClosure = NULL;

    /* Initialize the virtual function table (see payloadbridge.h) */
    INITIALIZE_PAYLOAD_BRIDGE (impl, qpidmsg);

    *result     = (mamaPayloadBridge)impl;
    *identifier = (char)MAMA_PAYLOAD_QPID;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_init (char* identifier)
{
    *identifier = (char)MAMA_PAYLOAD_QPID;

    return MAMA_STATUS_OK;
}


mamaPayloadType
qpidmsgPayload_getType (void)
{
    return MAMA_PAYLOAD_QPID;
}

mama_status
qpidmsgPayload_create (msgPayload* msg)
{
    qpidmsgPayloadImpl* impl    = NULL;
    mama_status         status  = MAMA_STATUS_OK;

    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    status = qpidmsgPayloadImpl_createImplementationOnly (msg);

    if (MAMA_STATUS_OK == status)
    {
        impl            = (qpidmsgPayloadImpl*) *msg;
        impl->mQpidMsg  = pn_message ();

        if (NULL == impl->mQpidMsg)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "qpidmsgPayload_create():"
                      " Could not create new pn_message_t.");
            qpidmsgFieldPayload_destroy(impl->mField);
            free (impl);
            return MAMA_STATUS_NOMEM;
        }
        impl->mBody     = pn_message_body (impl->mQpidMsg);

        pn_data_put_list (impl->mBody);
        pn_data_enter    (impl->mBody);
    }

    return status;
}

mama_status
qpidmsgPayload_createForTemplate (msgPayload*        msg,
                                  mamaPayloadBridge  bridge,
                                  mama_u32_t         templateId)
{
    mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidmsgPayload_createForTemplate(): "
              "Method not applicable for qpid as it is not a template "
              "based payload.");
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgPayload_copy (const msgPayload    msg,
                     msgPayload*         copy)
{
    qpidmsgPayloadImpl*  impl        = (qpidmsgPayloadImpl*) msg;
    qpidmsgPayloadImpl*  copyImpl    = NULL;
    mama_status          status      = MAMA_STATUS_OK;
    int                  qpidStatus  = 0;

    if (NULL == msg || NULL == copy)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /*
     * Create destination message object if the user hasn't already created
     * one
     */
    if (NULL == *copy)
    {
        status = qpidmsgPayload_create (copy);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
    }

    copyImpl    = (qpidmsgPayloadImpl*) *copy;
    qpidStatus  = pn_data_copy (copyImpl->mBody, impl->mBody);

    return qpidmsgPayloadInternal_toMamaStatus (qpidStatus);
}

mama_status
qpidmsgPayload_clear (msgPayload msg)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Just clear out the underlying buffer - reuse everything else */
    pn_message_clear (impl->mQpidMsg);
    pn_data_put_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_destroy (msgPayload msg)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;
    int                 i    = 0;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Release the underlying field object */
    if (NULL != impl->mField)
    {
        qpidmsgFieldPayload_destroy(impl->mField);
        impl->mField = NULL;
    }

    /* Release the main reusable buffer */
    if (NULL != impl->mBuffer)
    {
        free (impl->mBuffer);
        impl->mBuffer       = NULL;
        impl->mBufferSize   = 0;
    }

    /* Release the vector message reusable buffer */
    if (NULL != impl->mNestedMsgBuffer)
    {
        /* destroy each MAMA message object created */
        for (i = 0; i < impl->mNestedMsgBufferCount; i++)
        {
            mamaMsg_destroy ((mamaMsg) impl->mNestedMsgBuffer[i]);
        }
        /* Free the parent buffer */
        free (impl->mNestedMsgBuffer);
        impl->mNestedMsgBuffer      = NULL;
        impl->mNestedMsgBufferSize  = 0;
    }

    /* Release the underlying payload object */
    if(NULL != impl->mQpidMsg)
    {
        pn_message_free (impl->mQpidMsg);
        impl->mQpidMsg = NULL;
    }

    /* Finally, release the payload implementation object */
    free (impl);
    impl = NULL;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_setParent (msgPayload    msg,
                          const mamaMsg parent)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    impl->mParent = (mamaMsg) parent;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getByteSize (msgPayload    msg,
                            mama_size_t*  size)
{
	mama_status status = MAMA_STATUS_OK;
	const void* buffer = NULL;

    if (NULL == msg || NULL == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Proton provides no way to figure this out without serializing */
    status = qpidmsgPayload_serialize (msg, &buffer, size);

    return status;
}

mama_status
qpidmsgPayload_getNumFields (const msgPayload    msg,
                             mama_size_t*        numFields)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;

    if (NULL == msg || NULL == numFields)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *numFields = 0;

    /* Move to where the first field is entered */
    qpidmsgPayloadImpl_moveDataToContentLocation (impl->mBody);

    while (0 != pn_data_next (impl->mBody))
    {
        (*numFields)++;
    }

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getSendSubject (const msgPayload    msg,
                               const char**        subject)
{
    /*
     * Not implemented for now. Used by self describing messages and the qpid
     * payload is not self describing. We could add this capability later.
     */
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

const char*
qpidmsgPayload_toString (const msgPayload msg)
{
    qpidmsgPayloadImpl* impl      = (qpidmsgPayloadImpl*) msg;
    char*               bufPos    = NULL;
    int                 result    = 0;
    mama_size_t         bufferLen = QPID_BYTE_BUFFER_SIZE;
    mama_size_t         rootNodes = 0;

    if (NULL == impl)
    {
        return NULL;
    }

    /* Allocate buffers and pointer to increment */
    qpidmsgPayloadImpl_allocateBufferMemory (&impl->mBuffer,
                                             &impl->mBufferSize,
                                             bufferLen);

    bufPos = (char*) impl->mBuffer;

    pn_data_rewind (impl->mBody);

    /* pn_data_next returns a typedef bool, in which 0 is false */
    while (0 != pn_data_next (impl->mBody))
    {
        pn_atom_t atom  =  pn_data_get_atom (impl->mBody);
        result         +=  qpidmsgPayloadInternal_elementToString (impl->mBody,
                                                                   atom,
                                                                   bufPos,
                                                                   bufferLen);

        if (result > 0 && result <= bufferLen)
        {
            bufPos    += result;
            bufferLen -= result;
        }
        rootNodes++;
    }

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return (const char*) impl->mBuffer;
}

mama_status
qpidmsgPayload_iterateFields (const msgPayload    msg,
                              const mamaMsg       parent,
                              mamaMsgField        field,
                              mamaMsgIteratorCb   cb,
                              void*               closure)
{
    qpidmsgPayloadImpl* impl      = (qpidmsgPayloadImpl*) msg;
    mamaMsgFieldImpl*   mamaField = (mamaMsgFieldImpl*) field;

    if (   NULL == impl
        || NULL == parent
        || NULL == mamaField
        || NULL == cb)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl->mField->mParentBody = impl->mBody;

    /* Mark this message as being iterated */
    impl->mDataIteratorOffset = 0;

    /* Go to start of the message */
    pn_data_rewind   (impl->mBody);

    /* First entry is expected to be NULL, move onto next */
    pn_data_next     (impl->mBody);

    /* Enter into the main message */
    pn_data_get_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    while (0 != pn_data_next(impl->mBody))
    {
        mamaMsgFieldImpl_setPayload (mamaField, impl->mField);
        qpidmsgPayloadImpl_getFieldFromBuffer (impl->mBody, impl->mField);
        impl->mDataIteratorOffset++;
        cb (parent, field, closure);
    }

    pn_data_exit (impl->mBody);

    /* Mark the message as no longer being iterated */
    impl->mDataIteratorOffset = -1;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_serialize (const msgPayload  msg,
                          const void**      buffer,
                          mama_size_t*      bufferLength)
{
    qpidmsgPayloadImpl* impl       = (qpidmsgPayloadImpl*) msg;
    int                 err        = 0;
    mama_size_t         actualSize = 0;

    if (   NULL == msg
        || NULL == buffer
        || NULL == bufferLength)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Create buffer if it hasn't been created yet */
    qpidmsgPayloadImpl_allocateBufferMemory (&impl->mBuffer,
                                             &impl->mBufferSize,
                                             QPID_BYTE_BUFFER_SIZE);

    /*
     * Using the first byte of the buffer to store the a reference to the MAMA
     * payload type, so adjusting the actual size before and after, and passing
     * the second byte into pn_message_encode.
     */
    actualSize = impl->mBufferSize - 1;

    ((unsigned char*) (impl->mBuffer))[0] = (uint8_t) MAMA_PAYLOAD_QPID;

    err = pn_message_encode (impl->mQpidMsg,
                             (char*) impl->mBuffer + 1,
                             &actualSize);

    actualSize++;

    if (PN_OK != err)
    {
        *buffer       = NULL;
        *bufferLength = 0;

        return MAMA_STATUS_PLATFORM;
    }

    *buffer       = impl->mBuffer;
    *bufferLength = actualSize;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_unSerialize (const msgPayload    msg,
                            const void**        buffer,
                            mama_size_t         bufferLength)
{
    qpidmsgPayloadImpl*  impl       = (qpidmsgPayloadImpl*) msg;
    int                  err        = 0;

    /* Note the buffer is actually a void*, so recasting */
    void*                rawBuffer  = (void*) buffer;

    if (   NULL == msg
        || NULL == rawBuffer
        || 0    == bufferLength)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mQpidMsg)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    /*
     * Decode the payload message. First byte is a MAMA payload identifier, so
     * we ignore it, and start the decoding at the second byte (ensuring the buffer
     * length is also reduced by one.)
     */
    err = pn_message_decode (impl->mQpidMsg,
                             (char*) rawBuffer + 1,
                             bufferLength - 1);

    impl->mBody = pn_message_body (impl->mQpidMsg);

    if (PN_OK != err)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "setByteBuffer failed: %d", err);
        return MAMA_STATUS_PLATFORM;
    }
    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getByteBuffer (const msgPayload  msg,
                              const void**      buffer,
                              mama_size_t*      bufferLength)
{
    qpidmsgPayloadImpl*  impl = (qpidmsgPayloadImpl*) msg;

    if (   NULL == msg
        || NULL == buffer
        || 0    == bufferLength)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mQpidMsg)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *buffer         = (void*) impl->mQpidMsg;
    *bufferLength   = sizeof (pn_message_t*);

    return MAMA_STATUS_OK;
}

/*
 * Note this function sets an underlying implementation handle - not
 * necessarily a serialized version of the message.
 */
mama_status
qpidmsgPayload_setByteBuffer (const msgPayload    msg,
                              mamaPayloadBridge   bridge,
                              const void*         buffer,
                              mama_size_t         bufferLength)
{
    qpidmsgPayloadImpl*  impl       = (qpidmsgPayloadImpl*) msg;
    mama_status          status     = MAMA_STATUS_OK;

    if (   NULL == msg
        || NULL == buffer
        || 0    == bufferLength)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (bufferLength == sizeof(pn_message_t*))
    {
        impl->mQpidMsg = (pn_message_t*) buffer;
        impl->mBody    = pn_message_body (impl->mQpidMsg);
    }
    else
    {
        status = qpidmsgPayload_unSerialize (msg,
                                             (const void**)buffer,
                                             bufferLength);
    }

    return status;
}

mama_status
qpidmsgPayload_createFromByteBuffer (msgPayload*         msg,
                                     mamaPayloadBridge   bridge,
                                     const void*         buffer,
                                     mama_size_t         bufferLength)
{
    mama_status status = MAMA_STATUS_OK;

    if (0 == bufferLength)
    {
    	return MAMA_STATUS_INVALID_ARG;
    }

    // If this is a byte handle
    if (bufferLength == sizeof(pn_message_t*))
    {
        status = qpidmsgPayloadImpl_createImplementationOnly (msg);
    }
    else
    {
        status = qpidmsgPayload_create (msg);
    }
    if (MAMA_STATUS_OK == status)
    {
        status = qpidmsgPayload_setByteBuffer (*msg,
                                               bridge,
                                               buffer,
                                               bufferLength);
    }
    return status;

}

mama_status
qpidmsgPayload_apply (msgPayload          dest,
                      const msgPayload    src)
{
    /*
     * Iterate the src message, then for each field we come across, update the
     * equivalent field in the dest. The update calls should take care of creating
     * the field if they don't exist at present.
     */
    msgPayloadIter  iter    = NULL;
    msgFieldPayload field   = NULL;
    mamaFieldType   type    = MAMA_FIELD_TYPE_UNKNOWN;
    const char*     name    = NULL;
    uint16_t        fid     = 0;

    if (NULL == dest || NULL == src)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadIter_create (&iter, src);

    while (NULL != (field = qpidmsgPayloadIter_next (iter, NULL, src)))
    {
        qpidmsgFieldPayload_getType (field, &type);
        qpidmsgFieldPayload_getName (field, NULL, NULL, &name);
        qpidmsgFieldPayload_getFid  (field, NULL, NULL, &fid);

        switch (type) {
        case MAMA_FIELD_TYPE_BOOL:
        {
            UPDATE_SCALAR_FIELD (Bool, mama_bool_t);
            break;
        }
        case MAMA_FIELD_TYPE_CHAR:
        {
            UPDATE_SCALAR_FIELD (Char, char);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            UPDATE_SCALAR_FIELD (U8, mama_u8_t);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            UPDATE_SCALAR_FIELD (I8, mama_i8_t);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            UPDATE_SCALAR_FIELD (U16, mama_u16_t);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            UPDATE_SCALAR_FIELD (I16, mama_i16_t);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            UPDATE_SCALAR_FIELD (U32, mama_u32_t);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            UPDATE_SCALAR_FIELD (I32, mama_i32_t);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            UPDATE_SCALAR_FIELD (U64, mama_u64_t);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            UPDATE_SCALAR_FIELD (I64, mama_i64_t);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            UPDATE_SCALAR_FIELD (F32, mama_f32_t);
            break;
        }
        case MAMA_FIELD_TYPE_F64:
        {
            UPDATE_SCALAR_FIELD (F64, mama_f64_t);
            break;
        }
        case MAMA_FIELD_TYPE_STRING:
        {
            const char* result = NULL;

            qpidmsgFieldPayload_getString (field, &result);
            qpidmsgPayload_updateString   (dest, name, fid, result);
            break;
        }
        case MAMA_FIELD_TYPE_MSG:
        {
            /*
             * Slight difference in naming convention for get and update methods
             * so we don't use the define expansion here.
             */
            msgPayload result = NULL;

            qpidmsgFieldPayload_getMsg  (field, &result);
            qpidmsgPayload_updateSubMsg (dest, name, fid, result);
            break;
        }
        case MAMA_FIELD_TYPE_OPAQUE:
        {
            /*
             * Slight difference in function prototypes, so we don't use the
             * define expansion here.
             */
            const void* result  = NULL;
            mama_size_t size    = 0;

            qpidmsgFieldPayload_getOpaque (field, &result, &size);
            qpidmsgPayload_updateOpaque   (dest, name, fid, result, size);
            break;
        }
        case MAMA_FIELD_TYPE_TIME:
        {
            mamaDateTime result = NULL;

            mamaDateTime_create             (&result);
            qpidmsgFieldPayload_getDateTime (field, result);
            qpidmsgPayload_updateDateTime   (dest, name, fid, result);
            mamaDateTime_destroy            (result);

            break;
        }
        case MAMA_FIELD_TYPE_PRICE:
        {
            mamaPrice result = NULL;

            mamaPrice_create             (&result);
            qpidmsgFieldPayload_getPrice (field, result);
            qpidmsgPayload_updatePrice   (dest, name, fid, result);
            mamaPrice_destroy            (result);

            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            UPDATE_VECTOR_FIELD (U8, mama_u8_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_BOOL:
        {
            UPDATE_VECTOR_FIELD (Bool, mama_bool_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_CHAR:
        {
            UPDATE_VECTOR_FIELD (Char, char);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            UPDATE_VECTOR_FIELD (I8, mama_i8_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            UPDATE_VECTOR_FIELD (U16, mama_u16_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            UPDATE_VECTOR_FIELD (I16, mama_i16_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            UPDATE_VECTOR_FIELD (U32, mama_u32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            UPDATE_VECTOR_FIELD (I32, mama_i32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            UPDATE_VECTOR_FIELD (U64, mama_u64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            UPDATE_VECTOR_FIELD (I64, mama_i64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            UPDATE_VECTOR_FIELD (F32, mama_f32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            UPDATE_VECTOR_FIELD (F64, mama_f64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            UPDATE_VECTOR_FIELD (String, char*);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_MSG:
        {
            const mamaMsg*  result      = NULL;
            mama_size_t     size        = 0;
            mamaMsg         tempMsg     = NULL;

            /*
             * Take the source payload, then convert it to a byte buffer,
             * so we can create a mamaMsg from it.
             */
            qpidmsgPayloadImpl_payloadToMamaMsg ((qpidmsgPayloadImpl*) src,
				    &tempMsg);

            /*
             * This allows us to use the mamaMsg_get... calls, instead of native
             * qpidmsgPayload... (which will return the payloads rather than
             * messages)
             */
            mamaMsg_getVectorMsg (tempMsg, name, fid, &result, &size);

            /*
             * Then we can pass the array of mamaMsg's directly to update, rather
             * than having to build a new array ourselves.
             */
            qpidmsgPayload_updateVectorMsg (dest, name, fid, result, size);
            mamaMsg_destroy (tempMsg);

            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
        case MAMA_FIELD_TYPE_VECTOR_TIME:
        case MAMA_FIELD_TYPE_QUANTITY:
        case MAMA_FIELD_TYPE_UNKNOWN:
        default:
            break;
        }
    }

    qpidmsgPayloadIter_destroy (iter);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getNativeMsg (const msgPayload    msg,
                             void**              nativeMsg)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*) msg;

    if (NULL == impl || NULL == nativeMsg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *nativeMsg = impl->mQpidMsg;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getFieldAsString (const msgPayload    msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 char*               buffer,
                                 mama_size_t         len)
{
    qpidmsgPayloadImpl* impl    = (qpidmsgPayloadImpl*) msg;
    mama_status         status  = MAMA_STATUS_OK;
    mama_size_t         written = 0;
    pn_atom_t           atom;

    if (NULL == impl || NULL == buffer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (0 == len)
    {
    	return MAMA_STATUS_INVALID_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    pn_data_next (impl->mBody);

    atom = pn_data_get_atom (impl->mBody);

    written = qpidmsgPayloadInternal_elementToString (impl->mBody,
                                                      atom,
                                                      buffer, 
                                                      len);

    if (0 == written)
    {
        buffer[0] = '\0';
    }

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return status;
}

mama_status
qpidmsgPayload_addBool (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_bool_t value)
{
    ADD_SCALAR (_bool);
}

mama_status
qpidmsgPayload_addChar (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        char        value)
{
    ADD_SCALAR (_char);
}

mama_status
qpidmsgPayload_addI8   (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_i8_t   value)
{
    ADD_SCALAR (_byte);
}

mama_status
qpidmsgPayload_addU8   (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_u8_t   value)
{
    ADD_SCALAR (_ubyte);
}

mama_status
qpidmsgPayload_addI16  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_i16_t  value)
{
    ADD_SCALAR (_short);
}

mama_status
qpidmsgPayload_addU16  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_u16_t  value)
{
    ADD_SCALAR (_ushort);
}

mama_status
qpidmsgPayload_addI32  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_i32_t  value)
{
    ADD_SCALAR (_int);
}

mama_status
qpidmsgPayload_addU32  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_u32_t  value)
{
    ADD_SCALAR (_uint);
}

mama_status
qpidmsgPayload_addI64  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_i64_t  value)
{
    ADD_SCALAR (_long);
}

mama_status
qpidmsgPayload_addU64  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_u64_t  value)
{
    ADD_SCALAR (_ulong);
}

mama_status
qpidmsgPayload_addF32  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_f32_t  value)
{
    ADD_SCALAR (_float);
}

mama_status
qpidmsgPayload_addF64  (msgPayload  msg,
                        const char* name,
                        mama_fid_t  fid,
                        mama_f64_t  value)
{
    ADD_SCALAR (_double);
}

mama_status
qpidmsgPayload_addString (msgPayload  msg,
                          const char* name,
                          mama_fid_t  fid,
                          const char* str)
{
    pn_bytes_t value;

    /* Pass Proton strings as a pn_bytes_t struct */
    if (NULL == str)
    {
        return MAMA_STATUS_NULL_ARG;
        value.size  = 1;
        value.start = "\0";
    }
    else
    {
        value.size  = strlen (str);
        value.start = (char*) str;
    }

    ADD_SCALAR (_string);
}

mama_status
qpidmsgPayload_addOpaque (msgPayload  msg,
                          const char* name,
                          mama_fid_t  fid,
                          const void* opaque,
                          mama_size_t size)
{
    pn_bytes_t value;

    if (NULL == opaque)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Pass Proton opaque data as a pn_bytes_t struct */
    value.size  = size;
    value.start = (char*) opaque;

    ADD_SCALAR (_binary);
}

mama_status
qpidmsgPayload_addDateTime (msgPayload          msg,
                            const char*         name,
                            mama_fid_t          fid,
                            const mamaDateTime  value)
{
    qpidmsgPayloadImpl*     impl         = (qpidmsgPayloadImpl*) msg;
    mama_u32_t              seconds      = 0;
    mama_u32_t              micros       = 0;
    mamaDateTimeHints       hints        = 0;
    mamaDateTimePrecision   precision    = MAMA_DATE_TIME_PREC_UNKNOWN;
    pn_timestamp_t          stamp        = 0;

    if (NULL == value || NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);

    /* Each field is a separate list of name | FID | value */
    pn_data_put_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    /* strlen +1 to encode NULL terminator */
    if (NULL == name)
    {
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));
    }
    else
    {
        pn_data_put_string (impl->mBody, pn_bytes (strlen (name) + 1,
                                                   (char*) name));
    }

    /* add in the fid */
    pn_data_put_ushort (impl->mBody, fid);

    mamaDateTime_getWithHints (value,
                               &seconds,
                               &micros,
                               &precision,
                               &hints);

    /*
     * The timestamp is simply 64 bits of data. Place seconds in leftmost and
     * microseconds in rightmost 32 bits of format. Expected to be faster than
     * multiplication.
     */
    stamp = micros | ((mama_u64_t) seconds << 32);

    /* add the price value */
    pn_data_put_timestamp (impl->mBody, stamp);

    /* add the hints value */
    pn_data_put_ubyte     (impl->mBody, (mama_u8_t) hints);

    /* add the precision value */
    pn_data_put_ubyte     (impl->mBody, (mama_u8_t) precision);

    /* Exit list */
    pn_data_exit          (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_addPrice (msgPayload      msg,
                         const char*     name,
                         mama_fid_t      fid,
                         const mamaPrice value)
{
    qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*) msg;
    double              priceValue  = 0;
    mamaPriceHints      priceHints  = 0;

    if (NULL == impl || NULL == value)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);

    /* Each field is a separate list of name | FID | value */
    pn_data_put_list   (impl->mBody);
    pn_data_enter      (impl->mBody);

    /* strlen +1 to encode NULL terminator */
    if (NULL == name)
    {
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));
    }
    else
    {
        pn_data_put_string (impl->mBody, pn_bytes (strlen (name) + 1,
                                                   (char*) name));
    }

    /* add in the fid */
    pn_data_put_ushort (impl->mBody, fid);

    mamaPrice_getValue (value, &priceValue);
    mamaPrice_getHints (value, &priceHints);

    /* add the price value */
    pn_data_put_double  (impl->mBody, priceValue);

    /* add the hints value */
    pn_data_put_ubyte   (impl->mBody, (mama_u8_t)priceHints);

    /* Exit list */
    pn_data_exit        (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_addMsg (msgPayload  msg,
                       const char* name,
                       mama_fid_t  fid,
                       msgPayload  value)
{
    qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*) msg;
    mama_status         status      = MAMA_STATUS_OK;
    mamaMsg             tmpMsg      = NULL;

    if (NULL == impl || NULL == value)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);

    /* Each field is a separate list of name | FID | value */
    pn_data_put_list   (impl->mBody);
    pn_data_enter      (impl->mBody);

    /* strlen +1 to encode NULL terminator */
    if (NULL == name)
    {
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));
    }
    else
    {
        pn_data_put_string (impl->mBody, pn_bytes (strlen (name) + 1,
                                                   (char*) name));
    }

    /* add in the fid */
    pn_data_put_ushort (impl->mBody, fid);
    pn_data_put_list   (impl->mBody);
    pn_data_enter      (impl->mBody);

    /*
     * addBareMsg expects a mamaMsg, so we must detach payload to create one
     * here
     */

    status = qpidmsgPayloadImpl_payloadToMamaMsg ((qpidmsgPayloadImpl*) value,
		             &tmpMsg);

    qpidmsgPayloadImpl_addBareMsg (msg, tmpMsg);

    /* finished with the temporary message - destroy */
    mamaMsg_destroy (tmpMsg);

    /* Exit list */
    pn_data_exit (impl->mBody);
    pn_data_exit (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return status;
}

mama_status
qpidmsgPayload_addVectorBool (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_bool_t   value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_bool, _BOOL);
}

mama_status
qpidmsgPayload_addVectorChar (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const char          value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_char, _CHAR);
}

mama_status
qpidmsgPayload_addVectorI8   (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_i8_t     value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_byte,_BYTE);
}

mama_status
qpidmsgPayload_addVectorU8   (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_u8_t     value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_ubyte, _UBYTE);
}

mama_status
qpidmsgPayload_addVectorI16  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_i16_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_short, _SHORT);
}

mama_status
qpidmsgPayload_addVectorU16  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_u16_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_ushort, _USHORT);
}

mama_status
qpidmsgPayload_addVectorI32  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_i32_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_int, _INT);
}

mama_status
qpidmsgPayload_addVectorU32  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_u32_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_uint, _UINT);
}

mama_status
qpidmsgPayload_addVectorI64  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_i64_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_long, _LONG);
}

mama_status
qpidmsgPayload_addVectorU64  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_u64_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_ulong, _ULONG);
}

mama_status
qpidmsgPayload_addVectorF32  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_f32_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_float, _FLOAT);
}

mama_status
qpidmsgPayload_addVectorF64  (msgPayload          msg,
                              const char*         name,
                              mama_fid_t          fid,
                              const mama_f64_t    value[],
                              mama_size_t         size)
{
    ADD_VECTOR (_double, _DOUBLE);
}

mama_status
qpidmsgPayload_addVectorString (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char*         value[],
                                mama_size_t         size)
{


    mama_size_t         i           = 0;
    qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*) msg;
    pn_bytes_t          tmpPnBytes;

    if (NULL == impl || 0 == size || NULL == value 
            || (NULL == name && 0 == fid))
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);

    pn_data_put_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    /* strlen +1 to encode NULL terminator */
    if (NULL == name)
    {
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));
    }
    else
    {
        pn_data_put_string (impl->mBody, pn_bytes (strlen (name) + 1,
                                                   (char*) name));
    }

    pn_data_put_ushort (impl->mBody, fid);
    pn_data_put_array  (impl->mBody, 0, PN_STRING);
    pn_data_enter      (impl->mBody);

    for (i = 0; i != size; i++)
    {
        if(NULL == value[i])
        {
            tmpPnBytes.size  = 1;
            tmpPnBytes.start = "\0";
        }
        else
        {
            tmpPnBytes.size  = strlen (value[i]);
            tmpPnBytes.start = (char*) value[i];
        }
        pn_data_put_string (impl->mBody, tmpPnBytes);
    }

    pn_data_exit (impl->mBody);
    pn_data_exit (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_addVectorMsg (msgPayload          msg,
                             const char*         name,
                             mama_fid_t          fid,
                             const mamaMsg       value[],
                             mama_size_t         size)
{
    mama_size_t          i    = 0;
    qpidmsgPayloadImpl*  impl = (qpidmsgPayloadImpl*) msg;

    if (NULL == impl || 0 == size || NULL == value
            || (NULL == name && 0 == fid))
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_moveDataToInsertLocation (impl->mBody, impl);

    pn_data_put_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    /* strlen +1 to encode NULL terminator */
    if(NULL == name)
    {
        pn_data_put_string (impl->mBody, pn_bytes (1, "\0"));
    }
    else
    {
        pn_data_put_string (impl->mBody,
                            pn_bytes (strlen (name) + 1, (char*) name));
    }

    pn_data_put_ushort (impl->mBody, fid);

    /* Create an array of lists */
    pn_data_put_array  (impl->mBody, 0, PN_LIST);
    pn_data_enter      (impl->mBody);

    for (i=0; i != size; i++)
    {
        pn_data_put_list   (impl->mBody);
        pn_data_enter      (impl->mBody);

        qpidmsgPayloadImpl_addBareMsg (msg, value[i]);

        pn_data_exit (impl->mBody);

    }

    pn_data_exit (impl->mBody);
    pn_data_exit (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

/*
 * Postponing implementation until this type of vectors has a standard protocol
 * or is removed from the implementation
 */
mama_status
qpidmsgPayload_addVectorDateTime (msgPayload          msg,
                                  const char*         name,
                                  mama_fid_t          fid,
                                  const mamaDateTime  value[],
                                  mama_size_t         size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

/*
 * Postponing implementation until this type of vectors has a standard protocol
 * or is removed from the implementation
 */
mama_status
qpidmsgPayload_addVectorPrice (msgPayload      msg,
                               const char*     name,
                               mama_fid_t      fid,
                               const mamaPrice value[],
                               mama_size_t     size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgPayload_updateBool   (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_bool_t  value)
{
    UPDATE_SCALAR (_bool, Bool);
}

mama_status
qpidmsgPayload_updateChar   (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             char         value)
{
    UPDATE_SCALAR (_char, Char);
}

mama_status
qpidmsgPayload_updateI8     (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_i8_t    value)
{
    UPDATE_SCALAR (_byte, I8);
}

mama_status
qpidmsgPayload_updateU8     (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_u8_t    value)
{
    UPDATE_SCALAR (_ubyte, U8);
}

mama_status
qpidmsgPayload_updateI16    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_i16_t   value)
{
    UPDATE_SCALAR (_short, I16);
}

mama_status
qpidmsgPayload_updateU16    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_u16_t   value)
{
    UPDATE_SCALAR (_ushort, U16);
}

mama_status
qpidmsgPayload_updateI32    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_i32_t   value)
{
    UPDATE_SCALAR (_int, I32);
}

mama_status
qpidmsgPayload_updateU32    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_u32_t   value)
{
    UPDATE_SCALAR (_uint, U32);
}

mama_status
qpidmsgPayload_updateI64    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_i64_t   value)
{
    UPDATE_SCALAR (_long, I64);
}

mama_status
qpidmsgPayload_updateU64    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_u64_t   value)
{
    UPDATE_SCALAR (_ulong, U64);
}

mama_status
qpidmsgPayload_updateF32    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_f32_t   value)
{
    UPDATE_SCALAR (_float, F32);
}

mama_status
qpidmsgPayload_updateF64    (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             mama_f64_t   value)
{
    UPDATE_SCALAR (_double, F64);
}

mama_status
qpidmsgPayload_updateString (msgPayload   msg,
                             const char*  name,
                             mama_fid_t   fid,
                             const char*  str)
{
    qpidmsgPayloadImpl* impl    = (qpidmsgPayloadImpl*) msg;
    mama_status         status  = MAMA_STATUS_OK;
    pn_bytes_t          value;

    /* Pass Proton strings as a pn_bytes_t struct */
    if (NULL == str)
    {
        return MAMA_STATUS_NULL_ARG;
        value.size  = 1;
        value.start = "\0";
    }
    else
    {
        value.size  = strlen(str);
        value.start = (char*)str;
    }

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addString (msg, name, fid, str);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Store value */
    pn_data_put_string (impl->mBody, value);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;

}

mama_status
qpidmsgPayload_updateOpaque (msgPayload          msg,
                             const char*         name,
                             mama_fid_t          fid,
                             const void*         opaque,
                             mama_size_t         size)
{
    qpidmsgPayloadImpl* impl    = (qpidmsgPayloadImpl*) msg;
    mama_status         status  = MAMA_STATUS_OK;
    pn_bytes_t          value;

    if (opaque == NULL || NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Pass Proton opaque data as a pn_bytes_t struct */
    value.size  = size;
    value.start = (char*) opaque;

    impl   = (qpidmsgPayloadImpl*) msg;

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addOpaque (msg, name, fid, opaque, size);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Store value */
    pn_data_put_binary (impl->mBody, value);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return status; /* Should be == MAMA_STATUS_OK at this point */
}

mama_status
qpidmsgPayload_updateDateTime (msgPayload          msg,
                               const char*         name,
                               mama_fid_t          fid,
                               const mamaDateTime  value)
{
    qpidmsgPayloadImpl*     impl            = (qpidmsgPayloadImpl*) msg;
    mama_status             status          = MAMA_STATUS_OK;
    mama_u32_t              seconds         = 0;
    mama_u32_t              micros          = 0;
    mamaDateTimeHints       hints           = 0;
    mamaDateTimePrecision   precision       = MAMA_DATE_TIME_PREC_UNKNOWN;
    pn_timestamp_t          stamp           = 0;

    if (NULL == impl || NULL == value)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addDateTime (msg, name, fid, value);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    mamaDateTime_getWithHints (value,
                               &seconds,
                               &micros,
                               &precision,
                               &hints);

    /*
     * The timestamp is simply 64 bits of data. Place seconds in leftmost and
     * microseconds in rightmost 32 bits of format. Expected to be faster than
     * multiplication.
     */
    stamp = micros | ((mama_u64_t) seconds << 32);

    /* add the price value */
    pn_data_put_timestamp (impl->mBody, stamp);

    /* add the hints value */
    pn_data_put_ubyte     (impl->mBody, (mama_u8_t) hints);

    /* add the precision value */
    pn_data_put_ubyte     (impl->mBody, (mama_u8_t) precision);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_updatePrice (msgPayload          msg,
                            const char*         name,
                            mama_fid_t          fid,
                            const mamaPrice     value)
{
    qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*) msg;
    mama_status         status      = MAMA_STATUS_OK;
    double              priceValue  = 0;
    mamaPriceHints      priceHints  = 0;

    if (NULL == impl || NULL == value)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addPrice (msg, name, fid, value);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    mamaPrice_getValue (value, &priceValue);
    mamaPrice_getHints (value, &priceHints);
    /* add the price value */
    pn_data_put_double (impl->mBody, priceValue);

    /* add the hints value */
    pn_data_put_ubyte  (impl->mBody, (mama_u8_t) priceHints);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_updateSubMsg (msgPayload          msg,
                             const char*         name,
                             mama_fid_t          fid,
                             const msgPayload    subMsg)
{
    qpidmsgPayloadImpl* impl    = (qpidmsgPayloadImpl*) msg;

    /* Despite the prototype, subMsg seems to be a mamaMsg, so treat as such */
    mamaMsg             valMsg  = (mamaMsg) subMsg;
    mama_status         status  = MAMA_STATUS_OK;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field - takes us directly to the content */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addMsg (msg, name, fid, subMsg);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Put a new list in place of the old one */
    pn_data_put_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    status = qpidmsgPayloadImpl_addBareMsg (msg, valMsg);

    pn_data_exit     (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return status;
}

mama_status
qpidmsgPayload_updateVectorMsg (msgPayload          msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mamaMsg       value[],
                                mama_size_t         size)
{
    qpidmsgPayloadImpl  *impl   = (qpidmsgPayloadImpl*) msg;
    mama_status         status  = MAMA_STATUS_OK;
    mama_size_t         i       = 0;

    if (NULL == impl || NULL == value || 0 == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == name && 0 == fid)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field - takes us directly to the content */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addVectorMsg (msg, name, fid, value, size);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Store value */
    pn_data_put_array (impl->mBody, 0, PN_LIST);
    pn_data_enter     (impl->mBody);

    for (; i != size; i++)
    {
        pn_data_put_list (impl->mBody);
        pn_data_enter    (impl->mBody);

        qpidmsgPayloadImpl_addBareMsg (msg, value[i]);

        pn_data_exit (impl->mBody);
    }

    /* exit array */
    pn_data_exit (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_updateVectorString (msgPayload   msg,
                                   const char*  name,
                                   mama_fid_t   fid,
                                   const char*  value[],
                                   mama_size_t  size)
{
    qpidmsgPayloadImpl*  impl   = (qpidmsgPayloadImpl*) msg;
    mama_status          status  = MAMA_STATUS_OK;
    mama_size_t          i       = 0;

    if (NULL == impl || NULL == value || 0 == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == name && fid == 0)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field - takes us directly to the content */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    /* Create if this does not exist */
    if (MAMA_STATUS_NOT_FOUND == status)
    {
        return qpidmsgPayload_addVectorString (msg, name, fid, value, size);
    }
    else if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Store value */
    pn_data_put_array (impl->mBody, 0, PN_STRING);
    pn_data_enter     (impl->mBody);

    for (; i != size; i++)
    {
        pn_bytes_t pn_value;

        /* Pass Proton strings as a pn_bytes_t struct */
        pn_value.size  = strlen (value[i]);
        pn_value.start = (char*) value[i];

        /* add value to array element*/
        pn_data_put_string (impl->mBody, pn_value);
    }

    /* exit array */
    pn_data_exit (impl->mBody);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_updateVectorBool (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_bool_t   value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_bool,_BOOL, Bool);
}

mama_status
qpidmsgPayload_updateVectorChar (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const char          value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_char,_CHAR, Char);
}

mama_status
qpidmsgPayload_updateVectorI8   (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i8_t     value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_byte,_BYTE, I8);
}

mama_status
qpidmsgPayload_updateVectorU8   (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u8_t     value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_ubyte,_UBYTE, U8);
}

mama_status
qpidmsgPayload_updateVectorI16  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i16_t    value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_short,_SHORT, I16);
}

mama_status
qpidmsgPayload_updateVectorU16  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u16_t    value[],
                                 mama_size_t         size)
{
	UPDATE_VECTOR (_ushort,_USHORT, U16);
}

mama_status
qpidmsgPayload_updateVectorI32  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i32_t    value[],
                                 mama_size_t         size)
{
	UPDATE_VECTOR (_int,_INT, I32);
}

mama_status
qpidmsgPayload_updateVectorU32  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u32_t    value[],
                                 mama_size_t         size)
{
	UPDATE_VECTOR (_uint,_UINT, U32);
}

mama_status
qpidmsgPayload_updateVectorI64  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_i64_t    value[],
                                 mama_size_t         size)
{
	UPDATE_VECTOR (_long,_LONG, I64);
}

mama_status
qpidmsgPayload_updateVectorU64  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_u64_t    value[],
                                 mama_size_t         size)
{
	UPDATE_VECTOR (_ulong,_ULONG, U64);
}

mama_status
qpidmsgPayload_updateVectorF32  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f32_t    value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_float,_FLOAT, F32);
}

mama_status
qpidmsgPayload_updateVectorF64  (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mama_f64_t    value[],
                                 mama_size_t         size)
{
    UPDATE_VECTOR (_double,_DOUBLE, F64);
}

mama_status
qpidmsgPayload_updateVectorPrice (msgPayload          msg,
                                  const char*         name,
                                  mama_fid_t          fid,
                                  const mamaPrice*    value[],
                                  mama_size_t         size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgPayload_updateVectorTime (msgPayload          msg,
                                 const char*         name,
                                 mama_fid_t          fid,
                                 const mamaDateTime  value[],
                                 mama_size_t         size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


mama_status
qpidmsgPayload_getBool (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_bool_t*        result)
{
    GET_SCALAR (_bool, mama_bool_t);
}

mama_status
qpidmsgPayload_getChar (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        char*               result)
{
    GET_SCALAR (_char, char);
}

mama_status
qpidmsgPayload_getI8   (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_i8_t*          result)
{
    GET_SCALAR (_byte, mama_i8_t);
}

mama_status
qpidmsgPayload_getU8   (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_u8_t*          result)
{
    GET_SCALAR (_ubyte, mama_u8_t);
}

mama_status
qpidmsgPayload_getI16  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_i16_t*         result)
{
    GET_SCALAR (_short, mama_i16_t);
}

mama_status
qpidmsgPayload_getU16  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_u16_t*         result)
{
    GET_SCALAR (_ushort, mama_u16_t);
}

mama_status
qpidmsgPayload_getI32  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_i32_t*         result)
{
    GET_SCALAR (_int, mama_i32_t);
}

mama_status
qpidmsgPayload_getU32  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_u32_t*         result)
{
    GET_SCALAR (_uint, mama_u32_t);
}

mama_status
qpidmsgPayload_getI64  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_i64_t*         result)
{
    GET_SCALAR (_long, mama_i64_t);
}

mama_status
qpidmsgPayload_getU64  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_u64_t*         result)
{
    GET_SCALAR (_ulong, mama_u64_t);
}

mama_status
qpidmsgPayload_getF32  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_f32_t*         result)
{
    GET_SCALAR (_float, mama_f32_t);
}

mama_status
qpidmsgPayload_getF64  (const msgPayload    msg,
                        const char*         name,
                        mama_fid_t          fid,
                        mama_f64_t*         result)
{
    GET_SCALAR (_double, mama_f64_t);
}

mama_status
qpidmsgPayload_getString (const msgPayload    msg,
                          const char*         name,
                          mama_fid_t          fid,
                          const char**        result)
{
    qpidmsgPayloadImpl*  impl    = (qpidmsgPayloadImpl*) msg;
    mama_status          status  = MAMA_STATUS_OK;
    pn_bytes_t           bytes;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Move onto value and extract */
    pn_data_next (impl->mBody);
    bytes = pn_data_get_string (impl->mBody);

    /* Populate result */
    *result = bytes.start;

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getOpaque (const msgPayload    msg,
                          const char*         name,
                          mama_fid_t          fid,
                          const void**        result,
                          mama_size_t*        size)
{
    qpidmsgPayloadImpl*  impl    = (qpidmsgPayloadImpl*) msg;
    mama_status          status  = MAMA_STATUS_OK;
    pn_bytes_t           bytes;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Move onto value and extract */
    pn_data_next (impl->mBody);
    bytes = pn_data_get_binary (impl->mBody);

    /* Populate result */
    *result = bytes.start;
    *size   = bytes.size;

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getDateTime (const msgPayload    msg,
                            const char*         name,
                            mama_fid_t          fid,
                            mamaDateTime        result)
{
    qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*) msg;
    mama_status         status      = MAMA_STATUS_OK;
    pn_timestamp_t      stamp       = 0;
    mama_u8_t           hints       = 0;
    mama_u8_t           precision   = 0;
    mama_u32_t          micros      = 0;
    mama_u32_t          seconds     = 0;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Step into the field */
    pn_data_next (impl->mBody);

    /* Pull out the 64 bit time stamp */
    stamp = pn_data_get_atom (impl->mBody).u.as_timestamp;

    /* Step into the next field */
    pn_data_next (impl->mBody);

    /* Extract the hints */
    hints = pn_data_get_atom (impl->mBody).u.as_ubyte;

    /* Step into the next field */
    pn_data_next (impl->mBody);

    /* Extract the precision */
    precision = pn_data_get_atom (impl->mBody).u.as_ubyte;

    /* Perform casts / bitwise operators to extract timestamps */
    micros  = (mama_u32_t) stamp;
    seconds   = (mama_u32_t) (stamp >> 32);

    mamaDateTime_setWithHints (result,
                               seconds,
                               micros,
                               (mamaDateTimePrecision) precision,
                               hints);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getPrice (const msgPayload    msg,
                         const char*         name,
                         mama_fid_t          fid,
                         mamaPrice           result)
{
    qpidmsgPayloadImpl* impl            = (qpidmsgPayloadImpl*)msg;
    mama_status         status          = MAMA_STATUS_OK;
    double              value           = 0;
    mama_u8_t           displayHints    = 0;
    pn_atom_t           pnValue;
    pn_atom_t           pnHints;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Move onto value and extract */
    pn_data_next (impl->mBody);

    /* Value will be the first atom */
    pnValue = pn_data_get_atom (impl->mBody);

    /* then comes the hints */
    pn_data_next (impl->mBody);
    pnHints = pn_data_get_atom (impl->mBody);

    /* Map proton types to primitives */
    value        = pnValue.u.as_double;
    displayHints = pnHints.u.as_ubyte;

    /* Update the provided price object */
    mamaPrice_setValue (result, value);
    mamaPrice_setHints (result, displayHints);

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getMsg (const msgPayload    msg,
                       const char*         name,
                       mama_fid_t          fid,
                       msgPayload*         result)
{
    qpidmsgPayloadImpl*  impl       = (qpidmsgPayloadImpl*) msg;
    mama_status          status     = MAMA_STATUS_OK;
    msgPayload           childMsg   = NULL;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field - takes us directly to the content */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    status = qpidmsgPayload_create (&childMsg);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidmsgPayload_getMsg(): "
                  "Create child message failed.");
        return status;
    }

    pn_data_next     (impl->mBody);
    pn_data_get_atom (impl->mBody);
    pn_data_get_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    status = qpidmsgPayloadImpl_getMessageFromBuffer (impl->mBody,
		             (qpidmsgPayloadImpl*) childMsg);

    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidmsgPayload_getMsg(): Could not get message (%s).",
                  mamaStatus_stringForStatus (status));
        return status;
    }

    *result = childMsg;

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getVectorBool   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_bool_t** result,
                                mama_size_t*        size)
{
    GET_VECTOR (_bool, mama_bool_t);
}

mama_status
qpidmsgPayload_getVectorChar   (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char**        result,
                                mama_size_t*        size)
{
    GET_VECTOR (_char, char);
}

mama_status
qpidmsgPayload_getVectorI8     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i8_t**   result,
                                mama_size_t*        size)
{
    GET_VECTOR (_byte, mama_i8_t);
}

mama_status
qpidmsgPayload_getVectorU8     (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u8_t**   result,
                                mama_size_t*        size)
{
    GET_VECTOR (_ubyte, mama_u8_t);
}

mama_status
qpidmsgPayload_getVectorI16    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i16_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_short, mama_i16_t);
}

mama_status
qpidmsgPayload_getVectorU16    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u16_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_ushort, mama_u16_t);
}

mama_status
qpidmsgPayload_getVectorI32    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i32_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_int, mama_i32_t);
}

mama_status
qpidmsgPayload_getVectorU32    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u32_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_uint, mama_u32_t);
}

mama_status
qpidmsgPayload_getVectorI64    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_i64_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_long, mama_i64_t);
}

mama_status
qpidmsgPayload_getVectorU64    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_u64_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_ulong, mama_u64_t);
}

mama_status
qpidmsgPayload_getVectorF32    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f32_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_float, mama_f32_t);
}

mama_status
qpidmsgPayload_getVectorF64    (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const mama_f64_t**  result,
                                mama_size_t*        size)
{
    GET_VECTOR (_double, mama_f64_t);
}

mama_status
qpidmsgPayload_getVectorString (const msgPayload    msg,
                                const char*         name,
                                mama_fid_t          fid,
                                const char***       result,
                                mama_size_t*        size)
{
    const char**         temp    = NULL;
    mama_size_t          i       = 0;
    qpidmsgPayloadImpl*  impl    = (qpidmsgPayloadImpl*) msg;
    mama_status          status  = MAMA_STATUS_OK;

    if (NULL == impl || NULL == result || NULL == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Move onto value[] */
    pn_data_next (impl->mBody);

    /* get size of array */
    *size  = pn_data_get_array (impl->mBody);

    /* allocate space for resulting array*/
    qpidmsgPayloadImpl_allocateBufferMemory (&impl->mBuffer,
                                             &impl->mBufferSize,
                                             (*size) * sizeof (char*));
    temp = (const char**) impl->mBuffer;

    /* enter array */
    pn_data_enter (impl->mBody);

    for (; i != *size; ++i)
    {
        /* go to next list element */
        pn_data_next (impl->mBody);

        /* get value of list element and store in result array*/
        temp[i] = pn_data_get_string (impl->mBody).start;
    }
    /* exit array */
    pn_data_exit (impl->mBody);

    /* exit field */
    pn_data_exit (impl->mBody);

    *result = temp;

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayload_getVectorDateTime (const msgPayload    msg,
                                  const char*         name,
                                  mama_fid_t          fid,
                                  const mamaDateTime* result,
                                  mama_size_t*        size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgPayload_getVectorPrice (const msgPayload    msg,
                               const char*         name,
                               mama_fid_t          fid,
                               const mamaPrice*    result,
                               mama_size_t*        size)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgPayload_getVectorMsg (const msgPayload    msg,
                             const char*         name,
                             mama_fid_t          fid,
                             const msgPayload**  result,
                             mama_size_t*        size)
{
    qpidmsgPayloadImpl*  impl   = (qpidmsgPayloadImpl*) msg;
    const msgPayload*    temp   = NULL;
    mama_size_t          i      = 0;

    mama_status status = MAMA_STATUS_OK;

    if (NULL == impl || NULL == result || NULL == size )
    {
        return MAMA_STATUS_NULL_ARG;
    }

     /* Find field */
     status = qpidmsgPayloadImpl_findField (impl, name, fid);

     if (MAMA_STATUS_OK != status)
     {
         return status;
     }

     /* Move onto value[] */
     pn_data_next (impl->mBody);

     /* get size of array */
     *size  = pn_data_get_array (impl->mBody);

     /* allocate space for resulting array*/
     qpidmsgPayloadImpl_allocateBufferMemory (
             &impl->mBuffer,
             &impl->mBufferSize,
             *size * sizeof (qpidmsgPayloadImpl**));

     temp = (msgPayload*) impl->mBuffer;

     /* enter array */
     pn_data_enter (impl->mBody);

     for (; i != *size; ++i)
     {
         /* go to next list element */
         pn_data_next (impl->mBody);

         /* Messages themselves are lists - step inside */
         pn_data_get_list (impl->mBody);
         pn_data_enter    (impl->mBody);

         status = qpidmsgPayload_create ((msgPayload*) &(temp[i]));
         if (MAMA_STATUS_OK != status)
         {
             mama_log (MAMA_LOG_LEVEL_ERROR, 
                       "qpidmsgPayload_getVectorMsg(): "
                       "Failed to crete temp message.");
             return status;
         }

         /* parse the payload to populate the message */
         qpidmsgPayloadImpl_getMessageFromBuffer (impl->mBody,
	             (qpidmsgPayloadImpl*) temp[i]);

         pn_data_exit (impl->mBody);
     }
     /* exit array */
     pn_data_exit (impl->mBody);

     /* exit field */
     pn_data_exit (impl->mBody);

     *result = temp;

     /* Revert to the previous iterator state if applicable */
     qpidmsgPayloadImpl_resetToIteratorState (impl);

     return status;
}

mama_status
qpidmsgPayload_getField (const msgPayload    msg,
                         const char*         name,
                         mama_fid_t          fid,
                         msgFieldPayload*    result)
{
    qpidmsgPayloadImpl*  impl    = (qpidmsgPayloadImpl*) msg;
    mama_status          status  = MAMA_STATUS_OK;
    msgFieldPayload      field   = NULL;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Find field */
    status = qpidmsgPayloadImpl_findField (impl, name, fid);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Find field leaves you half way through the field, so exit and reenter
     * to be in the position which getFieldFromBuffer expects. */
    pn_data_exit     (impl->mBody);
    pn_data_get_list (impl->mBody);
    pn_data_enter    (impl->mBody);

    field = impl->mField;

    /* Get the buffer and translate it to a field */
    qpidmsgPayloadImpl_getFieldFromBuffer (impl->mBody, impl->mField);

    impl->mField->mParentBody = impl->mBody;

    *result = field;

    /* Revert to the previous iterator state if applicable */
    qpidmsgPayloadImpl_resetToIteratorState (impl);

    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                   Public implementation functions                     =
  =========================================================================*/

mama_status
qpidmsgPayloadImpl_allocateBufferMemory (void**       buffer,
                                         mama_size_t* size,
                                         mama_size_t  newSize)
{
    void* newbuf = NULL;

    if (NULL == buffer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (0 == *size || NULL == *buffer)
    {
        newbuf = calloc (newSize, 1);

        if (newbuf == NULL)
        {
            mama_log (MAMA_LOG_LEVEL_SEVERE, "qpidmsgPayloadImpl_allocateBufferMemory(): "
                      "Could not allocate memory for buffer of size [%u]",
                      newSize, *size);
            return MAMA_STATUS_NOMEM;
        }
        else
        {
            *buffer = newbuf;
            *size   = newSize;
            return MAMA_STATUS_OK;
        }
    }
    else if (newSize > *size)
    {
        newbuf = realloc (*buffer, newSize);

        if (newbuf == NULL)
        {
            mama_log (MAMA_LOG_LEVEL_SEVERE,
                      "qpidmsgPayloadImpl_allocateBufferMemory(): "
                      "Could not reallocate memory for buffer of size [%u]"
                      "(was: %u).",
                      newSize,
                      *size);
            return MAMA_STATUS_NOMEM;
        }
        else
        {
            /* set newly added bytes to 0 */
            memset ((uint8_t*) newbuf + *size, 0, newSize - *size);

            *buffer = newbuf;
            *size   = newSize;
            return MAMA_STATUS_OK;
        }
    }
    else
    {
        /* No allocation / reallocation required */
        return MAMA_STATUS_OK;
    }
}

mama_status
qpidmsgPayloadImpl_getFieldFromBuffer (pn_data_t*               buffer,
                                       qpidmsgFieldPayloadImpl* target)
{
    mama_status status = MAMA_STATUS_OK;

    /* Enter field - stored as a list of Name, FID, data*/
    pn_data_get_list (buffer);
    pn_data_enter (buffer);

    /* Name */
    pn_data_next (buffer);
    target->mName = pn_data_get_string (buffer);

    /* FID */
    pn_data_next (buffer);
    target->mFid  = pn_data_get_ushort (buffer);

    /* Value */
    pn_data_next (buffer);
    target->mData = pn_data_get_atom   (buffer);

    switch(target->mData.type)
    {
    /* Arrays are really vectors, so will need traversed */
    case PN_ARRAY:
    {
        mama_size_t     element_count   = 0;
        pn_type_t       content_type    = PN_NULL;
        mama_size_t     i               = 0;
        msgPayload      childMsg        = NULL;

        /* Move onto value[] */
        pn_data_next (buffer);

        /* get size of array */
        element_count = pn_data_get_array (buffer);

        /* get type of element for resulting array */
        content_type = pn_data_get_array_type (buffer);

        /* enter array */
        pn_data_enter (buffer);

        /* If this looks like an array of messages */
        if (content_type == PN_LIST)
        {
            qpidmsgFieldPayloadImpl_setDataVectorSize (target,
                                                       element_count);

            target->mMamaType = MAMA_FIELD_TYPE_VECTOR_MSG;

            for (; i < target->mDataVectorCount; i++)
            {
                /* go to next array element */
                pn_data_next (buffer);

                status = qpidmsgPayload_create (&childMsg);

                if (MAMA_STATUS_OK != status)
                {
                    mama_log (MAMA_LOG_LEVEL_ERROR,
                              "qpidmsgPayloadImpl_getFieldFromBuffer(): "
                              "Failed to create child message");
                    return status;
                }

                /* Messages themselves are lists - step inside */
                pn_data_get_list (buffer);
                pn_data_enter    (buffer);

                /* parse the payload to populate the message */
                status = qpidmsgPayloadImpl_getMessageFromBuffer (buffer,
                                 (qpidmsgPayloadImpl*) childMsg);

                if (MAMA_STATUS_OK != status)
                {
                    mama_log (MAMA_LOG_LEVEL_ERROR,
                              "qpidmsgPayloadImpl_getFieldFromBuffer(): "
                              "Failed to get message from buffer");
                    return status;
                }

                /* get the contents and add it to the array */
                target->mDataVector[i] = childMsg;

                pn_data_exit (buffer);
            }
        }
        /* If this is a scalar vector */
        else
        {
            target->mDataArrayCount  = element_count;
            target->mMamaType =
                    qpidmsgPayloadImpl_arrayToMamaType (content_type);

            /* If there are elements inside and we know how to interpret */
            if (element_count > 0)
            {
                mama_size_t i               = 0;
                pn_type_t   secondary_type;

                /* Move onto the first, then second element to inspect */
                pn_data_next (buffer);
                pn_data_next (buffer);

                secondary_type = pn_data_get_atom (buffer).type;

                /* Move back so we're before the first element again */
                pn_data_prev (buffer);
                pn_data_prev (buffer);

                /* If this is an array of prices */
                if (content_type == PN_DOUBLE && secondary_type == PN_UBYTE)
                {
                    target->mMamaType = MAMA_FIELD_TYPE_VECTOR_PRICE;
                    /* Twice as many atoms required for a price */
                    element_count *= 2;
                }

                qpidmsgFieldPayloadImpl_setDataArraySize (target,
                                                          element_count);


                for (; i < target->mDataArrayCount; i++)
                {
                    /* get the contents and add it to the array */
                    target->mDataArray[i] = pn_data_get_atom (buffer);

                    /* go to next array element */
                    pn_data_next (buffer);
                }
            }
        }
        /* exit array */
        pn_data_exit (buffer);

        /* exit field */
        pn_data_exit (buffer);
        break;
    }
    /* timestamps are really 3 fields: stamp, hints and precision */
    case PN_TIMESTAMP:
    {
        /* allocate memory for storage in field object */
        qpidmsgFieldPayloadImpl_setDataArraySize (target, 3);

        /* Get the timestamp */
        target->mDataArray[0] = pn_data_get_atom (buffer);

        /* Get the hints */
        pn_data_next (buffer);
        target->mDataArray[1] = pn_data_get_atom (buffer);

        /* Get the precision */
        pn_data_next (buffer);
        target->mDataArray[2] = pn_data_get_atom (buffer);

        /* Set the MAMA Field Type */
        target->mMamaType = MAMA_FIELD_TYPE_TIME;

        /* Exit field */
        pn_data_exit     (buffer);
        break;
    }
    /* Is this a price? We will need further analysis to find out */
    case PN_DOUBLE:
    {
        /* Consider a price if there are items remaining in this list */
        if (0 != pn_data_next (buffer))
        {
            pn_data_prev (buffer);

            /* This is how we represent prices - 2 atoms required */
            qpidmsgFieldPayloadImpl_setDataArraySize (target, 2);

            /* First atom is the price value */
            target->mDataArray[0] = target->mData;

            /* Move on to the next field */
            pn_data_next     (buffer);

            /* Second atom is the hints value */
            target->mDataArray[1] = pn_data_get_atom (buffer);

            target->mMamaType = MAMA_FIELD_TYPE_PRICE;
        }
        /* If this is just a regular double, treat as any other */
        else
        {
            target->mData = pn_data_get_atom (buffer);
            target->mMamaType =
                    qpidmsgPayloadInternal_toMamaType (
                            target->mData.type);
        }

        /* Exit field */
        pn_data_exit     (buffer);
        break;
    }
    /* MamaMsg entries will be represented as lists */
    case PN_LIST:
    {
        msgPayload  childMsg = NULL;

        /* Move onto value[] */
        pn_data_enter (buffer);

        /*
         * set size of array, even though we assume it is a message and
         * so will always only be 1
         */
        qpidmsgFieldPayloadImpl_setDataVectorSize(target, 1);

        status = qpidmsgPayload_create (&childMsg);

        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, 
                      "qpidmsgPayloadImpl_getFieldFromBuffer: "
                      "Failed to build child message.");
            return status;
        }

        /* parse the payload to populate the message */
        status = qpidmsgPayloadImpl_getMessageFromBuffer (buffer,
                         (qpidmsgPayloadImpl*) childMsg);

        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "qpidmsgPayloadImpl_getFieldFromBuffer: "
                      "Failed to get message from buffer.");
            return status;
        }

        /* get the contents and add it to the array */
        target->mDataVector[0] = childMsg;

        pn_data_exit (buffer);
        pn_data_exit (buffer);

        target->mMamaType = MAMA_FIELD_TYPE_MSG;
        break;
    }
    default:
    {
        target->mData = pn_data_get_atom (buffer);
        target->mMamaType =
                qpidmsgPayloadInternal_toMamaType (target->mData.type);

        /* Exit field */
        pn_data_exit     (buffer);
        break;
    }
    }
    return status;
}

mama_status
qpidmsgPayloadImpl_moveDataToContentLocation (pn_data_t* buffer)
{
    /* Go to the top of the buffer */
    pn_data_rewind   (buffer);

    /* Skip past the NULL atom */
    pn_data_next     (buffer);

    /* Get the main container list and step inside */
    pn_data_get_list (buffer);
    pn_data_enter    (buffer);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayloadImpl_moveDataToInsertLocation (pn_data_t* buffer,
                                             qpidmsgPayloadImpl* impl)
{
    if (QPID_INSERT_MODE_INLINE == impl->mInsertMode)
    {
        return MAMA_STATUS_OK;
    }
    qpidmsgPayloadImpl_moveDataToContentLocation (buffer);

    /* Go to the end of this list */
    while (0 != pn_data_next (buffer));

    return MAMA_STATUS_OK;
}

/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

mama_status
qpidmsgPayloadImpl_payloadToMamaMsg (qpidmsgPayloadImpl* impl,
                                     mamaMsg* target)
{
    const void*     buffer      = NULL;
    mama_size_t     bufferLen   = 0;
    mama_status     status      = MAMA_STATUS_OK;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the byte buffer for the impl */
    status = qpidmsgPayload_serialize (impl, &buffer, &bufferLen);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    status = mamaMsg_createFromByteBuffer (target, buffer, bufferLen);

    return status;
}

mama_status
qpidmsgPayloadImpl_resetToIteratorState (qpidmsgPayloadImpl* impl)
{
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Offset will be negative if no iterator is accessing this message */
    if (impl->mDataIteratorOffset < 0)
    {
        return MAMA_STATUS_OK;
    }
    else
    {
        uint16_t i = 0;
        /* First move to the start of the content */
        qpidmsgPayloadImpl_moveDataToContentLocation (impl->mBody);

        /* For each offset found, skip ahead */
        for (i = 0; i < impl->mDataIteratorOffset; i++)
        {
            /*
             * If there is no next message, iterator has marked an invalid
             * offset
             */
            if (0 == pn_data_next (impl->mBody))
            {
                return MAMA_STATUS_INVALID_ARG;
            }
        }
        return MAMA_STATUS_OK;
    }

}

mama_status
qpidmsgPayloadImpl_addFieldToPayload (msgPayload                 msg,
                                     qpidmsgFieldPayloadImpl*   field)
{
    const char*                 name    = NULL;
    mama_fid_t                  fid     = 0;
    mamaFieldType               type    = MAMA_FIELD_TYPE_UNKNOWN;

    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    name    = field->mName.start;
    fid     = field->mFid;
    type    = field->mMamaType;

    switch(type)
    {
    case MAMA_FIELD_TYPE_MSG:
    {
        /* Get the message implementation from the field's array */
        qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*)field->mDataVector[0];
        return qpidmsgPayload_addMsg (msg, name, fid, impl);
        break;
    }
    case MAMA_FIELD_TYPE_OPAQUE:
    {
        const void* result;
        mama_size_t op_size = 0;

        qpidmsgFieldPayload_getOpaque   (field, &result, &op_size);
        return qpidmsgPayload_addOpaque (msg, name, fid, result, op_size);

        break;
    }
    case MAMA_FIELD_TYPE_STRING:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(String,const char*);
        break;
    case MAMA_FIELD_TYPE_BOOL:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(Bool,mama_bool_t);
        break;
    case MAMA_FIELD_TYPE_CHAR:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(Char,char);
        break;
    case MAMA_FIELD_TYPE_I8:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(I8,mama_i8_t);
        break;
    case MAMA_FIELD_TYPE_U8:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(U8,mama_u8_t);
        break;
    case MAMA_FIELD_TYPE_I16:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(I16,mama_i16_t);
        break;
    case MAMA_FIELD_TYPE_U16:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(U16,mama_u16_t);
        break;
    case MAMA_FIELD_TYPE_I32:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(I32,mama_i32_t);
        break;
    case MAMA_FIELD_TYPE_U32:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(U32,mama_u32_t);
        break;
    case MAMA_FIELD_TYPE_I64:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(I64,mama_i64_t);
        break;
    case MAMA_FIELD_TYPE_U64:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(U64,mama_u64_t);
        break;
    case MAMA_FIELD_TYPE_F32:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(F32,float);
        break;
    case MAMA_FIELD_TYPE_F64:
        ADD_SCALAR_FIELD_VALUE_TO_MESSAGE(F64,double);
        break;
    case MAMA_FIELD_TYPE_TIME:
    {
        mamaDateTime    result = NULL;
        mama_status     status = MAMA_STATUS_OK;

        mamaDateTime_create (&result);
        qpidmsgFieldPayload_getDateTime (field, result);
        status = qpidmsgPayload_addDateTime(msg, name, fid, result);
        mamaDateTime_destroy(result);

        return status;
        break;
    }
    case MAMA_FIELD_TYPE_PRICE:
    {
        mamaPrice   result = NULL;
        mama_status status = MAMA_STATUS_OK;

        mamaPrice_create (&result);
        qpidmsgFieldPayload_getPrice (field, result);
        status = qpidmsgPayload_addPrice(msg, name, fid, result);
        mamaPrice_destroy(result);

        return status;
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_BOOL:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(Bool, mama_bool_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_CHAR:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(Char, char);
        break;
    case MAMA_FIELD_TYPE_VECTOR_I8:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(I8, mama_i8_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_U8:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(U8, mama_u8_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_I16:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(I16, mama_i16_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_U16:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(U16, mama_u16_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_I32:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(I32, mama_i32_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_U32:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(U32, mama_u32_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_I64:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(I64, mama_i64_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_U64:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(U64, mama_u64_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_F32:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(F32, mama_f32_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_F64:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(F64, mama_f64_t);
        break;
    case MAMA_FIELD_TYPE_VECTOR_STRING:
        ADD_VECTOR_FIELD_VALUE_TO_MESSAGE(String, char*);
        break;
    case MAMA_FIELD_TYPE_VECTOR_MSG:
    {
        /*
         * Translate the qpid message to a mamaMsg to enable iterators to
         * work
         */
        qpidmsgPayloadImpl* impl        = (qpidmsgPayloadImpl*)msg;
        mamaMsg*            outputArray = NULL;
        mama_size_t         i           = 0;
        mama_status         status      = MAMA_STATUS_OK;

        qpidmsgPayloadImpl_allocateBufferMemory (
                (void**)&(impl->mNestedMsgBuffer),
                &(impl->mNestedMsgBufferSize),
                field->mDataVectorCount * sizeof (mamaMsg) );

        outputArray = impl->mNestedMsgBuffer;

        /* Reuse the buffer which will be destroyed in _destroy later */
        for (i = 0; i < field->mDataVectorCount; i++)
        {
            const void* buf         = NULL;
            mama_size_t buf_size    = 0;

            /* Get the message implementation from the field's array */
            qpidmsgPayloadImpl* subMsgImpl =
                    (qpidmsgPayloadImpl*)field->mDataVector[i];

            status     = qpidmsgPayload_serialize (subMsgImpl,
                                                   &buf,
                                                   &buf_size);

            if (MAMA_STATUS_OK != status)
            {
                return status;
            }

            /* If this is crossing into a boundary that has never been
             * populated before */
            if (   impl->mNestedMsgBufferCount == 0
                || i >= impl->mNestedMsgBufferCount)
            {
                /* Message will need to be re-created */
                status = mamaMsg_createFromByteBuffer (&outputArray[i],
                                                       buf,
                                                       buf_size);

                /* If the allocation here has failed, drop out */
                if (MAMA_STATUS_OK != status)
                {
                    return status;
                }
                /* Update the elements inside for next time */
                impl->mNestedMsgBufferCount = i + 1;
            }
            else
            {
                /* Recycle the old message */
                mamaMsg_setNewBuffer (outputArray[i], (void*)buf, buf_size);
            }
        }

        /* Now prereqs have been acquired, add the message */
        status = qpidmsgPayload_addVectorMsg (msg,
                                              name,
                                              fid,
                                              outputArray,
                                              field->mDataVectorCount);
        return status;
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_TIME:
    {
        const mamaDateTime* result  = NULL;
        mama_size_t         size    = 0;

        qpidmsgPayload_getVectorDateTime (field, name, fid, result, &size);
        return qpidmsgPayload_addVectorDateTime (msg,
                                                 name,
                                                 fid,
                                                 result,
                                                 size);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_PRICE:
    {
        const mamaPrice* result = NULL;
        mama_size_t      size   = 0;

        qpidmsgPayload_getVectorPrice (field, name, fid, result, &size);
        return qpidmsgPayload_addVectorPrice (msg,
                                              name,
                                              fid,
                                              result,
                                              size);
        break;
    }
    case MAMA_FIELD_TYPE_QUANTITY:
    case MAMA_FIELD_TYPE_COLLECTION:
    case MAMA_FIELD_TYPE_UNKNOWN:
    default:
        return MAMA_STATUS_NOT_IMPLEMENTED;
        break;
    }
}


/*
 * Method assumes we already know we are dealing with a message and buffer
 * is pointing there in its underlying implmentation.
 */
mama_status
qpidmsgPayloadImpl_getMessageFromBuffer (pn_data_t*           buffer,
                                         qpidmsgPayloadImpl*  target)
{
    msgFieldPayload             field   = NULL;

    if (NULL == buffer || NULL == target)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* reuse the supplied payload message's field memory */
    field = target->mField;

    /* For each field in the message */
    while (0 != pn_data_next (buffer))
    {
        /* Get the buffer and translate it to a field */
        qpidmsgPayloadImpl_getFieldFromBuffer (buffer,
                (qpidmsgFieldPayloadImpl*) field);

        /* Add the instance of the field to the target to be returned */
        qpidmsgPayloadImpl_addFieldToPayload (target,
                (qpidmsgFieldPayloadImpl*) field);
    }

    return MAMA_STATUS_OK;
}

mamaFieldType
qpidmsgPayloadImpl_arrayToMamaType (pn_type_t type)
{
    switch(type)
    {
    case PN_NULL:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_BOOL:       return MAMA_FIELD_TYPE_VECTOR_BOOL;
    case PN_UBYTE:      return MAMA_FIELD_TYPE_VECTOR_U8;
    case PN_BYTE:       return MAMA_FIELD_TYPE_VECTOR_I8;
    case PN_USHORT:     return MAMA_FIELD_TYPE_VECTOR_U16;
    case PN_SHORT:      return MAMA_FIELD_TYPE_VECTOR_I16;
    case PN_UINT:       return MAMA_FIELD_TYPE_VECTOR_U32;
    case PN_INT:        return MAMA_FIELD_TYPE_VECTOR_I32;
    case PN_CHAR:       return MAMA_FIELD_TYPE_VECTOR_CHAR;
    case PN_ULONG:      return MAMA_FIELD_TYPE_VECTOR_U64;
    case PN_LONG:       return MAMA_FIELD_TYPE_VECTOR_I64;
    case PN_TIMESTAMP:  return MAMA_FIELD_TYPE_VECTOR_TIME;
    case PN_FLOAT:      return MAMA_FIELD_TYPE_VECTOR_F32;
    case PN_DOUBLE:     return MAMA_FIELD_TYPE_VECTOR_F64;
    case PN_DECIMAL32:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DECIMAL64:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DECIMAL128: return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_UUID:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_BINARY:     return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_STRING:     return MAMA_FIELD_TYPE_VECTOR_STRING;
    case PN_SYMBOL:     return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DESCRIBED:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_ARRAY:      return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_LIST:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_MAP:        return MAMA_FIELD_TYPE_UNKNOWN;
    default:            return MAMA_FIELD_TYPE_UNKNOWN;
    }
}

void MAMACALLTYPE
qpidmsgPayloadImpl_addMsgFieldCb (const mamaMsg      msg,
                                  const mamaMsgField field,
                                  void*              closure)
{
    qpidmsgPayloadImpl_addFieldToPayload (
            (msgPayload) closure,
            (qpidmsgFieldPayloadImpl*) field->myPayload);
}

/* iterate through value and add its contents to msg */
mama_status
qpidmsgPayloadImpl_addBareMsg (msgPayload msg,
                               mamaMsg    value)
{
    qpidmsgPayloadImpl* impl = (qpidmsgPayloadImpl*)msg;

    /*
     * Set insertion to inline mode so new additions don't get added to the end
     * of the message
     */
    qpidInsertMode previous = impl->mInsertMode;
    impl->mInsertMode       = QPID_INSERT_MODE_INLINE;

    /* Iterate through value fields and add them to message */
    mamaMsg_iterateFields (value,
                           qpidmsgPayloadImpl_addMsgFieldCb,
                           NULL,
                           msg);

    /* Reset to previous inline mode */
    impl->mInsertMode = previous;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayloadImpl_findField (qpidmsgPayloadImpl* impl,
                              const char*         name,
                              mama_fid_t          fid)
{
    /* Go to first field in the message */
    qpidmsgPayloadImpl_moveDataToContentLocation (impl->mBody);

    if (0 != fid)
    {
        while (0 != pn_data_next (impl->mBody))
        {
            /* Enter field - stored as a list of Name, FID, data*/
            pn_data_get_list (impl->mBody);
            pn_data_enter    (impl->mBody);

            /* Skip over name and onto the FID */
            pn_data_next     (impl->mBody);
            pn_data_next     (impl->mBody);

            /* Check if FIDs are equal */
            if (fid == pn_data_get_ushort (impl->mBody))
            {
                /* Field found */
                return MAMA_STATUS_OK;
            }

            /* Exit field and continue searching */
            pn_data_exit     (impl->mBody);
        }

        return MAMA_STATUS_NOT_FOUND;
    }
    else if (NULL != name)
    {

        while (0 != pn_data_next (impl->mBody))
        {
            pn_bytes_t  foundName;

            /* Enter field - stored as a list of Name, FID, data*/
            pn_data_get_list (impl->mBody);
            pn_data_enter    (impl->mBody);

            /* First value is the name */
            pn_data_next     (impl->mBody);
            foundName = pn_data_get_string (impl->mBody);

            /* Check if sames are equal */
            if (0 == strcmp (name, foundName.start))
            {
                /* Skip over FID */
                pn_data_next (impl->mBody);

                /* Field found */
                return MAMA_STATUS_OK;
            }

            /* Exit field and continue searching */
            pn_data_exit     (impl->mBody);
        }
    }
    else
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    return MAMA_STATUS_NOT_FOUND;
}

mama_status
qpidmsgPayloadImpl_createImplementationOnly (msgPayload* msg)
{
    qpidmsgPayloadImpl* impl = NULL;

    if (NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (qpidmsgPayloadImpl*) calloc (1, sizeof (qpidmsgPayloadImpl));
    NOMEM_STATUS_CHECK (impl);

    impl->mBuffer                 = NULL; /* Created when first used */
    impl->mBufferSize             = 0;
    impl->mNestedMsgBuffer        = NULL;
    impl->mNestedMsgBufferSize    = 0;
    impl->mNestedMsgBufferCount   = 0;
    impl->mInsertMode             = QPID_INSERT_MODE_MAIN_LIST;
    impl->mDataIteratorOffset     = -1;

    *msg = impl;

    qpidmsgFieldPayload_create ((msgFieldPayload*)(&impl->mField));

    return MAMA_STATUS_OK;
}
