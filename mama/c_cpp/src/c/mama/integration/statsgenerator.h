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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_STATSGENERATOR_H__)
#define OPENMAMA_INTEGRATION_STATSGENERATOR_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

MAMAExpDLL
extern mama_status
mamaStatsGenerator_create (mamaStatsGenerator* statsGenerator, mama_f64_t reportInterval);

/**
 * This function should be called to stop the stats report timer before the internal event
 * queue has been destroyed.
 *
 * @param[in] statsGenerator The stats generator.
 * @returns mama_status can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaStatsGenerator_stopReportTimer(mamaStatsGenerator statsGenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_destroy (mamaStatsGenerator statsGenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_removeStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector statsCollector);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setLogStats (mamaStatsGenerator statsGenerator, int logStats);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setStatsLogger (mamaStatsGenerator statsGenerator, mamaStatsLogger* statsLogger);

MAMAExpDLL
extern void
mamaStatsGenerator_generateStats (mamaStatsGenerator statsgenerator);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_setQueue (mamaStatsGenerator statsgenerator, mamaQueue queue);

MAMAExpDLL
extern mama_status
mamaStatsGenerator_addStatsCollector (mamaStatsGenerator statsGenerator, mamaStatsCollector statsCollector);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_STATSGENERATOR_H__ */
