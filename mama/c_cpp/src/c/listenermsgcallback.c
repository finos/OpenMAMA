/* $Id: listenermsgcallback.c,v 1.62.4.1.14.6 2011/09/01 09:41:02 emmapollock Exp $
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
#include "mama/reservedfields.h"
#include "mamainternal.h"
#include "msgutils.h"
#include "subscriptionimpl.h"
#include "transportimpl.h"
#include "bridge.h"
#include "listenermsgcallback.h"
#include "mama/statfields.h"
#include "msgimpl.h"
#include "queueimpl.h"
#include "mama/statscollector.h"

#ifdef WITH_ENTITLEMENTS
#include <OeaClient.h>
extern oeaClient *   gEntitlementClient;
#endif /* WITH_ENTITLEMENTS */


extern int gGenerateTransportStats;
extern int gGenerateGlobalStats;
extern int gGenerateQueueStats;

/* Function prototypes. */
void listenerMsgCallback_invokeErrorCallback(listenerMsgCallback callback,
        SubjectContext *ctx, mama_status mamaStatus, mamaSubscription
        subscription, const char *userSymbol);

void listenerMsgCallbackImpl_logUnknownStatus(SubjectContext *ctx, int status,
        mamaSubscription subscription);

/**
 * Main callback for MamaListener. This is the base strategy for
 * handling messages.
 */

#define self ((msgCallback*)(callback))
#define userSymbolFormatted userSymbol ? userSymbol : "", \
                            userSymbol ? ": " : ""
#define ctxSymbolFormatted ctx->mSymbol ? ctx->mSymbol : "", \
                           ctx->mSymbol ? ":" : ""

typedef struct msgCallback_
{
    mamaSubscription mSubscription;
} msgCallback;

static int isInitialMessageOrRecap( msgCallback *callback, int msgType );
static void handleNoSubscribers (msgCallback*       callback,
                                 mamaMsg            msg,
                                 SubjectContext*    ctx);

static int checkEntitlement     (msgCallback*       callback,
                                 mamaMsg            msg,
                                 SubjectContext*    ctx);

mama_status
listenerMsgCallback_create( listenerMsgCallback *result,
                            mamaSubscription subscription )
{
    msgCallback* callback = (msgCallback*)calloc( 1, sizeof( msgCallback ) );

#ifdef WITH_ENTITLEMENTS  /* No listener creation without a client. */
    if( gEntitlementClient == 0 )
    {
        return MAMA_ENTITLE_NO_SERVERS_SPECIFIED;
    }
#endif  /* WITH_ENTITLEMENTS */

    if( callback == NULL )
    {
        return MAMA_STATUS_NOMEM;
    }

    *result = (listenerMsgCallback)callback;

    self->mSubscription  = subscription;

    return MAMA_STATUS_OK;
}

mama_status
listenerMsgCallback_destroy( listenerMsgCallback callback )
{
    free( callback );
    return MAMA_STATUS_OK;
}


static void processPointToPointMessage (msgCallback*    callback,
                                        mamaMsg         msg,
                                        int             msgType,
                                        SubjectContext  *ctx)
{
    const char* userSymbol  = NULL;
    short       total       = 0;
    short       msgNo       = 0;
    mamaTransport   tport       = NULL;

    mamaSubscription_getSymbol (self->mSubscription, &userSymbol);

    mamaSubscription_getTransport (self->mSubscription, &tport);

    if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINER) ||
        (mamaSubscription_checkDebugLevel (self->mSubscription,
                                           MAMA_LOG_LEVEL_FINER)))
    {
        const char* subscSymbol = NULL;
        mamaSubscription_getSubscSymbol (self->mSubscription, &subscSymbol);
        mama_log (MAMA_LOG_LEVEL_FINER,
                       "processPointToPointMessage(): Got unicast message(?) "
                       "for %s (%s) (type=%d; subsc=%p) %p",
                       subscSymbol == NULL ? "" : subscSymbol,
                       userSymbol  == NULL ? "" : userSymbol,
                       mamaMsgType_typeForMsg(msg),
                       self->mSubscription,
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
        mamaSubscription_incrementInitialCount(self->mSubscription);
        ++ctx->mImageCount;
        /*For group subs the timeout for the imagerequest will
         call stopWaitForResponse*/
        ctx->mInitialArrived = 1;


        if( gMamaLogLevel >= MAMA_LOG_LEVEL_FINE )
        {
            const char *msgString = mamaMsg_toString( msg );
            mama_log (MAMA_LOG_LEVEL_FINE, "Received Initial: (%s) %s", userSymbol, msgString);
            mamaMsg_freeString( msg, msgString );
        }
        if (!mamaSubscription_getAcceptMultipleInitials (self->mSubscription))
        {
            mamaSubscription_stopWaitForResponse (self->mSubscription, ctx);
        }
    }

    mamaSubscription_checkSeqNum (self->mSubscription, msg, msgType, ctx);

    /* Mark the subscription as inactive if we are not expecting
     * any more updates. */
    if (!mamaSubscription_isExpectingUpdates (self->mSubscription) &&
            !mamaSubscription_getAcceptMultipleInitials (self->mSubscription))
    {
        mamaSubscription_deactivate (self->mSubscription);
    }

    mamaSubscription_forwardMsg (self->mSubscription, msg);

    /*
       NB!!!  - can't destroy a subscription after an initial!!!!!
       After we forward this message we need to see if we should fill from
     the pre initial cache*/
    if (PRE_INITIAL_SCHEME_ON_INITIAL==
            mamaTransportImpl_getPreInitialScheme (tport))
    {
        if (msgType==MAMA_MSG_TYPE_INITIAL || msgType == MAMA_MSG_TYPE_BOOK_INITIAL)
        {
            dqContext_applyPreInitialCache (&ctx->mDqContext, self->mSubscription);

            /*Clear the messages - no longer needed*/
            dqContext_clearCache (&ctx->mDqContext, 0);
        }
    }


    /* Note: do NOT access the "self" members because the subscription
     * may have been destroyed in the callback! */
}

/* Description : This function will invoke the subscription's onError callback
 * passing in a particular error code.
 */
void listenerMsgCallback_invokeErrorCallback(listenerMsgCallback callback,
        SubjectContext *ctx, mama_status mamaStatus, mamaSubscription
        subscription, const char *userSymbol)
{
    /* Local variables. */
    void *closure = NULL;

    /* Get the callback object from the subscription. */
    mamaMsgCallbacks *cbs = mamaSubscription_getUserCallbacks (subscription);

    /* Wait for a response. */
    mamaSubscription_stopWaitForResponse(subscription, ctx);

    /* Get the closure from the subscription. */
    mamaSubscription_getClosure (subscription, &closure);

    mama_setLastError (MAMA_ERROR_DEFAULT);
    cbs->onError (subscription,
                  mamaStatus,
                  NULL,
                  userSymbol,
                  closure);
}

static int isInitialMessageOrRecap (msgCallback *callback, int msgType)
{
    return msgType == MAMA_MSG_TYPE_INITIAL        ||
           msgType == MAMA_MSG_TYPE_SNAPSHOT       ||
           msgType == MAMA_MSG_TYPE_DDICT_SNAPSHOT ||
           msgType == MAMA_MSG_TYPE_RECAP          ||
           msgType == MAMA_MSG_TYPE_BOOK_INITIAL   ||
           msgType == MAMA_MSG_TYPE_BOOK_SNAPSHOT  ||
           msgType == MAMA_MSG_TYPE_BOOK_RECAP;
}

void
listenerMsgCallback_processMsg( listenerMsgCallback callback, mamaMsg msg,
                                SubjectContext *ctx)
{
    int               msgType           = mamaMsgType_typeForMsg (msg);
    int               status            = mamaMsgImpl_getStatusFromMsg (msg);
    msgCallback*      impl              = (msgCallback*)callback;
    mamaSubscription  subscription      = impl->mSubscription;
    int               expectingInitial  = 0;
    mamaQueue           queue;
    mamaTransport       transport;
    mamaStatsCollector* queueStatsCollector = NULL;
    mamaStatsCollector* tportStatsCollector = NULL;
    const char* userSymbol = NULL;

    if (!ctx)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "No context for subscription."
                  " Cannot proceed with message processing.");
        return;
    }

   /* Only check entitlements for initials and/or recaps. */
    if(!checkEntitlement (self, msg, ctx))
        return;

    if (gGenerateQueueStats)
    {
        mamaSubscription_getQueue (subscription, &queue);
        queueStatsCollector = mamaQueueImpl_getStatsCollector (queue);
    }

    if (gGenerateTransportStats)
    {
        mamaSubscription_getTransport (subscription, &transport);
        tportStatsCollector = mamaTransport_getStatsCollector (transport);
    }

    if (queueStatsCollector)
        mamaStatsCollector_incrementStat (*queueStatsCollector, MamaStatNumMessages.mFid);

    if (tportStatsCollector)
        mamaStatsCollector_incrementStat (*tportStatsCollector, MamaStatNumMessages.mFid);

    if (mamaInternal_getGlobalStatsCollector() != NULL)
        mamaStatsCollector_incrementStat (*(mamaInternal_getGlobalStatsCollector()),
                                          MamaStatNumMessages.mFid);

    /* Get the user symbol from the subscription. */
    mamaSubscription_getSymbol(subscription, &userSymbol);

    if (status != MAMA_MSG_STATUS_OK)
    {
        switch (status)
        {
        case MAMA_MSG_STATUS_NOT_PERMISSIONED:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_NOT_PERMISSIONED, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_BAD_SYMBOL:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_BAD_SYMBOL, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_NOT_FOUND:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_NOT_FOUND, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_NO_SUBSCRIBERS:
        {
            handleNoSubscribers (impl, msg, ctx);
            return;
        }

           /* The possibly stale messages are sent by the MAMACACHE with a miscellanious type and should be translated
            * into a quality event.
            */
        case MAMA_MSG_STATUS_POSSIBLY_STALE:
        {
            /* Verify that the type is misc. */
            if(msgType == MAMA_MSG_TYPE_MISC)
            {
                /* Change the state to maybe stale and invoke the onquality callback. */
                mamaSubscription_setPossiblyStale(subscription);
                break;
            }

            /* Otherwise log the fact we have received an unknown message. */
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
            break;
        }

        case MAMA_MSG_STATUS_MISC:
        {
            if (msgType == MAMA_MSG_TYPE_REFRESH)
            {
                /* PAPA older than 2.99.6 publishes refresh messages as
                 * MISC status. */
                break;
            }

            /* Otherwise log the fact we have received an unknown message. */
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
            break;
        }
        case MAMA_MSG_STATUS_UNKNOWN:
        {
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
            mamaSubscription_setPossiblyStale(subscription);

            if (queueStatsCollector)
            {
                mamaStatsCollector_incrementStat (*queueStatsCollector,
                        MamaStatUnknownMsgs.mFid);
            }
            if (tportStatsCollector)
            {
                mamaStatsCollector_incrementStat (*tportStatsCollector,
                        MamaStatUnknownMsgs.mFid);
            }
            if (mamaInternal_getGlobalStatsCollector())
            {
                mamaStatsCollector_incrementStat
                    (*(mamaInternal_getGlobalStatsCollector()),
                     MamaStatUnknownMsgs.mFid);
            }
            return; //throw away msg
            break;
        }
        default:
        {
            /* Log the fact we have received an unknown message. */
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
        }
        }
    }

    switch (msgType)
    {
        case MAMA_MSG_TYPE_INITIAL :
             if (queueStatsCollector)
             {
                mamaStatsCollector_incrementStat (*queueStatsCollector, MamaStatInitials.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (*tportStatsCollector, MamaStatInitials.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector() != NULL)
             {
                mamaStatsCollector_incrementStat (*(mamaInternal_getGlobalStatsCollector()),
                                                  MamaStatInitials.mFid);
             }
             break;
        case MAMA_MSG_TYPE_RECAP :
        case MAMA_MSG_TYPE_BOOK_RECAP :
             if (queueStatsCollector)
             {
                mamaStatsCollector_incrementStat (*queueStatsCollector, MamaStatRecaps.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (*tportStatsCollector, MamaStatRecaps.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector())
             {
                mamaStatsCollector_incrementStat (*(mamaInternal_getGlobalStatsCollector()),
                                                  MamaStatRecaps.mFid);
             }
             break;
        case MAMA_MSG_TYPE_UNKNOWN :
             if (queueStatsCollector)
             {
                mamaStatsCollector_incrementStat (*queueStatsCollector, MamaStatUnknownMsgs.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (*tportStatsCollector, MamaStatUnknownMsgs.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector())
             {
                mamaStatsCollector_incrementStat (*(mamaInternal_getGlobalStatsCollector()),
                                                  MamaStatUnknownMsgs.mFid);
             }
             break;
             default : break;
        }

    if (isInitialMessageOrRecap(self, msgType))
    {
        processPointToPointMessage (impl, msg, msgType, ctx);
        return;
    }

    /* If we're waiting on an initial for this subscription
        then we ignore all messages. Note that after the initial
        request timeout period individual symbols in a group
        subscription may have not received an initial, but this
        is ok. Also, we continue to process messages when waiting
        for recaps for in individual symbol
     */

    mamaSubscription_getExpectingInitial (subscription, &expectingInitial);

    /*While we are waiting for initial values we also check whether we have an
     * initial for an individual context.
      If we are no longer waiting for initials we assume that it is ok to pass
     on the update - (probably a new symbol for a group)*/
    if (expectingInitial && !ctx->mInitialArrived
        && msgType != MAMA_MSG_TYPE_DELETE
        && msgType != MAMA_MSG_TYPE_EXPIRE
        && msgType != MAMA_MSG_TYPE_UNKNOWN)
    {
        /*Add this message to the cache. If the message after the initial
         * results in a gap we will attempt to fill the gap from this cache
         * before asking for a recap.*/
        dqContext_cacheMsg (&(ctx->mDqContext), msg);

        /* Response for initial value not yet received. */
        if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINE) ||
            (mamaSubscription_checkDebugLevel (self->mSubscription,
                                               MAMA_LOG_LEVEL_FINE)))
        {
            const char* userSymbol = NULL;
            mamaSubscription_getSymbol (subscription, &userSymbol);
            mama_log (MAMA_LOG_LEVEL_FINE,
                           "%s%s %s%s"
                           " Subscription ignoring message received prior"
                           " to initial update. Type: %d %s %p",
                           userSymbolFormatted, ctxSymbolFormatted,
                           msgType, mamaMsg_toString(msg), ctx);
        }
        return;
    }

    switch (msgType)
    {
    case MAMA_MSG_TYPE_QUOTE:
    case MAMA_MSG_TYPE_TRADE:
        mamaSubscription_checkSeqNum(subscription, msg, msgType, ctx);
        if (!ctx->mDqContext.mDoNotForward)
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
    case MAMA_MSG_TYPE_REFRESH:
        mamaSubscription_respondToRefreshMessage(subscription);
        break;
    case MAMA_MSG_TYPE_DELETE:
        mamaSubscription_stopWaitForResponse(subscription,ctx);
        mamaSubscription_forwardMsg(subscription, msg);
        break;
    case MAMA_MSG_TYPE_UNKNOWN:
        mama_log (MAMA_LOG_LEVEL_FINE,
                           "%s%s %s%s"
                           " Subscription ignoring message without type"
                           ". Type: %d %s (%p)",
                           userSymbolFormatted, ctxSymbolFormatted,
                           msgType, mamaMsg_toString(msg), ctx);
        break;
    default:
        mamaSubscription_checkSeqNum(subscription, msg, msgType, ctx);
        if (!ctx->mDqContext.mDoNotForward)
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
}

static void handleNoSubscribers (msgCallback *callback, mamaMsg msg, SubjectContext* ctx)
{
    if( !mamaSubscription_hasWildcards( self->mSubscription ) )
    {
        /* FH may be restarted while waiting for initial value. */
        mamaSubscription_stopWaitForResponse (self->mSubscription, ctx);
    }

    mamaMsg_updateU8( msg,
                       MamaFieldMsgType.mName,
                       MamaFieldMsgType.mFid,
                       MAMA_MSG_TYPE_MISC);

    msgUtils_setStatus(msg,MAMA_MSG_STATUS_STALE);

    mamaSubscription_forwardMsg( self->mSubscription, msg );
}

/**
 * Entitlement checking functions.
 */


static int
checkEntitlement( msgCallback *callback, mamaMsg msg, SubjectContext* ctx )
{

#ifdef WITH_ENTITLEMENTS 
    int result = 0;
    int32_t value;
    if( ctx->mEntitlementAlreadyVerified )
    {
        return 1;
    }

    if( MAMA_STATUS_OK == mamaMsg_getEntitleCode( msg,
                                                  &value ) )
    {
        if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINER) ||
            (mamaSubscription_checkDebugLevel (self->mSubscription,
                                               MAMA_LOG_LEVEL_FINER)))
        {
            mama_log (MAMA_LOG_LEVEL_FINER,
                           "Checking injected entitlement: %d\n", value);
        }

        if( value == 0 )
        {
            return 1;
        }
        ctx->mEntitleCode = value;
        if (ctx->mOeaSubscription != NULL)
        {
            oeaSubscription_addEntitlementCode (ctx->mOeaSubscription, ctx->mEntitleCode);
            oeaSubscription_open (ctx->mOeaSubscription);
            result = oeaSubscription_isOpen (ctx->mOeaSubscription);

            if (!result)
            {
                const char* userSymbol  = NULL;
                void*       closure = NULL;
                mamaMsgCallbacks *cbs =
                    mamaSubscription_getUserCallbacks (self->mSubscription);

                mamaSubscription_getSymbol  (self->mSubscription, &userSymbol),
                mamaSubscription_getClosure (self->mSubscription, &closure);

                mama_setLastError (MAMA_ERROR_NOT_ENTITLED);

                mamaSubscription_deactivate (self->mSubscription);

                cbs->onError (self->mSubscription,
                              MAMA_STATUS_NOT_ENTITLED,
                              NULL,
                              userSymbol,
                              closure);
            }
        }
    }
    else
    {
         /* This is only temporary until the entitle code is sent with
          * every message. */
        return 1;
    }

    if( result )
    {
        ctx->mEntitlementAlreadyVerified = 1;
    }

    return result;
#else 
    return 1;
#endif /* WITH_ENTITLEMENTS */
}

void listenerMsgCallbackImpl_logUnknownStatus(SubjectContext *ctx, int status,
        mamaSubscription subscription)
{
    /* Write the log at fine level. */
    if ((gMamaLogLevel >= MAMA_LOG_LEVEL_FINE) ||
            (mamaSubscription_checkDebugLevel (subscription,
                                               MAMA_LOG_LEVEL_FINE)))
    {
        const char* userSymbol = NULL;
        mamaSubscription_getSymbol (subscription, &userSymbol);
        mama_log (MAMA_LOG_LEVEL_FINE,
                        "%s%s%s%s Unexpected status: %s",
                        userSymbolFormatted, ctxSymbolFormatted,
                        mamaMsgStatus_stringForStatus( status ) );
    }
}
