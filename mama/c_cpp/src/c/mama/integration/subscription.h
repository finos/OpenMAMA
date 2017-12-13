/**
* This file contains methods which are intended for use only by OpenMAMA core,
* bridges and plugins. These methods typically only make sense to developers
* who have intimate knowledge of the inner workings of OpenMAMA. Its use in
* applications is heavily discouraged and entirely unsupported.
*
* Note that you can only use these methods if you have defined the
* OPENMAMA_INTEGRATION macro. If you think a case is to be made for accessing
* one of these methods, please raise to the mailing list and we'll investigate
* opening up a more stable formal interface for the standard public API.
*/

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_SUBSCRIPTION_H__)
#define OPENMAMA_INTEGRATION_SUBSCRIPTION_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

/**
 * This function returns the bridge from the subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return The bridge.
 */
MAMAExpDLL
extern mamaBridgeImpl*
mamaSubscription_getBridgeImpl(
    mamaSubscription subscription);

/**
 * This function returns the subscription bridge from the subscription.
 *
 * @param[in] subscription The subscription.
 *
 * @return The subscription bridge. Structure is defined by bridge implementation.
 */
MAMAExpDLL
extern subscriptionBridge
mamaSubscription_getSubscriptionBridge(
    mamaSubscription subscription);

/**
 * This function returns a flag indicating whether or not the subscription is expecting an initial to
 * be delivered.
 *
 * @param[in] subscription The subscription.
 * @param[out] expectingInitial 1 if the subscription is expecting an initial else 0.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_getExpectingInitial(
    mamaSubscription subscription,
    int *expectingInitial);

/**
* This function returns the transport index used by the subscription.
*
* @param[in] subscription The subscription.
* @param[out] tportIndex To return the transport index.
*
* @return mama_status value can be one of:
*          MAMA_STATUS_NULL_ARG
*          MAMA_STATUS_OK
*/
MAMAExpDLL
extern mama_status
mamaSubscription_getTransportIndex(
	mamaSubscription subscription,
	int *tportIndex);

/**
* This function returns the type of wildcard subscription.
*
* @param[in] subscription The subscription.
*
* @return The wildcard subscription enumeration value.
*/
MAMAExpDLL
extern wildCardType
mamaSubscription_getWildCardType(
	mamaSubscription subscription);

/**
 * This function determines if the transport backing up the subscription has been disconnected.
 *
 * @param[in] subscription The subscription.
 *
 * @return 1 if the transport has been disconnected else 0.
 */
MAMAExpDLL
extern int
mamaSubscription_isTportDisconnected(
    mamaSubscription subscription);

/**
 * This function will process an error by invoking the user supplied error callback.
 *
 * @param[in] subscription The subscription.
 * @param[in] deactivate If set to 0 the subscription will also be deactivated.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NULL_ARG
 *              MAAM_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processErr(
    mamaSubscription subscription,
    int deactivate);

/**
 * This function will process a new message when it arrives and is typically called
 * by the bridge.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processMsg(
    mamaSubscription subscription,
    mamaMsg message);

/**
 * This function will process a new message when it arrives and is typically called
 * by the bridge. This function is used for wildcard subscriptions.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process.
 * @param[in] topic The topic the message has been received for.
 * @param[in] topicClosue Symbol specific closure.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processWildCardMsg(
    mamaSubscription subscription,
    mamaMsg msg,
    const char *topic,
    void *topicClosure);

/**
 * This function will process a tport message when it arrives and is typically called
 * by the bridge. This function is used for wildcard subscriptions.
 *
 * @param[in] subscription The subscription.
 * @param[in] message The message to process.
 * @param[in] topicClosue Symbol specific closure.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_processTportMsg(
    mamaSubscription subscription,
    mamaMsg msg,
    void *topicClosure);

/**
 * This function will change the state of the subscription to possibly stale and will
 * result in the quality callback being invoked.
 *
 * @param[in] subscription The subscription.
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setPossiblyStale(
    mamaSubscription subscription);

/**
 * This function sets the transport index used by the subscription.
 *
 * @param[in] subscription The subscription.
 * @param[in] tportIndex The new transport index.
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setTransportIndex(
    mamaSubscription subscription,
    int tportIndex);

/**
 * Set the parameters for a subscription that may be actually
 * activated later.  Activate the subscription using
 * mamaSubscription_activate().
 *
 * @param subscription The subscription.
 * @param transport The transport to use.
 * @param queue The mama queue.
 * @param callbacks The mamaMsgCallbacks structure containing the callback
 * functions.
 * @param source The source name of the feed handler to provide the
 * subscription.
 * @param symbol The symbol name.
 * @param closure The closure will be passed to subsequent callback
 * invocations for this subscription.
 *
 * @return mama_status value can be one of
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSubscription_setupBasic(
    mamaSubscription          subscription,
    mamaTransport             transport,
    mamaQueue                 queue,
    const mamaMsgCallbacks*   callbacks,
    const char*               source,
    const char*               symbol,
    void*                     closure);


#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_SUBSCRIPTION_H__ */
