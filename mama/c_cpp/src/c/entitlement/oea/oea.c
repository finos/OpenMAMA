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

#include <string.h>
#include <stdio.h>
/* OEA  */
#include <OeaClient.h>
#include <OeaStatus.h>
#include <OeaSubscription.h>
/* mama */
#include "mama/mama.h"
#include "mama/entitlement.h"
#include "subscriptionimpl.h"
#include "oea.h"
#include "subscriptionimpl.h"
#include "mamainternal.h"

const char **
oeaEntitlmentBridge_parseServersProperty(void);


 const char*     gServers[OEA_MAX_ENTITLEMENT_SERVERS];

#if (OEA_MAJVERSION == 2 && OEA_MINVERSION >= 11) || OEA_MAJVERSION > 2

void MAMACALLTYPE entitlementDisconnectCallback (oeaClient*     client,
                                    const OEA_DISCONNECT_REASON reason,
                                    const char * const          userId,
                                    const char * const          host,
                                    const char * const          appName);
void MAMACALLTYPE entitlementUpdatedCallback (oeaClient* client,
                                 int openSubscriptionForbidden);
void MAMACALLTYPE entitlementCheckingSwitchCallback (oeaClient* client,
                                        int isEntitlementsCheckingDisabled);

#else

void entitlementDisconnectCallback (oeaClient* client,
                                    const OEA_DISCONNECT_REASON reason,
                                    const char * const          userId,
                                    const char * const          host,
                                    const char * const          appName);
void entitlementUpdatedCallback (oeaClient*,
                                 int openSubscriptionForbidden);
void entitlementCheckingSwitchCallback (oeaClient*,
                                        int isEntitlementsCheckingDisabled);

#endif


mama_status
oeaEntitlementBridge_registerSubjectContext(SubjectContext* ctx)
{
    mama_log(MAMA_LOG_LEVEL_ERROR, "oeaEntitlementBridge_registerSubjectContext():");

    oeaSubscription*    oeaSub = (oeaSubscription*) ctx->mEntitlementSubscription->mImpl;

    oeaSubscription_addEntitlementCode (oeaSub, ctx->mEntitleCode);
    oeaSubscription_open (oeaSub);
    int result = oeaSubscription_isOpen (oeaSub);

    if (0 == result)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR,
                 "Could not handle entitlements for new subscription [%s].",
                 ctx->mSymbol);
        return MAMA_STATUS_NOT_ENTITLED;
    }

    return MAMA_STATUS_OK;
}


mama_status
oeaEntitlementBridge_destroy(mamaEntitlementBridge bridge)
{
    oeaEntitlementBridge* entBridge = (oeaEntitlementBridge*) bridge->mImpl;
    if (entBridge->mOeaClient)
    {
        oeaClient_destroy (entBridge->mOeaClient);
    }
    free(entBridge);
    return MAMA_STATUS_OK;
}


mama_status
oeaEntitlementBridge_init(entitlementBridge* bridge)
{
    const char*     portLowStr                  = NULL;
    const char*     portHighStr                 = NULL;
    int             portLow                     = 8000;
    int             portHigh                    = 8001;
    int             size                        = 0;
    int             entitlementsRequired        = 0; /*boolean*/
    mamaMiddleware  middleware                  = 0;
    oeaStatus       entitlementStatus           = OEA_STATUS_INVALID;
    oeaCallbacks    entitlementCallbacks;
    const char*     altUserId;
    const char*     altIp;
    const char*     site;
    const char**    entitlementServers;

    oeaEntitlementBridge* oeaBridge = calloc(1, sizeof(oeaEntitlementBridge));
    const char*     appName;
    mama_status     status;

    status = mama_getApplicationClassName (&appName);
    if (MAMA_STATUS_OK != status )
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, "Could not get application name.");
        return status;
    }

    oeaClient* oClient = malloc(sizeof(oeaEntitlementBridge));
    if (NULL == oClient)
    {
        mama_log(MAMA_LOG_LEVEL_SEVERE,
                 "Could not allocate memory for oea entitlements bridge.");
        return MAMA_STATUS_NOMEM;
    }

    if (NULL == (entitlementServers = oeaEntitlmentBridge_parseServersProperty()))
    {
        return MAMA_ENTITLE_NO_SERVERS_SPECIFIED;
    }

    while (entitlementServers[size] != NULL)
    {
        size = size + 1;
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "oeaEntitlementBridge_init: Attempting to connect to entitlement server");

    portLowStr  = mama_getProperty (OEA_PORTLOW_PROPERTY);
    portHighStr = mama_getProperty (OEA_PORTHIGH_PROPERTY);

    /* mama_getProperty returns NULL if property does not exist, in which case
     * we just use defaults 
     */
    if (portLowStr != NULL)
    {
        portLow  = (int)atof(portLowStr);
    }

    if (portHighStr != NULL)
    {
        portHigh = (int)atof(portHighStr);
    }

    altUserId = mama_getProperty (OEA_ALTUSERID_PROPERTY);
    site      = mama_getProperty (OEA_SITE_PROPERTY);
    altIp     = mama_getProperty (OEA_IP_ADDR_PROPERTY);

    entitlementCallbacks.onDisconnect                 = entitlementDisconnectCallback;
    entitlementCallbacks.onEntitlementsUpdated        = entitlementUpdatedCallback;
    entitlementCallbacks.onSwitchEntitlementsChecking = entitlementCheckingSwitchCallback;

    
    oClient = oeaClient_create(&entitlementStatus,
                                site,
                                portLow,
                                portHigh,
                                entitlementServers,
                                size);

    if (entitlementStatus != OEA_STATUS_OK)
    {
        mama_log(MAMA_LOG_LEVEL_ERROR, 
                 "oeaEntitlementBridge_init: Error creating oea client[%s].",
                 mamaStatus_stringForStatus((mama_status) entitlementStatus));
        return entitlementStatus;
    }

    if (oClient != 0)
    {
        if (OEA_STATUS_OK != (entitlementStatus = oeaClient_setCallbacks (oClient, &entitlementCallbacks)))
        {
            return entitlementStatus;
        }

        if (OEA_STATUS_OK != (entitlementStatus = oeaClient_setAlternativeUserId (oClient, altUserId)))
        {
            return entitlementStatus;
        }

        if (OEA_STATUS_OK != (entitlementStatus = oeaClient_setEffectiveIpAddress (oClient, altIp)))
        {
            return entitlementStatus;
        }

        if (OEA_STATUS_OK != (entitlementStatus = oeaClient_setApplicationId (oClient, appName)))
        {
            return entitlementStatus;
        }

        if (OEA_STATUS_OK != (entitlementStatus = oeaClient_downloadEntitlements ((oeaClient*const)oClient)))
        {
            return entitlementStatus;
        }
    }

    /* set client in oeaEntitlementBridge struct */
    oeaBridge->mOeaClient= oClient;

    /* set mamaEntitlemententitlement bridge pointer to bridge implementation struct */
    *bridge = oeaBridge;


    return MAMA_STATUS_OK;
}


const char **
oeaEntitlmentBridge_parseServersProperty()
{
    char *ptr;
    int idx = 0;
    const char*     serverProperty = mama_getProperty(OEA_SERVER_PROPERTY);


    memset (gServers, 0, sizeof(gServers));

    if (NULL == serverProperty)
    {
        mama_log( MAMA_LOG_LEVEL_WARN,
                  "Failed to open properties file "
                  "or no entitlement.servers property." );
        return NULL;
    }

    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "entitlement.servers=%s",
              serverProperty == NULL ? "NULL" : serverProperty);

    while( idx < OEA_MAX_ENTITLEMENT_SERVERS - 1 )
    {
        gServers[idx] = strtok_r (idx == 0 ? (char *)serverProperty : NULL
                                  , ",",
                                  &ptr);


        if (gServers[idx++] == NULL) /* last server parsed */
        {
            break;
        }

        if (gMamaLogLevel)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "Parsed entitlement server: %s",
                      gServers[idx-1]);
        }
    }
    return gServers;
}


mama_status
oeaEntitlementBridge_createSubscription(mamaEntitlementBridge mamaEntBridge, SubjectContext* ctx)
{
    oeaEntitlementBridge*  oeaBridge = (oeaEntitlementBridge*) mamaEntBridge->mImpl;

    oeaEntitlementSubscriptionHandle* oeaSubHandle = calloc(1,sizeof(oeaEntitlementSubscriptionHandle));

    oeaStatus status;
    oeaSubHandle->mOeaSubscription = oeaClient_newSubscription(&status, oeaBridge->mOeaClient);
    if (OEA_STATUS_OK != status)
    {
        return MAMA_STATUS_NOT_ENTITLED;
    }

    ctx->mEntitlementBridge = mamaEntBridge;

    /* Allocate mama_level entitlement subscription object and set implementation struct pointer. */
    mamaEntitlementSubscription mamaEntSub;
    mamaEntitlementBridge_createSubscription(&mamaEntSub);

    mamaEntSub->mImpl              = oeaSubHandle;
    mamaEntSub->mEntitlementBridge = mamaEntBridge;

    /* Add mama level struct to subscription SubjectContext. */
    ctx->mEntitlementSubscription = mamaEntSub;
    ctx->mEntitlementBridge       = mamaEntBridge;

    return MAMA_STATUS_OK;
}

mama_status
oeaEntitlementBridge_destroySubscription(entitlementSubscriptionHandle* handle)
{
    oeaEntitlementSubscriptionHandle* oeaSubHandle = (oeaEntitlementSubscriptionHandle*) handle;
    if (NULL != oeaSubHandle->mOeaSubscription)
    {
        oeaSubscription_destroy(oeaSubHandle->mOeaSubscription);
    }
    return MAMA_STATUS_OK;
}

mama_status
oeaEntitlementBridge_setIsSnapshot(entitlementSubscriptionHandle* handle, int isSnapshot)
{
    oeaEntitlementSubscriptionHandle* oeaSubHandle = (oeaEntitlementSubscriptionHandle*) handle;
    oeaSubscription_setIsSnapshot(oeaSubHandle->mOeaSubscription, isSnapshot);
}

int
oeaEntitlementBridge_isAllowed(entitlementSubscriptionHandle* handle, char* subject)
{
    oeaEntitlementSubscriptionHandle* oeaSubHandle = (oeaEntitlementSubscriptionHandle*) handle;
    oeaSubscription_setSubject (oeaSubHandle->mOeaSubscription, subject);
    return oeaSubscription_isAllowed (oeaSubHandle->mOeaSubscription); 

}

void
entitlementDisconnectCallback (oeaClient*                   client,
                               const OEA_DISCONNECT_REASON  reason,
                               const char * const           userId,
                               const char * const           host,
                               const char * const           appName)
{
    mamaImpl_entitlementDisconnectCallback(reason, userId, host, appName);
}

void
entitlementUpdatedCallback (oeaClient* client,
                            int openSubscriptionForbidden)
{
    mamaImpl_entitlementUpdatedCallback();
}

void
entitlementCheckingSwitchCallback (oeaClient*   client,
                                   int isEntitlementsCheckingDisabled)
{
    mamaImpl_entitlementCheckingSwitchCallback(isEntitlementsCheckingDisabled);
}




