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

#include <wombat/wtable.h>
#include <wombat/property.h>
#include <mama/mama.h>
#include <mama/version.h>

#if defined (__cplusplus)
extern "C"
{
#endif

#define MAMA_PAYLOAD_MAX        UINT8_MAX
#define MAMA_MAX_MIDDLEWARES    UINT8_MAX
#define MAMA_MAX_ENTITLEMENTS   UINT8_MAX
#define MAX_ENTITLEMENT_BRIDGES UINT8_MAX

/* These are the bare parameters which may be reused in other properties */
#define MAMA_PROP_BARE_ENT_DEFERRED        "entitlements.deferred"
#define MAMA_PROP_BARE_COMPILE_TIME_VER    "compile_version"
#define MAMA_PROP_EXTENDS_BASE_BRIDGE      "extends_base_bridge"

/* These properties will be set by MAMA */
#define MAMA_PROP_MAMA_RUNTIME_VER         "mama.runtime_version"

/* %s = Bridge Name - these properties may be populated by bridge */
#define MAMA_PROP_BRIDGE_ENT_DEFERRED      "mama.%s." MAMA_PROP_BARE_ENT_DEFERRED
#define MAMA_PROP_BRIDGE_COMPILE_TIME_VER  "mama.%s." MAMA_PROP_BARE_COMPILE_TIME_VER

/* Maximum internal property length */
#define MAX_INTERNAL_PROP_LEN   1024

#define MAMA_SET_BRIDGE_COMPILE_TIME_VERSION(bridgeName)                       \
do                                                                             \
{                                                                              \
    char valString[MAX_INTERNAL_PROP_LEN];                                     \
    char propString[MAX_INTERNAL_PROP_LEN];                                    \
                                                                               \
    /* Advise MAMA which version of MAMA the bridge was compiled against */    \
    snprintf (propString,                                                      \
              sizeof(propString),                                              \
              MAMA_PROP_BRIDGE_COMPILE_TIME_VER,                               \
              bridgeName);                                                     \
    /* Advise MAMA which version of MAMA the bridge was compiled against */    \
    snprintf (valString,                                                       \
              sizeof(valString),                                               \
              "%d.%d.%s",                                                      \
              MAMA_VERSION_MAJOR,                                              \
              MAMA_VERSION_MINOR,                                              \
              MAMA_VERSION_RELEASE);                                           \
    mamaInternal_setMetaProperty (                                             \
              propString,                                                      \
              valString);                                                      \
}                                                                              \
while(0)

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

/**
 * Sets the internal default event queue for the specified middleware.
 *
 * @param bridgeImpl The middleware for which the default event queue is being
 * provided.
 * @param defaultQueue Address of the defaultQueue to be written.
 *
 * @return MAMA_STATUS_OK if the function returns successfully.
 */
MAMAExpDLL
mama_status
mamaImpl_setDefaultEventQueue (mamaBridge bridgeImpl,
                               mamaQueue defaultQueue);

/**
 * This is a local function for parsing long configuration parameters from the
 * MAMA properties object, and supports minimum and maximum limits as well
 * as default values to reduce the amount of code duplicated throughout.
 *
 * @param defaultVal This is the default value to use if the parameter does not
 *                   exist in the configuration file
 * @param minimum    If the parameter obtained from the configuration file is
 *                   less than this value, this value will be used.
 * @param maximum    If the parameter obtained from the configuration file is
 *                   greater than this value, this value will be used.
 * @param format     This is the format string which is used to build the
 *                   name of the configuration parameter which is to be parsed.
 * @param ...        This is the variable list of arguments to be used along
 *                   with the format string.
 *
 * @return long int containing the parameter value, default, minimum or maximum.
 */
MAMAExpDLL
extern long int
mamaImpl_getParameterAsLong (
    long defaultVal,
    long minimum,
    long maximum,
    const char* format, ...);

/**
 * This is a local function for parsing string configuration parameters from the
 * MAMA properties object, and supports default values. This function should
 * be used where the configuration parameter itself can be variable.
 *
 * @param defaultVal This is the default value to use if the parameter does not
 *                   exist in the configuration file
 * @param format     This is the format string which is used to build the
 *                   name of the configuration parameter which is to be parsed.
 * @param ...        This is the variable list of arguments to be used along
 *                   with the format string.
 *
 * @return const char* containing the parameter value or the default.
 */
MAMAExpDLL
extern const char*
mamaImpl_getParameter (
    const char* defaultVal,
    const char* format, ...);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_MAMA_H__ */
