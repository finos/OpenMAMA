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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_STAT_H__)
#define OPENMAMA_INTEGRATION_STAT_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

typedef int (MAMACALLTYPE *pollStatCb) (void* closure);

MAMAExpDLL
extern mama_status
mamaStat_setIntervalValueFromTotal (mamaStat stat, mama_u32_t value);

MAMAExpDLL
extern mama_status
mamaStat_setPollCallback (mamaStat stat, pollStatCb callback, void* closure);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_STAT_H__ */
