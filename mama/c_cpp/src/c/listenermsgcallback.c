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
#include "plugin.h"
#include "mama/types.h"
#include "mama/statscollector.h"

#include "entitlementinternal.h"

extern const char*  gEntitlementBridges [MAX_ENTITLEMENT_BRIDGES];
extern int          gGenerateTransportStats;
extern int          gGenerateGlobalStats;
extern int          gGenerateQueueStats;

/* Function prototypes. */
void listenerMsgCallback_invokeErrorCallback(listenerMsgCallback callback,
        SubjectContext *ctx, mama_status mamaStatus, mamaSubscription
        subscription, const char *userSymbol);

void listenerMsgCallbackImpl_logUnknownStatus(SubjectContext *ctx, mamaMsgStatus status,
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
                                 SubjectContext*    ctx,
                                 const char*        userSymbol);

static mama_bool_t isEntitled   (msgCallback*       callback,
                                 mamaMsg            msg,
                                 SubjectContext*    ctx);

mama_status
listenerMsgCallback_create( listenerMsgCallback *result,
                            mamaSubscription subscription )
{
    msgCallback* callback = (msgCallback*)calloc( 1, sizeof( msgCallback ) );

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
    int                msgType              = mamaMsgType_typeForMsg (msg);
    mamaMsgStatus      status               = mamaMsgImpl_getStatusFromMsg (msg);
    msgCallback*       impl                 = (msgCallback*)callback;
    mamaSubscription   subscription         = impl->mSubscription;
    int                expectingInitial     = 0;
    mamaQueue          queue;
    mamaTransport      transport;
    mamaStatsCollector queueStatsCollector  = NULL;
    mamaStatsCollector tportStatsCollector  = NULL;
    const char*        userSymbol           = NULL;
	dqState            state                = DQ_STATE_NOT_ESTABLISHED;
    int                isDqEnabled          = 1;
    
    mamaSubscription_getTransport (subscription, &transport);
    mamaTransportImpl_getDqEnabled(transport, &isDqEnabled);

    if (!ctx)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "No context for subscription."
                  " Cannot proceed with message processing.");
        return;
    }

   /* Only check entitlements for initials and/or recaps. */
    if(!isEntitled (self, msg, ctx))
        return;

    if (gGenerateQueueStats)
    {
        mamaSubscription_getQueue (subscription, &queue);
        queueStatsCollector = mamaQueueImpl_getStatsCollector (queue);
    }

    if (gGenerateTransportStats)
    {
        tportStatsCollector = mamaTransport_getStatsCollector (transport);
    }

    if (queueStatsCollector)
        mamaStatsCollector_incrementStat (queueStatsCollector, MamaStatNumMessages.mFid);

    if (tportStatsCollector)
        mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatNumMessages.mFid);

    if (mamaInternal_getGlobalStatsCollector() != NULL)
        mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                          MamaStatNumMessages.mFid);

    /* Get the user symbol from the subscription. */
    mamaSubscription_getSymbol(subscription, &userSymbol);

    if (status != MAMA_MSG_STATUS_OK)
    {
        switch (status)
        {
        case MAMA_MSG_STATUS_LINE_DOWN:
            break;

        case MAMA_MSG_STATUS_NOT_PERMISSIONED:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_NOT_PERMISSIONED, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_BAD_SYMBOL:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_BAD_SYMBOL, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_EXPIRED:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_EXPIRED, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_NOT_FOUND:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_NOT_FOUND, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_TIMEOUT:
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_TIMEOUT, subscription, userSymbol);
            return;

        case MAMA_MSG_STATUS_NO_SUBSCRIBERS:
        {
            handleNoSubscribers (impl, msg, ctx, userSymbol);
            return;
        }

        /* The possibly stale messages are sent by the MAMACACHE with
         * a miscellaneous type and should be translated into a quality event.
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
                /* Without this, refresh acknowledgements will be ignored
                 * and every client will refresh every symbol.
                 */
                break;
            }

            /* Otherwise log the fact we have received an unknown message. */
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
            break;
        }
        case MAMA_MSG_STATUS_STALE:
        {
            break;
        }
        case MAMA_MSG_STATUS_PLATFORM_STATUS:
        {
            break;
        }
        case MAMA_MSG_STATUS_NOT_ENTITLED:
        {
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_NOT_ENTITLED, subscription, userSymbol);
            return;
        }
        case MAMA_MSG_STATUS_TOPIC_CHANGE:
        {
            break;
        }
        case MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED:
        {
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
                    MAMA_STATUS_BANDWIDTH_EXCEEDED, subscription, userSymbol);
            return;
        }
        case MAMA_MSG_STATUS_DUPLICATE:
        {
            break;
        }
        case MAMA_MSG_STATUS_UNKNOWN:
        {
            listenerMsgCallbackImpl_logUnknownStatus(ctx, status, subscription);
            mamaSubscription_setPossiblyStale(subscription);

            if (queueStatsCollector)
            {
                mamaStatsCollector_incrementStat (queueStatsCollector,
                        MamaStatUnknownMsgs.mFid);
            }
            if (tportStatsCollector)
            {
                mamaStatsCollector_incrementStat (tportStatsCollector,
                        MamaStatUnknownMsgs.mFid);
            }
            if (mamaInternal_getGlobalStatsCollector())
            {
                mamaStatsCollector_incrementStat
                (mamaInternal_getGlobalStatsCollector(),
                     MamaStatUnknownMsgs.mFid);
            }
            return; /* throw away msg */
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
                mamaStatsCollector_incrementStat (queueStatsCollector, MamaStatInitials.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatInitials.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector() != NULL)
             {
                mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                                  MamaStatInitials.mFid);
             }
             break;
        case MAMA_MSG_TYPE_RECAP :
        case MAMA_MSG_TYPE_BOOK_RECAP :
             if (queueStatsCollector)
             {
                mamaStatsCollector_incrementStat (queueStatsCollector, MamaStatRecaps.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatRecaps.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector())
             {
                mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                                  MamaStatRecaps.mFid);
             }
             break;
        case MAMA_MSG_TYPE_UNKNOWN :
             if (queueStatsCollector)
             {
                mamaStatsCollector_incrementStat (queueStatsCollector, MamaStatUnknownMsgs.mFid);
             }
             if (tportStatsCollector)
             {
                mamaStatsCollector_incrementStat (tportStatsCollector, MamaStatUnknownMsgs.mFid);
             }
             if (mamaInternal_getGlobalStatsCollector())
             {
                mamaStatsCollector_incrementStat (mamaInternal_getGlobalStatsCollector(),
                                                  MamaStatUnknownMsgs.mFid);
             }
             break;
             default : break;
        }
        
    mamaPlugin_fireSubscriptionPreMsgHook(subscription, msgType, msg);

    switch (msgType)
    {
        case MAMA_MSG_TYPE_INITIAL:
        case MAMA_MSG_TYPE_RECAP:
        case MAMA_MSG_TYPE_SNAPSHOT:
        case  MAMA_MSG_TYPE_DDICT_SNAPSHOT:
        case MAMA_MSG_TYPE_BOOK_INITIAL:
        case MAMA_MSG_TYPE_BOOK_SNAPSHOT:
        case MAMA_MSG_TYPE_BOOK_RECAP:
            if(!isDqEnabled)
            {
               mamaSubscription_stopWaitForResponse (subscription, ctx);
               mamaSubscription_forwardMsg(subscription, msg);
            }
            break;
        case MAMA_MSG_TYPE_REFRESH:
            mamaSubscription_respondToRefreshMessage(subscription);
            break;
        case MAMA_MSG_TYPE_DELETE:
            mamaSubscription_stopWaitForResponse(subscription,ctx);
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
            MAMA_STATUS_DELETE, subscription, userSymbol);
            return;
            break;
        case MAMA_MSG_TYPE_EXPIRE:
            mamaSubscription_stopWaitForResponse(subscription,ctx);
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
            MAMA_STATUS_EXPIRED, subscription, userSymbol);
            return;
            break;
        case MAMA_MSG_TYPE_NOT_PERMISSIONED:
            mamaSubscription_stopWaitForResponse(subscription,ctx);
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
            MAMA_STATUS_NOT_PERMISSIONED, subscription, userSymbol);
            return;
            break;
        case MAMA_MSG_TYPE_NOT_FOUND:
            mamaSubscription_stopWaitForResponse(subscription,ctx);
            listenerMsgCallback_invokeErrorCallback(callback, ctx,
            MAMA_STATUS_NOT_FOUND, subscription, userSymbol);
            return;
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
            if(!isDqEnabled)
            {
                mamaSubscription_forwardMsg(subscription, msg);
            }
            break;

    }
}

static void handleNoSubscribers (msgCallback *callback,
                                 mamaMsg msg,
                                 SubjectContext* ctx,
                                 const char *userSymbol)
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

    listenerMsgCallback_invokeErrorCallback(callback, ctx,
            MAMA_STATUS_NO_SUBSCRIBERS, self->mSubscription, userSymbol);
}

/**
 * Entitlement checking functions.
 */


static mama_bool_t
isEntitled( msgCallback *callback, mamaMsg msg, SubjectContext* ctx )
{
    int32_t         value  = 0;
    mamaBridgeImpl* bridge = NULL;
    mama_status     status = MAMA_STATUS_NOT_ENTITLED;

    if( ctx->mEntitlementAlreadyVerified )
    {
        return 1;
    }

    bridge = mamaSubscription_getBridgeImpl(self->mSubscription);

    if (bridge && (mamaBridgeImpl_areEntitlementsDeferred(bridge)))
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                       "Deferred checking injected entitlement to %s bridge [%p]",
                        bridge->bridgeGetName(), bridge);
        ctx->mEntitlementAlreadyVerified = 1;
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
        status = ctx->mEntitlementBridge->registerSubjectContext(ctx);
        if (MAMA_STATUS_OK != status)
        {
            const char* userSymbol  = NULL;
            void*       closure     = NULL;
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
            return 0;
        }
    }
    else
    {
         /* This is only temporary until the entitle code is sent with
          * every message. */
        return 1;
    }

    ctx->mEntitlementAlreadyVerified = 1;
    return 1;
}

void listenerMsgCallbackImpl_logUnknownStatus(SubjectContext *ctx, mamaMsgStatus status,
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
