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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "mama/mamacpp.h"

using namespace Wombat;

class StatsCallback : public MamaTimerCallback
{
    void onTimer(MamaTimer* timer);
    void onDestroy(MamaTimer* timer, void* closure) {};
};

class LifetimeCallback : public MamaTimerCallback
{
    void onTimer(MamaTimer* timer);
    void onDestroy(MamaTimer* timer, void* closure) {};
};

class RecreateCallback : public MamaQueueEventCallback
{
    void onEvent (
        MamaQueue&  queue,
        void*       closure);
};

class ChurnCallback : public MamaTimerCallback
{
    void onTimer(MamaTimer* timer);
    void onDestroy(MamaTimer* timer, void* closure) {};

    RecreateCallback 	recreateCallback;
};

class TransportCallback : public MamaTransportCallback
{
    void onDisconnect(MamaTransport* transport, const void* platformInfo)
    {
        printf("**** Disconnected\n");
    };

    void onReconnect(MamaTransport* transport, const void* platformInfo)
    {
        printf("**** Reconnected\n");
    };

    void onQuality(MamaTransport* transport, short cause, const void* platformInfo)
    {
        mamaQuality quality = transport->getQuality();
        printf("**** Transport quality %s\n", mamaQuality_convertToString(quality));
    };
};  

class SubscriptionCallbacks : public MamaSubscriptionCallback
{
    void onCreate(MamaSubscription* subscription);
    void onError(MamaSubscription* subscription, const MamaStatus& satus, const char* symbol);
    void onMsg(MamaSubscription* subscription, MamaMsg& msg);
    void onDestroy(MamaSubscription* subscription) {};   
    void onQuality(MamaSubscription* subscription, 
                    mamaQuality quality, 
                    const char* symbol, 
                    short cause, 
                    const void* platformInfo) {};
};
