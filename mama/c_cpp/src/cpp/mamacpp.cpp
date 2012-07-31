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

#include <mama/mamacpp.h>
#include <bridge.h>
#include <mama/timer.h>
#include <mamacppinternal.h>
#include <mama/reservedfields.h>
#include <mama/msg.h>
#include <mama/inbox.h>
#include <string>
#include "MamaSubscriptionImpl.h"
#include <mama/queue.h>
#include <mama/MamaReservedFields.h>

namespace Wombat
{
    /******************************************************************************
     * Mama Implementation
     */

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


    void Mama::open()
    {
        // Open MAMA
        mama_status status = mamaTry (mama_open ());

        MamaReservedFields::initReservedFields();
    }

    void Mama::open (const char* path,
                     const char* filename)
    {
        // Open MAMA
        mama_status status = mamaTry (mama_openWithProperties (path, filename));

        MamaReservedFields::initReservedFields();
    }

    #ifdef WITH_ENTITLEMENTS 
    static MamaEntitlementCallback* gMamaEntitlementCallback = NULL;

    extern "C"
    {
        void entitlementDisconnectCB (const sessionDisconnectReason reason,
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

        void entitlementUpdateCB ()
        {
            if (gMamaEntitlementCallback != NULL)
            {
                gMamaEntitlementCallback->onEntitlementUpdate ();
            }
        }

        void entitlementCheckingSwitchCB (const int isEntitlementCheckingDisabled)
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
    #endif //WITH_ENTITLEMENTS

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
        // Close mama
        mamaTry (mama_close ());
    }

    void Mama::start (mamaBridge bridgeImpl)
    {
        mamaTry (mama_start (bridgeImpl));
    }

    static MamaStartCallback* gMamaStartCallback = NULL;

    extern "C"
    {
        void MAMACALLTYPE startCb (mama_status status)
        {
            if (gMamaStartCallback != NULL)
            {
                gMamaStartCallback->onStartComplete (MamaStatus (status));
            }
        }
    }

    void Mama::startBackground (mamaBridge bridgeImpl,
                                MamaStartCallback* cb)
    {
        gMamaStartCallback = cb;
        mamaTry (mama_startBackground (bridgeImpl, startCb));
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
                defaultQueue = new MamaQueue();
                defaultQueue->setCValue(defaultQueueC);
                mamaQueue_setClosure(defaultQueueC, (void*) defaultQueue);
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

    template <typename T>
    void Mama::deleteObject (T* object)
    {
        delete object;
    }

} // namespace Wombat
