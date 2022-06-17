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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_TRANSPORT_H__)
#define OPENMAMA_INTEGRATION_TRANSPORT_H__

#include <mama/mama.h>
#include <mama/integration/types.h>
#include <wombat/list.h>

#if defined (__cplusplus)
extern "C"
{
#endif

MAMAExpDLL
extern mama_status
mamaTransport_getBridgeTransport(
    mamaTransport     transport,
    transportBridge*  result);

MAMAExpDLL
extern mama_status
mamaTransport_getBridgeTransportByIndex (
    mamaTransport    tport,
    int              tportIndex,
    transportBridge* result);

MAMAExpDLL
extern mama_status
mamaTransport_throttleRemoveForOwner (mamaTransport transport,
                                      mamaThrottleInstance instance,
                                      void *handle);

MAMAExpDLL
extern mama_status
mamaTransport_throttleRemoveFromList (mamaTransport transport,
                                      mamaThrottleInstance instance,
                                      wList list);

MAMAExpDLL
extern mama_status mamaTransport_throttleAction (
    mamaTransport         transport,
    mamaThrottleInstance  instance,
    throttleCb            throttleAction,
    void*                 owner,
    void*                 closure1,
    void*                 closure2,
    int                   immediate,
    wombatThrottleAction* handle);

MAMAExpDLL
extern mama_status mamaTransport_addSubscription(
    mamaTransport     transport,
    mamaSubscription  subscription,
    void**            result);

MAMAExpDLL
extern mama_status
mamaTransport_removeListener(
    mamaTransport     transport,
    void*             handle);

MAMAExpDLL
extern void
mamaTransportImpl_processAdvisory (mamaTransport transport,
                                   short         cause,
                                   const void*   platformInfo);

MAMAExpDLL
extern void
mamaTransportImpl_reconnect (mamaTransport      transport,
                             mamaTransportEvent event,
                             const void*        platformInfo,
                             const void*        connectionInfo);

MAMAExpDLL
extern void
mamaTransportImpl_disconnect (mamaTransport      transport,
                              mamaTransportEvent event,
                              const void*        platformInfo,
                              const void*        connectionInfo);

MAMAExpDLL
extern void
mamaTransportImpl_disconnectNoStale (mamaTransport      transport,
                                     mamaTransportEvent event,
                                     const void*        platformInfo,
                                     const void*        connectionInfo);

MAMAExpDLL
extern void
mamaTransportImpl_setPossiblyStale (mamaTransport      transport,
                                    mamaSubscription   subscription);

MAMAExpDLL
extern void
mamaTransportImpl_getTransportTopicCallback (mamaTransport transport,
                                             mamaTransportTopicCB* callback,
                                             void** closure);

MAMAExpDLL
extern void
mamaTransportImpl_getWriteQueueWatermarks (mamaTransport transport,
                                           uint32_t* high,
                                           uint32_t* low);

MAMAExpDLL
extern void
mamaTransportImpl_resetRefreshForListener (mamaTransport tport, void *handle);

MAMAExpDLL
extern mama_status
mamaTransportImpl_getTopicsAndTypesForSource (mamaTransport tport,
                                              int transportIndex,
                                              const char*   source,
                                              const char*** topics,
                                              mama_i32_t**  types,
                                              int *len);

MAMAExpDLL
extern int
mamaTransportImpl_isPossiblyStale (mamaTransport tport);

MAMAExpDLL
extern void
mamaTransportImpl_unsetAllPossiblyStale (mamaTransport tport);

MAMAExpDLL
extern void
mamaTransportImpl_getAdvisoryCauseAndPlatformInfo (mamaTransport tport,
                                                   short*        cause,
                                                   const void**  platformInfo);
/*
   Get the bridge impl associated with the specified transport.
   This will be how other objects gain access to the bridge.

   @param queue A valid mamaTransport object.
   @return A pointer to a valid mamaBridgeImpl object.
*/
MAMAExpDLL
extern mamaBridge
mamaTransportImpl_getBridgeImpl (mamaTransport transport);

MAMAExpDLL
extern void
mamaTransportImpl_getTransportIndex (mamaTransport tport,
                                     int*          tportIndex);

MAMAExpDLL
extern void
mamaTransportImpl_nextTransportIndex (mamaTransport tport,
                                      const char*   source,
                                      const char*   symbol);

MAMAExpDLL
extern wombatThrottle
mamaTransportImpl_getThrottle (mamaTransport transport,
                               mamaThrottleInstance instance);

MAMAExpDLL
extern mama_status
mamaTransportImpl_isConnectionIntercepted (mamaTransport  transport,
                                           mamaConnection connection,
                                           uint8_t*       result);

MAMAExpDLL
extern mama_status
mamaTransportImpl_installConnectConflateMgr (mamaTransport         transport,
                                             mamaConflationManager mgr,
                                             mamaConnection        connection,
                                             conflateProcessCb     processCb,
                                             conflateGetMsgCb      msgCb);

MAMAExpDLL
extern mama_status
mamaTransportImpl_uninstallConnectConflateMgr (
                                        mamaTransport         transport,
                                        mamaConflationManager mgr,
                                        mamaConnection        connection);

MAMAExpDLL
extern mama_status
mamaTransportImpl_startConnectionConflation (
                                        mamaTransport          transport,
                                        mamaConflationManager  mgr,
                                        mamaConnection         connection);

MAMAExpDLL
extern mama_status
mamaTransportImpl_sendMsgToConnection (mamaTransport   transport,
                                       mamaConnection  connection,
                                       mamaMsg         msg,
                                       const char*     topic);

MAMAExpDLL
extern int
mamaTransportImpl_getGroupSizeHint (mamaTransport transport);

MAMAExpDLL
extern mama_status
mamaTransport_initStats (mamaTransport transport);

/**
 * This function will invoke the transport callback with appropriate values.
 * @param(in) transport The transport.
 * @param(in) event The transport event that has occured.
 * @param(in) cause Cause of the event.
 * @param(in) platformInfo Additional bridge specific information that will be passed to all listeners.
 */
MAMAExpDLL extern void mamaTransportImpl_invokeTransportCallback(mamaTransport transport, mamaTransportEvent event, short cause, const void *platformInfo);

/**
 * This function will invoke the transport topic callback with appropriate values.
 * @param(in) transport The transport.
 * @param(in) event The transport topic event that has occured.
 * @param(in) topic The topic for the event.
 * @param(in) platformInfo Additional bridge specific information that will be passed to all listeners.
 * @param(in) closure Closure supplied when cb was set.
 */
MAMAExpDLL extern void mamaTransportImpl_invokeTransportTopicCallback(mamaTransport transport, mamaTransportTopicEvent event, const char* topic, const void *platformInfo);

/**
 * This function will allocate an internal transport for use with the internal event queue, this sort
 * of transport is limited and does not support certain features, including
 *  The CM Responder.
 *
 * @param[out] transport To return the transport.
 *
 * @returns mama_status value can be one of:
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status mamaTransportImpl_allocateInternalTransport(mamaTransport *transport);

/**
 * This function will set the cause and platform info for the transport.
 *
 * @param[in] transport    The transport.
 * @param[in] cause The cause.
 * @param[in] platformInfo Bridge specific info.
 *
 */
MAMAExpDLL
extern void
mamaTransportImpl_setAdvisoryCauseAndPlatformInfo(
        mamaTransport transport,
        short cause,
        const void *platformInfo);

/**
 * Disconnect a client with the specified IP Address and port. This information
 * may be retrieved from a mamaConnection object or out of band.
 *
 * For middleware that does not provide this functionality (non WMW middleware),
 * the method returns MAMA_STATUS_NOT_IMPL.
 */
MAMAExpDLL
extern mama_status
mamaTransportImpl_forceClientDisconnect (mamaTransport   transport,
                                         const char*     ipAddress,
                                         uint16_t        port);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_TRANSPORT_H__ */
