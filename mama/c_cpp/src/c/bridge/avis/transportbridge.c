/* $Id: transportbridge.c,v 1.1.2.6 2011/09/27 11:39:48 emmapollock Exp $
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
#include <avis/avis_client_config.h>

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
    mama_log (logLevel, "avis error code=%d, error msg=%s", avis->error.code, avis->error.message);
}

void closeListener(Elvin* avis, CloseReason reason, const char* message, void* closure)
{
    const char* errMsg;
    if (avisBridge(closure) == NULL) {
        mama_log (MAMA_LOG_LEVEL_FINE, "Avis closeListener: could not get Avis bridge");
        return;
    }

    switch( reason )
    {
        case REASON_CLIENT_SHUTDOWN:            errMsg = "Avis client shutdown"; break;
        case REASON_ROUTER_SHUTDOWN:            errMsg = "Avis router shutdown"; break;
        case REASON_PROTOCOL_VIOLATION:         errMsg = "Avis protocol violation"; break;
        case REASON_ROUTER_STOPPED_RESPONDING:  errMsg = "Avis router stopped responding"; break;
        default:                                errMsg = "Unknown Avis error";
    }
    mamaTransportImpl_disconnect(avisBridge(closure)->mTransportBridge->mTransport, MAMA_TRANSPORT_DISCONNECT, NULL, NULL);
    mama_log (MAMA_LOG_LEVEL_FINE, "%s : %s", errMsg, message);
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
    mama_status status = mamaTransport_getBridgeTransport (transport, (void*) &pTransportBridge);
    if ((status != MAMA_STATUS_OK) || (pTransportBridge == NULL))
       return NULL;

    return pTransportBridge->mAvis;
}


void* avisDispatchThread(void* closure)
{
    avisTransportBridge* transportBridge = (avisTransportBridge*) closure;
    elvin_event_loop(transportBridge->mAvis);
    wsem_post(&transportBridge->mAvisDispatchSem);
    return NULL;
}


mama_status avisTransportBridge_start(avisTransportBridge* transportBridge)
{
   // stop Avis event loop
   wthread_t tid;
   int rc;
   CHECK_TRANSPORT(transportBridge); 

   if (0 != (rc = wthread_create(&tid, NULL, avisDispatchThread, transportBridge))) {
      mama_log (MAMA_LOG_LEVEL_ERROR, "wthread_create returned %d", rc);
      return MAMA_STATUS_SYSTEM_ERROR;
   }

   return MAMA_STATUS_OK;   
}



mama_status avisTransportBridge_stop(avisTransportBridge* transportBridge)
{
   CHECK_TRANSPORT(transportBridge); 
   
   // stop Avis event loop
   elvin_remove_close_listener(transportBridge->mAvis, closeListener);
   if (!elvin_invoke_close(transportBridge->mAvis)) {
      // there appears to be a race condition in Avis libs where router socket can sometimes be closed before
      // we receive the disconnect reply -- log it, and continue
      log_avis_error(MAMA_LOG_LEVEL_FINE, transportBridge->mAvis);
   }
   while (-1 == wsem_wait(&transportBridge->mAvisDispatchSem)) 
   {
      if (errno != EINTR) return MAMA_STATUS_SYSTEM_ERROR;
   }

   return MAMA_STATUS_OK;
}      

/*=========================================================================
  =                    Functions for the mamaTransport                    =
  =========================================================================*/

mama_status
avisBridgeMamaTransport_getNumLoadBalanceAttributes (
                                          const char* name,
                                          int*        numLoadBalanceAttributes)
{
    *numLoadBalanceAttributes = 0;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_getLoadBalanceSharedObjectName (
                                      const char*  name,
                                      const char** loadBalanceSharedObjectName)
{
    *loadBalanceSharedObjectName = NULL;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_getLoadBalanceScheme (
                                    const char*    name,
                                    tportLbScheme* scheme)
{
    *scheme = TPORT_LB_SCHEME_STATIC;
    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_create (transportBridge* result,
                                 const char*      name,
                                 mamaTransport    mamaTport )
{
    mama_status          status;
    avisBridgeImpl*      avisBridge = NULL;
    avisTransportBridge* transport  = NULL;
    mamaBridgeImpl*      bridgeImpl = NULL;
    const char*          url        = NULL; 
    
    transport = (avisTransportBridge*)calloc( 1, sizeof( avisTransportBridge ) );
    if (transport == NULL)
        return MAMA_STATUS_NOMEM;

    transport->mTransport = (mamaTransport) mamaTport;

    bridgeImpl = mamaTransportImpl_getBridgeImpl(mamaTport);
    if (!bridgeImpl) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaTransport_create(): Could not get bridge");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }
    if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) bridgeImpl, (void**) &avisBridge))) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaTransport_create(): Could not get Avis bridge object");
        free(transport);
        return status;
    }
    if (avisBridge->mTransportBridge != NULL) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaTransport_create(): Avis already connected");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }

    // create the Elvin object
    transport->mAvis = (Elvin*)calloc (1, sizeof (Elvin));
    if (transport->mAvis == NULL) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridge_createImpl(): Could not create Elvin object");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }

    // open the server connection
    url = getURL(name);
    if (url == NULL) {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "No %s property defined for transport : %s", TPORT_PARAM, name);
        return MAMA_STATUS_INVALID_ARG;
    }
    if (!elvin_open(transport->mAvis, url)) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "open failed for %s: %s", TPORT_PARAM, name);
        log_avis_error(MAMA_LOG_LEVEL_ERROR, transport->mAvis);
        avisBridgeMamaTransport_destroy((transportBridge)transport);
        return MAMA_STATUS_PLATFORM;
    }

    avisBridge->mTransportBridge = transport;
    elvin_add_close_listener(transport->mAvis, closeListener,  avisBridge);
    wsem_init(&transport->mAvisDispatchSem, 0, 0);

    *result = (transportBridge) transport;

    return MAMA_STATUS_OK;
}

mama_status
avisBridgeMamaTransport_destroy (transportBridge transport)
{
    mama_status status;
    avisBridgeImpl* avisBridge = NULL;
    mamaBridgeImpl* bridgeImpl = NULL;
    
    
    bridgeImpl = mamaTransportImpl_getBridgeImpl(avisTransport(transport)->mTransport);
    if (!bridgeImpl) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaTransport_create(): Could not get bridge");
        free(transport);
        return MAMA_STATUS_PLATFORM;
    }
    if (MAMA_STATUS_OK != (status = mamaBridgeImpl_getClosure((mamaBridge) bridgeImpl, (void**) &avisBridge))) {
        mama_log (MAMA_LOG_LEVEL_ERROR, "avisBridgeMamaTransport_create(): Could not get Avis bridge object");
        free(transport);
        return status;
    }

    wsem_destroy(&avisTransport(transport)->mAvisDispatchSem);
    elvin_error_free(&avisTransport(transport)->mAvis->error);
    free(avisTransport(transport)->mAvis);
    free(avisTransport(transport));
    avisBridge->mTransportBridge = NULL;
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

