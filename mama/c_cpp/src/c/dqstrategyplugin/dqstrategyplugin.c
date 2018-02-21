/*
 * dqstrategyplugin.c
 *
 *  Created on: 25 Oct 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mama/mama.h"
#include "mama/status.h"
#include "mama/types.h"
#include "dqstrategyplugin.h"
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
#include "refreshtransport.h"
/**
 * Implementations should subclass this method and call queueLimitExceeded if
 * a message queue overflows.
 */

#define strategyImpl ((dqStrategyImpl*)(strategy))
#define DQSTRATEGY_PLUGIN_NAME "dqstrategy"

static int 
handleEvent (mamaPluginInfo pluginInfo, mamaTransport transport);

static int 
isInitialMessageOrRecap(int msgType);

static int
fillGap (mamaDqContext *ctx,
                   mama_seqnum_t seqNum,
                   mamaSubscription subscription);

static mama_status
checkSeqNum (dqStrategy strategy, mamaMsg msg, int msgType, mamaDqContext *ctx);

static mama_status
applyPreInitialCache (mamaDqContext *ctx, mamaSubscription subscription);

static void
resetDqContext (mamaDqContext* ctx, mama_seqnum_t seqNum,
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

        mamaSubscription_getClosure (strategyImpl->mSubscription, &closure);

        /* Callback last in the event that client destroys */
        cb = mamaSubscription_getUserCallbacks (strategyImpl->mSubscription);
        if (cb != NULL && cb->onQuality != NULL)
        {
            const char* symbol = NULL;
            short       cause;
            const void* platformInfo = NULL;
            mamaSubscription_getSymbol (strategyImpl->mSubscription, &symbol);
            mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                        strategyImpl->mSubscription,
                                                        &cause, &platformInfo);
            cb->onQuality (strategyImpl->mSubscription, MAMA_QUALITY_OK, symbol,
                           cause, platformInfo, closure);
        }
    }
    ctx->mDQState = DQ_STATE_OK;
}

static mama_status
handleStaleData (dqStrategy     strategy,
                 mamaMsg        msg,
                 mamaDqContext* ctx)
{
    const char*  symbol = NULL;
    mamaSubscription_getSymbol (strategyImpl->mSubscription, &symbol);

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

        dqStrategy_sendRecapRequest (strategyImpl, msg, ctx);

        /* Callback last in the event that client destroys */
        mamaSubscription_getClosure (strategyImpl->mSubscription, &closure);
        cb = mamaSubscription_getUserCallbacks (strategyImpl->mSubscription);

        if (cb != NULL && cb->onQuality != NULL)
        {
            short       cause;
            const void* platformInfo = NULL;
            mamaSubscription_getAdvisoryCauseAndPlatformInfo (
                                                        strategyImpl->mSubscription,
                                                        &cause, &platformInfo);
            cb->onQuality (strategyImpl->mSubscription, MAMA_QUALITY_STALE, symbol,
                           cause, platformInfo, closure);
        }
    }
    else if (ctx->mDQState == DQ_STATE_POSSIBLY_STALE &&
             ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP)
    {
        dqStrategy_sendRecapRequest (strategyImpl, msg, ctx);
    }

    return MAMA_STATUS_OK;
}

static void processPointToPointMessage (mamaMsg         msg,
                                        int             msgType,
                                        SubjectContext  *ctx,
                                        mamaSubscription subsc)
{
    const char*    userSymbol  = NULL;
    short          total       = 0;
    short          msgNo       = 0;
    mamaTransport  tport       = NULL;
    dqStrategy     strategy;
    mamaDqContext* context;

    mamaSubscription_getSymbol (subsc, &userSymbol);
    mamaSubscription_getTransport (subsc, &tport);

    if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINER) ||
        (mamaSubscription_checkDebugLevel (subsc,
                                           MAMA_LOG_LEVEL_FINER)))
    {
        const char* subscSymbol = NULL;
        mamaSubscription_getSubscSymbol (subsc, &subscSymbol);
        mama_log (MAMA_LOG_LEVEL_FINER,
                       "processPointToPointMessage(): Got unicast message(?) "
                       "for %s (%s) (type=%d; subsc=%p) %p",
                       subscSymbol == NULL ? "" : subscSymbol,
                       userSymbol  == NULL ? "" : userSymbol,
                       mamaMsgType_typeForMsg(msg),
                       subsc,
                       ctx);
    }

    /* The caller should not see the inbox as the symbol. */
    mamaMsgImpl_setSubscInfo (msg, NULL, NULL, userSymbol, 0);

    msgUtils_msgTotal (msg, &total);
    msgUtils_msgNum (msg, &msgNo);

    /*Regular updates cannot stop the subscription waiting on a response.
     We will be receiving updates while waiting on a recap.*/
    if (total == 0 || msgNo == total)
    {
        mamaSubscription_incrementInitialCount(subsc);
        ++ctx->mImageCount;
        /*For group subs the timeout for the imagerequest will
         call stopWaitForResponse*/
        ctx->mInitialArrived = 1;


        if( gMamaLogLevel >= MAMA_LOG_LEVEL_FINE )
        {
            const char *msgString = mamaMsg_toString( msg );
            mama_log (MAMA_LOG_LEVEL_FINE, "Received Initial: (%s) %s", userSymbol, msgString);
        }
        if (!mamaSubscription_getAcceptMultipleInitials (subsc))
        {
            mamaSubscription_stopWaitForResponse (subsc, ctx);
        }
    }

    strategy = mamaSubscription_getDqStrategy(subsc);
    context  = mamaSubscription_getDqContext(subsc);
    checkSeqNum(strategy, msg, msgType, context);

    /* Mark the subscription as inactive if we are not expecting
     * any more updates. */
    if (!mamaSubscription_isExpectingUpdates (subsc) &&
            !mamaSubscription_getAcceptMultipleInitials (subsc))
    {
        mamaSubscription_deactivate (subsc);
    }

    if (!ctx->mDqContext.mDoNotForward)
    {
        mamaSubscription_forwardMsg (subsc, msg);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "Subscription for %s not forwarded as message seqnum is before seqnum expecting", userSymbol);
    }

    /*
       NB!!!  - can't destroy a subscription after an initial!!!!!
       After we forward this message we need to see if we should fill from
     the pre initial cache*/
    if (PRE_INITIAL_SCHEME_ON_INITIAL==
            mamaTransportImpl_getPreInitialScheme (tport))
    {
        if (msgType==MAMA_MSG_TYPE_INITIAL || msgType == MAMA_MSG_TYPE_BOOK_INITIAL ||
           (mamaTransportImpl_preRecapCacheEnabled (tport) &&  (msgType == MAMA_MSG_TYPE_RECAP || msgType == MAMA_MSG_TYPE_BOOK_RECAP )))
        {
            applyPreInitialCache(&ctx->mDqContext, subsc);
            /*Clear the messages - no longer needed*/
            dqContext_clearCache(&ctx->mDqContext, 0);
        }
    }


    /* Note: do NOT access the "strategyImpl" members because the subscription
     * may have been destroyed in the callback! */
}

mama_status
dqstrategyMamaPlugin_initHook (mamaPluginInfo* pluginInfo)
{
    *pluginInfo = calloc (1, sizeof (mamaPluginInfo));

    mama_status status = MAMA_STATUS_OK;

    return status;
}

mama_status
dqstrategyMamaPlugin_transportPostCreateHook (mamaPluginInfo pluginInfo, mamaTransport transport)
{
    const char*     middleware = NULL;
    const char*     tportName  = NULL;
    const char*     pluginName = NULL;
    mamaBridgeImpl* bridgeImpl = NULL;
    int             dqEnabled  = 1;
    mamaPluginImpl* pluginImpl;
    char            propName[256];

    mamaTransportImpl_getDqEnabled(transport, &dqEnabled);

    pluginImpl = (mamaPluginImpl*)pluginInfo;

    bridgeImpl = mamaTransportImpl_getBridgeImpl (transport);
    middleware = bridgeImpl->bridgeGetName ();
    
    mamaTransport_getName (transport, &tportName);

    snprintf(propName, sizeof (propName), "mama.%s.transport.%s.%s", middleware, tportName, DQ_PLUGIN_PROPERTY);
    pluginName = mama_getProperty(propName);
    //Load the default DQ plugin if no other DQ plugin is specified, but also allow it to be explicitly specified
    if (dqEnabled)
    {
        if(!pluginName || (0 == strcmp(pluginName , "dqstrategy")))
        {
            mamaTransportImpl_setDqPluginInfo (transport, pluginInfo);
        }
    }

    mama_status status = MAMA_STATUS_OK;

    return status;
}

mama_status
dqstrategyMamaPlugin_transportEventHook(mamaPluginInfo pluginInfo, mamaTransport transport, int setStale, mamaTransportEvent tportEvent)
{
    int possiblyStaleForAll;
    refreshTransport refreshTransport;

    if (!(handleEvent (pluginInfo, transport)))
    {
        return MAMA_STATUS_OK;
    }
   
    switch (tportEvent)
    {
        case MAMA_TRANSPORT_DISCONNECT:
            
            if (setStale)
            {
                mamaTransportImpl_getPossiblyStaleForAll(transport, &possiblyStaleForAll);

                if (possiblyStaleForAll)
                {
                    mamaTransportImpl_setPossiblyStaleForListeners (transport);
                }

                mamaTransportImpl_setQuality(transport, MAMA_QUALITY_MAYBE_STALE);

            }

            break;

        case MAMA_TRANSPORT_QUALITY:

            mamaTransportImpl_getPossiblyStaleForAll(transport, &possiblyStaleForAll);

            if (possiblyStaleForAll)
            {
                 mamaTransportImpl_setPossiblyStaleForListeners (transport);
                 mamaTransportImpl_getRefreshTransport(transport, &refreshTransport);

                if (refreshTransport)
                {
                    refreshTransport_startStaleRecapTimer (refreshTransport);
                }
            }
            
            mamaTransportImpl_setQuality(transport, MAMA_QUALITY_MAYBE_STALE);

            break;

    }
    
    return MAMA_STATUS_OK;
}

mama_status
dqstrategyMamaPlugin_subscriptionPostCreateHook (mamaPluginInfo pluginInfo, mamaSubscription subscription)
{
    mamaTransport   transport = NULL;
    dqStrategy strategy  = NULL;

    mamaSubscription_getTransport (subscription, &transport);

    if (!handleEvent (pluginInfo, transport))
    {
        return MAMA_STATUS_OK;
    }

    dqStrategy_create(&strategy, subscription);

    if (strategy == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }

    strategyImpl->mSubscription = subscription;
    mamaSubscription_setDqStrategy (subscription, strategy);

    return MAMA_STATUS_OK;
}

mama_status
dqstrategyMamaPlugin_subscriptionPreMsgHook(mamaPluginInfo pluginInfo, mamaSubscription subscription, int msgType, mamaMsg msg)
{
    mamaDqContext   *ctx             = mamaSubscription_getDqContext(subscription);
    SubjectContext* subjectContext   = mamaSubscription_getSubjectContext(subscription, msg);
    dqStrategy      strategy         = mamaSubscription_getDqStrategy(subscription);
    int             expectingInitial = 0;
    dqState         state            = DQ_STATE_NOT_ESTABLISHED;
    const char*     userSymbol       = NULL;
    mamaTransport   transport;
   
    mamaSubscription_getTransport (subscription, &transport);

    if (!handleEvent (pluginInfo, transport))
    {
        return MAMA_STATUS_OK;
    }

    if (isInitialMessageOrRecap(msgType))
    {
        processPointToPointMessage (msg, msgType, subjectContext, subscription);
        return MAMA_STATUS_OK;
    }

    /* If we're waiting on an initial for this subscription
        then we ignore all messages. Note that after the initial
        request timeout period individual symbols in a group
        subscription may have not received an initial, but this
        is ok. Also, we continue to process messages when waiting
        for recaps for in individual symbol
     */
    mamaSubscription_getExpectingInitial (subscription, &expectingInitial);
    dqStrategy_getDqState (&subjectContext->mDqContext, &state);

    /*While we are waiting for initial values we also check whether we have an
     * initial for an individual context.
      If we are no longer waiting for initials we assume that it is ok to pass
     on the update - (probably a new symbol for a group)*/
    if ((expectingInitial && !subjectContext->mInitialArrived) ||
        (state == DQ_STATE_WAITING_FOR_RECAP && mamaTransportImpl_preRecapCacheEnabled (transport)
        && msgType != MAMA_MSG_TYPE_DELETE
        && msgType != MAMA_MSG_TYPE_EXPIRE
        && msgType != MAMA_MSG_TYPE_UNKNOWN))
    {
        /* If we are waiting for a recap and using the pre-recap cache, we
           want to pass on any cached updates as STALE when the recap arrives */
        if (state == DQ_STATE_WAITING_FOR_RECAP && mamaTransportImpl_preRecapCacheEnabled (transport))
        {
            subjectContext->mDqContext.mSetCacheMsgStale = 1;
        }
        /*Add this message to the cache. If the message after the initial
         * results in a gap we will attempt to fill the gap from this cache
         * before asking for a recap.*/
        dqContext_cacheMsg(ctx, msg);
        /* Response for initial value not yet received. */
        if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINE) ||
            (mamaSubscription_checkDebugLevel (subscription,
                                               MAMA_LOG_LEVEL_FINE)))
        {
            mamaSubscription_getSymbol (subscription, &userSymbol);

            mama_log (MAMA_LOG_LEVEL_FINE,
                           "%s%s %s%s"
                           " Subscription ignoring message received prior"
                           " to initial or recap. Type: %d %s %p",
                           userSymbol ? userSymbol : "", userSymbol ? ":" : "" , userSymbol ? userSymbol : "", subjectContext->mSymbol ? ":" : "", msgType, mamaMsg_toString(msg), subjectContext);
        }

        return MAMA_STATUS_OK;
    }

    switch (msgType)
    {
        case MAMA_MSG_TYPE_REFRESH:
        case MAMA_MSG_TYPE_DELETE:
        case MAMA_MSG_TYPE_EXPIRE:
        case MAMA_MSG_TYPE_NOT_PERMISSIONED:
        case MAMA_MSG_TYPE_NOT_FOUND:
        case MAMA_MSG_TYPE_UNKNOWN:
            break; 
        case MAMA_MSG_TYPE_QUOTE:
        case MAMA_MSG_TYPE_TRADE:
            checkSeqNum(strategy, msg, msgType, ctx);
            if (!subjectContext->mDqContext.mDoNotForward)
            {
                mamaSubscription_forwardMsg(subscription, msg);
            }
            else
            {
                mamaSubscription_getSymbol (subscription, &userSymbol);
                mama_log (MAMA_LOG_LEVEL_FINER, "Subscription for %s not forwarded"
                        " as message seqnum is before seqnum expecting", userSymbol);
            }
            break;
        default:
            checkSeqNum(strategy, msg, msgType, ctx);
            if (!subjectContext->mDqContext.mDoNotForward)
            {
                mamaSubscription_forwardMsg(subscription, msg);
            }
            else
            {
                mamaSubscription_getSymbol (subscription, &userSymbol);
                mama_log (MAMA_LOG_LEVEL_FINER, "Subscription for %s not forwarded"
                        " as message seqnum is before seqnum expecting", userSymbol);
            }
    }

    return MAMA_STATUS_OK;
} 

mama_status
applyPreInitialCache(mamaDqContext *ctx, mamaSubscription subscription)
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
    mamaSubscription_getSymbol (strategyImpl->mSubscription, &symbol);

    mama_log (MAMA_LOG_LEVEL_NORMAL, "Detected FT takeover. "
            "Original SenderId: %llu. New SenderId: %llu. "
            "Previous SeqNum: %u. New SeqNum: %u. [%s]",
            ctx->mSenderId, senderId, ctx->mSeqNum, seqNum, symbol);

    if (recoverOnRecap)
    {
        ctx->mSeqNum = seqNum;
        ctx->mDQState = DQ_STATE_WAITING_FOR_RECAP_AFTER_FT;
    }
    else
    {
        resetDqState (strategy, ctx);

        /*In all cases we reset the data quality context*/
        resetDqContext (ctx, seqNum, senderId);
    }

    return MAMA_STATUS_OK;
}

mama_status
checkSeqNum(dqStrategy strategy, mamaMsg msg, int msgType, mamaDqContext *ctx)
{
    mama_seqnum_t                seqNum                  = 0;
    mama_seqnum_t                ctxSeqNum               = ctx->mSeqNum;
    mama_u64_t                   senderId                = 0;
    mama_u64_t                   ctxSenderId             = ctx->mSenderId;
    dqState                      ctxDqState              = ctx->mDQState;
    mamaSubscription             subscription            = strategyImpl->mSubscription;
    mama_u16_t                   conflateCnt             = 1;
    wombat_subscriptionGapCB     onGap                   = NULL;
    mamaStatsCollector           transportStatsCollector = NULL;
    wombat_subscriptionQualityCB onQuality               = NULL;
    mamaMsgStatus                msgStatus               = MAMA_MSG_STATUS_UNKNOWN;
    mamaTransport                transport               = NULL;

    if(strategy == NULL || !strategy->mRecoverGaps)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    mamaSubscription_getTransport (strategyImpl->mSubscription, &transport);

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
        if (mama_getGenerateTransportStats())
        {
            transportStatsCollector = mamaTransport_getStatsCollector (transport);

            mamaStatsCollector_incrementStat (transportStatsCollector,
                                              MamaStatFtTakeovers.mFid);
        }
        
        if (mamaInternal_getGlobalStatsCollector() != NULL)
        {
            mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                              MamaStatFtTakeovers.mFid);
        }
        
        if (DQ_FT_WAIT_FOR_RECAP==mamaTransportImpl_getFtStrategyScheme(transport))
        {
            ctx->mDoNotForward = 1;
            handleFTTakeover (strategy, msg, msgType, ctx, seqNum, senderId, 1);
        }
        else
        {
            return handleFTTakeover (strategy, msg, msgType, ctx, seqNum, senderId, 0);
        }
    }

    if (gMamaLogLevel >= MAMA_LOG_LEVEL_FINEST)
    {
        const char*  symbol = NULL;
        mamaSubscription_getSymbol (subscription, &symbol);
        symbol = symbol == NULL ? "" : symbol;
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "checkSeqNum(): %s : seq# %ld",
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
            if (strategyImpl->mTryToFillGap)
            {
                strategyImpl->mTryToFillGap = 0;
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
            {
                resetDqState (strategy, ctx);

                ctx->mSeqNum = seqNum;
                return MAMA_STATUS_OK;
            }
        }

        /* For late joins or middlewares that support a publish cache, it is possible that you will get old updates
           in this case take no action */
        if (DQ_SCHEME_INGORE_DUPS == mamaTransportImpl_getDqStrategyScheme(transport))
        {
            if ((seqNum <= ctxSeqNum) && ((ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP) &&
                                          (ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)))
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
                void*       closure = NULL;
                const char* symbol  = NULL;
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
             if ((PRE_INITIAL_SCHEME_ON_GAP==mamaTransportImpl_getPreInitialScheme(transport))
                  &&(strategyImpl->mTryToFillGap))
             {
                strategyImpl->mTryToFillGap = 0;
                if (fillGap (ctx, seqNum, subscription))
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

            handleStaleData (strategyImpl, msg, ctx);
        }
        break;
    case MAMA_MSG_TYPE_INITIAL      :
    case MAMA_MSG_TYPE_BOOK_INITIAL :
        msgStatus = MAMA_MSG_STATUS_UNKNOWN;

        strategyImpl->mTryToFillGap = 1;

        msgStatus = mamaMsgStatus_statusForMsg (msg);
        /* Check the status of the message.  If it is stale,
           do not request a recap and do not set status OK. */
        if ((msgStatus == MAMA_MSG_STATUS_POSSIBLY_STALE) ||
            (msgStatus == MAMA_MSG_STATUS_STALE))
        {
            ctx->mDQState = DQ_STATE_STALE_NO_RECAP;
            resetDqContext (ctx, seqNum, senderId);
            return MAMA_STATUS_OK;
        }
    case MAMA_MSG_TYPE_RECAP        :
    case MAMA_MSG_TYPE_BOOK_RECAP   :
        /* For late joins or middlewares that support a publish cache, it is possible that you will get old updates
           in this case take no action */
        if (DQ_SCHEME_INGORE_DUPS == mamaTransportImpl_getDqStrategyScheme(transport))
        {
            if (MAMA_MSG_TYPE_RECAP == msgType)
            {
                /* Feed-handlers maintain sequence number for a Record FT Recap. */
                if ((seqNum <= ctxSeqNum) && ((ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP) &&
                                              (ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)))
                {
                    ctx->mDoNotForward = 1;
                    return MAMA_STATUS_OK;
                }
            }
            else if (MAMA_MSG_TYPE_BOOK_RECAP == msgType)
            {
                if (0 == seqNum && ctxSeqNum > 0)
                {
                    /* Special case of an FT Order Book Recap where a SeqNum of 0 is used. */
                    if (ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)
                    {
                        ctx->mDoNotForward = 1;
                        return MAMA_STATUS_OK;
                    }
                }
                /* Solicited Recap from Feed-Handler or
                 * solicited / unsolicited Recap from mid-tier. */
                else if ((seqNum <= ctxSeqNum) && ((ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP) &&
                                                   (ctx->mDQState != DQ_STATE_WAITING_FOR_RECAP_AFTER_FT)))
                {
                    ctx->mDoNotForward = 1;
                    return MAMA_STATUS_OK;
                }
            }
        }

        if (mamaTransportImpl_preRecapCacheEnabled (transport))
        {
            strategyImpl->mTryToFillGap = 1;
        }
        mamaSubscription_unsetAllPossiblyStale (subscription);
        resetDqState (strategy, ctx);
        resetDqContext (ctx, seqNum, senderId);
        ctx->mDoNotForward = 0;
        return MAMA_STATUS_OK;
    case MAMA_MSG_TYPE_DDICT_SNAPSHOT : /*No DQ checking for Datadictionary*/
        return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_OK;
}

mama_status
dqstrategyMamaPlugin_shutdownHook (mamaPluginInfo pluginInfo)
{
    free(pluginInfo);
    return MAMA_STATUS_OK;
}

static int
fillGap (mamaDqContext *ctx, mama_seqnum_t end, mamaSubscription subscription)
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
                if (ctx->mSetCacheMsgStale)
                {
                    msgUtils_setStatus (ctx->mCache[cur], MAMA_MSG_STATUS_STALE);
                }
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
    ctx->mSetCacheMsgStale = 0;

    return 0;
}

static int 
isInitialMessageOrRecap (int msgType)
{
    return msgType == MAMA_MSG_TYPE_INITIAL        ||
           msgType == MAMA_MSG_TYPE_SNAPSHOT       ||
           msgType == MAMA_MSG_TYPE_DDICT_SNAPSHOT ||
           msgType == MAMA_MSG_TYPE_RECAP          ||
           msgType == MAMA_MSG_TYPE_BOOK_INITIAL   ||
           msgType == MAMA_MSG_TYPE_BOOK_SNAPSHOT  ||
           msgType == MAMA_MSG_TYPE_BOOK_RECAP;
}

static int 
handleEvent (mamaPluginInfo pluginInfo, mamaTransport transport)
{
    mamaPluginInfo dqPluginInfo = NULL;

    mamaTransportImpl_getDqPluginInfo (transport, &dqPluginInfo);
    if (dqPluginInfo != pluginInfo)
    {
        return 0;
    }

    return 1;
}
