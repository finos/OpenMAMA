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

/**
 * @brief Allocation of memory for the mama conflation manager 
 *
 * @param[out] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_allocate (mamaConflationManager* mgr);

/**
 * @brief Creation of the mama conflation manager 
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_CONFLATE_ERROR
 */
mama_status
mamaConflationManager_create (mamaConflationManager mgr);

/**
 * @brief Destroy the mama conflation manager
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_destroy (mamaConflationManager mgr);

/*
 * @brief Start conflating. After this call, MAMA routes outbound messages 
 * through the conflation manager until the publisher calls end().
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_start (mamaConflationManager mgr);

/* 
 * @brief End conflation. Invoking this method flushes the queue and resumes
 * sending messages directly through the transport. Calling start() 
 * again restarts conflation.
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOT_INSTALLED
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_end (mamaConflationManager mgr);

/*
 * @brief If set, new messages are passed to the specified callback rather than
 * being queued directly to the conflation queue. If it is set to NULL or
 * not set, new messages are automatically appended to the end of the
 * conflation queue in the conflation manager.
 *
 * @details In general this method should be called prior to invoking start(). 
 *
 * @details If you subclass the MamaConflationManager, you can override the 
 * mamaConflationManager_enqueue (mamaConflationManager mgr) method rather than setting the enqueue callback.
 *
 * @param[in] mgr      The mama conflation manager
 * @param[in] callback Conflation manager callback
 * @param[in] closure  User supplied data
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_setEnqueueCallback (mamaConflationManager         mgr,
                                          mamaConflationEnqueueCallback callback,
                                          void*                         closure);

/*
 * @brief Enqueue a message to the MamaConflationManager's queue. If there is no
 * MamaConflationEnqueueCallback specified, the conflation manager invokes
 * this automatically. Applications may call this from the Enqueue callback
 * to append the message to the queue.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The Topic 
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOT_INSTALLED
 *              MAMA_STATUS_NOMEM
 */
mama_status
mamaConflationManager_enqueue (mamaConflationManager mgr, 
                               mamaMsg msg,
                               const char* topic);

/*
 * @brief Remove the first message from the queue. This method blocks if the queue is empty.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOT_INSTALLED
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_dequeue(mamaConflationManager mgr, 
                              mamaMsg* msg,
                              const char** topic);

/* 
 * @brief Remove a message from the queue. If the queue is empty, return immediately.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOT_INSTALLED
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_poll(mamaConflationManager mgr, 
                           mamaMsg *msg,
                           const char** topic);

/*
 * @brief Send a message. Pass the message to the transport to be sent. This
 * message does not return to the conflation queue, it gets sent over the
 * network.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOT_IMPLEMENTED
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_publish (mamaConflationManager mgr, 
                               mamaMsg msg,
                               const char* topic);

/*
 * @brief Send all the messages on the queue until it is empty.
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
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

/* @brief Get the current message without removing it. Or advancing the iterator.
 * Returns MAMA_STATUS_QUEUE_END if the queue is empty.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 * 
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_END
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_currentMsg (mamaConflationManager mgr, 
                                  mamaMsg* msg,
                                  const char** topic);

/* 
 * @brief Get the next message in the queue. Returns MAMA_STATUS_QUEUE_END if the
 * iterator is at the end of the queue.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 * 
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_END
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_nextMsg (mamaConflationManager mgr, 
                               mamaMsg* msg,
                               const char** topic);

/* 
 * @brief Get the previous message in the queue. Returns MAMA_STATUS_QUEUE_END if the
 * iterator is at the beginning of the queue.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 * 
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_END
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_prevMsg (mamaConflationManager mgr,
                               mamaMsg* msg,
                               const char** topic);

/* @brief Remove and return the current message. The iterator advances to the next 
 * message. Returns MAMA_STATUS_QUEUE_END if queue not positioned on valid
 * element.
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_END
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_removeMsg (mamaConflationManager mgr, 
                                 mamaMsg* msg,
                                 const char** topic);

/* @brief Insert a message in front of the current message. 
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 * 
 * @param[in] mgr   The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_FULL
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_insertBefore (mamaConflationManager mgr,
                                    mamaMsg msg,
                                    const char* topic);

/* @brief Insert a message behind the current message
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 * 
 * @param[in] mgr The mama conflation manager
 * @param[in] msg   The mama message
 * @param[in] topic
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_FULL
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_insertAfter (mamaConflationManager mgr,
                                   mamaMsg msg,
                                   const char* topic);

/* @brief Replace the current message. Return message being replaced 
 *
 * @details NOTE: Applications should not modify messages as the data may be shared
 * in the underlying middleware for efficiency. Use mamaMsg_copy().
 *
 * @param[in] mgr The mama conflation manager
 * @param[in] newMsg
 * @param[in] topic
 * @param[in] topic The topic
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_QUEUE_FULL
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_replaceMsg (mamaConflationManager mgr,
                                  mamaMsg newMsg,
                                  const char* topic);

/* @brief Put the iterator at the tail of the queue. Algorithms that
 * process the most recent message first may use this.
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_gotoTail (mamaConflationManager mgr);

/**
 * @brief Put the iterator at the head of the queue. Algorithms that
 * process the most recent message first may use this.
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_gotoHead (mamaConflationManager mgr);

/**
 * @brief Associate this conflation manager with a transport. The manager must
 * not be already installed for a transport or connection.
 *
 * @param[in] mgr       The mama conflation manager
 * @param[in] transport
 *
 * @return mama_status return code MAMA_STATUS_NOT_IMPLEMENTED
 */
mama_status
mamaConflationManager_installTransportConflationManager (
        mamaConflationManager mgr,
        mamaTransport         transport);
                                                                        
/**
 * @brief Associate this conflation manager with a connection. The manager must
 * not be already installed for a transport or connection.
 * 
 * @param[in] mgr       The mama conflation manager
 * @param[in] transport 
 * @param[in] connection
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_installConnectionConflationManager (
            mamaConflationManager mgr,
            mamaTransport         transport,
            mamaConnection        connection);
   
/**
 * @brief Uninstall the conflation manager if it is associated with a transport
 * or connection. After invoking this method it may be installed for
 * another connection or transport.
 *
 * @param[in] mgr The mama conflation manager
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_CONFLATE_ERROR
 *              MAMA_STATUS_OK
 */
mama_status
mamaConflationManager_uninstallConflationManager (mamaConflationManager mgr);

#if defined(__cplusplus)
}
#endif

#endif
