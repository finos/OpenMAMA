/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/mama.h>
#include <mama/integration/transport.h>
#include "bridgefunctions.h"


typedef struct implTransportBridge_
{
    int                 mIsValid;
    mamaTransport       mTransport;
    const char*         mName;
} implTransportBridge;

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

int
noopBridgeMamaTransport_isValid (transportBridge transport)
{
    return ((implTransportBridge*)transport)->mIsValid;
}

mama_status
noopBridgeMamaTransport_destroy (transportBridge transport)
{
    free (transport);
    return MAMA_STATUS_OK;
}

mama_status
noopBridgeMamaTransport_create (transportBridge*    result,
                                    const char*         name,
                                    mamaTransport       parent)
{
    *result = (transportBridge) calloc (1, sizeof(implTransportBridge));
    if (NULL == *result)
    {
        return MAMA_STATUS_NOMEM;
    }
    return MAMA_STATUS_OK;
}
