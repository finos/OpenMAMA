#ifndef MAMA_BRIDGE_BASE_BASEDEFS_H__
#define MAMA_BRIDGE_BASE_BASEDEFS_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <wombat/wSemaphore.h>
#include <wombat/wtable.h>
#include <list.h>
#include <wombat/mempool.h>
#include <timers.h>

#include <mama/integration/bridge.h>

#if defined(__cplusplus)
extern "C" {
#endif


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Maximum topic length */
#define     MAX_SUBJECT_LENGTH              256
#define     MAX_URI_LENGTH                  1024

/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/
/* Message types */
typedef enum baseMsgType_
{
    BASE_MSG_PUB_SUB        =               0x00,
    BASE_MSG_INBOX_REQUEST,
    BASE_MSG_INBOX_RESPONSE,
    BASE_MSG_SUB_REQUEST,
    BASE_MSG_TERMINATE      =               0xff
} baseMsgType;

typedef struct baseBridgeIoImpl
{
    struct event_base*  mEventBase;
    wthread_t           mDispatchThread;
    uint8_t             mActive;
    uint8_t             mEventsRegistered;
    wsem_t              mResumeDispatching;
} baseBridgeIoImpl;

typedef struct baseBridgeClosure_
{
    // Note that mClosure is first - contains implementation bridge's own closure
    void*                 mImplClosure;
    timerHeap             mTimerHeap;
    baseBridgeIoImpl      mIoState;
} baseBridgeClosure;

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_BASE_BASEDEFS_H__ */
