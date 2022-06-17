/**
* This file contains methods which are intended for use only by OpenMAMA core,
* bridges and plugins. These methods typically only make sense to developers
* who have intimate knowledge of the inner workings of OpenMAMA. Its use in
* applications is heavily discouraged and entirely unsupported.
*
* Note that you can only use these methods if you have defined the
* OPENMAMA_INTEGRATION macro. If you think a case is to be made for accessing
* one of these methods, please raise to the mailing list and we'll investigate
* opening up a more stable formal interface for the standard public API.
*/

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_TYPES_H__)
#define OPENMAMA_INTEGRATION_TYPES_H__

#include <mama/mama.h>
#include <wombat/list.h>

#if defined (__cplusplus)
extern "C"
{
#endif

typedef struct  publisherBridge_*   publisherBridge;

typedef int (MAMACALLTYPE *pollStatCb) (void* closure);

typedef void (*collectorPollStatCb) (mamaStatsCollector statsCollector, void* closure);

typedef struct  subscriptonBridge_* subscriptionBridge;

/* For Wildcard subscriptions. The wc_transport type is likely only available
 * for LDMA/RDMA WDF transports. It is a subscription to every message on the
 * transport that bypasses the naming service. Other implementations may
 * ingore this.
 */
typedef enum
{
    wc_none      = 0,   /* Not wildcard subsc */
    wc_transport = 1,   /* "Transport" wildcard */
    wc_wildcard  = 2    /* normal wildcard */

} wildCardType;

typedef struct  timerBridge_*       timerBridge;

typedef struct  msgBridge_*         msgBridge;
typedef void*                       msgPayload;
typedef void*                       msgFieldPayload;
typedef void*                       msgPayloadIter;

typedef struct  inboxBridge_*       inboxBridge;
typedef struct  ioBridge_*          ioBridge;

/* Opaque types dereferenced in the implementation */
typedef void* endpoint_t;
typedef void* endpointPool_t;


typedef struct  wombatThrottle_*    wombatThrottle;
typedef struct  actionHandle*       wombatThrottleAction;
typedef struct  transportBridge_*   transportBridge;

typedef void (*throttleCb)(void *closure1, void *closure2 );

typedef void (*endpointDestroyCb)(endpoint_t endpoint);

typedef mama_status (*conflateProcessCb) (mamaConflationManager mgr, 
                                          mamaMsg               msg,
                                          const char*           topic);

typedef struct  queueBridge_*       queueBridge;

/* This typedef is used when locking and unlocking the queue. */
typedef void * mamaQueueLockHandle;

/**
 * Return the message wrapper used by this manager.
 */
typedef mamaMsg (*conflateGetMsgCb) (mamaConflationManager mgr); 

typedef struct imageReqImpl_* imageRequest;
typedef struct SubjectContext_ SubjectContext;

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
    mama_bool_t   mSetCacheMsgStale;
} mamaDqContext;

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_TYPES_H__ */
