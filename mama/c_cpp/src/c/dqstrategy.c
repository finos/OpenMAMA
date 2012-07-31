/* $Id: dqstrategy.c,v 1.66.4.1.16.5 2011/09/01 09:41:02 emmapollock Exp $
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

typedef struct dqStrategy_
{
    /* Data Quality Members */    
    mamaSubscription    mSubscription;
    short               mTryToFillGap;
} dqStrategyImpl;



static mama_status
handleStaleData (
        dqStrategy     strategy, 
        mamaMsg        msg, 
        mamaDqContext* ctx);

static int
dqContext_fillGap (mamaDqContext *ctx, 
                   mama_seqnum_t seqNum, 
                   mamaSubscription subscription);

mama_status
dqStrategy_create (dqStrategy*        strategy_,
                   mamaSubscription   subscription)
{
    dqStrategyImpl* strategy = 
        (dqStrategyImpl*)calloc (1, sizeof (dqStrategyImpl));

    if (strategy == NULL) return MAMA_STATUS_NOMEM;

    self->mSubscription = subscription;
    
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

/*
    Responding to a message with a sender id different to the previous one
    received is now straightforward.

    We simply accept that this is the next expected message and reset the
    context state - all is OK
 */
static mama_status
handleFTTakeover (dqStrategy        strategy,
                  mamaMsg           msg,
                  int               msgType,
                  mamaDqContext*    ctx,
                  mama_seqnum_t     seqNum,
                  mama_u64_t        senderId,
                  int               recoverOnRecap)
{
    const char*         symbol  = NULL;
    mamaSubscription_getSymbol (self->mSubscription, &symbol);

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Detected FT takeover. "
            "Original SenderId: %llu. New SenderId: %llu. "
            "Previous SeqNum: %u. New SeqNum: %u. [%s]",
            ctx->mSenderId, senderId, ctx->mSeqNum, seqNum, symbol);

    if (recoverOnRecap)
    {
        ctx->mSeqNum = senderId;
        ctx->mDQState = DQ_STATE_WAITING_FOR_RECAP_AFTER_FT;
    }
    else
    {
    resetDqState (strategy, ctx);
    
    /*In all cases we reset the data quality context*/
    dqStrategyImpl_resetDqContext (ctx, seqNum, senderId);
    
    }
    return MAMA_STATUS_OK;
}

mama_status 
dqStrategy_checkSeqNum (dqStrategy      strategy, 
                        mamaMsg         msg, 
                        int             msgType, 
                        mamaDqContext*  ctx)
{
    mama_seqnum_t    seqNum         = 0;
    mama_seqnum_t    ctxSeqNum      = ctx->mSeqNum;
    mama_u64_t       senderId       = 0;
    mama_u64_t       ctxSenderId    = ctx->mSenderId;
    dqState          ctxDqState     = ctx->mDQState;
    mamaSubscription subscription   = self->mSubscription;
    mama_u16_t       conflateCnt    = 1;
    wombat_subscriptionGapCB onGap  = NULL;
    mamaTransport    transport; 
    mamaStatsCollector* transportStatsCollector = NULL;
    wombat_subscriptionQualityCB onQuality = NULL;
    mamaMsgStatus    msgStatus      = MAMA_MSG_STATUS_UNKNOWN;
    mamaTransport    tport        = NULL;

    mamaSubscription_getTransport (self->mSubscription, &tport);
    
    mamaMsg_getSeqNum (msg, &seqNum);

    ctx->mDoNotForward = 0;
    if (mamaMsg_getU64 (msg, MamaFieldSenderId.mName, MamaFieldSenderId.mFid, 
                        &senderId) != MAMA_STATUS_OK)
    {
        /* We just ignore it as we might be running against an older FH */
        senderId = 0;
    }

    /*Special case for dealing with a fault tolerant takeover*/
    if (ctxSenderId != 0 && senderId != 0 && ctxSenderId != senderId)
    {
        /* Only record FT Takeovers per transport and globally */
        if (gGenerateTransportStats)
        {
            mamaSubscription_getTransport (subscription, &transport);
            transportStatsCollector = mamaTransport_getStatsCollector (transport);

            mamaStatsCollector_incrementStat (*transportStatsCollector, 
                                              MamaStatFtTakeovers.mFid);
        }
        if (mamaInternal_getGlobalStatsCollector() != NULL)
        {
            mamaStatsCollector_incrementStat (*(mamaInternal_getGlobalStatsCollector()), 
                                              MamaStatFtTakeovers.mFid);
        }
        if (DQ_FT_WAIT_FOR_RECAP==mamaTransportImpl_getFtStrategyScheme(tport))
        {
            handleFTTakeover (strategy, msg, msgType, ctx, seqNum, senderId, 1);
        }
        else
        {
            return handleFTTakeover (strategy, msg, msgType, ctx, seqNum, senderId, 0);
        }
    }

    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINER)
    {
        const char*  symbol = NULL;
        mamaSubscription_getSymbol (subscription, &symbol);
        symbol = symbol == NULL ? "" : symbol;
        mama_log (MAMA_LOG_LEVEL_FINER, 
                  "dqStrategy_checkSeqNum(): %s : seq# %ld",
                  symbol, 
                  seqNum);
    }

    switch (msgType)
    {
    case MAMA_MSG_TYPE_QUOTE:
    case MAMA_MSG_TYPE_TRADE:
    case MAMA_MSG_TYPE_UPDATE:
    default:
        if (MAMA_STATUS_OK != mamaMsg_getU16 (msg,
                          MamaFieldConflateCount.mName, 
                          MamaFieldConflateCount.mFid, 
                          &conflateCnt))
        {
            /*Need to set conflateCnt=1 as mamaMsg_getU16 sets conflateCnt=0 
            if not found */
            conflateCnt = 1;
        }
    /* Deliberate fallthrough.  Only the types above can be conflated, so only 
       try to extract the conflate count field for those types. */
    case MAMA_MSG_TYPE_SEC_STATUS:
    case MAMA_MSG_TYPE_BOOK_UPDATE:
        if (((ctxDqState == DQ_STATE_NOT_ESTABLISHED) ||
             (seqNum == 0) ||
             (seqNum == (ctxSeqNum + conflateCnt))) &&
             ((ctxDqState != DQ_STATE_WAITING_FOR_RECAP) ||
             (ctxDqState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)))
        {
            /* No gap */
            if (self->mTryToFillGap)
            {
                self->mTryToFillGap = 0;
                dqContext_clearCache (ctx, 0);
            }

            /* It is no longer the case that all subscriptions are possibly
               stale. */
            mamaSubscription_unsetAllPossiblyStale (subscription);
            /* If the sequence numbers for a message are correct then the
               subscription is OK. */
                
        msgStatus = mamaMsgStatus_statusForMsg (msg);
        /* Check the status of the message.  If it is stale,
           do not request a recap and do not set status OK. */
        if (msgStatus == MAMA_MSG_STATUS_OK) 
            resetDqState (strategy, ctx);

            ctx->mSeqNum = seqNum; 
            return MAMA_STATUS_OK;
        }

        /* For late joins or middlewares that support a publish cache, it is possible that you will get old updates
           in this case take no action */
        if (DQ_SCHEME_INGORE_DUPS==mamaTransportImpl_getDqStrategyScheme(tport))
        {
            if ((seqNum <= ctxSeqNum) && ((ctxDqState != DQ_STATE_WAITING_FOR_RECAP) ||
                                          (ctxDqState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)))
            {
                ctx->mDoNotForward = 1;
                return MAMA_STATUS_OK;
            }
        }

        if ((seqNum == ctxSeqNum) && (ctxDqState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT))
        {
            /* Duplicate data - set DQQuality to DUPLICATE, invoke quality callback */
            ctx->mDQState = DQ_STATE_DUPLICATE; 
            if ((onQuality = (mamaSubscription_getUserCallbacks (subscription))->onQuality))
            {
                void*               closure = NULL;
                const char* symbol = NULL;
                short       cause;
                const void* platformInfo = NULL;
                mamaSubscription_getClosure (subscription, &closure);
                mamaSubscription_getSymbol (subscription, &symbol);
                mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                            subscription,
                                                            &cause, &platformInfo);
                onQuality (subscription, MAMA_QUALITY_DUPLICATE, symbol,
                           cause, platformInfo, closure);
            }
            msgUtils_setStatus (msg, MAMA_MSG_STATUS_DUPLICATE);
            return MAMA_STATUS_OK;
        }
      
        if (ctxDqState == DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)
        {
            ctx->mDoNotForward = 1;
            return MAMA_STATUS_OK;
        }
        else
        {
        /* If we get here, we missed a sequence number. */
         if ((PRE_INITIAL_SCHEME_ON_GAP==mamaTransportImpl_getPreInitialScheme(tport))
                &&(self->mTryToFillGap))
        {
            self->mTryToFillGap = 0;
            if (dqContext_fillGap (ctx, seqNum, subscription))
            {
                /* we filled it */
                dqContext_clearCache (ctx, 0);
                ctx->mSeqNum = seqNum;
                return MAMA_STATUS_OK;
            }
            dqContext_clearCache (ctx, 0);
        }

        if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINE)
        {
            const char*  symbol = NULL;
            mamaSubscription_getSymbol (subscription, &symbol);
            symbol = symbol == NULL ? "" : symbol;
            mama_log (MAMA_LOG_LEVEL_FINE, "%s : SeqNum gap (%ld-%ld)",
                      symbol, ctxSeqNum+1, seqNum-1);
        }

        if ((onGap = 
            (mamaSubscription_getUserCallbacks (subscription)->onGap)))
        {
            void* closure = NULL;
            mamaSubscription_getClosure (subscription, &closure);
            onGap (subscription, closure);
        }

        handleStaleData (self, msg, ctx);
        }
        break;
    case MAMA_MSG_TYPE_INITIAL      :
    case MAMA_MSG_TYPE_BOOK_INITIAL :
        msgStatus = MAMA_MSG_STATUS_UNKNOWN;

        self->mTryToFillGap = 1;

        msgStatus = mamaMsgStatus_statusForMsg (msg);
        /* Check the status of the message.  If it is stale,
           do not request a recap and do not set status OK. */
        if ((msgStatus == MAMA_MSG_STATUS_POSSIBLY_STALE) ||
            (msgStatus == MAMA_MSG_STATUS_STALE))
        {
            ctx->mDQState = DQ_STATE_STALE_NO_RECAP; 
            dqStrategyImpl_resetDqContext (ctx, seqNum, senderId); 
            return MAMA_STATUS_OK; 
        }
    case MAMA_MSG_TYPE_RECAP        :
    case MAMA_MSG_TYPE_BOOK_RECAP   :

        mamaSubscription_unsetAllPossiblyStale (subscription);
        resetDqState (strategy, ctx);
        dqStrategyImpl_resetDqContext (ctx, seqNum, senderId);
        return MAMA_STATUS_OK;
    case MAMA_MSG_TYPE_DDICT_SNAPSHOT : /*No DQ checking for Datadictionary*/
        return MAMA_STATUS_OK;
    }
    return MAMA_STATUS_OK;
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

    mamaSubscription_getTimeout (self->mSubscription, &timeout);
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

static mama_status
handleStaleData (dqStrategy     strategy,
                 mamaMsg        msg,
                 mamaDqContext* ctx)
{
    const char*  symbol = NULL;
    mamaSubscription_getSymbol (self->mSubscription, &symbol);

    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINER)
    {
        symbol = symbol == NULL ? "" : symbol;
        mama_log (MAMA_LOG_LEVEL_FINER, "%s : stale data", symbol);
    }

    msgUtils_setStatus (msg, MAMA_MSG_STATUS_STALE);

    if (ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP &&
        ctx->mDQState != DQ_STATE_POSSIBLY_STALE)
    {
        void*               closure = NULL;
        mamaMsgCallbacks*   cb      = NULL;

        dqStrategy_sendRecapRequest (self, msg, ctx);

        /* Callback last in the event that client destroys */ 
        mamaSubscription_getClosure (self->mSubscription, &closure);
        cb = mamaSubscription_getUserCallbacks (self->mSubscription);

        if (cb != NULL && cb->onQuality != NULL)
        {
            short       cause;
            const void* platformInfo = NULL;
            mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                        self->mSubscription,
                                                        &cause, &platformInfo);
            cb->onQuality (self->mSubscription, MAMA_QUALITY_STALE, symbol, 
                           cause, platformInfo, closure);
        }
    }
    else if (ctx->mDQState == DQ_STATE_POSSIBLY_STALE &&
             ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP)
    {
        dqStrategy_sendRecapRequest (self, msg, ctx);
    }

    return MAMA_STATUS_OK;
}

mama_status dqStrategy_setPossiblyStale (mamaDqContext* ctx)
{
    ctx->mDQState = DQ_STATE_POSSIBLY_STALE;
    return MAMA_STATUS_OK;
}
mama_status dqStrategy_getDqState (mamaDqContext ctx, dqState* state)
{
    *state = ctx.mDQState;
    return MAMA_STATUS_OK;
}

mama_status dqContext_clearCache (mamaDqContext *ctx, int freeArray)
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

mama_status
dqContext_applyPreInitialCache (mamaDqContext*      ctx,
                                mamaSubscription    subscription)
{
    /*We looking for the next sequence number in the cache*/
    mama_seqnum_t expectedSeqNum = ctx->mSeqNum + 1; 
    int currentMessageindex = 0;
    const char* symbol = NULL;

    /*Ensure we have a cahce to iterate*/
    if (ctx->mCache == NULL) return MAMA_STATUS_OK;

    mamaSubscription_getSymbol (subscription, &symbol);

    mama_log (MAMA_LOG_LEVEL_NORMAL, 
              "%s: Attempting to apply pre initial cache after initial.",
              symbol);
    
    /*Loop from start to last added message - may have looped over*/
    while (currentMessageindex<ctx->mCurCacheIdx)
    {
        mama_seqnum_t cachedMessageSeqNum = 0;

        /*just in case*/
        if (!ctx->mCache[currentMessageindex]) return MAMA_STATUS_OK;
        
        mamaMsg_getSeqNum (ctx->mCache[currentMessageindex], 
                           &cachedMessageSeqNum);
        mama_log (MAMA_LOG_LEVEL_NORMAL, 
                  "%s: Found cached msg withseqNum: %d Current [%d]", 
                  symbol, cachedMessageSeqNum, ctx->mSeqNum);

        if (expectedSeqNum==cachedMessageSeqNum)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL, 
                     "%s: Applying cached message after initial", symbol);
            /*Next expected - pass it on!*/
            mamaSubscription_forwardMsg(subscription,
                                        ctx->mCache[currentMessageindex]);

            /*this is now the latest sequence number for the context*/
            ctx->mSeqNum   = cachedMessageSeqNum;
            expectedSeqNum = cachedMessageSeqNum+1;
            
            /*Lets see if there are any more in the cache*/
        }
        else /*Not the expected message - have a couple of options*/
        {
            /*Bomb out - we can't apply this or any subsequent messages
             as they are stored in arrival order*/
            if (cachedMessageSeqNum>expectedSeqNum)
            {
                mama_log (MAMA_LOG_LEVEL_NORMAL, 
                          "%s: Can't apply cached message", symbol);
                break;
            }
        }
        currentMessageindex++;
    }
    
    return MAMA_STATUS_OK;
}

int
dqContext_fillGap (mamaDqContext *ctx, mama_seqnum_t end, mamaSubscription subscription)
{
    mama_log (MAMA_LOG_LEVEL_FINE, "Attempting to fill gap from cache.");
    if (ctx->mCache != NULL)
    {
        mama_seqnum_t begin = ctx->mSeqNum + 1; 
        int cur = 0;
        mama_seqnum_t nextSeqNum = begin;

        if (ctx->mCache[ctx->mCurCacheIdx] == NULL)
        {
            /* Cache not full; */
            cur = 0;
        }
        else
        {
            cur = ctx->mCurCacheIdx;
        }
        
        do
        {
            mama_seqnum_t curSeqNum = 0;
            if (ctx->mCache[cur] == NULL) /* no more left */
            {
                break;
            }

            mamaMsg_getSeqNum (ctx->mCache[cur], &curSeqNum);

            mama_log (MAMA_LOG_LEVEL_FINE,
                      "Found cached msg with seqNum: %d",
                      curSeqNum);
            if (curSeqNum == nextSeqNum)
            {
                mama_log (MAMA_LOG_LEVEL_FINE, 
                               "Found a message for gap.");
                mamaSubscription_forwardMsg(subscription, ctx->mCache[cur]);

                if (++nextSeqNum == end)
                {
                    mama_log (MAMA_LOG_LEVEL_FINE, 
                               "Filled gap.");
                    return 1;
                }
            }

            cur++;
            if (cur == ctx->mCacheSize)
            {
                cur = 0;
            }

        } while (cur != ctx->mCurCacheIdx);
    }

    return 0;
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
