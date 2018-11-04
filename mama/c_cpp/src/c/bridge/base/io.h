#ifndef MAMA_BRIDGE_BASE_IO_H__
#define MAMA_BRIDGE_BASE_IO_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/


#if defined(__cplusplus)
extern "C" {
#endif

#include <mama/mama.h>

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

mama_status
baseBridgeMamaIoImpl_start (void* closure);

mama_status
baseBridgeMamaIoImpl_stop  (void* closure);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_BASE_IO_H__ */
