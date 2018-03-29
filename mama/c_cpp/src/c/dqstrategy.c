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

#include <stdlib.h>
#include <string.h>

#include "mama/mama.h"
#include "mama/msgstatus.h"
#include "mama/reservedfields.h"
#include "mama/timer.h"
#include "subscriptionimpl.h"
#include "imagerequest.h"
#include "dqstrategy.h"
#include "transportimpl.h"
#include "bridge.h"
#include "msgutils.h"
#include "mama/statfields.h"
#include "mama/statscollector.h"
#include "msgimpl.h"

/**
 * Implementations should subclass this method and call queueLimitExceeded if
 * a message queue overflows.
 */

#define self ((dqStrategyImpl*)(strategy))

extern int gGenerateTransportStats;

static mama_status
handleStaleData (
        dqStrategy     strategy, 
        mamaMsg        msg, 
        mamaDqContext* ctx);

mama_status
dqStrategy_create (dqStrategy*        strategy_,
                   mamaSubscription   subscription)
{
    dqStrategyImpl* strategy = 
        (dqStrategyImpl*)calloc (1, sizeof (dqStrategyImpl));

    if (strategy == NULL) return MAMA_STATUS_NOMEM;

    self->mSubscription = subscription;
    self->mRecoverGaps  = 1; 
    *strategy_ = (dqStrategy)strategy;

    return MAMA_STATUS_OK;
}

mama_status
dqStrategy_destroy (dqStrategy strategy)
{
    if (!self) return MAMA_STATUS_NULL_ARG;
    free (self);
    return MAMA_STATUS_OK;
}

static void
dqStrategyImpl_resetDqContext (mamaDqContext* ctx, mama_seqnum_t seqNum,
                               mama_u64_t senderId)
{
    ctx->mSeqNum  = seqNum;
    ctx->mSenderId = senderId;
}

static void
resetDqState (dqStrategy     strategy,
              mamaDqContext* ctx)
{
    if (ctx->mDQState != DQ_STATE_OK &&
            ctx->mDQState != DQ_STATE_NOT_ESTABLISHED)
    {
        void*             closure   =   NULL;
        mamaMsgCallbacks* cb        =   NULL;

        mamaSubscription_getClosure (self->mSubscription, &closure);

        /* Callback last in the event that client destroys */ 
        cb = mamaSubscription_getUserCallbacks (self->mSubscription);
        if (cb != NULL && cb->onQuality != NULL)
        {
            const char* symbol = NULL;
            short       cause;
            const void* platformInfo = NULL;
            mamaSubscription_getSymbol (self->mSubscription, &symbol);
            mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                        self->mSubscription,
                                                        &cause, &platformInfo);
            cb->onQuality (self->mSubscription, MAMA_QUALITY_OK, symbol,
                           cause, platformInfo, closure);
        }
    }
    ctx->mDQState = DQ_STATE_OK;
}

mama_status
dqStrategy_sendRecapRequest (dqStrategyImpl* strategy, mamaMsg srcMsg, mamaDqContext* ctx)
{
    mamaMsg     msg     = NULL;
    const char* symbol  = NULL;
    double      timeout = 0.0;
    int         retries = 0;
    wombat_subscriptionRecapCB onRecap = NULL;
    mamaSubscriptionType subscType;

    ctx->mDQState       = DQ_STATE_WAITING_FOR_RECAP;
    
    mamaSubscription_getSubscriptionType (self->mSubscription, &subscType);
    
    /*TODO Put wildcard subscriptions here also if we ever support them*/
    if (subscType == MAMA_SUBSC_TYPE_GROUP)
    {
        /*If this is a group subscription we must get the issue symbol
         from the message and not the subscription as we must recap on the 
         item symbol and not the group symbol*/
        msgUtils_getIssueSymbol (srcMsg, &symbol); 
    }
    else
    {
        /*Needed as not all updates will contain the issue symbol.
         e.g. book updates*/
        mamaSubscription_getSubscSymbol (self->mSubscription,&symbol);
    }

    mamaSubscription_getRecapTimeout (self->mSubscription, &timeout);
    mamaSubscription_getRetries (self->mSubscription, &retries);

    /* Send a request to the publisher to resend. */
    msgUtils_createRecoveryRequestMsg (self->mSubscription, DQ_UNKNOWN, &msg, symbol);

    if (subscType == MAMA_SUBSC_TYPE_GROUP)
    {
        mamaMsg_updateI32 (msg,
                           MamaFieldSubscriptionType.mName,
                           MamaFieldSubscriptionType.mFid,
                           MAMA_SUBSC_TYPE_NORMAL);

        mamaMsg_updateI32 (msg,
                           MamaFieldSubscMsgType.mName,
                           MamaFieldSubscMsgType.mFid,
                           MAMA_SUBSC_DQ_GROUP_SUBSCRIBER);
    }

    /* We want this throttled*/
    imageRequest_sendRequest (ctx->mRecapRequest,
                              msg, 
                              timeout, 
                              retries,
                              1,/*throttle the request*/
                              1); /*Is a recap request*/

    if ((onRecap = (mamaSubscription_getUserCallbacks (
                                    self->mSubscription)->onRecapRequest)))
    { 
        void* closure = NULL;
        mamaSubscription_getClosure (self->mSubscription, &closure);
        onRecap (self->mSubscription, closure); 
    }
    
    return MAMA_STATUS_OK;
}

mama_status dqStrategy_setPossiblyStale (mamaDqContext* ctx)
{
    ctx->mDQState = DQ_STATE_POSSIBLY_STALE;
    return MAMA_STATUS_OK;
}
mama_status dqStrategy_getDqState (mamaDqContext* ctx, dqState* state)
{
    *state = ctx->mDQState;
    return MAMA_STATUS_OK;
}

mama_status
dqContext_clearCache(mamaDqContext *ctx, int freeArray)
{
        if (ctx->mCache != NULL)
        {
            int i = 0;
            for (i = 0; i < ctx->mCacheSize; i++)
            {
                if (ctx->mCache[i] != NULL)
                {
                    mamaMsg_destroy (ctx->mCache[i]);
                    ctx->mCache[i] = NULL;
                }
            }
            ctx->mCurCacheIdx = 0;

            if (freeArray)
            {
                free (ctx->mCache);
                ctx->mCache = NULL;
            }
        }
        return MAMA_STATUS_OK;
}

mama_status
dqContext_initializeContext (mamaDqContext *ctx, int cacheSize,
                             imageRequest    recap)
{
    ctx->mDQState       = DQ_STATE_NOT_ESTABLISHED;
    ctx->mSeqNum        = 0;
    ctx->mCurCacheIdx   = 0;
    ctx->mSenderId      = 0;
    ctx->mSenderId      = 0;
    ctx->mRecapRequest  = recap;
    ctx->mSetCacheMsgStale = 0;

    if (cacheSize > 0)
    {
        /*We may be simply resetting the context*/
        if (!ctx->mCache)
        {
            ctx->mCache = (mamaMsg*)calloc (cacheSize, sizeof (mamaMsg));
            ctx->mCacheSize = cacheSize;
        }
    }
    else
    {
        ctx->mCache = NULL;
    }
    
    return MAMA_STATUS_OK;
}


mama_status
dqContext_cleanup (mamaDqContext* ctx)
{
    dqContext_clearCache (ctx, 1);
    if (ctx->mRecapRequest)
    {
        imageRequest_destroy (ctx->mRecapRequest);
        ctx->mRecapRequest = NULL;
    }
    return MAMA_STATUS_OK;
}

mama_status dqStrategyImpl_detachMsg (mamaDqContext* ctx, mamaMsg msg)
{
    mama_seqnum_t    seqNum         = 0;
    mama_seqnum_t    seqNumCached   = 0;
    int i = 0;

    mamaMsg_getSeqNum (msg, &seqNum);
    
    if (ctx->mCache != NULL)
    {
        for (i = 0; i < ctx->mCacheSize; i++)
        {
            if (ctx->mCache[i] != NULL)
            {
                mamaMsg_getSeqNum (ctx->mCache[i], &seqNumCached);      
                if( seqNum == seqNumCached)
                {
                    /*The message has been detached from the cache...
                    the cache is not responsible for destroying it*/
                    ctx->mCache[i]=NULL;

                    mama_log (MAMA_LOG_LEVEL_FINER, 
                    "A msg with seqNum:%d has been detached from cache.", seqNum);
    
                    break;
                }
            }
        }
    }

    return MAMA_STATUS_OK;
}

MAMAExpDLL
mama_status dqStrategy_setRecoverGaps(dqStrategy strategy, int newValue)
{
    if (NULL != strategy)
    {
        self->mRecoverGaps = newValue;
        return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NULL_ARG;
}

MAMAExpDLL
mama_status dqStrategy_getRecoverGaps(dqStrategy strategy, int *result)
{
    if (NULL != strategy)
    {
        *result = self->mRecoverGaps;
        return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NULL_ARG;
}

mama_status
dqContext_cacheMsg (mamaDqContext *ctx, mamaMsg msg)
{
    mama_status     status  =   MAMA_STATUS_OK;

    if (ctx->mCache == NULL)
    {
        return status;
    }

    if (ctx->mCache[ctx->mCurCacheIdx] != NULL)
    {
        mamaMsg_destroy (ctx->mCache[ctx->mCurCacheIdx]);
    }

    mamaMsg_detach (msg);
    ctx->mCache [ctx->mCurCacheIdx++] = msg;

    if (ctx->mCurCacheIdx == ctx->mCacheSize)
    {
        ctx->mCurCacheIdx = 0;
    }

    /*Associate the cache context with the message.
      This must be done after it has been detached*/
    if (MAMA_STATUS_OK!=(status=mamaMsgImpl_setDqStrategyContext (msg,
                    ctx)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "dqContext_cacheMsg: "
                  "Could not set context on detached message.");
        return status;
    }

    return status;
}

