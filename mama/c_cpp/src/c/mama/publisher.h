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

#ifndef MamaPublisherH__
#define MamaPublisherH__

#if defined( __cplusplus )
extern "C"
{
#endif /* defined( __cplusplus ) */

/**
 * Responsible for publishing mama messages.
 */

/**
 * The publisher states
 */
typedef enum
{
    /**< The state of the publisher is unknown. */
    MAMA_PUBLISHER_UNKNOWN                   = 0,

    /**< The publisher is being created and waiting on the bridge (sync wait) */
    MAMA_PUBLISHER_CREATING                   = 1,

    /**< The publisher is live. */
    MAMA_PUBLISHER_LIVE                      = 2,

    /* The publisher is being destroyed. */
    MAMA_PUBLISHER_DESTROYING                = 3,

    /* The publisher is being destroyed and the bridge has sent onDestroy. */
    MAMA_PUBLISHER_DESTROYED_BRIDGE          = 4,

    /* The publisher is being destroyed and waiting on the bridge's onDestroy. */
    MAMA_PUBLISHER_DESTROYING_WAIT_BRIDGE    = 5,

    /* The publisher has been fully destroyed. */
    MAMA_PUBLISHER_DESTROYED                 = 6

} mamaPublisherState;

/**
 * Prototype for callback invoked when a message sent with either
 * mamaPublisher_sendWithThrottle() or
 * mamaPublisher_sendFromInboxWithThrottle() has been sent from
 * the throttle queue.
 *
 * Messages sent on the throttle queue are no longer destroyed by the API. It
 * is the responsibility of the application developer to manage the lifecycle
 * of any messages sent on the throttle.
 *
 * @param publisher The publisher object used to send the message.
 * @param msg       The mamaMsg which has been sent from the throttle queue.
 * @param status    Whether the message was successfully sent from the
 *                  throttle. A value of MAMA_STATUS_OK indicates that the
 *                  send was successful.
 * @param closure   User supplied context data.
 */

typedef void (MAMACALLTYPE *mamaThrottledSendCompleteCb) (
        mamaPublisher publisher,
        mamaMsg       msg,
        mama_status   status,
        void*         closure);

typedef void (MAMACALLTYPE *mama_publisherOnCreateCb) (
    mamaPublisher publisher,
    void*         closure);

typedef void (MAMACALLTYPE *mama_publisherOnDestroyCb) (
    mamaPublisher publisher,
    void*         closure);

typedef void (MAMACALLTYPE *mama_publisherOnErrorCb) (
    mamaPublisher publisher,
    mama_status   status,
    const char*   info,
    void*         closure);

/**
 * Callbacks for publisher events.
 * If any cb is NULL then the callback will not be made.
 */
typedef struct mamaPublisherCallbacks
{
    mama_publisherOnCreateCb     onCreate;
    mama_publisherOnErrorCb      onError;
    mama_publisherOnDestroyCb    onDestroy;
} mamaPublisherCallbacks;


/**
 * Create a mama publisher for the corresponding transport. If the transport
 * is a marketdata transport, as opposed to a "basic" transport, the topic
 * corresponds to the symbol. For a basic transport, the source and root get
 * ignored.
 *
 * @param result A pointer to hold the resulting mamaPublisher.
 * @param tport The transport.
 * @param symbol Symbol on which to publish.
 * @param source The source for market data publishers. (e.g. source.symbol)
 * @param root The root for market data publishers. Used internally.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_create (mamaPublisher*    result,
                      mamaTransport     tport,
                      const char*       symbol,
                      const char*       source,
                      const char*       root);

/**
 *  Create a mama publisher for a corresponding transport. Include publisher
 *  event callbacks.
 *  
 *  @param result A pointer to hold the resulting mamaPublisher.
 *  @param tport The transport.
 *  @param queue The mama queue.
 *  @param symbol Symbol on which to publish.
 *  @param source The source for market data publishers. (e.g. source.symbol)
 *  @param root The root for market data publishers. Used internally.
 *  @param cb The callbacks requested by the app.
 *  @param closure App closure, unused by mamaPublisher.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_createWithCallbacks (
                      mamaPublisher*    result,
                      mamaTransport     tport,
                      mamaQueue         queue,
                      const char*       symbol,
                      const char*       source,
                      const char*       root,
                      mamaPublisherCallbacks* cb,
                      void*             closure);

/**
 * Send a message from the specified publisher.
 *
 * @param publisher The publisher from which to send a message.
 * @param msg The mamaMsg to send.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_send (mamaPublisher publisher,
                    mamaMsg       msg);

/**
 * Send a message with the throttle.
 * The lifecycle of the message sent is controlled by the user of the API. The
 * callback indicates when the API is no longer using the message and can be
 * destroyed/reused by the application.
 *
 * @param publisher The publish from which to send the throttled message.
 * @param msg The mamaMsg to send.
 * @param sendCompleteCb Callback invoked once the message has been send on
 * the throttle.
 * @param closure User supplied data returned in the send complete callback.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_sendWithThrottle (mamaPublisher               publisher,
                                mamaMsg                     msg,
                                mamaThrottledSendCompleteCb sendCompleteCb,
                                void*                       closure);


/*
 * Send a p2p message from the specified inbox using the throttle.
 * The lifecycle of the message sent is controlled by the user of the API. The
 * callback indicates when the API is no longer using the message and can be
 * destroyed/reused by the application.
 *
 * @param publisher The mamaPublisher from which to send the message.
 * @param inbox The mamaInbox which will process any response to the sent message.
 * @param msg The mamaMsg to send.
 * @param sendCompleteCb The callback which will be invoked when the message
 * is sent from the throttle queue.
 * @param closure User supplied data returned when the callback is invoked.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_sendFromInboxWithThrottle (mamaPublisher               publisher,
                                         mamaInbox                   inbox,
                                         mamaMsg                     msg,
                                         mamaThrottledSendCompleteCb sendCompleteCb,
                                         void*                       closure);

/*
 * Send a p2p message from the specified inbox.
 *
 * @param publisher The mamaPublisher from which to send the message
 * @param inbox The mamaInbox which will process any response to the sent
 * message.
 * @param msg The mamaMsg to send.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_sendFromInbox (mamaPublisher publisher,
                             mamaInbox     inbox,
                             mamaMsg       msg);

/**
 * Send a reply in response to a request to an inbox.
 *
 * @param publisher The mamaPublisher from which to send the response.
 * @param request The reply address to which you are responding.
 * @param reply The mamaMsg to be sent as the reply.
 *
 */

MAMAExpDLL
extern mama_status
mamaPublisher_sendReplyToInboxHandle (mamaPublisher publisher,
                                      mamaMsgReply  replyAddress,
                                      mamaMsg       reply);


/**
 * Send a reply in response to a request to an inbox.
 *
 * @param publisher The mamaPublisher from which to send the response.
 * @param request The mamaMsg to which you are responding.
 * @param reply The mamaMsg to be sent as the reply.
 *
 */
MAMAExpDLL
extern mama_status
mamaPublisher_sendReplyToInbox (mamaPublisher publisher,
                                mamaMsg       request,
                                mamaMsg       reply);

/**
 * Destroy the publisher.
 *
 * @param publisher The mamaPublisher to destroy.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_destroy (mamaPublisher publisher);

/**
 * Destroy the publisher after queueing request to publisher's queue/thread
 *
 * @param publisher The mamaPublisher to destroy.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_destroyEx(mamaPublisher publisher);

/**
 * @brief Return the <code>mamaTransport</code> for this publisher.
 *
 * @param[in] publisher     The mamaPublisher from which to retrieve the transport for.
 * @param[out] transport    A pointer to hold the transport.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaPublisher_getTransport (mamaPublisher   publisher,
                            mamaTransport*  transport);

/**
 * Allocate a publisher events callback structure.
 * This is done to allow the structure to change more easily.
 * This structure is disposed by calling mamaPublisherCallbacks_deallocate.
 *
 * @param cb Pointer to pointer to a structure.
 */
MAMAExpDLL
extern mama_status
mamaPublisherCallbacks_allocate (mamaPublisherCallbacks** cb);

/**
 * Deallocate a publisher events callback structure.
 * This structure was obtained by calling mamaPublisherCallbacks_allocate.
 *
 * @param cb Pointer to a structure.
 */
MAMAExpDLL
extern mama_status
mamaPublisherCallbacks_deallocate (mamaPublisherCallbacks* cb);

/**
 * Retrieve the publisher callbacks.
 *
 * @param publisher The mamaPublisher from which to send the response.
 * @param cb Pointer to a cb structure to be populated with the current callbacks.
 */
MAMAExpDLL
extern mama_status
mamaPublisher_getUserCallbacks (mamaPublisher           publisher,
                                mamaPublisherCallbacks* cb);

MAMAExpDLL
extern mama_status
mamaPublisher_getState (mamaPublisher       publisher,
                        mamaPublisherState* state);

MAMAExpDLL
extern const char*
mamaPublisher_stringForState (mamaPublisherState state);

MAMAExpDLL
extern mama_status
mamaPublisher_getRoot (mamaPublisher publisher, const char** root);

MAMAExpDLL
extern mama_status
mamaPublisher_getSource (mamaPublisher publisher, const char** source);

MAMAExpDLL
extern mama_status
mamaPublisher_getSymbol (mamaPublisher publisher, const char** symbol);

#if defined( __cplusplus )
}
#endif /* defined( __cplusplus ) */

#endif /* MamaPublisherH__ */
