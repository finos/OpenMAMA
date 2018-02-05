/*
 * mamaPluginTemplate.c
 *
 *  Created on: 23 Mar 2015
 */

#include <stdio.h>
#include "mama/mama.h"
#include "mama/status.h"
#include "mama/types.h"
#include "mamaPluginTemplate.h"


mama_status templateMamaPlugin_initHook (mamaPluginInfo* pluginInfo)
{
    mama_status status = MAMA_STATUS_OK;
    mama_log(MAMA_LOG_LEVEL_FINEST,"templateMamaPlugin_initHook fired!");

    return status;
}

mama_status
templateMamaPlugin_transportPostCreateHook (mamaPluginInfo pluginInfo, mamaTransport transport)
{
    mama_status status = MAMA_STATUS_OK;
    mama_log(MAMA_LOG_LEVEL_FINEST,"templateMamaPlugin_transportPostCreateHook fired");

    return status;
}

mama_status
templateMamaPlugin_publisherPreSendHook (mamaPluginInfo pluginInfo, mamaPublisher publisher, mamaMsg message)
{
    mama_status status = MAMA_STATUS_OK;
    mama_log(MAMA_LOG_LEVEL_FINEST,"templateMamaPlugin_publisherPreSendHook fired!");

    return status;
}

mama_status
templateMamaPlugin_shutdownHook (mamaPluginInfo pluginInfo)
{
    mama_status status = MAMA_STATUS_OK;
    mama_log(MAMA_LOG_LEVEL_FINEST,"templateMamaPlugin_shutdownHook fired!");

    return status;
}
