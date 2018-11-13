/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <wombat/port.h>
#include <mama/mama.h>
#include <mama/integration/bridge/base.h>
#include <mama/integration/transport.h>


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

int
baseBridgeMamaTransport_isValid (transportBridge transport)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_destroy (transportBridge transport)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_create (transportBridge*    result,
                                const char*         name,
                                mamaTransport       parent)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_forceClientDisconnect (transportBridge*   transports,
                                               int                numTransports,
                                               const char*        ipAddress,
                                               uint16_t           port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_findConnection (transportBridge*    transports,
                                        int                 numTransports,
                                        mamaConnection*     result,
                                        const char*         ipAddress,
                                        uint16_t            port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getAllConnections (transportBridge*    transports,
                                           int                 numTransports,
                                           mamaConnection**    result,
                                           uint32_t*           len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getAllConnectionsForTopic (
                    transportBridge*    transports,
                    int                 numTransports,
                    const char*         topic,
                    mamaConnection**    result,
                    uint32_t*           len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_requestConflation (transportBridge*     transports,
                                           int                  numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_requestEndConflation (transportBridge*  transports,
                                              int               numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getAllServerConnections (
        transportBridge*        transports,
        int                     numTransports,
        mamaServerConnection**  result,
        uint32_t*               len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_freeAllServerConnections (
        transportBridge*        transports,
        int                     numTransports,
        mamaServerConnection*   result,
        uint32_t                len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_freeAllConnections (transportBridge*    transports,
                                            int                 numTransports,
                                            mamaConnection*     result,
                                            uint32_t            len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getNumLoadBalanceAttributes (
        const char*     name,
        int*            numLoadBalanceAttributes)
{
    if (NULL == numLoadBalanceAttributes || NULL == name)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *numLoadBalanceAttributes = 0;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaTransport_getLoadBalanceSharedObjectName (
        const char*     name,
        const char**    loadBalanceSharedObjectName)
{
    if (NULL == loadBalanceSharedObjectName)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *loadBalanceSharedObjectName = NULL;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getLoadBalanceScheme (const char*       name,
                                              tportLbScheme*    scheme)
{
    if (NULL == scheme || NULL == name)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *scheme = TPORT_LB_SCHEME_STATIC;
    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaTransport_sendMsgToConnection (transportBridge    tport,
                                             mamaConnection     connection,
                                             mamaMsg            msg,
                                             const char*        topic)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_isConnectionIntercepted (mamaConnection connection,
                                                 uint8_t*       result)
{
    if (NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *result = 0;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_installConnectConflateMgr (
        transportBridge         handle,
        mamaConflationManager   mgr,
        mamaConnection          connection,
        conflateProcessCb       processCb,
        conflateGetMsgCb        msgCb)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_uninstallConnectConflateMgr (
        transportBridge         handle,
        mamaConflationManager   mgr,
        mamaConnection          connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_startConnectionConflation (
        transportBridge         tport,
        mamaConflationManager   mgr,
        mamaConnection          connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
baseBridgeMamaTransport_getNativeTransport (transportBridge     transport,
                                            void**              result)
{
    if (NULL == transport || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *result = transport;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                     void**          result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}
