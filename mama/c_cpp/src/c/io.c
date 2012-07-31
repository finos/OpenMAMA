/* $Id: io.c,v 1.3.32.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include <mama/mama.h>
#include <mama/io.h>
#include "bridge.h"
#include "queueimpl.h"

typedef struct mamaIoImpl_
{
    mamaBridgeImpl* mBridgeImpl;
    ioBridge        mMamaIoBridgeImpl;
} mamaIoImpl;

mama_status
mamaIo_create (mamaIo*    result,
               mamaQueue  queue,
               uint32_t   descriptor,
               mamaIoCb   action, 
               mamaIoType ioType,
               void*      closure)
{
    mama_status     status              =   MAMA_STATUS_OK;
    mamaIoImpl*     impl                =   NULL;
    mamaBridgeImpl* bridgeImpl          =   NULL;
    void*           nativeQueueHandle   =   NULL;

    if (!result)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaIo_create(): NULL"
                  "IO address.");
        return MAMA_STATUS_NULL_ARG;
    }

    *result = NULL;

    if (!queue)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaIo_create(): NULL queue.");
        return MAMA_STATUS_INVALID_QUEUE;
    }

    /*Get the bridge impl from the queue - mandatory*/
    bridgeImpl = mamaQueueImpl_getBridgeImpl (queue);

    if (!bridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;

    /*Get the native queue handle*/
    mamaQueue_getNativeHandle (queue, &nativeQueueHandle);
    if (!nativeQueueHandle)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "mamaIo_create():"
                  "Could not get native queue handle.");
        return MAMA_STATUS_INVALID_QUEUE;
    }
     
    impl = (mamaIoImpl*)calloc (1, sizeof(mamaIoImpl));

    if (!impl)
        return MAMA_STATUS_NOMEM;

    /*Needed for later bridge invocations*/
    impl->mBridgeImpl = bridgeImpl;

    /*Create the correct corresponding bridge implementation of the io*/
    if (MAMA_STATUS_OK!=(status=bridgeImpl->bridgeMamaIoCreate(
                            &impl->mMamaIoBridgeImpl,
                            nativeQueueHandle,
                            descriptor,
                            action,
                            ioType,
                            (mamaIo)impl,/*Pass this as the parent to the bridge
                                         object*/
                            closure)))
    {
        free (impl);
        return status;
    }
   
    *result = (mamaIo)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaIo_getDescriptor (mamaIo    io,
                      uint32_t* d)
{
    mamaIoImpl* impl = (mamaIoImpl*)io;
    
    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (!d)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mBridgeImpl)
        return MAMA_STATUS_NO_BRIDGE_IMPL;

    return impl->mBridgeImpl->bridgeMamaIoGetDescriptor (impl->mMamaIoBridgeImpl, d);
}

mama_status mamaIo_destroy (mamaIo io)
{
    mamaIoImpl* impl = (mamaIoImpl*)io;
    mama_status status = MAMA_STATUS_OK;
    
    if (!impl)
        return MAMA_STATUS_NULL_ARG;

    if (!impl->mBridgeImpl) status = MAMA_STATUS_NO_BRIDGE_IMPL;
    if (!impl->mMamaIoBridgeImpl) status = MAMA_STATUS_INVALID_ARG;

    if (impl->mMamaIoBridgeImpl)
    {
        status = impl->mBridgeImpl->bridgeMamaIoDestroy (impl->mMamaIoBridgeImpl);
    }
    
    impl->mMamaIoBridgeImpl = NULL;
    
    impl->mBridgeImpl = NULL;
    free (impl);

    return status;
}
