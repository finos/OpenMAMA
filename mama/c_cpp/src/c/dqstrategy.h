/* $Id: dqstrategy.h,v 1.14.4.1.16.3 2011/09/01 16:34:37 emmapollock Exp $
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

#ifndef DqStrategyH__
#define DqStrategyH__
#include "imagerequest.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum dqState_
{

    DQ_STATE_OK = 0,
    /**
     * No Sequence Number Established Yet.
     */
    DQ_STATE_NOT_ESTABLISHED  = 1,

    /**
     * Received a stale message and are waiting for a recap.
     */
    DQ_STATE_WAITING_FOR_RECAP = 2,
    DQ_STATE_POSSIBLY_STALE    = 3,
    /**
     * Received a message with duplicate sequence number
     */ 
    DQ_STATE_DUPLICATE         = 4,

    /** 
     * In the case of a stale initial, we do not want
     * a recap because it may also be stale data.
     */
    DQ_STATE_STALE_NO_RECAP    = 5,

    DQ_STATE_WAITING_FOR_RECAP_AFTER_FT    = 6
} dqState;
    
typedef struct 
{
    mama_seqnum_t mSeqNum;
    dqState       mDQState;

    mamaMsg*      mCache;
    int           mCurCacheIdx;
    int           mCacheSize;
    imageRequest  mRecapRequest;
    mama_u64_t    mSenderId;

    uint8_t       mDoNotForward;
} mamaDqContext;

typedef struct dqStrategy_* dqStrategy;

mama_status
dqStrategy_create (
    dqStrategy*         strategy,  
    mamaSubscription    subscription);

mama_status
dqContext_initializeContext (
    mamaDqContext*  ctx,
    int             cacheSize,
    imageRequest    recap);

mama_status
dqContext_cleanup (mamaDqContext* ctx);

mama_status
dqContext_applyPreInitialCache (mamaDqContext*      ctx,
                                mamaSubscription    subscription);
mama_status
dqContext_clearCache (mamaDqContext *ctx, int freeArray);

mama_status
dqContext_cacheMsg (mamaDqContext *ctx, mamaMsg msg);

mama_status
dqStrategyImpl_detachMsg (mamaDqContext* ctx, mamaMsg msg);

mama_status
dqStrategy_destroy (
    dqStrategy          strategy);

mama_status
dqStrategy_checkSeqNum (
    dqStrategy          strategy,
    mamaMsg             msg,
    int                 msgType,
    mamaDqContext       *ctx);

mama_status
dqStrategy_getDqState (
    mamaDqContext       ctx,
    dqState*            state);

mama_status
dqStrategy_setPossiblyStale (
    mamaDqContext*      ctx);

    

mama_status
dqStrategy_sendRecapRequest (
        dqStrategy strategy, 
        mamaMsg         srcMsg, 
        mamaDqContext*  ctx);
        
#ifdef QLIMIT
mama_status
dqStrategy_setAdvisoryInterval (
    dqStrategy          strategy,
    double              howLong);

double 
dqStrategy_getAdvisoryInterval (
    dqStrategy          strategy);

/**
 * Subclasses invoke this method when the queue limit is execeeded.
 *
 */
protected synchronized  void queueLimitExceeded ( Object closure)

private void startQueueLimitExceededTimer ()

#endif /* QLIMIT */

#if defined(__cplusplus)
}
#endif

#endif /* DqStrategyH__ */
