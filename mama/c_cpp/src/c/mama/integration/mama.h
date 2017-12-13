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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_MAMA_H__)
#define OPENMAMA_INTEGRATION_MAMA_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

/**
* Check whether Callbacks are run in 'debug' catch exceptions mode
*/
MAMAExpDLL
extern int
mamaInternal_getCatchCallbackExceptions (void);

/**
 * Exposes the property object from mama
 */
MAMAExpDLL
extern wproperty_t
mamaInternal_getProperties (void);

/* Used by the bridges to register themselves with MAMA. Should not
   be called from anywhere else */
MAMAExpDLL
extern void
mamaInternal_registerBridge (mamaBridge     bridge,
                             const char*    middleware);

/**
 * getVersion for use when mama is wrapped
 */
MAMAExpDLL
const char*
mama_wrapperGetVersion(mamaBridge     bridge);

MAMAExpDLL
extern mamaStatsGenerator
mamaInternal_getStatsGenerator (void);

MAMAExpDLL
extern mamaStatsCollector
mamaInternal_getGlobalStatsCollector (void);

MAMAExpDLL
extern int
mamaInternal_generateLbmStats (void);

MAMAExpDLL
mamaBridge
mamaInternal_findBridge (void);

MAMAExpDLL
mamaPayloadBridge
mamaInternal_findPayload (char id);

MAMAExpDLL
mamaPayloadBridge
mamaInternal_getDefaultPayload (void);

MAMAExpDLL
mama_bool_t
mamaInternal_getAllowMsgModify (void);

/**
 * @brief Helper function for initialization of internal wtables used by
 * OpenMAMA core.
 *
 * @param table Pointer to the wtable to be initialised.
 * @param name Name to be attached to the table
 * @param size Size of the table to be initialised.
 *
 * @return mama_status indicating the success or failure of the initialisation.
 */
MAMAExpDLL
mama_status
mamaInternal_initialiseTable (wtable_t*   table,
                              const char* name,
                              mama_size_t size);

/**
 * @brief Initialise core MAMA data structures.
 *
 * mamaInternal_init should be triggered once, and initialises the various data
 * structures used by MAMA internally. This includes:
 *     - Loading properties.
 *     - Initialising the bridges wtable structure.
 *
 * @return A mama_status indicating the success or failure of the initialisation.
 *
 */
MAMAExpDLL
mama_status
mamaInternal_init (void);

/**
 * @brief Return payload character for the passed payload.
 *
 * Method checks various mechanisms for the payload character associated with
 * the named payload. Firstly, it will check the global properties object for
 * a property matching the pattern mama.payload.<name>.payloadId=
 *
 * If no character is set in properties, the method will then check with the
 * payload bridge itself, via the <name>Payload_getPayloadType method.
 *
 * The response is then stored in the payloadChar parameter.
 *
 * @param[in] payloadName The name of the payload for which the char is required.
 * @param[in] payload The payloadBridge struct  for which the char is required
 * @param[out] payloadChar The character into which the response is passed.
 *
 * @return A mama_status indicating the success or failure of the method.
 */
MAMAExpDLL
mama_status
mamaInternal_getPayloadId (const char*       payloadName,
                           mamaPayloadBridge payload,
                           char*             payloadChar);

/**
 * @brief Return count of entitlementBridges loaded.
 * @return Integer number of loaded bridges.
 */
MAMAExpDLL
mama_i32_t
mamaInternal_getEntitlementBridgeCount (void);

/**
 * @brief Find loaded entitlement bridge by name.
 *
 * @param[in] name The name of the entitlement bridge to be found.
 * @param[out] entBridge The loaded entitlement bridge (if found).
 *
 * @return MAMA_STATUS_OK if successful.
 */
MAMAExpDLL
mama_status
mamaInternal_getEntitlementBridgeByName(mamaEntitlementBridge* entBridge, const char* name);

MAMAExpDLL
const char*
mamaInternal_getMetaProperty (const char* name);

MAMAExpDLL
mama_status
mamaInternal_setMetaProperty (const char* name, const char* value);

/* ************************************************************************* */
/* Callbacks. */
/* ************************************************************************* */

MAMAExpDLL
void MAMACALLTYPE mamaImpl_entitlementDisconnectCallback(
                            const  sessionDisconnectReason reason,
                            const  char * const            userId,
                            const  char * const            host,
                            const  char * const            appName);
MAMAExpDLL
void MAMACALLTYPE mamaImpl_entitlementUpdatedCallback (void);

MAMAExpDLL
void MAMACALLTYPE mamaImpl_entitlementCheckingSwitchCallback (
                            int isEntitlementsCheckingDisabled);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_MAMA_H__ */
