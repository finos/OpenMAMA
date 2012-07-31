/* $Id: dictionaryimpl.h,v 1.3.34.3 2011/09/01 16:34:37 emmapollock Exp $
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

#ifndef DictionaryImplH__
#define DictionaryImplH__

#if defined(__cplusplus)
extern "C" {
#endif

const mamaMsgCallbacks*
mamaDictionary_getSubscCb(
    mamaDictionary   dictionary);

mama_status
mamaDictionary_setSubscPtr(
    mamaDictionary    dictionary,
    mamaSubscription  subsc);

mama_status
mamaDictionary_setCompleteCallback(
    mamaDictionary                   dictionary, 
    mamaDictionary_completeCallback  cb);

mama_status
mamaDictionary_setTimeoutCallback(
    mamaDictionary                   dictionary, 
    mamaDictionary_timeoutCallback   cb);

mama_status
mamaDictionary_setErrorCallback(
    mamaDictionary                   dictionary, 
    mamaDictionary_errorCallback     cb);

mama_status
mamaDictionary_setClosure(
    mamaDictionary                   dictionary, 
    void*                            closure);

#if defined(__cplusplus)
}
#endif

#endif /*DictionaryImplH__ */
