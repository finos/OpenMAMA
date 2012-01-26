/* $Id: timers.h,v 1.1.2.2 2011/09/07 09:45:08 emmapollock Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#include "port.h"

typedef void* timerElement;
typedef void* timerHeap;

typedef void (*timerFireCb)(timerElement timer, void* mClosure);

int createTimerHeap (timerHeap* heap);
int startDispatchTimerHeap (timerHeap heap);
wthread_t timerHeapGetTid (timerHeap heap);
int destroyHeap (timerHeap heap);

int createTimer (timerElement* timer, timerHeap heap, timerFireCb cb, struct timeval* timeout, void* closure);
int destroyTimer (timerHeap heap, timerElement timer);

#endif
