/* $Id: manager.h,v 1.1.2.3 2011/09/01 16:34:38 emmapollock Exp $
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


#ifndef CONFLATION_MANAGER_H__
#define CONFLATION_MANAGER_H__

#include <string.h>

#include "wombat/port.h"
#include "mama/mama.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (MAMACALLTYPE *mamaConflationEnqueueCallback)(mamaMsg     msg, 
                                                           const char* topic,
                                                           void*       closure);

mama_status
mamaConflationManager_allocate (mamaConflationManager* mgr);

mama_status
mamaConflationManager_create (mamaConflationManager mgr);

mama_status
mamaConflationManager_destroy (mamaConflationManager mgr);

/*
 * Start conflating. After this call, MAMA routes outbound messages 
 * through the conflation manager until the publisher calls end().
 */
mama_status
mamaConflationManager_start (mamaConflationManager mgr);

/* 
 * End conflation. Invoking this method flushes the queue and resumes
 * sending messages directly through the transport. Calling start() 
 * again restarts conflation.
 */
mama_status
mamaConflationManager_end (mamaConflationManager mgr);

/*
 * If set, new messages are passed to the specified callback rather than
 * being queued directly to the conflation queue. If it is set to NULL or
 * not set, new messages are automatically appended to the end of the
 * conflation queue in the conflation manager.
 *
 * In general this method should be called prior to invoking start(). 
 *
 * If you subclass the MamaConflationManager, you can override the 
 * mamaConflationManager_enqueue (mamaConflationManager mgr) method rather than setting the enqueue callback.
 */
mama_status
mamaConflationManager_setEnqueueCallback (mamaConflationManager         mgr,
                                          mamaConflationEnqueueCallback callback,
                                          void*                         closure);

/*
 * Enqueue a message to the MamaConflationManager's queue. If there is no
 * MamaConflationEnqueueCallback specified, the conflation manager invokes
 * this automatically. Applications may call this from the Enqueue callback
 * to append the message to the queue.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_enqueue (mamaConflationManager mgr, 
                               mamaMsg msg,
                               const char* topic);

/*
 * Remove the first message from the queue. This method blocks if the queue is empty.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_dequeue(mamaConflationManager mgr, 
                              mamaMsg* msg,
                              const char** topic);

/* 
 * Remove a message from the queue. If the queue is empty, return immediately.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_poll(mamaConflationManager mgr, 
                           mamaMsg *msg,
                           const char** topic);

/*
 * Send a message. Pass the message to the transport to be sent. This
 * message does not return to the conflation queue, it gets sent over the
 * network.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_publish (mamaConflationManager mgr, 
                               mamaMsg msg,
                               const char* topic);

/*
 * Send all the messages on the queue until it is empty.
 */
mama_status
mamaConflationManager_flush (mamaConflationManager mgr);

/*
 * In addition to enqueue() and dequeue() the following methods allow the
 * conflation algorithm to manipulate the outbound message queue. After 
 * invoking mamaConflationManager_start (mamaConflationManager mgr), 
 * conflation manager positions its iterator at the 
 * first message in the mamaConflationManager_queue (mamaConflationManager mgr
 * the message that dequeue() would remove).
 */

/* Get the current message without removing it. Or advancing the iterator.
 * Returns MAMA_STATUS_QUEUE_END if the queue is empty.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_currentMsg (mamaConflationManager mgr, 
                                  mamaMsg* msg,
                                  const char** topic);

/* 
 * Get the next message in the queue. Returns MAMA_STATUS_QUEUE_END if the
 * iterator is at the end of the queue.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_nextMsg (mamaConflationManager mgr, 
                               mamaMsg* msg,
                               const char** topic);

/* 
 * Get the previous message in the queue. Returns MAMA_STATUS_QUEUE_END if the
 * iterator is at the beginning of the queue.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_prevMsg (mamaConflationManager mgr,
                               mamaMsg* msg,
                               const char** topic);

/* Remove and return the current message. The iterator advances to the next 
 * message. Returns MAMA_STATUS_QUEUE_END if queue not positioned on valid
 * element.
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_removeMsg (mamaConflationManager mgr, 
                                 mamaMsg* msg,
                                 const char** topic);

/* Insert a message in front of the current message. 
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_insertBefore (mamaConflationManager mgr,
                                    mamaMsg msg,
                                    const char* topic);

/* Insert a message behind the current message
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_insertAfter (mamaConflationManager mgr,
                                   mamaMsg msg,
                                   const char* topic);

/* Replace the current message. Return message being replaced 
 *
 * NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 */
mama_status
mamaConflationManager_replaceMsg (mamaConflationManager mgr,
                                  mamaMsg newMsg,
                                  const char* topic);

/* Put the iterator at the tail of the queue. Algorithms that
 * process the most recent message first may use this.
 */
mama_status
mamaConflationManager_gotoTail (mamaConflationManager mgr);

mama_status
mamaConflationManager_gotoHead (mamaConflationManager mgr);

/**
 * Associate this conflation manager with a transport. The manager must
 * not be already installed for a transport or connection.
 */
mama_status
mamaConflationManager_installTransportConflationManager (
        mamaConflationManager mgr,
        mamaTransport         transport);
                                                                        
/**
 * Associate this conflation manager with a connection. The manager must
 * not be already installed for a transport or connection.
 */
mama_status
mamaConflationManager_installConnectionConflationManager (
            mamaConflationManager mgr,
            mamaTransport         transport,
            mamaConnection        connection);
   
/**
 * Uninstall the conflation manager if it is associated with a transport
 * or connection. After invoking this method it may be installed for
 * another connection or transport.
 */
mama_status
mamaConflationManager_uninstallConflationManager (mamaConflationManager mgr);

#if defined(__cplusplus)
}
#endif

#endif
