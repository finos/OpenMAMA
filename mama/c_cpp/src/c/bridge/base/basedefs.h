#ifndef MAMA_BRIDGE_BASE_BASEDEFS_H__
#define MAMA_BRIDGE_BASE_BASEDEFS_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <wombat/wSemaphore.h>
#include <wombat/wtable.h>
#include <wombat/list.h>
#include <wombat/mempool.h>
#include <wombat/timers.h>

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

typedef struct baseBridgeClosure_
{
    // Note that mClosure is first - contains implementation bridge's own closure
    void*                 mImplClosure;
    timerHeap             mTimerHeap;
    void*                 mIoState;
} baseBridgeClosure;

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_BASE_BASEDEFS_H__ */
