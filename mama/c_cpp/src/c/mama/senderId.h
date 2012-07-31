/* $Id: senderId.h,v 1.5.22.4 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MamaSenderIdH__
#define MamaSenderIdH__

#include "mama/mama.h"

#if defined(__cplusplus)
extern "C" {
#endif


/**
 * Determine the IP address (as a string) of the sender ID in the
 * buffer provided.  The sender ID is a field sent in many MAMA
 * messages (see the set of reserved fields).
 */
MAMAExpDLL
extern void mamaSenderId_getIpAddr (uint64_t  senderId,
                                    char*     buffer,
                                    size_t    maxLen);

/**
 * Determine the process id in the buffer provided.  The
 * sender ID is a field sent in many MAMA messages (see the set of
 * reserved fields).
 */
MAMAExpDLL
extern void mamaSenderId_getPid (uint64_t   senderId,
                                 uint16_t*  result);

MAMAExpDLL
extern void mamaSenderId_getPid64 (uint64_t   senderId,
                                 uint32_t*  result);
/**
 * Return a sender ID for possible use when publishing messages.  The
 * sender ID is a field sent in many MAMA messages (see the set of
 * reserved fields).
 */
MAMAExpDLL
extern uint64_t mamaSenderId_getSelf (void);

/**
 * Manually set a sender ID for the process.  If a sender_id is set this 
 * way, then as of that point, mamaSenderId_getSelf() will always return
 * that value.
 */
MAMAExpDLL
extern void mamaSenderId_setSelf (uint64_t self);

#if defined(__cplusplus)
}
#endif

#endif /* MamaSenderIdH__ */
