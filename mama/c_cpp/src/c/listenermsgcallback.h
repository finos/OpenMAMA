/* $Id: listenermsgcallback.h,v 1.2.34.5 2011/09/01 16:34:37 emmapollock Exp $
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

#ifndef ListenerMsgCallbackH__
#define ListenerMsgCallbackH__

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Main callback for MamaListener. This is the base strategy for
 * handling messages.
 */

typedef struct msgCallback_ *listenerMsgCallback;

mama_status
listenerMsgCallback_create (
    listenerMsgCallback*  callback,
    mamaSubscription      subscription);

mama_status
listenerMsgCallback_destroy (
    listenerMsgCallback   callback);

void 
listenerMsgCallback_processMsg (
    listenerMsgCallback   callback,
    mamaMsg               msg,
    SubjectContext*       ctx);

#if defined(__cplusplus)
}
#endif

#endif /* ListenerMsgCallbackH__ */
