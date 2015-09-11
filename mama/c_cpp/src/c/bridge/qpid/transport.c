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

#include <wombat/port.h>
#include <property.h>
#include <mama/mama.h>
#include <queueimpl.h>
#include <msgimpl.h>
#include <mama/types.h>
#include <queueimpl.h>
#include <msgimpl.h>
#include <subscriptionimpl.h>
#include <transportimpl.h>
#include <timers.h>
#include <stdio.h>
#include <errno.h>
#include <wombat/queue.h>
#include <wombat/wUuid.h>

#include "transport.h"
#include "publisher.h"
#include "qpidbridgefunctions.h"
#include "qpiddefs.h"
#include "qpidcommon.h"
#include "msg.h"
#include "codec.h"
#include "endpointpool.h"

/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

/* Transport configuration parameters */
#define     TPORT_PARAM_PREFIX              "mama.qpid.transport"
#define     TPORT_PARAM_OUTGOING_URL        "outgoing_url"
#define     TPORT_PARAM_INCOMING_URL        "incoming_url"
#define     TPORT_PARAM_REPLY_URL           "reply_url"
#define     TPORT_PARAM_SUB_POOL_SIZE       "msg_pool_size"
#define     TPORT_PARAM_SUB_POOL_INC_SIZE   "msg_pool_inc_size"
#define     TPORT_PARAM_RECV_BLOCK_SIZE     "recv_block_size"
#define     TPORT_PARAM_TPORT_TYPE          "type"

/* Default values for corresponding configuration parameters */
#define     DEFAULT_OUTGOING_URL            "amqp://127.0.0.1:7777"
#define     DEFAULT_INCOMING_URL            "amqp://~127.0.0.1:6666"
#define     DEFAULT_REPLY_URL               "amqp://127.0.0.1:6666"
#define     DEFAULT_TPORT_TYPE              "p2p"
#define     DEFAULT_TPORT_TYPE_VALUE        QPID_TRANSPORT_TYPE_P2P
#define     DEFAULT_SUB_POOL_SIZE           128
#define     DEFAULT_SUB_POOL_INC_SIZE       128
#define     DEFAULT_RECV_BLOCK_SIZE         10

/* Non configurable runtime defaults */
#define     PN_MESSENGER_TIMEOUT            1
#define     PARAM_NAME_MAX_LENGTH           1024L
#define     MIN_SUB_POOL_SIZE               1L
#define     MAX_SUB_POOL_SIZE               30000L
#define     MIN_SUB_POOL_INC_SIZE           1L
#define     MAX_SUB_POOL_INC_SIZE           2000L
#define     MIN_RECV_BLOCK_SIZE             -1L
#define     MAX_RECV_BLOCK_SIZE             100L
#define     CONFIG_VALUE_TPORT_TYPE_BROKER  "broker"
#define     CONFIG_VALUE_TPORT_TYPE_P2P     "p2p"
#define     UUID_STRING_BUF_SIZE            37
#define     KNOWN_SOURCES_WTABLE_SIZE       10


/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * This function is called in the create function and is responsible for
 * actually subscribing to any transport level data sources and forking off the
 * recv dispatch thread for proton.
 *
 * @param impl  Qpid transport bridge to start
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidBridgeMamaTransportImpl_start (qpidTransportBridge* impl);

/**
 * This function is called in the destroy function and is responsible for
 * stopping the proton messengers and joining back with the recv thread created
 * in qpidBridgeMamaTransportImpl_start.
 *
 * @param impl  Qpid transport bridge to start
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static mama_status
qpidBridgeMamaTransportImpl_stop (qpidTransportBridge* impl);

/**
 * This function is a queue callback which is enqueued in the recv thread and
 * is then fired once it has reached the head of the queue.
 *
 * @param queue   MAMA queue from which this callback was fired
 * @param closure In this instance, the closure is the qpidMsgNode which was
 *                pulled from the pool in the recv callback and then sent
 *                down the MAMA queue.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
static void MAMACALLTYPE
qpidBridgeMamaTransportImpl_queueCallback (mamaQueue queue, void* closure);

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
 * @return long int containing the paramter value, default, minimum or maximum.
 */
static long int
qpidBridgeMamaTransportImpl_getParameterAsLong (long        defaultVal,
                                                long        minimum,
                                                long        maximum,
                                                const char* format,
                                                ...);

/**
 * This is a local function for parsing string configuration parameters from the
 * MAMA properties object, and supports default values. This function should
 * be used where the configuration parameter itself can be variable.
 *
 * @param defaultVal This is the default value to use if the parameter does not
 *                   exist in the configuration file
 * @param paramName  The format and variable list combine to form the real
 *                   configuration parameter used. This configuration parameter
 *                   will be stored at this location so the calling function
 *                   can log this.
 * @param format     This is the format string which is used to build the
 *                   name of the configuration parameter which is to be parsed.
 * @param ...        This is the variable list of arguments to be used along
 *                   with the format string.
 *
 * @return const char* containing the parameter value or the default.
 */
static const char*
qpidBridgeMamaTransportImpl_getParameterWithVaList (char*       defaultVal,
                                                    char*       paramName,
                                                    const char* format,
                                                    va_list     arguments);

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
static const char*
qpidBridgeMamaTransportImpl_getParameter (const char* defaultVal,
                                          const char* format,
                                          ...);

/**
 * This function is called on its own thread to run the main recv dispatch
 * for all messages coming off the mIncoming messenger. This function is
 * responsible for routing all incoming messages to their required destination
 * and parsing all administrative messages.
 *
 * @param closure    In this case, the closure refers to the qpidTransportBridge
 */
static void*
qpidBridgeMamaTransportImpl_dispatchThread (void* closure);

/**
 * This is a callback function which will be triggered while iterating over each
 * memory pool member to ensure that it's pn_message_t member is free'd
 *
 * @param pool       The pool which is being iterated over.
 * @param node       The node which is being considered during iteration.
 */
void
qpidBridgeMamaTransportImpl_msgNodeFree (memoryPool* pool, memoryNode* node);

/**
 * This is a callback function which will be triggered while iterating over each
 * memory pool member to ensure that it's pn_message_t member is free'd
 *
 * @param pool       The pool which is being iterated over.
 * @param node       The node which is being considered during iteration.
 */
void
qpidBridgeMamaTransportImpl_msgNodeInit (memoryPool* pool, memoryNode* node);



/**
 * This function is a wrapper for pn_messenger_stop as it caused deadlock
 * before qpid proton 0.5 and pn_messenger_interrupt as it was not introduced
 * until 0.5.
 *
 * @param messenger  The messenger to stop.
 */
static void
qpidBridgeMamaTransportImpl_stopProtonMessenger (pn_messenger_t* messenger);

/**
 * This function is a wrapper for pn_messenger_stop as it caused deadlock
 * until qpid proton 0.5.
 *
 * @param messenger  The messenger to free.
 */
static void
qpidBridgeMamaTransportImpl_freeProtonMessenger (pn_messenger_t* messenger);

/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

int
qpidBridgeMamaTransport_isValid (transportBridge transport)
{
    qpidTransportBridge*    impl   = (qpidTransportBridge*) transport;
    int                     status = 0;

    if (NULL != impl)
    {
        status = impl->mIsValid;
    }
    return status;
}

mama_status
qpidBridgeMamaTransport_destroy (transportBridge transport)
{
    qpidTransportBridge*    impl    = NULL;
    mama_status             status  = MAMA_STATUS_OK;
    qpidMsgNode*            next    = NULL;

    if (NULL == transport)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl  = (qpidTransportBridge*) transport;

    status = qpidBridgeMamaTransportImpl_stop (impl);

    // pn_message_free required here
    memoryPool_destroy (impl->mQpidMsgPool,
                        qpidBridgeMamaTransportImpl_msgNodeFree);

    wthread_mutex_destroy (&impl->mQpidMsgPool->mLock);

    pn_message_free(impl->mMsg);

    /* Macro wrapped as these caused deadlock prior to v0.5 of qpid proton */
    qpidBridgeMamaTransportImpl_freeProtonMessenger (impl->mIncoming);
    qpidBridgeMamaTransportImpl_freeProtonMessenger (impl->mOutgoing);

    endpointPool_destroy (impl->mSubEndpoints);
    endpointPool_destroy (impl->mPubEndpoints);

    /* Free the strdup-ed keys still held by the wtable */
    wtable_free_all_xdata (impl->mKnownSources);
    /* Finally, destroy the wtable */
    wtable_destroy (impl->mKnownSources);

    if (NULL != impl->mReplyAddress)
    {
        free ((void*) impl->mReplyAddress);
    }

    if (NULL != impl->mUuid)
    {
        free ((void*) impl->mUuid);
    }

    free (impl);

    return status;
}

mama_status
qpidBridgeMamaTransport_create (transportBridge*    result,
                                const char*         name,
                                mamaTransport       parent)
{
    qpidTransportBridge*    impl       = NULL;
    int                     poolSize   = 0;
    mama_status             status     = MAMA_STATUS_OK;
    const char*             tportType  = NULL;
    const char*             tmpReply   = NULL;
    const char*             defOutUrl  = NULL;

    if (NULL == result || NULL == name || NULL == parent)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (qpidTransportBridge*) calloc (1, sizeof (qpidTransportBridge));

    /* Back reference the MAMA transport */
    impl->mTransport           = parent;

    /* Initialize the dispatch thread pointer */
    impl->mQpidDispatchThread  = 0;
    impl->mMsg                 = pn_message ();
    impl->mQpidDispatchStatus  = MAMA_STATUS_OK;
    impl->mName                = name;
    impl->mKnownSources        = wtable_create ("mKnownSources",
                                                KNOWN_SOURCES_WTABLE_SIZE);

    mama_log (MAMA_LOG_LEVEL_FINE,
              "qpidBridgeMamaTransport_create(): Initializing Transport %s",
              name);

    /* Set the transport type */
    tportType =
        qpidBridgeMamaTransportImpl_getParameter (
            DEFAULT_TPORT_TYPE,
            "%s.%s.%s",
            TPORT_PARAM_PREFIX,
            name,
            TPORT_PARAM_TPORT_TYPE);

    if (0 == strcmp (tportType, CONFIG_VALUE_TPORT_TYPE_BROKER))
    {
        impl->mQpidTransportType = QPID_TRANSPORT_TYPE_BROKER;
        defOutUrl = DEFAULT_OUTGOING_URL;
    }
    else if (0 == strcmp (tportType, CONFIG_VALUE_TPORT_TYPE_P2P))
    {
        impl->mQpidTransportType = QPID_TRANSPORT_TYPE_P2P;
        defOutUrl = NULL;
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "Could not parse %s.%s.%s=%s. Using [%s].",
                TPORT_PARAM_PREFIX,
                name,
                TPORT_PARAM_TPORT_TYPE,
                tportType,
                DEFAULT_TPORT_TYPE);
        impl->mQpidTransportType = DEFAULT_TPORT_TYPE_VALUE;
    }

    /* Set the incoming address */
    impl->mIncomingAddress = qpidBridgeMamaTransportImpl_getParameter (
            DEFAULT_INCOMING_URL,
            "%s.%s.%s",
            TPORT_PARAM_PREFIX,
            name,
            TPORT_PARAM_INCOMING_URL);

    /* Set the outgoing address */
    impl->mOutgoingAddress =
        qpidBridgeMamaTransportImpl_getParameter (
            defOutUrl,
            "%s.%s.%s",
            TPORT_PARAM_PREFIX,
            name,
            TPORT_PARAM_OUTGOING_URL);

    /* Set the reply address */
    tmpReply =
        qpidBridgeMamaTransportImpl_getParameter (
            DEFAULT_REPLY_URL,
            "%s.%s.%s",
            TPORT_PARAM_PREFIX,
            name,
            TPORT_PARAM_REPLY_URL);

    /* Expand the wildcards in the reply then populates mReplyAddress with a
     * newly allocated result */
    qpidBridgeCommon_generateSubjectUri (
            tmpReply,
            NULL,
            NULL,
            NULL,
            qpidBridgeMamaTransportImpl_getUuid ((transportBridge) impl),
            &impl->mReplyAddress);

    /* Set the message pool size for pool initialization later */
    poolSize =
        (int) qpidBridgeMamaTransportImpl_getParameterAsLong (
            DEFAULT_SUB_POOL_SIZE,
            MIN_SUB_POOL_SIZE,
            MAX_SUB_POOL_SIZE,
            "%s.%s",
            TPORT_PARAM_PREFIX,
            TPORT_PARAM_SUB_POOL_SIZE);

    /* Get the size which the pool is to increment by when full */
    impl->mQpidMsgPoolIncSize =
        (unsigned int) qpidBridgeMamaTransportImpl_getParameterAsLong (
            DEFAULT_SUB_POOL_INC_SIZE,
            MIN_SUB_POOL_INC_SIZE,
            MAX_SUB_POOL_INC_SIZE,
            "%s.%s",
            TPORT_PARAM_PREFIX,
            TPORT_PARAM_SUB_POOL_INC_SIZE);

    /* Get the receive window size */
    impl->mQpidRecvBlockSize =
        (int) qpidBridgeMamaTransportImpl_getParameterAsLong (
            DEFAULT_RECV_BLOCK_SIZE,
            MIN_RECV_BLOCK_SIZE,
            MAX_RECV_BLOCK_SIZE,
            "%s.%s",
            TPORT_PARAM_PREFIX,
            TPORT_PARAM_RECV_BLOCK_SIZE);

    /* Create the reusable message pool to recycle created messages */
    impl->mQpidMsgPool = memoryPool_create (poolSize, sizeof(qpidMsgNode));
    if (NULL == impl->mQpidMsgPool)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransport_create(): "
                  "Failed to create msgPool (%s). Exiting.", strerror(errno));
        free (impl);
        return MAMA_STATUS_PLATFORM;
    }

    memoryPool_iterate (impl->mQpidMsgPool,
                        qpidBridgeMamaTransportImpl_msgNodeInit);

    /* Create incoming and outgoing messengers */
    impl->mOutgoing = pn_messenger (NULL);
    impl->mIncoming = pn_messenger (NULL);

    if (NULL == impl->mOutgoing || NULL == impl->mIncoming)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransport_create(): "
                  "Failed to create a messenger");
        free (impl);
        return MAMA_STATUS_PLATFORM;
    }
    pn_messenger_set_timeout (impl->mIncoming, PN_MESSENGER_TIMEOUT);

    /* Start the admin messenger as it may be required for subscriptions */
    pn_messenger_start (impl->mOutgoing);

    status = endpointPool_create (&impl->mSubEndpoints, "mSubEndpoints");
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransport_create(): "
                  "Failed to create subscribing endpoints");
        free (impl);
        return MAMA_STATUS_PLATFORM;
    }

    /* Endpoints are only required if a broker is not being used */
    if (QPID_TRANSPORT_TYPE_P2P == impl->mQpidTransportType)
    {
        status = endpointPool_create (&impl->mPubEndpoints, "mPubEndpoints");
        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "qpidBridgeMamaTransport_create(): "
                      "Failed to create publishing endpoints");
            free (impl);
            return MAMA_STATUS_PLATFORM;
        }
    }

    impl->mIsValid = 1;

    *result = (transportBridge) impl;

    return qpidBridgeMamaTransportImpl_start (impl);
}

mama_status
qpidBridgeMamaTransport_forceClientDisconnect (transportBridge*   transports,
                                               int                numTransports,
                                               const char*        ipAddress,
                                               uint16_t           port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_findConnection (transportBridge*    transports,
                                        int                 numTransports,
                                        mamaConnection*     result,
                                        const char*         ipAddress,
                                        uint16_t            port)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_getAllConnections (transportBridge*    transports,
                                           int                 numTransports,
                                           mamaConnection**    result,
                                           uint32_t*           len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_getAllConnectionsForTopic (
                    transportBridge*    transports,
                    int                 numTransports,
                    const char*         topic,
                    mamaConnection**    result,
                    uint32_t*           len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_requestConflation (transportBridge*     transports,
                                           int                  numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_requestEndConflation (transportBridge*  transports,
                                              int               numTransports)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_getAllServerConnections (
        transportBridge*        transports,
        int                     numTransports,
        mamaServerConnection**  result,
        uint32_t*               len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_freeAllServerConnections (
        transportBridge*        transports,
        int                     numTransports,
        mamaServerConnection*   result,
        uint32_t                len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_freeAllConnections (transportBridge*    transports,
                                            int                 numTransports,
                                            mamaConnection*     result,
                                            uint32_t            len)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_getNumLoadBalanceAttributes (
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
qpidBridgeMamaTransport_getLoadBalanceSharedObjectName (
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
qpidBridgeMamaTransport_getLoadBalanceScheme (const char*       name,
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
qpidBridgeMamaTransport_sendMsgToConnection (transportBridge    tport,
                                             mamaConnection     connection,
                                             mamaMsg            msg,
                                             const char*        topic)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_isConnectionIntercepted (mamaConnection connection,
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
qpidBridgeMamaTransport_installConnectConflateMgr (
        transportBridge         handle,
        mamaConflationManager   mgr,
        mamaConnection          connection,
        conflateProcessCb       processCb,
        conflateGetMsgCb        msgCb)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_uninstallConnectConflateMgr (
        transportBridge         handle,
        mamaConflationManager   mgr,
        mamaConnection          connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_startConnectionConflation (
        transportBridge         tport,
        mamaConflationManager   mgr,
        mamaConnection          connection)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidBridgeMamaTransport_getNativeTransport (transportBridge     transport,
                                            void**              result)
{
    qpidTransportBridge* impl = (qpidTransportBridge*)transport;

    if (NULL == transport || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    *result = impl;

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMamaTransport_getNativeTransportNamingCtx (transportBridge transport,
                                                     void**          result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/* Call this every time a known MAMA symbol namespace is created */
void
qpidBridgeMamaTransportImpl_addKnownMamaSymbolNamespace (transportBridge transport,
        const char* symbolNamespace)
{
    qpidTransportBridge*    impl   = (qpidTransportBridge*) transport;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "qpidBridgeMamaTransportImpl_addKnownMamaSymbolNamespace(): "
                "transport NULL");
        return;
    }

    wtable_insert (impl->mKnownSources, symbolNamespace, (void*) symbolNamespace);

    return;
}

/* Check if this is a publisher for a namespace which we know about */
mama_bool_t
qpidBridgeMamaTransportImpl_isKnownMamaSymbolNamespace (transportBridge transport,
        const char* symbolNamespace)
{
    qpidTransportBridge*    impl   = (qpidTransportBridge*) transport;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "qpidBridgeMamaTransportImpl_isKnownMamaSymbolNamespace(): "
                "transport NULL");
        return 0;
    }

    if (NULL == wtable_lookup (impl->mKnownSources, symbolNamespace))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

qpidTransportBridge*
qpidBridgeMamaTransportImpl_getTransportBridge (mamaTransport transport)
{
    qpidTransportBridge*    impl;
    mama_status             status = MAMA_STATUS_OK;

    status = mamaTransport_getBridgeTransport (transport,
                                               (transportBridge*) &impl);

    if (status != MAMA_STATUS_OK || impl == NULL)
    {
        return NULL;
    }

    return impl;
}

qpidTransportType
qpidBridgeMamaTransportImpl_getType (transportBridge transport)
{
    qpidTransportBridge*    impl = (qpidTransportBridge*) transport;

    if (impl == NULL)
    {
        return DEFAULT_TPORT_TYPE_VALUE;
    }

    return impl->mQpidTransportType;
}

const char*
qpidBridgeMamaTransportImpl_getUuid (transportBridge transport)
{
    qpidTransportBridge*    impl = (qpidTransportBridge*) transport;
    if (NULL == impl->mUuid)
    {
        wUuid                tempUuid;
        char                 uuidStringBuffer[UUID_STRING_BUF_SIZE];

        wUuid_generate_time  (tempUuid);
        wUuid_unparse        (tempUuid, uuidStringBuffer);
        impl->mUuid = strdup (uuidStringBuffer);
    }
    return impl->mUuid;
}


const char*
qpidBridgeMamaTransportImpl_getOutgoingAddress (transportBridge transport)
{
    qpidTransportBridge*    impl = (qpidTransportBridge*) transport;

    if (impl == NULL)
    {
        return NULL;
    }

    return impl->mOutgoingAddress;
}

const char*
qpidBridgeMamaTransportImpl_getIncomingAddress (transportBridge transport)
{
    qpidTransportBridge*    impl = (qpidTransportBridge*) transport;

    if (impl == NULL)
    {
        return NULL;
    }

    return impl->mIncomingAddress;
}

const char*
qpidBridgeMamaTransportImpl_getReplyAddress (transportBridge transport)
{
    qpidTransportBridge*    impl = (qpidTransportBridge*) transport;

    if (impl == NULL)
    {
        return NULL;
    }

    return impl->mReplyAddress;
}

/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

mama_status
qpidBridgeMamaTransportImpl_start (qpidTransportBridge* impl)
{
    wthread_attr_t  attr;
    int             rc;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "qpidBridgeMamaTransportImpl_start(): transport NULL");
        return MAMA_STATUS_NULL_ARG;
    }

    /*
     * Only restart if this hasn't been started before - workaround until
     * deadlock bug in proton is resolved and we can correctly stop the
     * messenger ahead of this in the case of a start -> stop -> start workflow
     *
     * https://issues.apache.org/jira/browse/PROTON-346
     */
    if (0 == impl->mQpidDispatchThread)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST, "qpidBridgeMamaTransportImpl_start(): "
                  "Transport '%s' has not been started before - firing up "
                  "messengers.",
                  impl->mName);
        pn_messenger_start (impl->mIncoming);

        if (NULL == impl->mOutgoing || NULL == impl->mIncoming)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                    "qpidBridgeMamaTransportImpl_start(): messengers unset");
            return MAMA_STATUS_PLATFORM;
        }

        /* Blanket subscriptions are only required in p2p */
        if (QPID_TRANSPORT_TYPE_P2P == impl->mQpidTransportType)
        {
            if (pn_messenger_subscribe (impl->mIncoming,
                                        impl->mIncomingAddress) <= 0)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR, "qpidBridgeMamaTransportImpl_start(): "
                          "Error Subscribing to %s : %s",
                          impl->mIncomingAddress,
                          PN_MESSENGER_ERROR(impl->mIncoming));
                (impl->mIncoming);
                return MAMA_STATUS_PLATFORM;
            }
        }
        else if (QPID_TRANSPORT_TYPE_BROKER == impl->mQpidTransportType)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "qpidBridgeMamaTransportImpl_start(): "
                      "Subscribing to %s : %s",
                      impl->mReplyAddress,
                      PN_MESSENGER_ERROR(impl->mIncoming));
            /* Subscribe to the URL advertised in reply_to */
            if (pn_messenger_subscribe (impl->mIncoming,
                                        impl->mReplyAddress) <= 0)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR, "qpidBridgeMamaTransportImpl_start(): "
                          "Error Subscribing to %s : %s",
                          impl->mReplyAddress,
                          PN_MESSENGER_ERROR(impl->mIncoming));
                qpidBridgeMamaTransportImpl_stopProtonMessenger (impl->mIncoming);
                return MAMA_STATUS_PLATFORM;
            }
        }
    }

    /* Set the transport bridge mIsDispatching to true. */
    impl->mIsDispatching = 1;

    /* Initialize dispatch thread */
    rc = wthread_create (&(impl->mQpidDispatchThread),
                         NULL,
                         qpidBridgeMamaTransportImpl_dispatchThread,
                         impl);
    if (0 != rc)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "qpidBridgeMamaTransportImpl_start(): "
                  "wthread_create returned %d", rc);
        return MAMA_STATUS_PLATFORM;
    }
    return MAMA_STATUS_OK;
}

mama_status qpidBridgeMamaTransportImpl_stop (qpidTransportBridge* impl)
{
    /* There are two mechanisms by which we can stop the transport
     * - Send a special message, which will be picked up by recv 
     *   For the instance when there is very little data flowing.
     * - Set the mIsDispatching variable in the transportBridge object to
     *   false, for instances when there is a lot of data flowing.
     */
    mama_status     status = MAMA_STATUS_OK;

    /* Set the transportBridge mIsDispatching to false */
    impl->mIsDispatching = 0;

    mama_log (MAMA_LOG_LEVEL_FINEST, "qpidBridgeMamaTransportImpl_stop(): "
                  "Waiting on dispatch thread to terminate.");

    wthread_join (impl->mQpidDispatchThread, NULL);
    status = impl->mQpidDispatchStatus;

    mama_log (MAMA_LOG_LEVEL_FINE, "qpidBridgeMamaTransportImpl_stop(): "
                      "Stopping the outgoing messenger.");
    qpidBridgeMamaTransportImpl_stopProtonMessenger (impl->mOutgoing);

    mama_log (MAMA_LOG_LEVEL_FINE, "qpidBridgeMamaTransportImpl_stop(): "
                      "Stopping the incoming messenger.");
    qpidBridgeMamaTransportImpl_stopProtonMessenger (impl->mIncoming);

    mama_log (MAMA_LOG_LEVEL_FINEST, "qpidBridgeMamaTransportImpl_stop(): "
                      "Rejoined with status: %s.",
                      mamaStatus_stringForStatus(status));

    return MAMA_STATUS_OK;
}

/**
 * Called when message removed from queue by dispatch thread
 *
 * @param data The AMQP payload
 * @param closure The subscriber
 */
void MAMACALLTYPE
qpidBridgeMamaTransportImpl_queueCallback (mamaQueue queue, void* closure)
{
    memoryNode*          node            = (memoryNode*) closure;
    qpidMsgNode*         msgNode         = NULL;
    mama_status          status          = MAMA_STATUS_OK;
    mamaMsg              tmpMsg          = NULL;
    msgBridge            bridgeMsg       = NULL;
    memoryPool*          pool            = NULL;
    qpidTransportBridge* impl            = NULL;
    qpidSubscription*    subscription    = NULL;
    const char*          subject         = NULL;

    if (NULL == node)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Called with NULL msgNode (destroyed?)");
        return;
    }
    msgNode = (qpidMsgNode*) node->mNodeBuffer;
    pool = node->mPool;

    if (NULL == pool)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Called with NULL msgPool (destroyed?)");
        return;
    }
    impl = msgNode->mQpidTransportBridge;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Called with NULL transportBridge (destroyed?)");
        return;
    }
    /* Take the transport from the pool, then lookup the subscription */
    /* We need some way of finding the subject key, so we can actually
     * lookup the subscription. */
    subject = pn_message_get_subject (msgNode->mMsg);
    subscription = msgNode->mQpidSubscription;

    /* Can't do anything without a subscriber */
    if (NULL == subscription)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Called with NULL subscriber (destroyed?)");
        return;
    }

    /* If this is a broker connection, we don't need to be selective at our
     * layer */
    if (0 == endpointPool_isRegistedByContent (impl->mSubEndpoints,
                                               subject,
                                               subscription))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Subscriber has been unregistered since msg was enqueued.");
        return;
    }

    /* Make sure that the subscription is processing messages */
    if (1 != subscription->mIsNotMuted)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Skipping update - subscription %p is muted.", subscription);
        return;
    }

    /* This is the reuseable message stored on the associated MamaQueue */
    tmpMsg = mamaQueueImpl_getMsg (subscription->mMamaQueue);
    if (NULL == tmpMsg)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Could not get cached mamaMsg from event queue.");
        return;
    }

    /* Get the bridge message from the mamaMsg */
    status = mamaMsgImpl_getBridgeMsg (tmpMsg, &bridgeMsg);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "Could not get bridge message from cached "
                  "queue mamaMsg [%s]", mamaStatus_stringForStatus (status));
        return;
    }

    /* Unpack this bridge message into a MAMA msg implementation */
    status = qpidBridgeMsgCodec_unpack (bridgeMsg, tmpMsg, msgNode->mMsg);
    if (MAMA_STATUS_OK != status)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_queueCallback(): "
                  "qpidBridgeMamaMsgImpl_unpack() failed. [%s]",
                  mamaStatus_stringForStatus (status));
    }
    else
    {
        /* Process the message as normal */
        status = mamaSubscription_processMsg (subscription->mMamaSubscription,
                                              tmpMsg);
        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "qpidBridgeMamaTransportImpl_queueCallback(): "
                      "mamaSubscription_processMsg() failed. [%d]", status);
        }
    }

    /* No matter what happened we need to clean up */
    memoryPool_returnNode (pool, node);

    return;
}

long int qpidBridgeMamaTransportImpl_getParameterAsLong (
                                                long defaultVal,
                                                long minimum,
                                                long maximum,
                                                const char* format, ...)
{
    const char* returnVal     = NULL;
    long        returnLong    = 0;
    char        paramDefault[PARAM_NAME_MAX_LENGTH];
    char        paramName[PARAM_NAME_MAX_LENGTH];

    /* Create list for storing the parameters passed in */
    va_list arguments;

    /* Populate list with arguments passed in */
    va_start(arguments, format);

    snprintf (paramDefault, PARAM_NAME_MAX_LENGTH, "%ld", defaultVal);

    returnVal = qpidBridgeMamaTransportImpl_getParameterWithVaList (paramDefault,
                                                                    paramName,
                                                                    format,
                                                                    arguments);

    /* Translate the returned string to a long */
    returnLong = atol (returnVal);

    if (returnLong < minimum)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                "qpidBridgeMamaTransportImpl_getParameterAsLong: "
                "Value for %s too small (%ld) - reverting to: [%ld]",
                paramName,
                returnLong,
                minimum);
        returnLong = minimum;
    }
    else if (returnLong > maximum)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                "qpidBridgeMamaTransportImpl_getParameterAsLong: "
                "Value for %s too large (%ld) - reverting to: [%ld]",
                paramName,
                returnLong,
                maximum);
        returnLong = maximum;
    }
    /* These will be equal if unchanged */
    else if (returnVal == paramDefault)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "qpidBridgeMamaTransportImpl_getParameterAsLong: "
                  "parameter [%s]: [%ld] (Default)",
                  paramName, returnLong);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "qpidBridgeMamaTransportImpl_getParameterAsLong: "
                  "parameter [%s]: [%ld] (User Defined)",
                  paramName, returnLong);
    }

    return returnLong;
}

const char* qpidBridgeMamaTransportImpl_getParameterWithVaList (
                                            char* defaultVal,
                                            char* paramName,
                                            const char* format,
                                            va_list arguments)
{
    const char* property = NULL;

    /* Create the complete transport property string */
    vsnprintf (paramName, PARAM_NAME_MAX_LENGTH,
               format, arguments);

    /* Get the property out for analysis */
    property = properties_Get (mamaInternal_getProperties (),
                               paramName);

    /* Properties will return NULL if parameter is not specified in configs */
    if (property == NULL)
    {
        property = defaultVal;
    }

    return property;
}

const char* qpidBridgeMamaTransportImpl_getParameter (
                                            const char* defaultVal,
                                            const char* format, ...)
{
    char        paramName[PARAM_NAME_MAX_LENGTH];
    const char* returnVal = NULL;
    /* Create list for storing the parameters passed in */
    va_list     arguments;

    /* Populate list with arguments passed in */
    va_start (arguments, format);

    returnVal = qpidBridgeMamaTransportImpl_getParameterWithVaList (
                        (char*)defaultVal,
                        paramName,
                        format,
                        arguments);

    /* These will be equal if unchanged */
    if (returnVal == defaultVal)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "qpidBridgeMamaTransportImpl_getParameter: "
                  "parameter [%s]: [%s] (Default)",
                  paramName,
                  returnVal);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                  "qpidBridgeMamaTransportImpl_getParameter: "
                  "parameter [%s]: [%s] (User Defined)",
                  paramName,
                  returnVal);
    }

    /* Clean up the list */
    va_end(arguments);

    return returnVal;
}

void* qpidBridgeMamaTransportImpl_dispatchThread (void* closure)
{
    qpidTransportBridge*    impl          = (qpidTransportBridge*)closure;
    qpidMsgNode*            msgNode       = NULL;
    qpidMsgNode*            tmpMsgNode    = NULL;
    memoryNode*             tmpNode       = NULL;
    const char*             subject       = NULL;
    pn_data_t*              properties    = NULL;
    endpoint_t*             subs          = NULL;
    size_t                  subCount      = 0;
    size_t                  subInc        = 0;
    mama_status             status        = MAMA_STATUS_OK;
    qpidSubscription*       subscription  = NULL;
    memoryNode*             node          = NULL;

    if (NULL == impl->mIncoming)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidBridgeMamaTransportImpl_dispatchThread(): "
                  "NULL messenger");
    }

    /*
     * Check if we should be still dispatching.
     * We shouldn't need to lock around this, as we're performing a simple value
     * read - if it changes in the middle of the read, we don't actually care. 
     */
    while (1 == impl->mIsDispatching)
    {
        int pnErr = 0;

        pnErr = pn_messenger_recv (impl->mIncoming,
                impl->mQpidRecvBlockSize);
        if (PN_TIMEOUT == pnErr)
        {
            continue;
        }

        if (0 != pnErr)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "qpidBridgeMamaTransportImpl_dispatchThread(): "
                      "Recv Error [%d] (%s) '%s'",
                      pnErr,
                      pn_code(pnErr),
                      PN_MESSENGER_ERROR(impl->mIncoming));
            impl->mQpidDispatchStatus = MAMA_STATUS_PLATFORM;
            return NULL;
        }

        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "qpidBridgeMamaTransportImpl_dispatchThread(): recv-ed");

        while (pn_messenger_incoming (impl->mIncoming) > 0)
        {

            node = memoryPool_getNode (impl->mQpidMsgPool,
                                       sizeof(qpidMsgNode));
            if (NULL == node)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "Error getting msgNode from msgPool (%s).",
                          strerror(errno));
                impl->mQpidDispatchStatus = MAMA_STATUS_PLATFORM;
                return NULL;
            }
            msgNode = (qpidMsgNode*)node->mNodeBuffer;
            if (NULL == msgNode->mMsg)
            {
                msgNode->mMsg = pn_message ();
            }

            if (pn_messenger_get (impl->mIncoming, msgNode->mMsg))
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "error getting message, %s",
                          PN_MESSENGER_ERROR (impl->mIncoming));
                impl->mQpidDispatchStatus = MAMA_STATUS_PLATFORM;
                return NULL;
            }

            /* Get the subject which contains the topic */
            subject    = pn_message_get_subject (msgNode->mMsg);
            properties = pn_message_properties  (msgNode->mMsg);

            /* Move to the first element inside */
            pn_data_next     (properties); /* Move past first NULL byte */
            pn_data_get_map(properties);
            pn_data_enter    (properties); /* Enter into meta map */

            int found = 0;
            found = pn_data_lookup(properties,QPID_KEY_MSGTYPE);
            if (found)
            {
                msgNode->mMsgType = (qpidMsgType) pn_data_get_ubyte (properties);
            }
            else
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "Unable to retrieve message type from message");
                return NULL;
            }

            switch (msgNode->mMsgType)
            {
            case QPID_MSG_TERMINATE:
                mama_log (MAMA_LOG_LEVEL_FINER,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "Received request to stop "
                          "dispatching - exiting dispatch thread");
                impl->mQpidDispatchStatus = MAMA_STATUS_OK;

                memoryPool_returnNode (impl->mQpidMsgPool, node);
                return NULL;
                break;
            case QPID_MSG_SUB_REQUEST:
            {
                pn_data_t*  data            = pn_message_body (msgNode->mMsg);
                const char* topic           = NULL;
                const char* replyTo         = NULL;

                /* Move to the content which will contain the topic */
                pn_data_next (data);
                topic       = pn_data_get_string (data).start;
                replyTo     = pn_message_get_reply_to (msgNode->mMsg);

                mama_log (MAMA_LOG_LEVEL_FINER,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "Received subscription request "
                          "for subject %s, to be published to %s",
                          topic,
                          replyTo);

                endpointPool_registerWithIdentifier (impl->mPubEndpoints,
                                                     topic,
                                                     replyTo,
                                                     NULL);
                pn_data_exit (properties);

                memoryPool_returnNode (impl->mQpidMsgPool, node);
                continue;
                break;

            }
            /*
             * No further meta data needs extracting at this point - they will
             * be processed in the queue callback instead.
             */
            case QPID_MSG_PUB_SUB:
            case QPID_MSG_INBOX_REQUEST:
            case QPID_MSG_INBOX_RESPONSE:
            default:
                break;
            }

            pn_data_exit (properties);

            if (NULL == subject)
            {
                memoryPool_returnNode (impl->mQpidMsgPool, node);
                continue;
            }

            status = endpointPool_getRegistered (impl->mSubEndpoints,
                                                 subject,
                                                 &subs,
                                                 &subCount);

            if (MAMA_STATUS_OK != status)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "Could not query registration table "
                          "for symbol %s (%s)",
                          subject,
                          mamaStatus_stringForStatus (status));
                memoryPool_returnNode (impl->mQpidMsgPool, node);
                continue;
            }

            if (0 == subCount)
            {
                mama_log (MAMA_LOG_LEVEL_FINEST,
                          "qpidBridgeMamaTransportImpl_dispatchThread(): "
                          "discarding uninteresting message "
                          "for symbol %s", subject);

                memoryPool_returnNode (impl->mQpidMsgPool, node);
                continue;
            }

            /* Within this loop, queue callbacks release the pool messages */
            for (subInc = 0; subInc < subCount; subInc++)
            {
                subscription = (qpidSubscription*)subs[subInc];

                if (1 == subscription->mIsTportDisconnected)
                {
                	subscription->mIsTportDisconnected = 0;
                }

                if (1 != subscription->mIsNotMuted)
                {
                    mama_log (MAMA_LOG_LEVEL_FINEST,
                              "qpidBridgeMamaTransportImpl_dispatchThread(): "
                              "muted - not queueing update for symbol %s",
                              subject);
                    continue;
                }
                /* If this isn't the last one in the list */
                else if (subInc != (subCount - 1))
                {
                    tmpNode = memoryPool_getNode (impl->mQpidMsgPool,
                                                  sizeof(qpidMsgNode));
                    tmpMsgNode = (qpidMsgNode*) tmpNode;

                    /*
                     * Copy the original msg node to a new one. Note that
                     * Annotations or instructions not used so those can be
                     * safely ignored
                     */
                    tmpMsgNode->mMsgType = msgNode->mMsgType;
                    tmpMsgNode->mQpidSubscription = subscription;
                    tmpMsgNode->mQpidTransportBridge = impl;

                    pn_data_copy (pn_message_body (tmpMsgNode->mMsg),
                                  pn_message_body (msgNode->mMsg));
                    pn_data_copy (pn_message_properties (tmpMsgNode->mMsg),
                                  pn_message_properties (msgNode->mMsg));

                    qpidBridgeMamaQueue_enqueueEvent (
                            (queueBridge) subscription->mQpidQueue,
                            qpidBridgeMamaTransportImpl_queueCallback,
                            tmpMsgNode);
                }
                /*
                 * If this is the last (or only) element and all copies are
                 * done, make use of the original message node rather than copy.
                 */
                else
                {
                    msgNode->mQpidSubscription = subscription;
                    msgNode->mQpidTransportBridge = impl;
                    qpidBridgeMamaQueue_enqueueEvent (
                            (queueBridge) subscription->mQpidQueue,
                            qpidBridgeMamaTransportImpl_queueCallback,
                            node);
                }
            }
        }
    }

    impl->mQpidDispatchStatus = MAMA_STATUS_OK;
    return NULL;
}


void
qpidBridgeMamaTransportImpl_msgNodeFree (memoryPool* pool, memoryNode* node)
{
    qpidMsgNode* msgNode = (qpidMsgNode*) node->mNodeBuffer;
    if (NULL != msgNode->mMsg)
    {
        pn_message_free (msgNode->mMsg);
    }
}

void
qpidBridgeMamaTransportImpl_msgNodeInit (memoryPool* pool, memoryNode* node)
{
    qpidMsgNode* msgNode = (qpidMsgNode*) node->mNodeBuffer;
    if (NULL == msgNode->mMsg)
    {
        msgNode->mMsg = pn_message ();
    }
}


/* These functions depend on methods only introduced in qpid proton 0.5 */
void qpidBridgeMamaTransportImpl_stopProtonMessenger (pn_messenger_t* messenger)
{
    mama_log (MAMA_LOG_LEVEL_FINE,
              "qpidBridgeMamaTransportImpl_stopProtonMessenger(): "
              "Stopping the outgoing messenger %p (%s).",
              messenger,
              pn_messenger_name(messenger));

    pn_messenger_stop (messenger);
}

void qpidBridgeMamaTransportImpl_freeProtonMessenger (pn_messenger_t* messenger)
{
    pn_messenger_free (messenger);
}
