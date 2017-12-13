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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_STATSCOLLECTOR_H__)
#define OPENMAMA_INTEGRATION_STATSCOLLECTOR_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

typedef void (*collectorPollStatCb) (mamaStatsCollector statsCollector, void* closure);

MAMAExpDLL
extern mama_status
mamaStatsCollector_setPollCallback (mamaStatsCollector statsCollector, collectorPollStatCb cb, void* closure);

/**
 * Populate a reusable message to be published via the stats logger,
 * and a string to be logged via MAMA logging
 *
 * @param statscollector  The stats collector object to gather stats from
 * @param statsStr        A reusable string for MAMA logging
 * @param msg             A reusable message to be published via the Stats Logger
 * @param wasLogged       Whether or not any stats were logged
 */
MAMAExpDLL
extern void
mamaStatsCollector_populateMsg (mamaStatsCollector statsCollector, mamaMsg msg, int* wasLogged);

MAMAExpDLL
extern mama_status
mamaStatsCollector_setStatIntervalValueFromTotal (mamaStatsCollector statsCollector, mama_fid_t identifier, mama_u32_t value);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_STATSCOLLECTOR_H__ */
