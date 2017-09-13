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
#include <stdint.h>
#include <string.h>

#include <mama/mama.h>
#include <proton/message.h>
#include <proton/error.h>

#include "payloadbridge.h"
#include "msgfieldimpl.h"
#include "qpidcommon.h"
#include "qpidpayloadfunctions.h"
#include "payload.h"
#include "field.h"

/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidIterImpl_
{
    qpidmsgFieldPayloadImpl* mField; /* Reusable field impl*/
    qpidmsgPayloadImpl*      mMsg;   /* Original message being covered */
} qpidIterImpl;


/*=========================================================================
  =                   Public implementation functions                     =
  =========================================================================*/

mama_status
qpidmsgPayloadIter_create (msgPayloadIter* iter,
                           msgPayload      msg)
{
    qpidIterImpl*        impl       = NULL;
    qpidmsgPayloadImpl*  msgImpl    = (qpidmsgPayloadImpl*) msg;

    if (NULL == msg || NULL == iter)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Mark this message as being iterated over */
    msgImpl->mDataIteratorOffset    = 0;

    impl = (qpidIterImpl*) calloc (1, sizeof (qpidIterImpl));
    qpidmsgFieldPayload_create ((msgFieldPayload*) &impl->mField);

    impl->mField->mParentBody = msgImpl->mBody;
    impl->mMsg                = msgImpl;

    qpidmsgPayloadImpl_moveDataToContentLocation (msgImpl->mBody);

    *iter = impl;

    return MAMA_STATUS_OK;
}

msgFieldPayload
qpidmsgPayloadIter_next (msgPayloadIter          iter,
                         msgFieldPayload         field,
                         msgPayload              msg)
{
    qpidIterImpl*         impl      = (qpidIterImpl*) iter;
    qpidmsgPayloadImpl*   msgImpl   = (qpidmsgPayloadImpl*) msg;

    /* Field will be NULL on first call so don't check for it */
    if (NULL == iter || NULL == msg)
    {
        return NULL;
    }

    /* 0 signifies no more data */
    if (0 == pn_data_next (msgImpl->mBody))
    {
        return NULL;
    }

    qpidmsgPayloadImpl_getFieldFromBuffer (msgImpl->mBody, impl->mField);

    /* Increment the iterator offset */
    msgImpl->mDataIteratorOffset++;

    return impl->mField;
}

mama_bool_t
qpidmsgPayloadIter_hasNext (msgPayloadIter          iter,
                            msgPayload              msg)
{
    qpidmsgPayloadImpl*     msgImpl   = (qpidmsgPayloadImpl*)msg;
    mama_bool_t             hasNext   = 0;

    /* Field will be NULL on first call so don't check for it */
    if (NULL == iter || NULL == msg)
    {
        return 0;
    }

    /*
     * Proton does have a pn_data_peek() function but it is internal only
     * so we have to use pn_data_next() follow by pn_data_prev()
     */
    hasNext = pn_data_next (msgImpl->mBody);

    /* pn_data_next returns a typedef bool value, with 0 == false. */
    if (0 != hasNext)
    {
        pn_data_prev (msgImpl->mBody);
    }
 
    return hasNext;
}

msgFieldPayload
qpidmsgPayloadIter_begin (msgPayloadIter          iter,
                          msgFieldPayload         field,
                          msgPayload              msg)
{
    qpidIterImpl*         impl      = (qpidIterImpl*)iter;
    qpidmsgPayloadImpl*   msgImpl   = (qpidmsgPayloadImpl*)msg;

    if (NULL == iter || NULL == msg)
    {
        return NULL;
    }

    /* Move to where the message starts */
    qpidmsgPayloadImpl_moveDataToContentLocation (msgImpl->mBody);

    msgImpl->mDataIteratorOffset    = 0;

    /* NULL signifies no more data */
    if (0 == pn_data_next (msgImpl->mBody))
    {
        return NULL;
    }

    qpidmsgPayloadImpl_getFieldFromBuffer (msgImpl->mBody, impl->mField);

    /* Step back back out of the message again. */
    qpidmsgPayloadImpl_moveDataToContentLocation (msgImpl->mBody);

    return impl->mField;
}

/*
 * Postponing implementation until this method is included or removed from the
 * implementation
 */
msgFieldPayload
qpidmsgPayloadIter_end (msgPayloadIter          iter,
                        msgPayload              msg)
{
    return NULL;
}

mama_status
qpidmsgPayloadIter_associate (msgPayloadIter          iter,
                              msgPayload              msg)
{
    qpidmsgPayloadImpl*     msgImpl = (qpidmsgPayloadImpl*) msg;
    qpidIterImpl*           impl    = (qpidIterImpl*) iter;

    if (NULL == iter || NULL == msg)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    msgImpl->mDataIteratorOffset = 0;
    impl->mMsg                   = msgImpl;

    /* Move to where the message starts */
    qpidmsgPayloadImpl_moveDataToContentLocation (msgImpl->mBody);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgPayloadIter_destroy (msgPayloadIter iter)
{
    qpidIterImpl* impl = (qpidIterImpl*)iter;

    if (NULL == iter)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Free our re-usable message */
    if(NULL != impl->mField)
    {
        qpidmsgFieldPayload_destroy (impl->mField);
    }

    if (NULL != impl->mMsg)
    {
        impl->mMsg->mDataIteratorOffset = -1;
    }

    /* Then free the implementation */
    free (impl);
    impl = NULL;

    return MAMA_STATUS_OK;
}

