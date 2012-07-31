/* $Id: ft.c,v 1.1.2.7 2011/10/02 19:02:17 ianbell Exp $
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

#include <mama/mama.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <mama/ft.h>
#include <mama/io.h>
#include <wombat/wCommon.h>

#include <sys/types.h>


#define FT_SYMBOL_PREFIX          "MAMA_FT"
#define FT_MAX_PACKET_SIZE        1500
#define FT_NETWORK                "239.1.1.1"
#define FT_SERVICE                7702
#define FT_TTL                    1
#define FT_FIELD_GROUP_NAME       "MamaFtGroupName"
#define FT_FIELD_GROUP_NAME_FID   201
#define FT_FIELD_PID                "MamaPid"
#define FT_FIELD_PID_FID            202
#define FT_FIELD_WEIGHT           "MamaFtWeight"
#define FT_FIELD_WEIGHT_FID       203
#define FT_FIELD_INCARNATION      "MamaFtIncarnation"
#define FT_FIELD_INCARNATION_FID  204
#define FT_FIELD_PRIMARY            "MamaFtPrimary"
#define FT_FIELD_PRIMARY_FID        205
#define FT_FIELD_IPADDRESS          "MamaIPAddress"
#define FT_FIELD_IPADDRESS_FID      206

#define FT_MAX_PROPERTY_NAME_LENGTH 1024

mama_status
multicastFt_setup (
    mamaFtMember                   member,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure);

mama_status
multicastFt_activate (
    mamaFtMember    member);


mama_status
multicastFt_deactivate (
    mamaFtMember    member);

static void multicastFt_sendHeartbeat (mamaFtMember member);

mama_status
bridgeFt_setup (
    mamaFtMember                   member,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure);

mama_status
bridgeFt_activate (
    mamaFtMember    member);

mama_status
bridgeFt_deactivate (
    mamaFtMember    member);

static void bridgeFt_sendHeartbeat (mamaFtMember member);
static int multicastFt_receiveHeartbeat(void* member);
static int bridgeFt_receiveHeartbeat(void* member);

const char *multicastFt_getProperty(char *buffer, const char *propertyName, const char *transportName);

static int foundheartbeat=0;

/*****************************************
*    Function Pointers
*****************************************/
typedef mama_status (*ft_setup) ( mamaFtMember                   member,
                    mamaQueue                      eventQueue,
                    mamaFtMemberOnFtStateChangeCb  callback,
                    mamaTransport                  transport,
                    const char*                    groupName,
                    mama_u32_t                     weight,
                    mama_f64_t                     heartbeatInterval,
                    mama_f64_t                     timeoutInterval,
                    void*                          closure);

typedef mama_status (*ft_destroy) (mamaFtMember member);

typedef mama_status (*ft_activate) (mamaFtMember member);

typedef mama_status (*ft_deactivate) (mamaFtMember member);

typedef void (*ft_sendHeartbeat) (void*  member);

typedef int (*ft_recvHeartbeat) (void* member);

typedef struct mamaFtMemberImpl_
{
    mamaQueue                      myQueue;
    mamaFtMemberOnFtStateChangeCb  myCallback;
    mamaTransport                  myTransport;
    const char*                    myGroupName;
    mama_u32_t                     myWeight;
    mama_u32_t                     myIncarnation;
    mama_u32_t                     myNextIncarnation;
    mama_f64_t                     myHeartbeatInterval;
    mama_f64_t                     myTimeoutInterval;
    void*                          myClosure;
    int                            myIsActive;
    mamaSubscription               mySubscription;
    mamaPublisher                  myPublisher;
    mamaTimer                      myHeartbeatTimer;
    mamaTimer                      myTimeoutTimer;
    mamaMsg                        myHeartbeatMsg;
    const char*                    myInstanceId;
    const char*                    mySymbol;
    mamaFtState                    myState;
    int                            myRecvSocket;
    int                            mySendSocket;
    struct sockaddr_in             mySendAddr;
    int                            myIP;
    int                            myPid;
    int                            myRecvWindow;
    ft_recvHeartbeat               ftRecvHearbeat;
    ft_setup               ftSetup;
    ft_activate               ftActivate;
    ft_deactivate                  ftDeactivate;
    ft_sendHeartbeat               ftSendHeartbeat;
} mamaFtMemberImpl;

/*****************************************
*     General callback functions
******************************************/
static void MAMACALLTYPE
ftHeartbeatTimerCb (mamaTimer timer, void* closure);

static void MAMACALLTYPE
ftTimeoutTimerCb   (mamaTimer timer, void* closure);


static void        reset             (mamaFtMemberImpl*  impl);
static mama_status startTimeoutTimer (mamaFtMemberImpl*  impl);
static mama_status stopTimeoutTimer  (mamaFtMemberImpl*  impl);
static mama_status resetTimeoutTimer (mamaFtMemberImpl*  impl);
static char*       genInstanceId     (const char*        groupName);
static char*       genSymbol         (const char*        groupName);

/* A callback function that does nothing. */
static void MAMACALLTYPE
mamaFtCallbackNoOp (
    mamaFtMember  ftMember,
    const char*   groupName,
    mamaFtState   state,
    void*         closure)
{
}

/* FT callbacks: */
static void MAMACALLTYPE
ioCallback         (mamaIo io, mamaIoType ioType,  void* closure);

static mamaIo       gReadHandler  =  NULL;

/* General implementation functions. */
int                receiveHeartbeat  (mamaFtMemberImpl*  impl);

void mamaCheckHeartbeat (mamaFtMemberImpl*  impl,
                     unsigned int hbWeight,
                     unsigned int hbIncarnation,
                     int          hbPid,
                     int          hbIpAddr,
                     int          hbPrimary,
                     char*        hbGroupName);

/* FT callbacks: */
static void MAMACALLTYPE
ftSubCreateCb      (mamaSubscription   subscription, void* closure);

static void MAMACALLTYPE
ftSubErrorCb       (mamaSubscription   subscription,
                    mama_status        status,
                    void*              platformError,
                    const char*        subject,
                    void*              closure);
static void MAMACALLTYPE
ftSubMsgCb         (mamaSubscription   subscription,
                    mamaMsg            msg,
                    void*              closure,
                    void*              itemClosure);

mama_status
mamaFtMember_create (mamaFtMember* member)
{
    mamaFtMemberImpl* impl = calloc (1, sizeof(mamaFtMemberImpl));
    *member = impl;
    if (!impl)
        return MAMA_STATUS_NOMEM;
    impl->myCallback = mamaFtCallbackNoOp;
    impl->myPid=getpid();
    impl->myRecvWindow=20;
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_setup (
    mamaFtMember                   member,
    mamaFtType                     fttype,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    mama_status ret;

    switch (fttype)
    {
    case MAMA_FT_TYPE_MULTICAST:
        impl->ftSetup = multicastFt_setup;
        impl->ftActivate = multicastFt_activate;
            impl->ftDeactivate = multicastFt_deactivate;
            impl->ftSendHeartbeat = multicastFt_sendHeartbeat;
            impl->ftRecvHearbeat = multicastFt_receiveHeartbeat;
        break;
    case MAMA_FT_TYPE_BRIDGE:
        impl->ftSetup = bridgeFt_setup;
        impl->ftActivate = bridgeFt_activate;
            impl->ftDeactivate = bridgeFt_deactivate;
            impl->ftSendHeartbeat = bridgeFt_sendHeartbeat;
            impl->ftRecvHearbeat = bridgeFt_receiveHeartbeat;
        break;

        default:
            break;
    }

    ret =impl->ftSetup (member,eventQueue,callback,transport,groupName,weight,heartbeatInterval,timeoutInterval,closure);

    return (ret);
}

mama_status
mamaFtMember_activate (
    mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    mama_status status;
    const char *ipaddress=NULL;

    if ((status = impl->ftActivate (member)) != MAMA_STATUS_OK)
        return status;

    /* Set up the heartbeat publisher timer. */
    status = mamaTimer_create (&impl->myHeartbeatTimer,
                                               impl->myQueue,
                                               ftHeartbeatTimerCb,
                                               impl->myHeartbeatInterval,
                                               impl);

    if (status != MAMA_STATUS_OK)
        return status;

    mama_getIpAddress(&ipaddress);
    impl->myIP=inet_addr(ipaddress);
    impl->myNextIncarnation = 1;

    /* Set up the heartbeat timeout timer. */
    status = startTimeoutTimer (impl);
    if (status != MAMA_STATUS_OK)
        return status;

    impl->myIsActive = 1;

    return (status);
}

mama_status
mamaFtMember_deactivate (
    mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    mama_status ret;

    if ((ret = impl->ftDeactivate (member)) != MAMA_STATUS_OK)
        return ret;

    /* cancel timer. */
    mamaTimer_destroy (impl->myHeartbeatTimer);
    stopTimeoutTimer (impl);
    impl->myIsActive = 0;
    impl->myIncarnation = 0;
    impl->myNextIncarnation = 0;

    return (ret);
}

mama_status
mamaFtMember_destroy (
    mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    mamaFtMember_deactivate (member);  /* ignore status */
    if (impl->myGroupName)
        free ((void*)impl->myGroupName);
    if (impl->myInstanceId)
        free ((void*)impl->myInstanceId);
    if (impl->mySymbol)
        free ((void*)impl->mySymbol);
    free ((void*)impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_isActive (
    const mamaFtMember  member,
    int*                result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myIsActive;
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_getGroupName (
    const mamaFtMember  member,
    const char**        result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myGroupName;
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_getWeight (
    const mamaFtMember  member,
    mama_u32_t*         result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myWeight;
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_getHeartbeatInterval (
    const mamaFtMember  member,
    mama_f64_t*         result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myHeartbeatInterval;
   return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_getTimeoutInterval (
    const mamaFtMember  member,
    mama_f64_t*         result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myTimeoutInterval;
   return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_getClosure (
    const mamaFtMember  member,
    void**              result)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl || !result)
        return MAMA_STATUS_INVALID_ARG;
    *result = impl->myClosure;
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_setWeight (
    mamaFtMember  member,
    mama_u32_t    value)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    impl->myWeight = value;
    reset (impl);
    return MAMA_STATUS_OK;
}

mama_status
mamaFtMember_setInstanceId (
    mamaFtMember  member,
    const char*   id)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;
    if (impl->myInstanceId)
        free ((void*)impl->myInstanceId);
    if (id && id[0])
    {
        impl->myInstanceId = strdup (id);
    }
    else
    {
        impl->myInstanceId = genInstanceId (impl->myGroupName);;
    }
    if (!impl->myInstanceId)
    {
        return MAMA_STATUS_NOMEM;
    }
    return MAMA_STATUS_OK;
}


const char*
mamaFtStateToString (mamaFtState state)
{
    switch (state)
    {
    case MAMA_FT_STATE_STANDBY:
        return "standby";
    case MAMA_FT_STATE_ACTIVE:
        return "active";
    case MAMA_FT_STATE_UNKNOWN:
        return "unknown";
    default:
        return "error";
    }
}

void reset (mamaFtMemberImpl* impl)
{

}

char* genInstanceId (const char*  groupName)
{
    size_t len = strlen(groupName) + 8 + 4 + 3;
    char* instanceIdStr = (char*) malloc (len);
    if (instanceIdStr)
    {
        mama_u32_t       ipAddr = 0;
        mama_u16_t       pid = getpid();
        char             hostname[256];
        struct hostent*  hostinfo;
        gethostname (hostname, 255);
        hostname[255] = '\0';
        hostinfo = gethostbyname (hostname);
        if (hostinfo && (hostinfo->h_addrtype == AF_INET))
        {
            ipAddr = *(in_addr_t*)(hostinfo->h_addr_list[0]);
        }
        snprintf (instanceIdStr, len, "%s.%0x.%0x",
                  groupName, ipAddr, pid);
    }
    return instanceIdStr;
}

char* genSymbol (const char*  groupName)
{
    size_t len = strlen(groupName) + strlen(FT_SYMBOL_PREFIX) + 2;
    char* symbol = (char*) malloc (len);
    if (symbol)
    {
        snprintf (symbol, len, "%s.%s", FT_SYMBOL_PREFIX, groupName);
    }
    return symbol;
}


static void invokeStateChange (mamaFtMemberImpl*  impl,
                               mamaFtState        state)
{
    if (impl->myState == state)
        return;

    switch (state)
    {
    case MAMA_FT_STATE_ACTIVE:
        /* Increment the incarnation number.
         * Send an immediate heartbeat.
         * Reset the heartbeat timer.
         * Stop the timeout timer*/
        impl->myIncarnation = impl->myNextIncarnation;
        impl->ftSendHeartbeat (impl);
        mamaTimer_reset (impl->myHeartbeatTimer);
        stopTimeoutTimer (impl);
        break;
    case MAMA_FT_STATE_STANDBY:
    case MAMA_FT_STATE_UNKNOWN:
        startTimeoutTimer (impl);
        break;
    }

    impl->myState = state;
    impl->myCallback ((mamaFtMember)impl,
                      impl->myGroupName,
                      impl->myState,
                      impl->myClosure);
}

static void checkState (mamaFtMemberImpl*  impl,
                        mamaFtState        state)
{
    /* "state" is the FT state we *should* be in currently, so we need
     * to check to see whether we need to invoke a switch. */
    switch (state)
    {
    case MAMA_FT_STATE_STANDBY:
        switch (impl->myState)
        {
        case MAMA_FT_STATE_UNKNOWN:
        case MAMA_FT_STATE_ACTIVE:
            /* switch to STANDBY mode. */
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MAMA FT: switching state to STANDBY");
            invokeStateChange (impl, MAMA_FT_STATE_STANDBY);
            break;
        case MAMA_FT_STATE_STANDBY:
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "MAMA FT: no state change");
            break;
        }
        break;

    case MAMA_FT_STATE_ACTIVE:
        switch (impl->myState)
        {
        case MAMA_FT_STATE_UNKNOWN:
        case MAMA_FT_STATE_STANDBY:
            /* switch to ACTIVE mode. */
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MAMA FT: switching state to ACTIVE");
            invokeStateChange (impl, MAMA_FT_STATE_ACTIVE);
            break;
        case MAMA_FT_STATE_ACTIVE:
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "MAMA FT: no state change");
            break;
        }
        break;

    case MAMA_FT_STATE_UNKNOWN:
        switch (impl->myState)
        {
        case MAMA_FT_STATE_ACTIVE:
        case MAMA_FT_STATE_STANDBY:
            /* switch to ACTIVE mode. */
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "MAMA FT: switching state to UNKNOWN");
            invokeStateChange (impl, MAMA_FT_STATE_UNKNOWN);
            break;
        case MAMA_FT_STATE_UNKNOWN:
            mama_log (MAMA_LOG_LEVEL_FINER,
                      "MAMA FT: no state change");
            break;
        }
        break;
    }
}

static void MAMACALLTYPE
ftHeartbeatTimerCb (mamaTimer          timer,
                         void*              closure)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*)closure;
    mama_log (MAMA_LOG_LEVEL_FINE, "MAMA multicast FT: heartbeat timer has fired");
    impl->ftSendHeartbeat (impl);
}

static void MAMACALLTYPE
ftTimeoutTimerCb (mamaTimer          timer,
                       void*              closure)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*)closure;
    mama_log (MAMA_LOG_LEVEL_FINE,
              "MAMA multicast FT: timeout timer has fired; Going Active");

    foundheartbeat=0;

    while (impl->ftRecvHearbeat(impl) == 0);;

    if (foundheartbeat==0)
    checkState (impl, MAMA_FT_STATE_ACTIVE);
}

mama_status startTimeoutTimer (mamaFtMemberImpl* impl)
{
    mama_log (MAMA_LOG_LEVEL_FINE, "MAMA FT: starting timeout timer");
    if (impl->myTimeoutTimer)
    {
        return resetTimeoutTimer (impl);
    }
    else
    {
        return mamaTimer_create (&impl->myTimeoutTimer,
                                 impl->myQueue,
                                 ftTimeoutTimerCb,
                                 impl->myTimeoutInterval,
                                 impl);
    }
}

mama_status stopTimeoutTimer (mamaFtMemberImpl* impl)
{
    mama_status status;
    mama_log (MAMA_LOG_LEVEL_FINE, "MAMA FT: stopping timeout timer");
    status = mamaTimer_destroy (impl->myTimeoutTimer);
    impl->myTimeoutTimer = NULL;
    return status;
}

mama_status resetTimeoutTimer (mamaFtMemberImpl* impl)
{
    mama_log (MAMA_LOG_LEVEL_FINE, "MAMA FT: resetting timeout timer");
    return mamaTimer_reset (impl->myTimeoutTimer);
}

int mamaBetterCredentials (mamaFtMemberImpl*  impl, unsigned int weight, 
					  unsigned int incarnation, in_addr_t ipAddr, int pid)
{
    if (weight > impl->myWeight)
    {
        /* Higher weight */
        return 0;
    }
    else if ((weight == impl->myWeight) && (incarnation > impl->myIncarnation))
    {
        /* Other sender has same weight but later incarnation */
        return 0;
    }
    else if ((weight == impl->myWeight) && (incarnation == impl->myIncarnation) &&
             (ipAddr > impl->myIP))
    {
        /* This should not happen often. We have equal credentials
          so we use the ipAddr as an arbitrary tie-breaker */
        return 0;
    }
    else if ((weight == impl->myWeight) && (incarnation == impl->myIncarnation) &&
             (ipAddr == impl->myIP) && (pid > impl->myPid))
    {
        /* This should not happen often. We have equal credentials
           AND we are on the same machine, so we use the pid as
           another tie-breaker */
        return 0;
    }
    /* else other sender has lower weight and/or incarnation. */
    return -1;
}


void mamaCheckHeartbeat (mamaFtMemberImpl*  impl,
                     unsigned int hbWeight,
                     unsigned int hbIncarnation,
                     int          hbPid,
                     int          hbIpAddr,
                     int          hbPrimary,
                     char*        hbGroupName)
{

    if (strcmp (hbGroupName, impl->myGroupName)  != 0)
    {
        /* different ft group */
        mama_log (MAMA_LOG_LEVEL_FINEST, "MAMA multicast FT: received heartbeat from different group");
        return;
    }

    if ((hbPid == impl->myPid) && (hbIpAddr == impl->myIP))
    {
        /* We sent this one ourselves. */
         mama_log (MAMA_LOG_LEVEL_FINER, "MAMA multicast FT: received own heartbeat");
        return;
    }


    if (mamaBetterCredentials(impl, hbWeight,hbIncarnation,hbIpAddr,hbPid) == 0)
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                                  "MAMA NATIVE FT: received heartbeat");

        if (hbIncarnation >= impl->myNextIncarnation)
        {
            impl->myNextIncarnation = hbIncarnation + 1;
        }

        checkState (impl, MAMA_FT_STATE_STANDBY);
        resetTimeoutTimer (impl);
        foundheartbeat++;
        return;
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINER,
                "MAMA NATIVE FT: received heartbeat from an instance with lower"
                  " priority (ignored)");
        return ;
    }
}

/****************************************************************************
*       Multicast FT specific
******************************************************************************/

const char *multicastFt_getProperty(char *buffer, const char *propertyName, const char *transportName)
{
    /* Returns. */
    const char *ret = NULL;

    /* Format the full property name. */
    int numberWritten = snprintf(buffer, (FT_MAX_PROPERTY_NAME_LENGTH - 1), propertyName, transportName);

    /* If too many characters are written the log an error. */
    if((FT_MAX_PROPERTY_NAME_LENGTH - 1) < numberWritten)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, "%s Property name too long. Length [%d],"
                " Max Length [%d].", transportName, strlen(transportName),
                (FT_MAX_PROPERTY_NAME_LENGTH - 1));
    }

    else
    {
        /* Otherwise get the property. */
        ret = mama_getProperty(buffer);
    }

    return ret;
}

mama_status
multicastFt_setup (
    mamaFtMember                   member,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure)
{
    struct sockaddr_in stLocal;
    struct ip_mreq stMreq;
    int iRet = 0;
    int iTmp = 0;
    int sock = -1;
    int         udpBufSize  = 256*1024;
    long        service     = FT_SERVICE;
    int         iorecv      = 20;
    unsigned char    ttl    = FT_TTL;
    const char* ftInterface = NULL;
    const char* ftService   = NULL;
    const char* ftNetwork   = NULL;
    const char* ftTtl       = NULL;
    const char* iorecvstr   = NULL;
    const char* transportName   = NULL;
    mama_status status          = MAMA_STATUS_OK;    
    struct in_addr iface;
    struct in_addr cFtIfAddr;

    /* This buffer is used for formatting property names. */
    char propertyName[FT_MAX_PROPERTY_NAME_LENGTH] = "";

    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;

    if (!impl || !groupName)
        return MAMA_STATUS_INVALID_ARG;

    /* Get the transport name. */
    if(transport)
    {
        status = mamaTransport_getName(transport, &transportName);
        if((MAMA_STATUS_OK != status) || (NULL == transportName) || ('\0' == transportName[0]))
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "MAMA multicast FT: the transport name is invalid");
            return MAMA_STATUS_INVALID_ARG;
        }
    }
    else
        transportName = "ft";

    ftInterface = multicastFt_getProperty(propertyName,
            "mama.multicast.transport.%s.interface", transportName);
    if (ftInterface == NULL)
    {
        ftInterface = multicastFt_getProperty(propertyName,
                "mama.native.transport.%s.interface", transportName);        
        if (ftInterface == NULL)
            ftInterface = "";
    }
    ftNetwork = multicastFt_getProperty (propertyName,
            "mama.multicast.transport.%s.network", transportName);
    if (ftNetwork == NULL)
    {
        ftNetwork = multicastFt_getProperty (propertyName,
                "mama.native.transport.%s.network", transportName);
        if (ftNetwork == NULL)
            ftNetwork = FT_NETWORK;
    }

    ftService = multicastFt_getProperty (propertyName,
            "mama.multicast.transport.%s.service", transportName);
    if (ftService == NULL)
    {
        ftService = multicastFt_getProperty (propertyName,
                "mama.native.transport.%s.service", transportName);
        if (ftService != NULL)
            service = atol (ftService);
    }

    ftTtl = multicastFt_getProperty (propertyName,
            "mama.multicast.transport.%s.ttl", transportName);
    if (ftTtl == NULL)
    {
        ftTtl = multicastFt_getProperty (propertyName,
                "mama.native.transport.%s.ttl", transportName);
        if (ftTtl != NULL)
            ttl = (unsigned char)atol (ftTtl);
    }

    iorecvstr = multicastFt_getProperty (propertyName,
            "mama.multicast.transport.%s.iowindow", transportName);
    if (iorecvstr != NULL)
    {
        iorecv=atoi (iorecvstr);
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "MAMA multicast FT: INTERFACE %s NETWORK %s SERVICE %d TTL %d",
              ftInterface,ftNetwork,service,ttl);

    /* if interface name is set */
    if (ftInterface!=NULL && ftInterface[0]!='\0')
    {
        cFtIfAddr = wresolve_ip (ftInterface);

        if (cFtIfAddr.s_addr == INADDR_NONE)
        {
            mama_log (MAMA_LOG_LEVEL_WARN,"MAMA multicast FT: cannot resolve multicast interface %s",ftInterface);
            cFtIfAddr.s_addr = INADDR_ANY;
        }
    }
    else
        cFtIfAddr.s_addr = INADDR_ANY;

    impl->mySendSocket        = -1;
    impl->myQueue             = eventQueue;
    impl->myCallback          = callback;
    impl->myTransport         = transport;
    impl->myWeight            = weight;
    impl->myHeartbeatInterval = heartbeatInterval;
    impl->myTimeoutInterval   = timeoutInterval;
    impl->myGroupName         = strdup (groupName);
    impl->myInstanceId        = genInstanceId (groupName);
    impl->mySymbol            = genSymbol (groupName);
    impl->myClosure           = closure;
    impl->myState             = MAMA_FT_STATE_UNKNOWN;
    impl->mySendAddr.sin_addr.s_addr = inet_addr (ftNetwork);
    impl->mySendAddr.sin_port        = htons (service);
    impl->myRecvWindow          = iorecv;

    if (!impl->myGroupName || !impl->myInstanceId || !impl->mySymbol)
        return MAMA_STATUS_NOMEM;

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "MAMA multicast FT: Setting up member in group %s with instance ID %s",
              impl->myGroupName, impl->myInstanceId);

    /* Set up the heartbeat subscription. */
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
              "MAMA multicast FT: Starting heartbeat listener");
        memset(&stLocal, 0, sizeof(stLocal));
        memset(&stMreq, 0, sizeof(stMreq));
        /* get a datagram socket */
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock == -1)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: multicast socket failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }
        iRet = wsetnonblock(sock);
        if (iRet != 0)
        {
             mama_log (MAMA_LOG_LEVEL_ERROR,
                       "MAMA multicast FT: fcntl() failed %s",strerror(errno));
             return MAMA_STATUS_SYSTEM_ERROR;
        }
        /* avoid EADDRINUSE error on bind() */
        iTmp = 1;
        iRet = setsockopt(sock,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         (char *)&iTmp,
                         sizeof(iTmp));
        if (iRet == -1)
        {
            close (sock);
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: setsockopt() SO_REUSEADDR failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }
        /* set the receive buffer size */
        iRet = setsockopt(sock,
                          SOL_SOCKET,
                          SO_RCVBUF,
                          (char *)&udpBufSize,
                          sizeof(udpBufSize));
        if (iRet == -1)
        {
            close (sock);
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: setsockopt() SO_RCVBUG failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }

        /* name the socket */
        stLocal.sin_family      = AF_INET;
        stLocal.sin_addr.s_addr = htonl(INADDR_ANY);
        stLocal.sin_port        = htons(service);
        iRet = bind(sock, (struct sockaddr*) &stLocal, sizeof(stLocal));
        if (iRet == -1)
        {
            close (sock);
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: multicastFt: bind() failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }
        impl->myRecvSocket = sock;
        mama_log (MAMA_LOG_LEVEL_NORMAL, "MAMA multicast FT: listening ...");
    }

        /* join the multicast group */
        stMreq.imr_multiaddr.s_addr = inet_addr(ftNetwork);
        stMreq.imr_interface.s_addr = cFtIfAddr.s_addr;
        iRet = setsockopt(sock,
                          IPPROTO_IP,
                          IP_ADD_MEMBERSHIP,
                          (char *)&stMreq,
                          sizeof(stMreq));
        if (iRet == -1)
        {
            close (sock);
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: setsockopt() IP_ADD_MEMBERSHIP failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }

    /* Set up the heartbeat publisher. */
    {
        impl->mySendSocket = socket(PF_INET, SOCK_DGRAM, 0);
        memset(&(impl->mySendAddr), 0, sizeof(impl->mySendAddr));
        impl->mySendAddr.sin_family      = AF_INET;
        impl->mySendAddr.sin_addr.s_addr = inet_addr(ftNetwork);
        impl->mySendAddr.sin_port        = htons (service);

        memset(&iface, 0, sizeof(iface));
        iface.s_addr = cFtIfAddr.s_addr;
        iRet = setsockopt(impl->mySendSocket,
                             IPPROTO_IP,
                             IP_MULTICAST_IF,
                             (char *)&iface,
                             sizeof(iface));
        if (iRet == -1)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "MAMA multicast FT: setsockopt() IP_MULTICAST_IF failed %s",strerror(errno));
            return MAMA_STATUS_SYSTEM_ERROR;
        }

        if (ttl != (unsigned char)1)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,"MAMA multicast FT: resettting ttl");
            iRet = setsockopt (impl->mySendSocket,
                               IPPROTO_IP,
                               IP_MULTICAST_TTL,
                               &ttl,
                               sizeof(ttl));
            if (iRet == -1)
            {
                mama_log (MAMA_LOG_LEVEL_ERROR,
                          "MAMA multicast FT: setsockopt() IP_MULTICAST_TTL failed %s",strerror(errno));
                return MAMA_STATUS_SYSTEM_ERROR;
            }
        }

    }
    return MAMA_STATUS_OK;
}

mama_status multicastFt_activate (mamaFtMember member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->myIsActive)
    {
        mama_status status = mamaFtMember_deactivate (member);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    {
        mama_status status = mamaIo_create (&gReadHandler,
                                            impl->myQueue,
                                            impl->myRecvSocket,
                                            ioCallback,
                                            MAMA_IO_READ,
                                            impl);
        if (status != MAMA_STATUS_OK)
            return status;
    }
    return MAMA_STATUS_OK;
}

mama_status multicastFt_deactivate (mamaFtMember member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->myIsActive)
    {
        /* unsubscribe from io */
        if (gReadHandler != NULL)
            mamaIo_destroy (gReadHandler);
    }

    return MAMA_STATUS_OK;
}

static void multicastFt_sendHeartbeat (void*  member)
{
    size_t bytes = 0;
    char sendPacket [FT_MAX_PACKET_SIZE];
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;

    mama_log (MAMA_LOG_LEVEL_FINEST, "MAMA multicast FT: sending heartbeat");

    bytes = snprintf (sendPacket, FT_MAX_PACKET_SIZE, "%x\t%d\t%d\t%d\t%d\t%s",
                       htonl(impl->myIP), impl->myPid, impl->myWeight, impl->myIncarnation,
                       impl->myState, impl->myGroupName);

    if (sendto (impl->mySendSocket, sendPacket, bytes+1, 0, (struct sockaddr*)&(impl->mySendAddr), sizeof (impl->mySendAddr)) < 0)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "MAMA multicast FT: send failed");
    }
}

static void MAMACALLTYPE
ioCallback (mamaIo io, mamaIoType ioType, void *closure)
{
    int i = 0;
    mamaFtMemberImpl* impl   = (mamaFtMemberImpl*)closure;

    mama_log (MAMA_LOG_LEVEL_FINEST, "MAMA multicast FT: received data");

    for (i=0;i<impl->myRecvWindow && multicastFt_receiveHeartbeat(impl) == 0; i++);
}

int multicastFt_receiveHeartbeat(void* member)
{
    char msg [FT_MAX_PACKET_SIZE];
    int bytes = 0;
    unsigned int hbWeight;
    unsigned int hbIncarnation;
    unsigned int hbPid;
    unsigned int hbIpAddr;
    unsigned int hbPrimary;
    char         hbGroupName[100];
    mamaFtMemberImpl* impl   = (mamaFtMemberImpl*)member;

    bytes = recv (impl->myRecvSocket, msg, FT_MAX_PACKET_SIZE, 0);

    if (bytes < 0)
    {
        if (errno == EAGAIN)
            return -1;
        mama_log (MAMA_LOG_LEVEL_FINER, "MAMA multicast FT: read failed %s",strerror(errno));
        return -1;
    }

    if (bytes == 0)
    {
        mama_log (MAMA_LOG_LEVEL_FINER, "MAMA multicast FT: end of file on udp  %s",strerror(errno));
    return 0;
}

    if (sscanf(msg, "%x\t%u\t%u\t%u\t%u\t%s",
               &hbIpAddr, &hbPid, &hbWeight, &hbIncarnation, &hbPrimary,
               hbGroupName) != 6)
{
        mama_log (MAMA_LOG_LEVEL_FINEST, "error parsing incoming heartbeat");
        return 0;
}

    hbIpAddr = ntohl (hbIpAddr);

    mamaCheckHeartbeat (impl, hbWeight, hbIncarnation, hbPid, hbIpAddr,
                        hbPrimary, hbGroupName);

    return 0;
}

/****************************************************************************
*       Native (mama) FT specific
******************************************************************************/

mama_status
bridgeFt_setup (
    mamaFtMember                   member,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;

    if (!impl || !groupName || !transport)
        return MAMA_STATUS_INVALID_ARG;

    impl->myQueue             = eventQueue;
    impl->myCallback          = callback;
    impl->myTransport         = transport;
    impl->myWeight            = weight;
    impl->myHeartbeatInterval = heartbeatInterval;
    impl->myTimeoutInterval   = timeoutInterval;
    impl->myGroupName         = strdup (groupName);
    impl->myInstanceId        = genInstanceId (groupName);
    impl->mySymbol            = genSymbol (groupName);
    impl->myClosure           = closure;
    impl->myState             = MAMA_FT_STATE_UNKNOWN;
    if (!impl->myGroupName || !impl->myInstanceId || !impl->mySymbol)
        return MAMA_STATUS_NOMEM;

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "MAMA FT: Setting up member in group %s with instance ID %s",
              impl->myGroupName, impl->myInstanceId);

    /* Set up the heartbeat subscription. */
    {

        mamaMsgCallbacks ftCallbacks = { ftSubCreateCb, ftSubErrorCb, ftSubMsgCb, NULL };
        mama_status status = mamaSubscription_allocate (&impl->mySubscription);
        if (status == MAMA_STATUS_OK)
        {
            status = mamaSubscription_createBasic (impl->mySubscription,
                                                  impl->myTransport,
                                                  impl->myQueue,
                                                  &ftCallbacks,
                                                  impl->mySymbol,
                                                  impl);
        }
        if (status != MAMA_STATUS_OK)
            return status;
    }

    /* Set up the heartbeat publisher. */
    {
        mama_status status = mamaPublisher_create (&impl->myPublisher,
                                                   impl->myTransport,
                                                   impl->myGroupName,
                                                   FT_SYMBOL_PREFIX,
                                                   NULL);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    /* Set up the heartbeat message. */
    {
        mama_status status = mamaMsg_create(&impl->myHeartbeatMsg);
        if (status != MAMA_STATUS_OK)
            return status;
    }
    return MAMA_STATUS_OK;
}

mama_status
bridgeFt_activate (
    mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->myIsActive)
    {
        mama_status status = mamaFtMember_deactivate (member);
        if (status != MAMA_STATUS_OK)
            return status;
    }

    return MAMA_STATUS_OK;
}

mama_status
bridgeFt_deactivate (
    mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;
    if (!impl)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->myIsActive)
    {
        /* Unsubscribe and cancel timer. */
        mamaSubscription_deactivate (impl->mySubscription);
    }
    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE
ftSubCreateCb (mamaSubscription   subscription,
                    void*              closure)
{
    mama_log (MAMA_LOG_LEVEL_FINE,
              "MAMA FT: heartbeat subscription created successfully");
}

static void MAMACALLTYPE
ftSubErrorCb  (mamaSubscription   subscription,
                    mama_status        status,
                    void*              platformError,
                    const char*        subject,
                    void*              closure)
{
    mama_log (MAMA_LOG_LEVEL_ERROR,
              "MAMA FT: error on FT heartbeat subscription: %s",
              mamaStatus_stringForStatus(status));
}

int bridgeFt_receiveHeartbeat (void* member)
{
    /* Dummy procedure as no polling of io*/
    return -1;
}

void bridgeFt_sendHeartbeat (mamaFtMember    member)
{
    mamaFtMemberImpl* impl = (mamaFtMemberImpl*) member;

    mamaMsg_clear (impl->myHeartbeatMsg);
    mamaMsg_addU32    (impl->myHeartbeatMsg,
                       FT_FIELD_IPADDRESS,FT_FIELD_IPADDRESS_FID,
                       impl->myIP);
    mamaMsg_addU32    (impl->myHeartbeatMsg,
                       FT_FIELD_PID,FT_FIELD_PID_FID,
                       impl->myPid);
    mamaMsg_addU32    (impl->myHeartbeatMsg,
                       FT_FIELD_WEIGHT,FT_FIELD_WEIGHT_FID,
                       impl->myWeight);
    mamaMsg_addU32    (impl->myHeartbeatMsg,
                       FT_FIELD_INCARNATION,FT_FIELD_INCARNATION_FID,
                       impl->myIncarnation);
    mamaMsg_addU32    (impl->myHeartbeatMsg,
                       FT_FIELD_PRIMARY,FT_FIELD_PRIMARY_FID,
                       impl->myIncarnation);
    mamaMsg_addString (impl->myHeartbeatMsg,
                       FT_FIELD_GROUP_NAME,FT_FIELD_GROUP_NAME_FID,
                       impl->myGroupName);

    if (mama_getLogLevel() >= MAMA_LOG_LEVEL_FINEST)
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "MAMA FT: sending heartbeat message: %s",
                  mamaMsg_toString (impl->myHeartbeatMsg));

    mamaPublisher_send (impl->myPublisher, impl->myHeartbeatMsg);
}

static void MAMACALLTYPE
ftSubMsgCb    (mamaSubscription   subscription,
               mamaMsg            msg,
               void*              closure,
               void*              itemClosure)
{
    mamaFtMemberImpl* impl   = (mamaFtMemberImpl*)closure;
    unsigned int     hbWeight;
    unsigned int        hbIncarnation;
    unsigned int        hbPid;
    unsigned int        hbIpAddr;
    unsigned int        hbPrimary;
    const char *hbGroupName;

    if (mama_getLogLevel() >= MAMA_LOG_LEVEL_FINEST)
        mama_log (MAMA_LOG_LEVEL_FINEST,
              "MAMA FT: received heartbeat message: %s",
              mamaMsg_toString(msg));
    if ((MAMA_STATUS_OK != mamaMsg_getString (msg,
                                              FT_FIELD_GROUP_NAME,
                                              FT_FIELD_GROUP_NAME_FID,
                                              &hbGroupName)) ||
        (MAMA_STATUS_OK != mamaMsg_getU32    (msg,
                                              FT_FIELD_PID,
                                              FT_FIELD_PID_FID,
                                              &hbPid)) ||
        (MAMA_STATUS_OK != mamaMsg_getU32    (msg,
                                              FT_FIELD_WEIGHT,
                                              FT_FIELD_WEIGHT_FID,
                                              &hbWeight)) ||
        (MAMA_STATUS_OK != mamaMsg_getU32    (msg,
                                              FT_FIELD_INCARNATION,
                                              FT_FIELD_INCARNATION_FID,
                                              &hbIncarnation)) ||
        (MAMA_STATUS_OK != mamaMsg_getU32    (msg,
                                              FT_FIELD_PRIMARY,
                                              FT_FIELD_PRIMARY_FID,
                                              &hbPrimary)) ||
        (MAMA_STATUS_OK != mamaMsg_getU32    (msg,
                                              FT_FIELD_IPADDRESS,
                                              FT_FIELD_IPADDRESS_FID,
                                              &hbIpAddr)))
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                "MAMA FT: error extracting fields from heartbeat message: %s",
                mamaMsg_toString(msg));
        return;
    }

    mamaCheckHeartbeat (impl,
                    hbWeight,
                    hbIncarnation,
                    hbPid,
                    hbIpAddr,
                    hbPrimary,
                    (char*)hbGroupName);
}
