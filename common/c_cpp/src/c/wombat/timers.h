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

#ifndef TIMERS_INTERNAL_
#define TIMERS_INTERNAL_

#include "wombat/port.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void* timerElement;
typedef void* timerHeap;

typedef void (*timerFireCb)(timerElement timer, void* mClosure);

COMMONExpDLL int createTimerHeap (timerHeap* heap);
COMMONExpDLL int startDispatchTimerHeap (timerHeap heap);
COMMONExpDLL wthread_t timerHeapGetTid (timerHeap heap);
COMMONExpDLL int destroyHeap (timerHeap heap);

COMMONExpDLL int createTimer (timerElement* timer, timerHeap heap, timerFireCb cb, struct timeval* timeout, void* closure);
COMMONExpDLL int destroyTimer (timerHeap heap, timerElement timer);
COMMONExpDLL int resetTimer (timerHeap heap, timerElement timer, struct timeval* timeout);


COMMONExpDLL int lockTimerHeap (timerHeap heap);
COMMONExpDLL int unlockTimerHeap (timerHeap heap);


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
