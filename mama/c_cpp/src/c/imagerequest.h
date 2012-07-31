/* $Id: imagerequest.h,v 1.9.24.5 2011/09/01 16:34:37 emmapollock Exp $
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

#ifndef MamaImageRequestH__
#define MamaImageRequestH__

#include "wombat/port.h"

#include "wombat/wincompat.h"
#include "throttle.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct imageReqImpl_* imageRequest;
typedef struct SubjectContext_ SubjectContext;

mama_status
imageRequest_create (
    imageRequest*      request,  
    mamaSubscription   subscription,
    SubjectContext*    subCtx,
    mamaPublisher      publisher,
    wombatThrottle     throttle);

void
imageRequest_destroy (
    imageRequest       request);

mama_status
imageRequest_sendRequest (
    imageRequest       request,  
    mamaMsg            msg,
    double             timeout,
    int                retries,
    int                throttle,
    int                isRecapRequest);

void
imageRequest_stopWaitForResponse (
    imageRequest       request);


uint8_t 
imageRequest_isWaitingForResponse (
    imageRequest       request);

int
imageRequest_isRecapRequest (
    imageRequest       request);

/**
 * This function will cancel any pending image request.
 *
 * @param[in] request The image request to cancel.
 */
void
imageRequest_cancelRequest(
    imageRequest request);

#if defined(__cplusplus)
}
#endif

#endif /* MamaImageRequestH__ */
