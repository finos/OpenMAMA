#ifndef RESOURCEPOOLIMPL_H
#define RESOURCEPOOLIMPL_H

#include "mama/subscriptiontype.h"
#include "mama/types.h"
#include "mama/log.h"


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct mamaResourcePoolOptionsImpl_
{
    double mTimeout;
    int mRetries;
    mamaSubscriptionType mSubscriptionType;
    mama_bool_t mRequiresInitial;
    MamaLogLevel mDebugLevel;
} mamaResourcePoolOptionsImpl;

MAMAExpDLL
extern void
mamaResourcePoolImpl_applyOption (
    mamaResourcePoolOptionsImpl* options,
    const char* key,
    const char* value
);

MAMAExpDLL
extern void
mamaResourcePoolImpl_applyOptionsFromConfiguration (
    const char* poolName,
    mamaResourcePoolOptionsImpl* options
);

MAMAExpDLL
extern void
mamaResourcePoolImpl_applyOptionsFromQueryString (
    const char* query,
    mamaResourcePoolOptionsImpl* options
);

#if defined(__cplusplus)
}
#endif

#endif /*RESOURCEPOOLIMPL_H*/
