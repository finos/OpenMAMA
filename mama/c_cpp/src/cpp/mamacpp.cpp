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

#include <stdint.h>
#include <mama/mamacpp.h>
#include <bridge.h>
#include <mama/timer.h>
#include <mamacppinternal.h>
#include <mama/reservedfields.h>
#include <mama/msg.h>
#include <mama/inbox.h>
#include <string>
#include <vector>
#include "MamaSubscriptionImpl.h"
#include <mama/queue.h>
#include <mama/MamaQueue.h>
#include <mama/MamaReservedFields.h>
#include <map>

namespace Wombat
{
    /******************************************************************************
     * Mama Implementation
     */

    /**
    * vector of MamaQueue pointers to be tidied up at Mama::close()
    */
    static std::vector<MamaQueue*> gDefaultQueueWrappers;

    const char* Mama::getVersion (mamaBridge bridgeImpl)
    {
        return mama_getVersion (bridgeImpl);
    }

    mamaBridge Mama::loadBridge (const char* middleware)
    {
        mamaBridge bridge = NULL;
        mamaTry (mama_loadBridge (&bridge, middleware));
        return (bridge);
    }

    mamaBridge Mama::loadBridge (const char* middleware, const char* path)
    {
        mamaBridge bridge = NULL;
        mamaTry (mama_loadBridgeWithPath (&bridge, middleware, path));
        return (bridge);
    }

    mamaPayloadBridge Mama::loadPayloadBridge (const char* payload)
    {
        mamaPayloadBridge payloadBridge = NULL;
        mamaTry (mama_loadPayloadBridge (&payloadBridge, payload));
        return (payloadBridge);
    }

    /** Return a middleware bridge which matches the middleware string.
     * 
     * @param middleware The middleware to be returned
     *
     * @return mamaBridge The middleware bridge to be returned. Returns NULL
     * if none available. 
     */
    mamaBridge Mama::getMiddlewareBridge (const char* middleware)
    {
        mamaBridge bridge = NULL;

        if (NULL == middleware)
        {
            return bridge;
        }

        /* We could check the status here, but we know the bridge and
         * middleware can't be NULL. Clients can infer that a bridge isn't
         * available from a NULL return.
         */
        mama_getMiddlewareBridge (&bridge, middleware);
        
        return bridge;
    }

    /** Return a payload bridge which matches the payload string.
     * 
     * @param payload The payload to be returned
     *
     * @return mamaBridge The payload bridge to be returned. Returns NULL
     * if none available. 
     */
    mamaPayloadBridge Mama::getPayloadBridge (const char* payload)
    {
        mamaPayloadBridge payloadBridge = NULL;

        if (NULL == payload)
        {
            return payloadBridge;
        }

        /* We could check the status here, but we know the bridge and
         * payload can't be NULL. Clients can infer that a bridge isn't
         * available from a NULL return.
         */
        mama_getPayloadBridge (&payloadBridge, payload);

        return payloadBridge;
    }


    void Mama::open ()
    {
        openCount (NULL, NULL);
    }

    unsigned int Mama::openCount ()
    {
        return openCount (NULL, NULL);
    }

    void Mama::open (const char* path,
                     const char* filename)
    {
        openCount (path, filename);
    }

    unsigned int Mama::openCount (const char* path,
                                  const char* filename)
    {
        unsigned int refCount = 0;

        // Open MAMA
        mamaTry (mama_openWithPropertiesCount (path, filename, &refCount));
        
        if (1 == refCount)
        {
            MamaReservedFields::initReservedFields ();
        }

        return refCount;
    }

    static MamaEntitlementCallback* gMamaEntitlementCallback = NULL;

    extern "C"
    {
        void MAMACALLTYPE entitlementDisconnectCB (const sessionDisconnectReason reason,
                                      const char* userId,
                                      const char* host,
                                      const char* appName)
        {
            if (gMamaEntitlementCallback != NULL)
            {
                gMamaEntitlementCallback->onSessionDisconnect (reason,
                                                               userId,
                                                               host,
                                                               appName);
            }
        }

        void MAMACALLTYPE entitlementUpdateCB ()
        {
            if (gMamaEntitlementCallback != NULL)
            {
                gMamaEntitlementCallback->onEntitlementUpdate ();
            }
        }

        void MAMACALLTYPE entitlementCheckingSwitchCB (const int isEntitlementCheckingDisabled)
        {
            if (gMamaEntitlementCallback != NULL)
            {
                gMamaEntitlementCallback->onEntitlementCheckingSwitch (isEntitlementCheckingDisabled);
            }
        }
    }//end extern "C"

    void Mama::registerEntitlementCallbacks (MamaEntitlementCallback* callback)
    {
        gMamaEntitlementCallback = callback;
        static mamaEntitlementCallbacks entitlementCallback =
            {
                entitlementDisconnectCB,
                entitlementUpdateCB,
                entitlementCheckingSwitchCB
            };
        mamaTry (mama_registerEntitlementCallbacks (&entitlementCallback));
    }

    extern "C"
    {
        void MAMACALLTYPE bridgeMessageCCB (mamaBridge bridgeImpl, const char* message)
        {
            mamaBridgeImpl* impl = (mamaBridgeImpl*)bridgeImpl;
            MamaBridgeCallback* callback = static_cast<MamaBridgeCallback*>(impl->mCppCallback);

            callback->onBridgeInfo (bridgeImpl, message);
        }
    } //end extern "C"

    void Mama::setBridgeCallback (mamaBridge bridgeImpl, MamaBridgeCallback* callback)
    {
        mamaBridgeImpl_setCppCallback (bridgeImpl, (void*)callback);

        mamaTry (mama_setBridgeInfoCallback (bridgeImpl, bridgeMessageCCB));
    }

    void Mama::setProperty (const char* name, const char* value)
    {
        mamaTry (mama_setProperty (name, value));
    }

    const char * Mama::getProperty (const char* name)
    {
        return mama_getProperty (name);
    }

    void Mama::close ()
    {
        closeCount ();
    }

    unsigned int Mama::closeCount ()
    {
        unsigned int refCount = 0;

        // Close mama
        mamaTry (mama_closeCount (&refCount));

        if (0 == refCount)
        {
            MamaReservedFields::uninitReservedFields();
            for (std::vector<MamaQueue*>::iterator iter = gDefaultQueueWrappers.begin(); iter != gDefaultQueueWrappers.end(); ++iter)
            {
                MamaQueue* defaultQueue = *iter;
                defaultQueue->setCValue(NULL);  // middleware  bridge's respobsibility to clean up the c-queue
                delete(defaultQueue);           //delete CPP wrapper
            }
            gDefaultQueueWrappers.clear();
        }

        return refCount;
    }

    void Mama::start (mamaBridge bridgeImpl)
    {
        mamaTry (mama_start (bridgeImpl));
    }

    extern "C"
    {
        void MAMACALLTYPE stopCb (mama_status status, mamaBridge, void* closure)
        {
            static_cast<MamaStartCallback*>(closure)->onStartComplete(MamaStatus(status));
        }
    }

    void Mama::startBackground (mamaBridge bridgeImpl,
                                MamaStartCallback* cb)
    {
        mamaTry (mama_startBackgroundEx (bridgeImpl, mamaStopCBEx(stopCb), static_cast<void*>(cb)));
    }

    void Mama::stop (mamaBridge bridgeImpl)
    {
        mamaTry (mama_stop (bridgeImpl));
    }

    void Mama::stopAll (void)
    {
        mamaTry (mama_stopAll ());
    }

    static MamaLogFileCallback* gMamaLogFileCallback = NULL;

    extern "C"
    {
        void logSizeExceededCb ()
        {
            if (gMamaLogFileCallback != NULL)
            {
                gMamaLogFileCallback->onLogSizeExceeded ();
            }
        }
    }

    void Mama::setLogSizeCb(MamaLogFileCallback* callback)
    {
        gMamaLogFileCallback = callback;
        mamaTry ( mama_setLogSizeCb(logSizeExceededCb) );
    }

    void Mama::enableLogging (MamaLogLevel  level,
                              FILE          *logFile)
    {
        mamaTry (mama_enableLogging (logFile, level));
    }

    void Mama::logToFile (const char*     file,
                          MamaLogLevel    level)
    {
        mamaTry (mama_logToFile (file, level));
    }

    void Mama::disableLogging (void)
    {
        mamaTry (mama_disableLogging ());
        gMamaLogFileCallback = NULL;
    }

    void Mama::setLogLevel (MamaLogLevel level)
    {
        mamaTry (mama_setLogLevel (level));
    }

    MamaLogLevel Mama::getLogLevel (void)
    {
        return mama_getLogLevel ();
    }

    void Mama::setLogSize (unsigned long size)
    {
        mamaTry (mama_setLogSize (size));
    }

    void Mama::setNumLogFiles (int numFiles)
    {
        mamaTry (mama_setNumLogFiles (numFiles));
    }

    void Mama::setLogFilePolicy (mamaLogFilePolicy policy)
    {
        mamaTry (mama_setLogFilePolicy(policy));
    }

    void Mama::setAppendToLogFile (bool append)
    {
        if (false == append)
        {
            mamaTry (mama_setAppendToLogFile (0));
        }
        else
        {
            mamaTry (mama_setAppendToLogFile (1));
        }
    }

    bool Mama::loggingToFile(void)
    {
        if (0 == mama_loggingToFile ())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    MamaQueue* Mama::getDefaultEventQueue (mamaBridge bridgeImpl)
    {
        MamaQueue* defaultQueue = NULL;
        mamaQueue  defaultQueueC;

        if (MAMA_STATUS_OK ==
            mama_getDefaultEventQueue (bridgeImpl, &defaultQueueC))
        {
            mamaQueue_getClosure (defaultQueueC, (void**)(&defaultQueue));
            if (defaultQueue == NULL)
            {
                defaultQueue = new MamaQueue(defaultQueueC);
                mamaQueue_setClosure(defaultQueueC, (void*) defaultQueue);
                gDefaultQueueWrappers.push_back(defaultQueue);
            }
            return defaultQueue;
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR, "Mama::getDefaultEventQueue(): "
                      "Could not get C default event queue.");
            return NULL;
        }
    }

    void Mama::setApplicationName (const char* applicationName)
    {
        mamaTry (mama_setApplicationName (applicationName));
    }

    void Mama::setApplicationClassName (const char* className)
    {
        mamaTry (mama_setApplicationClassName (className));
    }

    void Mama::addStatsCollector (MamaStatsCollector* statsCollector)
    {
        mamaTry (mama_addStatsCollector (statsCollector->getStatsCollector()));
    }

    void Mama::removeStatsCollector (MamaStatsCollector* statsCollector)
    {
        mamaTry (mama_removeStatsCollector (statsCollector->getStatsCollector()));
    }

    template <typename T>
    void Mama::deleteObject (T* object)
    {
        delete object;
    }

} // namespace Wombat
