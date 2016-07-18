/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <avis/elvin.h>
#include <avis_client_config.h> // TBD or <avis/avis_client_config.h>?

#include "wombat/port.h"

#include <property.h>
#include <mama/mama.h>
#include <mama/types.h>
#include <transportimpl.h>
#include <timers.h>
#include <errno.h>
#include "transportbridge.h"
#include "avisbridgefunctions.h"
#include "avisdefs.h"
#include <wombat/wInterlocked.h>

#define TPORT_PREFIX    "mama.avis.transport"
#define TPORT_PARAM     "url"
#define DEFAULT_URL     "elvin://localhost"

#define avisTransport(transport) ((avisTransportBridge*) transport)
#define CHECK_TRANSPORT(transport) \
        do {  \
           if (avisTransport(transport) == 0) return MAMA_STATUS_NULL_ARG; \
           if (avisTransport(transport)->mAvis == 0) return MAMA_STATUS_INVALID_ARG; \
         } while(0)



mamaMsg  gMsg;

timerHeap gTimerHeap;

void* avisDispatchThread(void* closure);

void log_avis_error(MamaLogLevel logLevel, Elvin* avis)
{
    mama_log (logLevel, "avis error code=%d, error msg=%s", avis->error.code,
        avis->error.message);
}

static
void closeNotification (Elvin* avis,
                        void*  closure)
{
}

void closeListener(Elvin* avis, 
                   CloseReason reason, 
                   const char* message, 
                   void* closure)
{
    const char*          errMsg = NULL;
    avisTransportBridge* bridge = (avisTransportBridge*) closure;

    if (bridge == NULL) 
    {
        mama_log (MAMA_LOG_LEVEL_FINE, 
            "Avis closeListener: could not get Avis transport bridge");
        return;
    }

    switch( reason )
    {
        case REASON_CLIENT_SHUTDOWN:            
            errMsg = "Avis client shutdown";
            break;
        case REASON_ROUTER_SHUTDOWN:            
            errMsg = "Avis router shutdown";
            break;
        case REASON_PROTOCOL_VIOLATION:         
            errMsg = "Avis protocol violation"; 
            break;
        case REASON_ROUTER_STOPPED_RESPONDING:  
            errMsg = "Avis router stopped responding"; 
            break;
        default:                                
            errMsg = "Unknown Avis error";
    }

    mama_log (MAMA_LOG_LEVEL_FINE, "%s : %s", errMsg, message);

    if (REASON_PROTOCOL_VIOLATION == reason)
    {
        /* Ignore protocol violations */
        return;
    }

    mamaTransportImpl_disconnect(
        bridge->mTransport, 
        MAMA_TRANSPORT_DISCONNECT, 
        NULL, 
        NULL);

    wInterlocked_set (0, &bridge->mDispatching);
}

static const char*
getURL( const char *name )
{
    int len = 0;
    char* buff = NULL;
    const char* property = NULL;
    
    if (name == NULL)
        return NULL;

    mama_log (MAMA_LOG_LEVEL_FINE, "initializing Avis transport: %s", name);
    len = strlen(name) + strlen( TPORT_PREFIX ) + strlen(TPORT_PARAM) + 4;
    buff = (char *)alloca( len );
    memset(buff, '\0', len);
    snprintf( buff, len, "%s.%s.%s", TPORT_PREFIX, name, TPORT_PARAM );

    property = properties_Get( mamaInternal_getProperties(), buff );
    if ( property == NULL )
      return DEFAULT_URL;

    mama_log (MAMA_LOG_LEVEL_FINER,
              "Avis transport (%s) main connection: %s", name, property);
    return property;
}


Elvin* getAvis(mamaTransport transport)
{
    avisTransportBridge* pTransportBridge;
    mama_status status;

    status = mamaTransport_getBridgeTransport (transport, 
        (void*) &pTransportBridge);
    if ((status != MAMA_STATUS_OK) || (pTransportBridge == NULL))
       return NULL;

    return pTransportBridge->mAvis;
}


void* avisDispatchThread(void* closure)
{
    avisTransportBridge* transportBridge = (avisTransportBridge*) closure;

    while (1 == wInterlocked_read (&transportBridge->mDispatching))
    {
        elvin_poll (transportBridge->mAvis);

        if (ELVIN_ERROR_TIMEOUT == transportBridge->mAvis->error.code)
        {
            /* Timeout is acceptable and expected */
            elvin_error_reset (&transportBridge->mAvis->error);
        }
    }

    wsem_post (&transportBridge->mAvisDispatchSem);
    return NULL;
}


mama_status avisTransportBridge_start(avisTransportBridge* transportBridge)
{
    /* stop Avis event loop */
    int rc;

    CHECK_TRANSPORT(transportBridge); 

    if (1 == wInterlocked_read (&transportBridge->mDispatching))
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "avisTransportBridge_start(): "
                                       "Avis already dispatching");
        log_avis_error (MAMA_LOG_LEVEL_WARN, transportBridge->mAvis);
        return MAMA_STATUS_OK;
    }

    wInterlocked_set (1, &transportBridge->mDispatching);

    rc = wthread_create (&transportBridge->mThreadId, NULL, 
                        avisDispatchThread, transportBridge);

    if (0 != rc)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "wthread_create returned %d", rc);
        return MAMA_STATUS_SYSTEM_ERROR;
    }

    return MAMA_STATUS_OK;   
}

mama_status avisTransportBridge_stop(avisTransportBridge* transportBridge)
{
    CHECK_TRANSPORT(transportBridge); 

    if (0 == wInterlocked_read (&transportBridge->mDispatching))
    {
        mama_log (MAMA_LOG_LEVEL_WARN, "avisTransportBridge_stop(): "
                                       "Avis already stopped");
        log_avis_error (MAMA_LOG_LEVEL_WARN, transportBridge->mAvis);
        return MAMA_STATUS_OK;
    }

    wInterlocked_set (0, &transportBridge->mDispatching);

    /* Dispatch a dummy notification to get the event polling to iterate
     * another loop and examine the mDispatching state */
    elvin_invoke (transportBridge->mAvis, &closeNotification, transportBridge);

    while (-1 == wsem_wait(&transportBridge->mAvisDispatchSem)) 
    {
        if (errno != EINTR) return MAMA_STATUS_SYSTEM_ERROR;
    }

    wthread_join (transportBridge->mThreadId, NULL);

    return MAMA_STATUS_OK;
}      

int
avisTransportBridge_isDispatching (
    avisTransportBridge* transportBridge)
{
    CHECK_TRANSPORT(transportBridge); 

    return wInterlocked_read (&transportBridge->mDispatching);
}

/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/

mama_status
avisBridgeMamaTransport_getNumLoadBalanceAttributes (
                                          const char* name,
                                          int*        numLoadBalanceAttributes)
{
    if (!numLoadBalanceAttributes || !name) return MAMA_STATUS_NULL_ARG;
    *numLoadBalanceAttributes = 0;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                      const char*  name,
                                      const char** loadBalanceSharedObjectName)
{
    if (!loadBalanceSharedObjectName || !name) return MAMA_STATUS_NULL_ARG;
    *loadBalanceSharedObjectName = NULL;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_getLoadBalanceScheme (
                                    const char*    name,
                                    tportLbScheme* scheme)
{
    if (!scheme || !name) return MAMA_STATUS_NULL_ARG;
    *scheme = TPORT_LB_SCHEME_STATIC;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_create (transportBridge* result,
                                const char*      name,
                                mamaTransport    mamaTport )
{
    mama_status          status;
    avisTransportBridge* transport  = NULL;
    mamaBridgeImpl*      bridgeImpl = NULL;
    const char*          url        = NULL; 

    if (!result || !name || !mamaTport) return MAMA_STATUS_NULL_ARG;
    
    transport = (avisTransportBridge*)calloc( 1, sizeof( avisTransportBridge ) );
    if (transport == NULL)
        return MAMA_STATUS_NOMEM;

    transport->mTransport = (mamaTransport) mamaTport;

    bridgeImpl = mamaTransportImpl_getBridgeImpl(mamaTport);
    if (!bridgeImpl) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
            "avisBridgeMamaTransport_create(): Could not get bridge");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }

    /* create the Elvin object */
    transport->mAvis = (Elvin*)calloc (1, sizeof (Elvin));
    if (transport->mAvis == NULL) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
            "avisBridge_createImpl(): Could not create Elvin object");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }

    /* open the server connection */
    url = getURL(name);
    if (url == NULL) 
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, 
            "No %s property defined for transport : %s", TPORT_PARAM, name);
        return MAMA_STATUS_INVALID_ARG;
    }
    if (!elvin_open(transport->mAvis, url)) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
            "open failed for %s: %s", TPORT_PARAM, name);
        log_avis_error(MAMA_LOG_LEVEL_ERROR, transport->mAvis);
        avisBridgeMamaTransport_destroy((transportBridge)transport);
        return MAMA_STATUS_PLATFORM;
    }

    wInterlocked_initialize (&transport->mDispatching);

    elvin_add_close_listener(transport->mAvis, closeListener, transport);
    wsem_init(&transport->mAvisDispatchSem, 0, 0);

    *result = (transportBridge) transport;

    return avisTransportBridge_start(transport);
}

mama_status
avisBridgeMamaTransport_destroy (transportBridge transport)
{
    mama_status status;
    avisTransportBridge* transportBridge = (avisTransportBridge*) transport;
    mamaBridgeImpl*      bridgeImpl      = NULL;

    if (!transport) return MAMA_STATUS_NULL_ARG;
    
    bridgeImpl = mamaTransportImpl_getBridgeImpl(
        avisTransport(transport)->mTransport);
    if (!bridgeImpl) 
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, 
            "avisBridgeMamaTransport_destroy(): Could not get bridge");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }

    if (1 == wInterlocked_read (&transportBridge->mDispatching))
    {
        avisTransportBridge_stop (transportBridge);
    }

    elvin_remove_close_listener (transportBridge->mAvis, closeListener);

    if (!elvin_close (transportBridge->mAvis)) 
    {
        /* there appears to be a race condition in Avis libs where router socket
         * can sometimes be closed before we receive the disconnect reply -- log
         * it, and continue */
        log_avis_error(MAMA_LOG_LEVEL_FINE, transportBridge->mAvis);
    }

    wInterlocked_destroy (&transportBridge->mDispatching);
    wsem_destroy(&avisTransport(transport)->mAvisDispatchSem);
    free(avisTransport(transport)->mAvis);
    free(avisTransport(transport));
    return MAMA_STATUS_OK;
}

int
avisBridgeMamaTransport_isValid (transportBridge transport)
{
    if (avisTransport(transport) == 0) return 0;
    if (avisTransport(transport)->mAvis == 0) return 0;
    return 1;
}

mama_status
avisBridgeMamaTransport_forceClientDisconnect (transportBridge* transports,
                                              int              numTransports,
                                              const char*      ipAddress,
                                              uint16_t         port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_findConnection (transportBridge* transports,
                            int              numTransports,
                            mamaConnection*  result,
                            const char*      ipAddress,
                            uint16_t         port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_getAllConnections (transportBridge* transports,
                               int              numTransports,
                               mamaConnection** result,
                               uint32_t*        len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_getAllConnectionsForTopic (transportBridge* transports,
                                                    int              numTransports,
                                                    const char*      topic,
                                                    mamaConnection** result,
                                                    uint32_t*        len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_freeAllConnections (
                                        transportBridge* transports,
                                        int              numTransports,
                                        mamaConnection*  result,
                                        uint32_t         len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_getAllServerConnections (
                                        transportBridge*       transports,
                                        int                    numTransports,
                                        mamaServerConnection** result,
                                        uint32_t*              len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_freeAllServerConnections (
                                          transportBridge*      transports,
                                          int                   numTransports,
                                          mamaServerConnection* result,
                                          uint32_t              len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_sendMsgToConnection (transportBridge tport,
                                              mamaConnection  connection,
                                              mamaMsg         msg,
                                              const char*     topic)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_installConnectConflateMgr (
                                           transportBridge       handle,
                                           mamaConflationManager mgr,
                                           mamaConnection        connection,
                                           conflateProcessCb     processCb,
                                           conflateGetMsgCb      msgCb)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


mama_status
avisBridgeMamaTransport_uninstallConnectConflateMgr (
                                            transportBridge       handle,
                                            mamaConflationManager mgr,
                                            mamaConnection        connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_isConnectionIntercepted (mamaConnection connection,
                                                  uint8_t* result)
{
    if (!result) return MAMA_STATUS_NULL_ARG;
    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_startConnectionConflation (
    transportBridge       tport,
    mamaConflationManager mgr,
    mamaConnection        connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_getNativeTransport (transportBridge transport,
                                             void**          result)
{
    if (!result) return MAMA_STATUS_NULL_ARG;
    CHECK_TRANSPORT(transport);
    *result = avisTransport(transport);
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                      void** result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_requestConflation (transportBridge* transports,
                                            int              numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisBridgeMamaTransport_requestEndConflation (transportBridge* transports,
                                               int              numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

