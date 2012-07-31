/* $Id: inboximpl.h,v 1.3.14.1.8.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#ifndef MamaInboxImplH__
#define MamaInboxImplH__

#include "bridge.h"

#if defined(__cplusplus)
extern "C" {
#endif

MAMAExpDLL 
extern inboxBridge
mamaInboxImpl_getInboxBridge (mamaInbox inbox);

MAMAExpDLL 
extern mama_status 
mamaInbox_createByIndex (mamaInbox*             inbox,
                         mamaTransport          tport,
                         int                    tportIndex,
                         mamaQueue              queue,
                         mamaInboxMsgCallback   msgCB,
                         mamaInboxErrorCallback errorCB,
                         void*                  closure);

MAMAExpDLL 
extern mama_status 
mamaInbox_createByIndex2 (mamaInbox*             inbox,
                         mamaTransport          tport,
                         int                    tportIndex,
                         mamaQueue              queue,
                         mamaInboxMsgCallback   msgCB,
                         mamaInboxErrorCallback errorCB,
                         mamaInboxDestroyCallback onInboxDestroyed,
                         void*                  closure);

#if defined(__cplusplus)
}
#endif

#endif /* MamaInboxmplH__ */
