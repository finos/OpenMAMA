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

#ifndef MAMA_QUEUE_GROUP_CPP_H__
#define MAMA_QUEUE_GROUP_CPP_H__

namespace Wombat
{

    class MamaQueue;
    class MamaDispatcher;

    /**
     * A simple class for allocating subscriptions amongst multiple queues in a
     * round robin. This class creates dispatchers for the queues as well.
     */
    class MAMACPPExpDLL MamaQueueGroup
    {
    public:
        virtual ~MamaQueueGroup ();

        /**
         * If numberOfQueues == 0, getNextQueue returns the default queue for the
         * bridge
         */
        MamaQueueGroup (
            int         numberOfQueues,
            mamaBridge  bridgeImpl);

        /**
         * Destroy all the queues. Note that a queue can only be destroyed if all of the objects created
         * on it, (timers, subscriptions etc), have been destroyed. This function will block until
         * all of the objects have been destroyed and will then destroy the queues.      
         */
        virtual void destroyWait ();

        /**
         * Return the next available queue from the queue group. Queues are
         * returned in a round robin fashion.
         */
        virtual MamaQueue* getNextQueue ();

        /**
         * Return the number of MamaQueues currently managed by this queue group.
         */
        virtual int getNumberOfQueues ();
        
        /**
         * Stop dispatching on queues in the queue group.
         */
        virtual void stopDispatch ();
        
        /**
         * Start dispatching on all queues in a group.
         * NOTE: This only should be used after a previous call to stopDispatch.
         * Dispatching on a queue is started hwen it is created
         */
        virtual void startDispatch ();

    private:
        int                  mQueueCount;
        unsigned int         mCallCount;
        MamaQueue**          mQueues;
        MamaDispatcher**     mDispatchers;
        MamaQueue*           mDefaultQueue;


        MamaQueueGroup ()
        {
        }

        MamaQueueGroup (
            const MamaQueueGroup&) 
        {
        }

    };

}/* namespace Wombat */

#endif // MAMA_QUEUE_GROUP_CPP_H__
