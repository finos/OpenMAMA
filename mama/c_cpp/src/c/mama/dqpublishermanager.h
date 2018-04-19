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

#ifndef MAMA_DQPUBLISHERMANAGER_H__
#define MAMA_DQPUBLISHERMANAGER_H__

#include "mama/types.h"
#include "wombat/wConfig.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined( __cplusplus ) */

typedef struct mamaPublishTopic_ {
    const char *    symbol;
    mamaDQPublisher pub;
    void *          cache;
} mamaPublishTopic;

/** 
 * Callback method triggered when the DQ Publisher Manager is initially created
 * and the subscription to the source root has been successfully created.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherCreateCB) (
    mamaDQPublisherManager manager);

/**
 * Callback method triggered when a subscription request is received for an
 * instrument which does not currently have a publisher in the current manager.
 *
 * @param manager The current manager instance.
 * @param symbol The instrument for which the subscription request was received.
 * @param subType Enum indicating the type of subscription request received.
 * @param msgType Enum indicating the type of message received.
 * @param msg The MAMA message received with the request.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherNewRequestCB) (
    mamaDQPublisherManager manager,
    const char *           symbol,
    short                  subType,
    short                  msgType,
    mamaMsg                msg);

/**
 * Callback method triggered when a subscription request is received for an
 * instrument which already has a publisher in the current manager.
 *
 * @param manager The current manager instance.
 * @param info The current publisher topic.
 * @param subType Enum indicating the type of subscription request received.
 * @param msgType Enum indicating the type of message received.
 * @param msg The MAMA message received with the request.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherRequestCB) (
    mamaDQPublisherManager manager,
    mamaPublishTopic *     info,
    short                  subType,
    short                  msgType,
    mamaMsg                msg);

/**
 * Callback method triggered when a subscription refresh request is received for
 * an instrument. The DQ Publisher Manager handles sending the response, so this
 * callback is purely informational.
 *
 * @param manager The current manager instance.
 * @param info The current publisher topic.
 * @param subType Enum indicating the type of subscription request received.
 * @param msgType Enum indicating the type of message received.
 * @param msg The MAMA message received with the request.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherRefreshCB) (
    mamaDQPublisherManager manager,
    mamaPublishTopic *     info,
    short                  subType,
    short                  msgType,
    mamaMsg                msg);

/**
 * Callback method triggered when a error is encountered processing a
 * message by the DQ Publisher Manager.
 *
 * @param manager The current manager instance.
 * @param status The status indicating the cause of the error.
 * @param errortxt A string representation of the cause of the error. (Stack
 *                 allocated)
 * @param msg The MAMA message received with the request.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherErrorCB) (
    mamaDQPublisherManager manager,
    mama_status            status,
    const char *           errortxt,
    mamaMsg                msg);

/**
 * Callback method triggered when a message is received by the DQ Publisher
 * Manager which doesn't contain the expected 'MdSubscriptionType' field.
 * 
 * Note: While such messages are unexpected, this feature may be used to 
 * implement publisher specific functionality, where such features are
 * supported at the 'source' or 'namespace' level. 
 *
 * @param manager The current manager instance.
 * @param msg The MAMA message received with the request.
 */
typedef void(MAMACALLTYPE *mamaDQPublisherMsgCB) (
    mamaDQPublisherManager manager, mamaMsg msg);

/** Structure containing each of the DQ Publisher Manager callbacks. */
typedef struct mamaDQPublisherManagerCallbacks_ {
    mamaDQPublisherCreateCB     onCreate;
    mamaDQPublisherNewRequestCB onNewRequest;
    mamaDQPublisherRequestCB    onRequest;
    mamaDQPublisherRefreshCB    onRefresh;
    mamaDQPublisherErrorCB      onError;
    mamaDQPublisherMsgCB        onMsg;
} mamaDQPublisherManagerCallbacks;

/**
 * Allocate memory for the DQ Publisher Manager, initialising the Sender
 * ID (either pre-defined, or based on the IP and PID of the host and
 * publisher) and Sequence Number.
 *
 * @param result A pointer to hold the resulting mamaDQPublisherManager.
 *
 * @return mama_status A status indicating the result of the allocation.
 *      May return:
 *          - MAMA_STATUS_NOMEM - Allocation failed due to lack of memory.
 *          - MAMA_STATUS_OK - Successfully allocated.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_allocate (mamaDQPublisherManager *result);

/**
 * Create the DQ Publisher Manager instance.
 *
 * Creates a DQ Publisher Manager instance. Creates the publisher
 * hashmap to store created publishers. Initialises the basic
 * subscription which receives requests from subscribing applications
 * for data. Also initialises the CM responder publisher to handle
 * sync requests when required.
 *
 * @param dqPublisher The DQ Publisher Manager instance to create.
 * @param transport The MAMA transport to use for publishers created
          by this manager.
 * @param queue The MAMA queue to use for publishers created by this
          manager.
 * @param callback The DQ Publisher Manager callbacks to use to
          interact with the manager.
 * @param sourcename The namespace on which publishers created by the
          manager.
 * @param root The root on which to create publishers.
 * @param closure A user defined structure which can be used as a
          reference.
 *
 * @result mama_status A status indicating the rest of the creation.
 *         May return:
 *              - MAMA_STATUS_OK - DQ Publisher Manager creation has
 *                                 succeeded.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_create (mamaDQPublisherManager dqPublisher,
                               mamaTransport          transport,
                               mamaQueue              queue,
                               const mamaDQPublisherManagerCallbacks *callback,
                               const char *sourcename,
                               const char *root,
                               void *      closure);

/**
 * Destroy the DQ Publisher Manager instance.
 *
 * @param manager The DQ Publisher Manager instance to destroy.
 */
MAMAExpDLL extern void
mamaDQPublisherManager_destroy (mamaDQPublisherManager manager);

/**
 * Add an already created MAMA Publisher to the given DQ Publisher
 * Manager.
 *
 * @param manager The manager to add the publisher to.
 * @param symbol The symbol which the publisher was created for.
 * @param pub The publisher to add to the manager.
 * @param cache The cache to assign to the publisher.
 *
 * @return mama_status A status indicating the success of the addition.
 *      May return:
 *           - MAMA_STATUS_OK for a successful addition.
 *           - MAMA_STATUS_INVALID_ARG where a publisher already exists
 *             for the given instrument.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_addPublisher (mamaDQPublisherManager manager,
                                     const char *           symbol,
                                     mamaDQPublisher        pub,
                                     void *                 cache);

/**
 * Remove the publisher specified by the 'symbol' paramater from
 * the current manager, but return a pointer to that publisher in the 'pub'
 * parameter.
 *
 * @param manager The DQ Publisher Manager instance to remove the publisher
 *                from.
 * @param symbol The instrument to remove the publisher for.
 * @param pub A pointer to the publisher returned by the method.
 *
 * @return mama_status Status indicating the successful removal of the
 *         publisher. May return:
 *              - MAMA_STATUS_OK - Publisher successfully removed.
 *              - MAMA_STATUS_INVALID_ARG - Publisher not found.
 */
MAMAExpDLL extern mama_status mamaDQPublisherManager_removePublisher (
    mamaDQPublisherManager manager, const char *symbol, mamaDQPublisher *pub);

/**
 * Create a new publisher for the given symbol, attaching a 'cache' to
 * the publisher and returning it in the 'newPublisher' parameter.
 *
 * @param manager The DQ Publisher Manager instance to create the publisher on.
 * @param symbol The instrument to remove the publisher for.
 * @param cache The cache to attach to the publisher.
 * @param newPublisher A pointer to the publisher created.
 *
 * @return mama_status Status indicating the successful removal of the
 *         publisher. May return:
 *              - MAMA_STATUS_OK - Publisher successful removed.
 *              - MAMA_STATUS_INVALID_ARG - Publisher not found.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_createPublisher (mamaDQPublisherManager manager,
                                        const char *           symbol,
                                        void *                 cache,
                                        mamaDQPublisher *      newPublisher);

/**
 * Destroy the publisher associated with the given symbol in the specified
 * publisher manager. Calls mamaPublisher_destroy on the object, and frees
 * the memory associated with it.
 *
 * @param manager The DQ Publisher Manager instance to destroy the publisher
 *                from.
 * @param symbol The instrument to destroy the publisher for.
 *
 * @return mama_status Status indicating the successful removal of the
 *         publisher. May return:
 *              - MAMA_STATUS_OK - Publisher successfully destroyed.
 *              - MAMA_STATUS_INVALID_ARG - Publisher not found.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_destroyPublisher (mamaDQPublisherManager manager,
                                         const char *           symbol);

/**
 * Set the MAMA message status to be associated with newly created publishers.
 * Must be set before creation of the publisher to take effect.
 *
 * @param manager The DQ Publisher Manager instance on which to set the status.
 * @param mamaMsgStatus The MAMA Msg Status to be set on the manager.
 */
MAMAExpDLL extern void
mamaDQPublisherManager_setStatus (mamaDQPublisherManager manager,
                                  mamaMsgStatus          status);

/**
 * Return the closure object associated with the DQ Publisher Manager.
 *
 * @param manager The DQ Publisher Manager instance to return the closure from.
 *
 * @return void* The closure returned from the manager.
 */
MAMAExpDLL extern void *
mamaDQPublisherManager_getClosure (mamaDQPublisherManager manager);

/**
 * Set the sender ID used in creation of new publishers. Must be set before
 * the creation of the publisher to take effect.
 *
 * @param manager The DQ Publisher Manager to set the sender ID for.
 * @param sendid The Sender ID to use for publishers created by the manager.
 */
MAMAExpDLL extern void
mamaDQPublisherManager_setSenderId (mamaDQPublisherManager manager,
                                    uint64_t               senderid);

/**
 * Set the sequence number used when creating new publishers. Must be set before
 * creation of the publisher to take effect.
 *
 * @param manager The DQ Publisher Manager instance to set the sequence number
 *                for.
 * @param num The sequence number to set.
 */
MAMAExpDLL extern void
mamaDQPublisherManager_setSeqNum (mamaDQPublisherManager manager,
                                  mama_seqnum_t          num);

/**
 * Send a 'sync request' message to subscribing applications.
 *
 * Sync requests are a mechanism by which a newly started MAMA publisher
 * can request that client applications inform the publisher of the instruments
 * to which they are subscribed. Required CM responder to be enabled in the
 * clients.
 *
 * @param manager The DQ Publisher Manager instance to send requests for.
 * @param nummsg The number of instruments to be returned per sync response
 * @param delay The delay between sending of sync responses.
 * @param duration The duration of responses.
 *
 * @return mama_status Status indicating the successful removal of the
 *         publisher. May return:
 *              - MAMA_STATUS_OK - Publisher successful removed.
 *              - MAMA_STATUS_INVALID_ARG - Publisher not found.
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_sendSyncRequest (mamaDQPublisherManager manager,
                                        mama_u16_t             nummsg,
                                        mama_f64_t             delay,
                                        mama_f64_t             duration);

/**
 * Send a 'no subscribers' message.
 *
 * @param manager The DQ Publisher Manager instance to remove the publisher
 *                from.
 * @param symbol The instrument to send the no subscribers message for (unused)
 *
 * @return mama_status Status indicating the successful sending of the message
 *                     (unused)
 */
MAMAExpDLL extern mama_status
mamaDQPublisherManager_sendNoSubscribers (mamaDQPublisherManager manager,
                                          const char *           symbol);

/**
 * Return the publisher specified by the 'symbol' paramater from
 * the current manager.
 *
 * @param manager The DQ Publisher Manager instance to return the publisher
 *                from.
 * @param symbol The instrument to return the publisher for.
 * @param pub The publisher returned from the publisher.
 *
 * @return mama_status Status indicating the successful removal of the
 *         publisher. May return:
 *              - MAMA_STATUS_OK - Publisher successful removed.
 *              - MAMA_STATUS_INVALID_ARG - Publisher or manager don't exist
 *                (NULL)
 *              - MAMA_STATUS_NOT_FOUND - The publisher has not been found in
 *                the manager.
 */
MAMAExpDLL extern mama_status mamaDQPublisherManager_getPublisher (
    mamaDQPublisherManager manager, const char *symbol, mamaDQPublisher *pub);

/**
 * Enable or disable the publishing of 'send times' by publishers created by
 * the manager. Must be set before creation of the publisher to take effect.
 *
 * @param manager The DQ Publisher Manager on which to enable the send time.
 * @param enable Specify whether to publish the send time or not.
 */
MAMAExpDLL extern void
mamaDQPublisherManager_enableSendTime (mamaDQPublisherManager manager,
                                       mama_bool_t            enable);

#if defined(__cplusplus)
}
#endif /* defined( __cplusplus ) */

#endif /* MAMA_DQPUBLISHERMANAGER_H__*/
