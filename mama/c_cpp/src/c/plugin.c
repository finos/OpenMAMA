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
#include <limits.h>

#include "wombat/port.h"
#include "wombat/environment.h"
#include "wombat/strutils.h"
#include <wombat/wtable.h>

#include <mama/mama.h>
#include <mama/error.h>
#include <mamainternal.h>
#include <mama/version.h>
#include <property.h>
#include <plugin.h>
#include <platform.h>
#include <dqstrategy.h>
#include <subscriptionimpl.h>
#include <mama/transport.h>
#include "transportimpl.h"


#define PLUGIN_PROPERTY "mama.plugin.name_"
#define PLUGIN_NAME "mamaplugin"

#define INITIAL_PLUGIN_ARRAY_SIZE 1
#define MAX_PLUGIN_STRING 1024

#define MAX_FUNC_STRING 256

int gNumPlugins        = 0;
int gCurrentPluginSize = INITIAL_PLUGIN_ARRAY_SIZE;
/**
 * @brief Mechanism for registering required plugin functions.
 *
 * Taking a function string name search the shared library handle, using the
 * loadLibFunc portability method, for the function. If it is found, set the
 * appropriate function pointer in the plugin struct to the result. If not,
 * log as an error the fact that the funciton cannot be found, and return
 * MAMA_STATUS_PLATFORM
 *
 * @param FUNCSTRINGNAME The string function name.
 * @param FUNCIMPLNAME The name of the function pointer in the plugin struct
 * @param FUNCIMPLTYPE The type of the function pointer expected.
 */
#define REGISTER_PLUGIN_FUNCTION(FUNCSTRINGNAME, FUNCIMPLNAME, FUNCIMPLTYPE)    \
do {                                                                            \
    snprintf (functionName, MAX_FUNC_STRING, "%s"#FUNCSTRINGNAME, name);        \
    result = loadLibFunc (pluginLib, functionName);                             \
                                                                                \
    if (NULL != result) {                                                       \
        (pluginImpl)->FUNCIMPLNAME = *(FUNCIMPLTYPE*)&result;                   \
        result = NULL;                                                          \
    } else {                                                                    \
        mama_log (MAMA_LOG_LEVEL_ERROR,                                         \
                  "mamaPlugin_registerFunctions(): "                            \
                  "Cannot load plugin, does not implement required function: [%s]",\
                  functionName);                                                \
        status = MAMA_STATUS_PLATFORM;                                          \
        return status;                                                          \
    }                                                                           \
} while (0)

/**
 * @brief Mechanism for registering required plugin functions.
 *
 * Taking a function string name search the shared library handle, using the
 * loadLibFunc portability method, for the function. If it is found, set the
 * appropriate function pointer in the plugin struct to the result. If not
 * log the fact that the function has not been found, and continue.
 *
 * @param FUNCSTRINGNAME The string function name.
 * @param FUNCIMPLNAME The name of the function pointer in the plugin struct
 * @param FUNCIMPLTYPE The type of the function pointer expected.
 */
#define REGISTER_OPTIONAL_PLUGIN_FUNCTION(FUNCSTRINGNAME,                       \
                                          FUNCIMPLNAME,                         \
                                          FUNCIMPLTYPE)                         \
do {                                                                            \
    snprintf (functionName, MAX_FUNC_STRING, "%s"#FUNCSTRINGNAME, name);        \
    result = loadLibFunc (pluginLib, functionName);                             \
                                                                                \
    if (NULL != result) {                                                       \
        (pluginImpl)->FUNCIMPLNAME = *(FUNCIMPLTYPE*)&result;                   \
        result = NULL;                                                          \
    } else {                                                                    \
        mama_log (MAMA_LOG_LEVEL_FINE,                                          \
                  "mamaPlugin_registerFunctions(): "                            \
                  "Optional plugin function [%s] not found. Unavailable.",      \
                  functionName);                                                \
    }                                                                           \
} while (0)

static mamaPluginImpl**      gPlugins;
static volatile int         gPluginNo = 0;

/**
 * @brief Used to load register all possible plugin functions to be
 *        used within Mama.
 *
 * param[in] pluginLib
 * param[in] name
 * param[in] pluginImpl The plugin impl to be used
 *
 * @return mama_status return code can be one of:
 *          MAMA_STATUS_OK
 */
mama_status
mamaPlugin_registerFunctions   (LIB_HANDLE      pluginLib,
                                const char*     name,
                                mamaPluginInfo  pluginInfo,
                                mamaPluginImpl* pluginImpl);

/**
*   Reallocate space for plugins when current limit is reached
*/
static void
pluginPropertiesCb(const char* name, const char* value, void* closure);

/**
 * Register function pointers associated with a specific plugin.
 */
mama_status
mamaPlugin_registerFunctions (LIB_HANDLE      pluginLib,
                              const char*     name,
                              mamaPluginInfo  pluginInfo,
                              mamaPluginImpl* pluginImpl)
{
    mama_status  status        = MAMA_STATUS_OK;
    void*        result        = NULL;
    char         functionName[MAX_FUNC_STRING];

    /* Save off some informatin on the plugin */
    pluginImpl->mPluginHandle = pluginLib;
    pluginImpl->mPluginName = strdup(name);
    pluginImpl->mPluginInfo = pluginInfo;


    /* Required fuctions */
    REGISTER_PLUGIN_FUNCTION (MamaPlugin_initHook, mamaPluginInitHook,
                              mamaPlugin_initHook);
    REGISTER_PLUGIN_FUNCTION (MamaPlugin_shutdownHook, mamaPluginShutdownHook,
                              mamaPlugin_shutdownHook);


    /* Optional fuctions */
    REGISTER_OPTIONAL_PLUGIN_FUNCTION (MamaPlugin_publisherPreSendHook, mamaPluginPublisherPreSendHook,
                                       mamaPlugin_publisherPreSendHook);
    REGISTER_OPTIONAL_PLUGIN_FUNCTION (MamaPlugin_transportPostCreateHook, mamaPluginTransportPostCreateHook,
                                       mamaPlugin_transportPostCreateHook);
    REGISTER_OPTIONAL_PLUGIN_FUNCTION (MamaPlugin_subscriptionPostCreateHook, mamaPluginSubscriptionPostCreateHook,
                                       mamaPlugin_subscriptionPostCreateHook);
    REGISTER_OPTIONAL_PLUGIN_FUNCTION (MamaPlugin_subscriptionPreMsgHook, mamaPluginSubscriptionPreMsgHook,
                                       mamaPlugin_subscriptionPreMsgHook);
    REGISTER_OPTIONAL_PLUGIN_FUNCTION (MamaPlugin_transportEventHook, mamaPluginTransportEventHook,
                                       mamaPlugin_transportEventHook);
   return status;
}

mama_status
mama_initPlugins(void)
{
    int             pluginCount      = 0;
    const char*     prop             = NULL;
    char            propString[MAX_PLUGIN_STRING];
    const char*     propVal;

    if(!gPlugins)
    {
        gPlugins = calloc (INITIAL_PLUGIN_ARRAY_SIZE, sizeof(mamaPluginImpl*));
    }

    properties_ForEach (mamaInternal_getProperties(), pluginPropertiesCb, NULL);

#ifdef WITH_ENTERPRISE
    mama_log (MAMA_LOG_LEVEL_FINE, "mama_initPlugins(): Initialising mamacenterprise");
    mama_loadPlugin ("mamacenterprise");
#endif /* WITH_ENTERPRISE */
    
    mama_loadPlugin("dqstrategy");

    return MAMA_STATUS_OK;
}

mama_status
mama_loadPlugin (const char* pluginName)
{
    LIB_HANDLE              pluginLib       = NULL;
    mamaPluginImpl*         pluginImpl      = NULL;
    mama_status             status          = MAMA_STATUS_OK;
    mamaPluginInfo          pluginInfo      = NULL;
    char                    loadPluginName  [MAX_PLUGIN_STRING];
    mamaPluginImpl*         aPluginImpl     = NULL;

    if (!pluginName)
        return MAMA_STATUS_NULL_ARG;

    pluginImpl = mamaPlugin_findPlugin(pluginName);

    /*
     * Check to see if pluginImpl has already been loaded
     */
    if (pluginImpl == NULL)
    {
       /* The plugin name should be of the format mamaplugin<name> */
#ifdef WITH_ENTERPRISE
        if (strncmp(pluginName, "mamacenterprise", MAX_PLUGIN_STRING) == 0)
            snprintf(loadPluginName, MAX_PLUGIN_STRING,
                "%s",
                pluginName);
        else
#endif /* WITH_ENTERPRISE */
            snprintf(loadPluginName, MAX_PLUGIN_STRING,
                "%s%s",
                PLUGIN_NAME,
                pluginName);

        pluginLib = openSharedLib (loadPluginName, NULL);

        if (!pluginLib)
        {

           mama_log (MAMA_LOG_LEVEL_ERROR,
                    "mama_loadPlugin(): "
                    "Could not open plugin library [%s] [%s]",
                    pluginName,
                    getLibError());
            return MAMA_STATUS_PLATFORM;
        }

        /* Create structure to hold plugin information */
        aPluginImpl = (mamaPluginImpl*)calloc (1, sizeof(mamaPluginImpl));

        status = mamaPlugin_registerFunctions (pluginLib,
                                               pluginName,
                                               pluginInfo,
                                               aPluginImpl);

        if (MAMA_STATUS_OK == status)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                     "mama_loadPlugin(): "
                     "Sucessfully registered plugin functions for [%s]",
                     pluginName);

        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                     "mama_loadPlugin(): "
                     "Failed to register plugin functions for [%s]",
                     pluginName);

            closeSharedLib (aPluginImpl->mPluginHandle);

            free ((char*)aPluginImpl->mPluginName);
            free ((mamaPluginImpl*)aPluginImpl);

            return status;
        }

        /* Invoke the init function */
        status = aPluginImpl->mamaPluginInitHook (&aPluginImpl->mPluginInfo);

        if (MAMA_STATUS_OK == status)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "mama_loadPlugin(): Successfully run the init hook for mama plugin [%s]",
                       aPluginImpl->mPluginName);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_WARN,
                      "mama_loadPlugin(): Init hook failed for mama plugin [%s]",
                       aPluginImpl->mPluginName);

            closeSharedLib (aPluginImpl->mPluginHandle);

            free ((char*)aPluginImpl->mPluginName);
            free ((mamaPluginImpl*)aPluginImpl);

            return status;
        }

        /* Save off the plugin impl and increment the plugin counter */
        gPlugins[gPluginNo] = aPluginImpl;
        gPluginNo++;

    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                 "mama_loadPlugin(): "
                 "Plugin [%s] has already been loaded and initialised",
                 pluginName);
    }

    return MAMA_STATUS_OK;
}

mama_status
mama_shutdownPlugins (void)
{
    mama_status status  = MAMA_STATUS_OK;
    int         plugin  = 0;
    int         ret     = 0;
    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mPluginHandle != NULL)
            {
                /* Fire the user shutdown hook first - if one is present */
                if (gPlugins[plugin]->mamaPluginShutdownHook != NULL)
                {
                    gPlugins[plugin]->mamaPluginShutdownHook (gPlugins[plugin]->mPluginInfo);
                }

                ret = closeSharedLib (gPlugins[plugin]->mPluginHandle);
                if (0!=ret)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mama_shutdownPlugins(): Failed to close Mama Plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
                else
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mama_shutdownPlugins(): Mama Plugin [%s] successfully closed",
                                gPlugins[plugin]->mPluginName);
                }

                free ((char*)gPlugins[plugin]->mPluginName);
                free (gPlugins[plugin]);
                gPlugins[plugin] = NULL;
            }
        }
    }

    free(gPlugins);
    gPlugins  = NULL;
    gPluginNo = 0;

    return status;
}

mama_status
mamaPlugin_firePublisherPreSendHook (mamaPublisher publisher, mamaMsg message)
{
    mama_status status  = MAMA_STATUS_OK;
    int         plugin  = 0;
    int         ret     = 0;
    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginPublisherPreSendHook != NULL)
            {
                status = gPlugins[plugin]->mamaPluginPublisherPreSendHook (gPlugins[plugin]->mPluginInfo, publisher, message);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_firePublisherPreSendHook(): Publisher pre send hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }
    return status;
}

mama_status
mamaPlugin_fireTransportPostCreateHook (mamaTransport transport)
{
    mama_status      status           = MAMA_STATUS_OK;
    int              plugin           = 0;
    int              tPluginNo        = 0;

    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginTransportPostCreateHook != NULL)
            {
                status = gPlugins[plugin]->mamaPluginTransportPostCreateHook (gPlugins[plugin]->mPluginInfo, transport);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_fireTransportPostCreateHook(): Transport post create hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }

    return status;
}

mama_status
mamaPlugin_fireShutdownHook (void)
{
    mama_status  status = MAMA_STATUS_OK;
    int          plugin = 0;

    for (plugin = 0; plugin <= gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginShutdownHook != NULL)
            {
                status = gPlugins[plugin]->mamaPluginShutdownHook (gPlugins[plugin]->mPluginInfo);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_fireShutdownHook(): Shutdown hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }
    return status;
}

mama_status
mamaPlugin_fireSubscriptionPostCreateHook (mamaSubscription subscription)
{ 
    mama_status      status           = MAMA_STATUS_OK;
    int              plugin           = 0;
    int              tPluginNo        = 0;
    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginSubscriptionPostCreateHook != NULL)
            {
                status = gPlugins[plugin]->mamaPluginSubscriptionPostCreateHook (gPlugins[plugin]->mPluginInfo, subscription);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_subscriptionPostCreateHook(): Subscription post create hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }

    return status;
}

mama_status
mamaPlugin_fireSubscriptionPreMsgHook (mamaSubscription subscription, int msgType, mamaMsg msg)
{
    mama_status status  = MAMA_STATUS_OK;
    int         plugin  = 0;
    int         ret     = 0;
    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginSubscriptionPreMsgHook != NULL)
            {
                status = gPlugins[plugin]->mamaPluginSubscriptionPreMsgHook (gPlugins[plugin]->mPluginInfo, subscription, msgType, msg);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_subscriptionPreMsgHook      (): Subscription pre msg hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }
    
    return status;
}

mama_status
mamaPlugin_fireTransportEventHook(mamaTransport transport, int setStale, mamaTransportEvent tportEvent)
{
    mama_status status  = MAMA_STATUS_OK;
    int         plugin  = 0;
    int         ret     = 0;
    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin] != NULL)
        {
            if (gPlugins[plugin]->mamaPluginTransportEventHook       != NULL)
            {
                status = gPlugins[plugin]->mamaPluginTransportEventHook(gPlugins[plugin]->mPluginInfo, transport, setStale, tportEvent);

                if (MAMA_STATUS_OK != status)
                {
                     mama_log (MAMA_LOG_LEVEL_WARN,
                                "mamaPlugin_transportEventHook      (): Transport event hook failed for mama plugin [%s]",
                                gPlugins[plugin]->mPluginName);
                }
            }
        }
    }

    return status;
}

mamaPluginImpl*
mamaPlugin_findPlugin (const char* name)
{
    int plugin = 0;

    for (plugin = 0; plugin < gPluginNo; plugin++)
    {
        if (gPlugins[plugin])
        {
            if ((strncmp(gPlugins[plugin]->mPluginName, name, MAX_PLUGIN_STRING) == 0))
            {
                return gPlugins[plugin];
            }
        }
    }
    return NULL;
}

static void pluginPropertiesCb(const char* name, const char* value, void* closure)
{
    if(name)
    {
        if(strstr(name, PLUGIN_PROPERTY) != NULL)
        {
            gNumPlugins++;
            if(gNumPlugins > gCurrentPluginSize)
            {
                gCurrentPluginSize *= 2;
                gPlugins = (mamaPluginImpl**) realloc (gPlugins,  sizeof(mamaPluginImpl*) * gCurrentPluginSize);
            }

            mama_log (MAMA_LOG_LEVEL_FINE, "mama_initPlugins(): Initialising %s", value);
            mama_loadPlugin (value);

        }
    }
}
