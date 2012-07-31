/* $Id: avisdefs.h,v 1.1.2.7 2011/09/16 19:42:31 billtorpey Exp $
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

#ifndef AVISDEFS_H__
#define AVISDEFS_H__

#include <avis/elvin.h>
#include <wombat/wSemaphore.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define CHECK_AVIS(avis) \
    do {  \
       if (avis == 0) return MAMA_STATUS_NULL_ARG; \
       if (!elvin_is_open(avis)) return MAMA_STATUS_INVALID_ARG; \
     } while(0)

#define SUBJECT_FIELD_NAME  "__subj"
#define INBOX_FIELD_NAME    "__inbx"
#define MAX_SUBJECT_LENGTH    256

typedef struct avisTransportBridge
{
    Elvin*               mAvis;
    mamaTransport        mTransport;
    wsem_t               mAvisDispatchSem;
} avisTransportBridge;


typedef struct avisBridgeImpl
{
    avisTransportBridge*   mTransportBridge;
} avisBridgeImpl;
#define avisBridge(bridgeImpl) ((avisBridgeImpl*) bridgeImpl)

void log_avis_error(MamaLogLevel logLevel, Elvin* avis);
void closeListener(Elvin* avis, CloseReason reason, const char* message, void* closure);

#if defined(__cplusplus)
}
#endif

#endif /* AVISDEFS_H__ */
