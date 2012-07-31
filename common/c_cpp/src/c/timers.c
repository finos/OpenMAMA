/* $Id: timers.c,v 1.1.2.2 2011/09/07 09:45:08 emmapollock Exp $
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

#include "wombat/port.h"
#include "timers.h"

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "systree.h"

typedef struct timerImpl_
{
    timerFireCb           mCb;
    void*                 mClosure;
    struct timeval        mTimeout;
    RB_ENTRY (timerImpl_) mTreeEntry;
} timerImpl;

RB_HEAD(orderedTimeRBTree_, timerImpl_);
typedef struct orderedTimeRBTree_ orderedTimeRBTree;

static int
orderedTimeRBTreeCmp(timerImpl* lhs, timerImpl* rhs)
{
    if (timercmp(&lhs->mTimeout, &rhs->mTimeout, <))
        return (-1);
    else if (timercmp(&lhs->mTimeout, &rhs->mTimeout, >))
        return (1);
    if (lhs < rhs)
        return (-1);
    else if (lhs > rhs)
        return (1);
    return (0);
}
RB_GENERATE_STATIC(orderedTimeRBTree_, timerImpl_, mTreeEntry, orderedTimeRBTreeCmp)

typedef struct timerHeapImpl_
{
    wthread_mutex_t   mLock;
    wthread_t         mDispatchThread;
    orderedTimeRBTree mTimeTree;
    int               mSockPair[2];
    wthread_mutex_t   mEndingLock;
    wthread_cond_t    mEndingCond;
    int               mEnding;
} timerHeapImpl;


int createTimerHeap (timerHeap* heap)
{
    wthread_mutexattr_t       attr;
    timerHeapImpl* heapImpl = (timerHeapImpl*)calloc (1, sizeof (timerHeapImpl));
    if (heapImpl == NULL)
        return -1;

    wthread_mutexattr_init    (&attr);
    wthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP);

    /* Need to make the lock recursive as it should be possible to remove timers
       in the call back */
    wthread_mutex_init (&heapImpl->mLock, &attr);

    wthread_mutex_init (&heapImpl->mEndingLock, NULL);
  
    wthread_cond_init  (&heapImpl->mEndingCond, NULL);

    RB_INIT (&heapImpl->mTimeTree);

    if (wsocketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, heapImpl->mSockPair) == -1)
    {
        free (heapImpl);
        return -1;
    }

    wsetnonblock(heapImpl->mSockPair[0]);
    wsetnonblock(heapImpl->mSockPair[1]);

    *heap = heapImpl;
    return 0;
}

void* dispatchEntry (void *closure)
{
    timerHeapImpl* heap = (timerHeapImpl*)closure;
    fd_set wakeUpDes;
    int selectReturn = 0;
    char buff;

    {
        /* Find the next timeout, if the tree is empty then sleep on pipe */
        timerImpl* ele = NULL;
        struct timeval timeout, now, *timeptr;

        wthread_mutex_lock (&heap->mLock);
        ele = RB_MIN (orderedTimeRBTree_, &heap->mTimeTree);        

        while (1)
        {
            if (ele==NULL)
                timeptr = NULL;
            else
            {
                timeptr = &timeout;
                gettimeofday (&now, NULL); 
                if (!timercmp(&ele->mTimeout, &now, >))
                    timerclear (&timeout);
                else
                    timersub(&ele->mTimeout, &now, &timeout);
            }
            wthread_mutex_unlock (&heap->mLock);

            /* Sit on Select as it has the best resolution */
            FD_ZERO(&wakeUpDes);
            FD_SET(heap->mSockPair[0], &wakeUpDes);
        
            selectReturn = select(heap->mSockPair[0] + 1, &wakeUpDes, NULL, NULL, timeptr);

            wthread_mutex_lock (&heap->mLock);
            if (selectReturn == -1)
            {
                if (errno != EINTR)
                    perror("select()");
            }
            else if (selectReturn)
            {
                int numRead = 0;
                do
                {
                    numRead = wread(heap->mSockPair[0], &buff, sizeof (buff));
                    if (numRead < 0)
                    {
                        if (errno == EINTR)
                            numRead = 1; /* keep reading */
                    }
                    if (buff == 'd')
                       goto endLoop;
                }
                while (numRead > 0);
            }

            /* Dispatch all expired timers */
            ele = RB_MIN (orderedTimeRBTree_, &heap->mTimeTree);
            /* It is possible that this could be empty if the timer was removed before timeout */
            if (ele != NULL)
            {
                gettimeofday (&now, NULL);
                while (!timercmp(&ele->mTimeout, &now, >))
                {
                    RB_REMOVE (orderedTimeRBTree_, &heap->mTimeTree, ele);
                    ele->mCb (ele, ele->mClosure);
                    ele = RB_MIN (orderedTimeRBTree_, &heap->mTimeTree);
                    /* No timers left so break */
                    if (ele == NULL)
                        break;
                }
            }
        }
endLoop:
        wthread_mutex_unlock (&heap->mLock);
        wthread_mutex_lock   (&heap->mEndingLock);
        heap->mEnding = 1;
        wthread_cond_signal  (&heap->mEndingCond);
        wthread_mutex_unlock (&heap->mEndingLock);
    }
    return NULL;
}
        
int startDispatchTimerHeap (timerHeap heap)
{
    if (heap == NULL)
        return -1;
    {
        timerHeapImpl* heapImpl = (timerHeapImpl*)heap;
        return wthread_create(&heapImpl->mDispatchThread, NULL, dispatchEntry, (void*)heapImpl);
    }
}

wthread_t timerHeapGetTid (timerHeap heap)
{
    if (heap == NULL)
        return INVALID_THREAD;

    {
        timerHeapImpl* heapImpl = (timerHeapImpl*)heap;
        return heapImpl->mDispatchThread;
    }
}

int destroyHeap (timerHeap heap)
{
    if (heap == NULL)
        return -1;

    {
        timerHeapImpl* heapImpl = (timerHeapImpl*)heap;

writeagain:
        if (wwrite (heapImpl->mSockPair[1], "d", 1) < 0)
        {
            if ((errno == EINTR) || (errno == EAGAIN))
                goto writeagain;
            else
            {
                perror ("write()");
                return -1; 
            }    
        }

        wthread_mutex_lock   (&heapImpl->mEndingLock);
        while (heapImpl->mEnding == 0)
        {
            wthread_cond_wait (&heapImpl->mEndingCond,
                               &heapImpl->mEndingLock);
        }
        wthread_mutex_unlock (&heapImpl->mEndingLock);
        free (heapImpl);
    }
    return 0;
}

int createTimer (timerElement* timer, timerHeap heap, timerFireCb cb, struct timeval* timeout, void* closure)
{

    if ((timer == NULL) || (heap == NULL) || (timeout == NULL))
        return -1;

    {
        timerHeapImpl* heapImpl = (timerHeapImpl*)heap;
        int kickPipe = 0;
        struct timeval now;
        timerImpl* nextTimeOut = NULL;
        timerImpl* ele = calloc (1, sizeof(timerImpl));
        if (!ele)
            return -1;

        gettimeofday (&now, NULL);
        timeradd (&now, timeout, &(ele->mTimeout));

        ele->mCb = cb;
        ele->mClosure = closure;

        wthread_mutex_lock (&heapImpl->mLock);
        nextTimeOut = RB_MIN (orderedTimeRBTree_, &heapImpl->mTimeTree);
        if (nextTimeOut == NULL)
            kickPipe = 1;
        else if (!timercmp(&ele->mTimeout, &nextTimeOut->mTimeout, >))
            kickPipe = 1;

        RB_INSERT (orderedTimeRBTree_, &heapImpl->mTimeTree, ele);

        if (kickPipe)
        {
writeagain:
            if (wwrite (heapImpl->mSockPair[1], "w", 1) < 0)
            {
                if ((errno == EINTR) || (errno == EAGAIN))
                    goto writeagain;
                else
                {
                    perror ("write()");
                    wthread_mutex_unlock (&heapImpl->mLock);
                    return -1;
                }
            }
        }
        wthread_mutex_unlock (&heapImpl->mLock);

        *timer = ele;
    }
    return 0;
}
    
int destroyTimer (timerHeap heap, timerElement timer)
{
    if ((heap == NULL) || (timer == NULL))
        return -1;

    {
        timerImpl* ele = (timerImpl*)timer;
        timerHeapImpl* heapImpl = (timerHeapImpl*)heap;

        wthread_mutex_lock (&heapImpl->mLock);
        if (RB_FIND (orderedTimeRBTree_, &heapImpl->mTimeTree, ele))
            RB_REMOVE (orderedTimeRBTree_, &heapImpl->mTimeTree, ele);
        wthread_mutex_unlock (&heapImpl->mLock);
        free (ele);
    }
    return 0;
}

