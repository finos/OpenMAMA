/* $Id: io.c,v 1.1.2.2 2011/08/30 15:51:49 billtorpey Exp $
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
#include <bridge.h>
#include "avisbridgefunctions.h"

mama_status
avisBridgeMamaIo_create(ioBridge*  result,
                         void*      nativeQueueHandle,
                         uint32_t   descriptor,
                         mamaIoCb   action,
                         mamaIoType ioType,
                         mamaIo     parent,
                         void*      closure)
{
    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaIo_destroy (ioBridge io)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaIo_getDescriptor (ioBridge io, uint32_t *result)
{
    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

