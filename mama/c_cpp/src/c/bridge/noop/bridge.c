/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <mama/version.h>
#include <wombat/timers.h>
#include <wombat/strutils.h>
#include <mama/integration/mama.h>
#include "bridgefunctions.h"


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

/* Version identifiers */
#define             BRIDGE_NAME            "noop"
#define             BRIDGE_VERSION         "1.0"

/* Default payload names and IDs to be loaded when this bridge is loaded */
static char*        PAYLOAD_NAMES[]         =   { "qpidmsg", NULL };
static char         PAYLOAD_IDS[]           =   { MAMA_PAYLOAD_QPID, '\0' };


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

/* Must be immplemented in overriding bridge */
mama_status
noopBridge_init (mamaBridge bridgeImpl)
{
    mama_status status         = MAMA_STATUS_OK;
    const char* runtimeVersion = NULL;

    /* Reusable buffer to populate with property values */
    char propString[MAX_INTERNAL_PROP_LEN];
    versionInfo rtVer;

    mama_log (MAMA_LOG_LEVEL_SEVERE, "noopBridge_init(): IN INIT");

    /* Will set the bridge's compile time MAMA version */
    MAMA_SET_BRIDGE_COMPILE_TIME_VERSION(BRIDGE_NAME);

    /* Enable extending of the base bridge implementation */
    status = mamaBridgeImpl_setReadOnlyProperty (bridgeImpl,
                                                 MAMA_PROP_EXTENDS_BASE_BRIDGE,
                                                 "true");

    /* Get the runtime version of MAMA and parse into version struct */
    runtimeVersion = mamaInternal_getMetaProperty (MAMA_PROP_MAMA_RUNTIME_VER);
    strToVersionInfo (runtimeVersion, &rtVer);

    /* NB checks are runtime only - assume build system will prevent accidental
     * compilation against incompatible versions. This is a demonstration to
     * show how you could do runtime version checking. */
    if (1 == rtVer.mMajor)
    {
        mama_log (MAMA_LOG_LEVEL_SEVERE, "noopBridge_init(): "
                                         "This version of the bridge (%s) cannot be used with MAMA %s.",
                  BRIDGE_VERSION,
                  runtimeVersion);
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    return status;
}

/* Must be immplemented in overriding bridge */
const char*
noopBridge_getVersion (void)
{
    return BRIDGE_VERSION;
}

/* Must be immplemented in overriding bridge */
const char*
noopBridge_getName (void)
{
    return BRIDGE_NAME;
}

/* Must be immplemented in overriding bridge */
mama_status
noopBridge_getDefaultPayloadId (char ***name, char **id)
{
    if (NULL == name || NULL == id)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /*
     * Populate name with the value of all supported payload names, the first
     * being the default
     */
    *name   = PAYLOAD_NAMES;

    /*
     * Populate id with the char keys for all supported payload names, the first
     * being the default
     */
    *id     = PAYLOAD_IDS;

    return MAMA_STATUS_OK;
}
