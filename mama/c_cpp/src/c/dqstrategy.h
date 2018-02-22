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

#ifndef DqStrategyH__
#define DqStrategyH__
#include "imagerequest.h"
#ifndef OPENMAMA_INTEGRATION
#define OPENMAMA_INTEGRATION
#endif

#include <mama/integration/types.h>
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct dqStrategy_
{
    /* Data Quality Members */
    mamaSubscription    mSubscription;
    short               mTryToFillGap;
    int                 mRecoverGaps;
} dqStrategyImpl;

typedef struct dqStrategy_* dqStrategy;

MAMAExpDLL
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

MAMAExpDLL
mama_status
dqContext_cacheMsg (mamaDqContext *ctx, mamaMsg msg);

mama_status
dqStrategyImpl_detachMsg (mamaDqContext* ctx, mamaMsg msg);

mama_status
dqStrategy_destroy (
    dqStrategy          strategy);

MAMAExpDLL
mama_status
dqStrategy_getDqState (
    mamaDqContext*       ctx,
    dqState*            state);

mama_status
dqStrategy_setPossiblyStale (
    mamaDqContext*      ctx);

MAMAExpDLL
mama_status
dqContext_clearCache(mamaDqContext *ctx, int freeArray);

MAMAExpDLL
mama_status
dqStrategy_sendRecapRequest (
        dqStrategy strategy, 
        mamaMsg         srcMsg, 
        mamaDqContext*  ctx);

MAMAExpDLL
mama_status
dqStrategy_setRecoverGaps(dqStrategy strategy, int newValue);

MAMAExpDLL
mama_status
dqStrategy_getRecoverGaps(dqStrategy strategy, int *result);

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
