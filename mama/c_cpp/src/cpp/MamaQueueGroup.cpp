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

/**
 * A simple class for allocating subscriptions amongst multiple queues in a
 * round robin. This class creates dispatchers for the queues as well.
 */

#include "mama/mamacpp.h"
#include "mama/MamaQueueGroup.h"

namespace Wombat
{

    MamaQueueGroup::~MamaQueueGroup (void)
    {
        if (mQueues != NULL)
        {
            for (int i = 0; i < mQueueCount; i++)
            {
                if (mDispatchers[i]) 
                {  
                    delete mDispatchers[i];
                }
                delete mQueues[i];
            }
        }

        delete [] mQueues;
        delete [] mDispatchers;
    }

    MamaQueueGroup::MamaQueueGroup (int         numberOfQueues, 
                                    mamaBridge  bridgeImpl) 
        : mQueueCount   (numberOfQueues)
        , mCallCount    (0)
        , mQueues       (NULL)
        , mDispatchers  (NULL)
        , mDefaultQueue (NULL)
         
    {
        if (mQueueCount > 0)
        {
            mQueues      = new MamaQueue*      [numberOfQueues];
            mDispatchers = new MamaDispatcher* [numberOfQueues];
        }

        for (int i = 0; i < mQueueCount; i++)
        {
            mQueues[i] = new MamaQueue;
            mQueues[i]->create (bridgeImpl);

            mDispatchers[i] = new MamaDispatcher;
            mDispatchers[i]->create (mQueues[i]);
        }

        mDefaultQueue = Mama::getDefaultEventQueue (bridgeImpl);
    }

    MamaQueue* MamaQueueGroup::getNextQueue (void) 
    {
        return mQueues == NULL ? mDefaultQueue 
                                : mQueues [mCallCount++ % mQueueCount];
    }

    int MamaQueueGroup::getNumberOfQueues (void)
    {
        return mQueueCount;
    }

    void  MamaQueueGroup::stopDispatch (void)
    {
        for (int i = 0; i < mQueueCount; i++)
        {
            if (mDispatchers[i])
            {
                delete mDispatchers[i];
                mDispatchers[i] = NULL;
            }
        }
    }
        
    void  MamaQueueGroup::startDispatch (void)
    {
        for (int i = 0; i < mQueueCount; i++)
        {
            if (!mDispatchers[i])
            {
                mDispatchers[i] = new MamaDispatcher;
                mDispatchers[i]->create (mQueues[i]);
            }
        }
    }

    void MamaQueueGroup::destroyWait(void)
    {
        // First of all stop dispatching to ensure that no more messages are being processed.
        stopDispatch();

        // The queues may already have been destroyed
        if (mQueues != NULL)
        {
            // Destroy all the queues in turn
            for (int index = 0; index < mQueueCount; index++)
            {   
                if (NULL != mQueues[index])
                {
                    // This will block until the queue destroy has completed
                    mQueues[index]->destroyWait ();

                    // Delete the queue
                    delete mQueues[index];
                    mQueues[index] = NULL;
                }
            }

            // Delete the array of queues
            delete[] mQueues;
            mQueues = NULL;
        }
    }
    
}/* namespace Wombat */

