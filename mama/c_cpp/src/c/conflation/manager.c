/* $Id: manager.c,v 1.1.2.3 2011/09/01 16:34:38 emmapollock Exp $
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
#include "wombat/port.h"

#include "mama/mama.h"
#include "mama/conflation/manager.h"
#include "mama/conflation/connection.h"
#include "wombat/queue.h"
#include "transportimpl.h"
#include <assert.h>

/*
 * Note we need to queue both the topic and the message. The best way to do
 * this is with two parallel queues since the queues provide reasonably
 * sophisticated memory management.
 */
typedef struct 
{
    mamaConflationEnqueueCallback mCallback;
    void*                         mClosure;
    wombatQueue                   mMsgQueue;

    /* These fields set by transport when we install the manager. */
    uint8_t                       mInstalled;
    uint8_t                       mStarted;
    mamaConnection                mConnection;
    mamaTransport                 mTransport;
    mamaMsg                       mMsg; /* wraps data */
} mamaConflationMgr;

mamaMsg 
mamaConflationManagerImpl_getMsg (mamaConflationManager mgr);

mama_status
mamaConflationManager_allocate (mamaConflationManager* mgr)
{
    mamaConflationMgr *impl = 
        (mamaConflationMgr*)calloc (1, sizeof(mamaConflationMgr));

    if (impl == NULL)
        return MAMA_STATUS_NOMEM;

    *mgr = (mamaConflationManager)impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_create (mamaConflationManager mgr)
{
    mama_status        status = MAMA_STATUS_OK;
    mamaConflationMgr* impl   = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_allocate (&impl->mMsgQueue) != WOMBAT_QUEUE_OK)
        return MAMA_STATUS_NOMEM;

    if (wombatQueue_create (impl->mMsgQueue, 0, 0, 0) != WOMBAT_QUEUE_OK)
        return MAMA_STATUS_CONFLATE_ERROR;

    status = mamaMsg_create (&impl->mMsg);

    return status;
}

mama_status
mamaConflationManager_destroy (mamaConflationManager mgr)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (impl->mStarted)
    {
        if (MAMA_STATUS_OK != 
            mamaConflationManager_end (mgr))
        {
            mama_log (MAMA_LOG_LEVEL_FINE, "Failed to stop conflation.");
        }
    }
            
    if (impl->mInstalled)
    {
        if (MAMA_STATUS_OK != 
            mamaConflationManager_uninstallConflationManager (mgr))
        {
            mama_log (MAMA_LOG_LEVEL_FINE, "Failed to uninstall conflation.");
        }
    }
    
    wombatQueue_destroy (impl->mMsgQueue);
    
    if (impl->mMsg)
        mamaMsg_destroy (impl->mMsg);

    free (impl);
    return  MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_start (mamaConflationManager mgr)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;

    if (impl == NULL || impl->mConnection == NULL || !impl->mInstalled)
        return MAMA_STATUS_INVALID_ARG;

    if (mamaTransportImpl_startConnectionConflation (
            impl->mTransport, 
            mgr, 
            impl->mConnection)!=MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Failed to start. "
                  "startConnectionConflationManager ()");
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    impl->mStarted = 1;

    return  MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_end (mamaConflationManager mgr)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mInstalled)
        return MAMA_STATUS_NOT_INSTALLED;

    /* TODO IMPLEMENT */
    return  MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_setEnqueueCallback(mamaConflationManager         mgr,
                                         mamaConflationEnqueueCallback callback,
                                         void*                         closure)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;
    /* callback can be NULL. In this case we just put the messages on the
     * conflation queue.
     */
    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    impl->mCallback = callback;
    impl->mClosure  = closure;

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManagerImpl_processData (mamaConflationManager mgr, 
                                       mamaMsg               msg,
                                       const char*           topic)
{
    mama_status status = MAMA_STATUS_OK;
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mInstalled)
        return MAMA_STATUS_NOT_INSTALLED;
    
    /* We invoke the callback if it is installed. By design we do not add the
     * message to the queue if the user supplies the callback. They may enqueue
     * it themselves. 
     *
     * This allows to mechanisms for implementing conflation. In the first
     * where the callback is NULL, the conflation routine dispatches from the
     * queue and process messages, and in the second the conflation routine
     * relies on the callback to process incoming messages. 
     */
    if (impl->mCallback)
    {
        impl->mCallback (msg, topic, impl->mClosure);
        /* Don't enqueue if there is a callback. */
        return MAMA_STATUS_OK;
    }
    if (status == MAMA_STATUS_OK &&
        wombatQueue_enqueue (impl->mMsgQueue, NULL, msg, (void*)topic) 
        != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_NOMEM;
    }

    return status;
}

mama_status
mamaConflationManager_enqueue (mamaConflationManager mgr, 
                               mamaMsg msg,
                               const char* topic)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;
    mama_status status = MAMA_STATUS_OK;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (!impl->mInstalled)
        return MAMA_STATUS_NOT_INSTALLED;

    /*
     * We need to detach the message to put it on the queue, and make sure
     * that the underlying buffer does not get freed.
     *
     * TODO: We may want to simply enqueue the buffer and wrap in a mamaMsg
     * when the caller examines it. The only tricky part is that we would have
     * to move some of the logic into the bridge since it knows how to convert
     * buffers into messages (trickier than you might think).
     *
     * We do this here rather than in processData so the C++ wrapper works.
     */
    if (msg == impl->mMsg)
    {
        status = mamaMsg_create (&impl->mMsg);
        mamaMsg_detach (msg);
       
    }
    
    if (status == MAMA_STATUS_OK &&
        wombatQueue_enqueue (impl->mMsgQueue, NULL, msg, (void*)topic) 
        != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_NOMEM;
    }

    return status;
}

mama_status
mamaConflationManager_dequeue(mamaConflationManager mgr, 
                              mamaMsg* msg, 
                              const char** topic)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;
    
    /**
     * The queue does not get set until we install the conflation manager 
     */
    if (impl->mMsgQueue == NULL)
        return MAMA_STATUS_NOT_INSTALLED;

    if (wombatQueue_dispatch (impl->mMsgQueue, (void**)msg, (void**)topic) 
        != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_CONFLATE_ERROR;
    }
    
    return MAMA_STATUS_OK;
}


mama_status
mamaConflationManager_poll(mamaConflationManager mgr, 
                           mamaMsg *msg,
                           const char** topic)
{
    mamaConflationMgr* impl = (mamaConflationMgr*)mgr;
    wombatQueueStatus status;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;
    
    /**
     * The queue does not get set until we install the conflation manager 
     */
    if (impl->mMsgQueue == NULL)
        return MAMA_STATUS_NOT_INSTALLED;

    *msg = NULL;
    status = wombatQueue_poll (impl->mMsgQueue, (void**)msg, (void**)topic); 
    if (status != WOMBAT_QUEUE_OK && status != WOMBAT_QUEUE_WOULD_BLOCK)
    {
        return MAMA_STATUS_CONFLATE_ERROR;
    }
    
    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_publish (mamaConflationManager mgr, 
                               mamaMsg msg, 
                               const char *topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    /* The connection gets set by the transport when we install a connection
     * level conflation manager.
     */
    if (impl->mConnection)
    {
      return mamaTransportImpl_sendMsgToConnection (impl->mTransport, 
                                                    impl->mConnection, 
                                                    msg,
                                                    topic);
    }
    else
    {
        /* TODO IMPLEMENT */
        /* int  implement_conflatiton_tport_forward; REMOVING WARNING */
        return MAMA_STATUS_NOT_IMPLEMENTED;
    }

    return MAMA_STATUS_OK;

}

static void MAMACALLTYPE
flushCallback (wombatQueue queue, void* data, void* itemClosure, void* closure)
{
    mamaConflationManager mgr = (mamaConflationManager)closure;
    const char* topic         = (const char*) itemClosure;
    mamaMsg     msg           = (mamaMsg)data;

    if (mamaConflationManager_publish (mgr, msg, topic) != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, 
           "Error in mamaConflationManager_publish(). flushing queue (%p)", 
           mgr);
    }
}

mama_status
mamaConflationManager_flush (mamaConflationManager mgr)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_flush (impl->mMsgQueue, flushCallback, impl) !=
        WOMBAT_QUEUE_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Error flushing queue (%p)", impl);
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_currentMsg (mamaConflationManager mgr, 
                                  mamaMsg*              msg, 
                                  const char**          topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_cur (impl->mMsgQueue, (void**)msg, (void**)topic) !=
        WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_END;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_nextMsg (mamaConflationManager mgr, 
                               mamaMsg*              msg,
                               const char**          topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_next (impl->mMsgQueue, (void**)msg, (void**)topic) !=
        WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_END;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_prevMsg (mamaConflationManager mgr, 
                               mamaMsg*              msg,
                               const char**          topic)

{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_prev (impl->mMsgQueue, (void**)msg, (void**)topic) !=
        WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_END;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_removeMsg (mamaConflationManager mgr, 
                                 mamaMsg*              msg, 
                                 const char**          topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_remove (impl->mMsgQueue, (void**)msg, (void**)topic) !=
        WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_END;
    }

    return MAMA_STATUS_OK;
}


mama_status
mamaConflationManager_insertBefore (mamaConflationManager mgr, 
                                    mamaMsg               msg, 
                                    const char*           topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_insertBefore (impl->mMsgQueue, NULL, (void*)msg,
                                  (void*)topic) != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_FULL;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_insertAfter (mamaConflationManager mgr, 
                                   mamaMsg               msg,
                                   const char*           topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_insertAfter (impl->mMsgQueue, NULL, (void*)msg,
                                  (void*)topic) != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_FULL;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_replaceMsg (mamaConflationManager mgr, 
                                  mamaMsg               newMsg,
                                  const char*           topic)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_replace (impl->mMsgQueue, NULL, (void*)newMsg,
                                  (void*)topic) != WOMBAT_QUEUE_OK)
    {
        return MAMA_STATUS_QUEUE_FULL;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_gotoTail (mamaConflationManager mgr)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_end (impl->mMsgQueue) != WOMBAT_QUEUE_OK)
    {
        /* Should never get here. */
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_gotoHead (mamaConflationManager mgr)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL)
        return MAMA_STATUS_INVALID_ARG;

    if (wombatQueue_begin (impl->mMsgQueue) != WOMBAT_QUEUE_OK)
    {
        /* Should never get here. */
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    return MAMA_STATUS_OK;
}

mama_status
mamaConflationManager_installTransportConflationManager (
        mamaConflationManager mgr,
        mamaTransport         transport)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}
                                                                        
mama_status
mamaConflationManager_installConnectionConflationManager (
            mamaConflationManager mgr,
            mamaTransport         transport,
            mamaConnection        connection)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL || impl->mInstalled || impl->mConnection)
        return MAMA_STATUS_INVALID_ARG;

    impl->mTransport = transport;
   
    if (mamaTransportImpl_installConnectConflateMgr (
            impl->mTransport, 
            mgr, 
            connection,
            mamaConflationManagerImpl_processData,
            mamaConflationManagerImpl_getMsg)!=MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Failed to install. "
                  "installConnectionConflationManager ()");
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    impl->mInstalled  = 1;
    impl->mConnection = connection;

    return MAMA_STATUS_OK;
}
   
mama_status
mamaConflationManager_uninstallConflationManager (mamaConflationManager mgr)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;

    if (impl == NULL || impl->mConnection == NULL || !impl->mInstalled)
        return MAMA_STATUS_INVALID_ARG;

    if (mamaTransportImpl_uninstallConnectConflateMgr (
            impl->mTransport, 
            mgr, 
            impl->mConnection)!=MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Failed to uninstall. "
                  "installConnectionConflationManager ()");
        return MAMA_STATUS_CONFLATE_ERROR;
    }

    impl->mInstalled  = 0;
    impl->mConnection = NULL;

    return MAMA_STATUS_OK;
}

mamaMsg 
mamaConflationManagerImpl_getMsg (mamaConflationManager mgr)
{
    mamaConflationMgr* impl  = (mamaConflationMgr*)mgr;
    return impl->mMsg;
}
