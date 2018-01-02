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

#ifndef MamaMsgImplH__
#define MamaMsgImplH__

#include "wombat/wConfig.h"
#include "bridge.h"
#include "payloadbridge.h"
#include "dqstrategy.h"

#ifndef OPENMAMA_INTEGRATION
  #define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/msg.h>

#define MAX_SUBJECT 256

#define CHECK_MODIFY(owner)                                                               \
do                                                                                        \
{                                                                                         \
    if (!owner && !mamaInternal_getAllowMsgModify()) return MAMA_STATUS_NOT_MODIFIABLE;   \
}                                                                                         \
while(0);

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mamaMsgReplyImpl_
{
    mamaBridgeImpl*     mBridgeImpl;
    void *              replyHandle;
}mamaMsgReplyImpl;

#if defined(__cplusplus)
}
#endif

#endif /* MamaMsgImplH__ */
