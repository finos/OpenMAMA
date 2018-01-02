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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_MSG_H__)
#define OPENMAMA_INTEGRATION_MSG_H__

#include <mama/mama.h>
#include <mama/integration/types.h>

#if defined (__cplusplus)
extern "C"
{
#endif

MAMAExpDLL extern mama_status
mamaMsgImpl_createForPayload (mamaMsg*               msg,
                              msgPayload             payload,
                              mamaPayloadBridge      payloadBridge,
                              short                  noDeletePayload);

/*Used by each bridge to set the incoming data on an existing message */
MAMAExpDLL extern mama_status
mamaMsgImpl_setMsgBuffer(mamaMsg        msg,
                         const void*    data,
                         uint32_t       len,
                         char id);

/*Used by the bridge publishers.*/
MAMAExpDLL extern mama_status
mamaMsgImpl_getSubject (const mamaMsg msg, const char** result);

/*Associate a queue with a message - this is the queue on which the
 reuseable message is associated. Needed in order to detach a message*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setQueue (mamaMsg msg, mamaQueue queue);

/*Associate a subscription context with a message - this is needed to
 inform the cache if a cache message has been detached*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setDqStrategyContext (mamaMsg msg, mamaDqContext* dqStrategyContext);

/*Set the bridge struct of function pointers on the message*/
MAMAExpDLL extern mama_status
mamaMsgImpl_setBridgeImpl (mamaMsg msg, mamaBridge bridgeImpl);

/*Get the bridge msg impl from the parent mamaMsg. This will have been set
 * when the mamaMsg called BridgeMamaMsg_create. It can only do this when it
 * is given the bridge impl object.*/
MAMAExpDLL extern mama_status
mamaMsgImpl_getBridgeMsg (mamaMsg msg, msgBridge* bridgeMsg);

/*Get the payload msg buffer from the parent mamaMsg. */
MAMAExpDLL extern mama_status
mamaMsgImpl_getPayloadBuffer(const mamaMsg  msg,
        const void**   buffer,
        mama_size_t*   bufferLength);

/*Get the underlying payload parent mamaMsg. */
MAMAExpDLL extern mama_status
mamaMsgImpl_getPayload (const mamaMsg msg, msgPayload* payload);

MAMAExpDLL
extern mama_status
mamaMsgImpl_setPayloadBridge (mamaMsg msg, mamaPayloadBridge payloadBridge);

/*Use the native payload format of the bridge, if any */
MAMAExpDLL
extern mama_status
mamaMsgImpl_useBridgePayload (mamaMsg msg, mamaBridge bridgeImpl);


MAMAExpDLL
extern mama_status
mamaMsgImpl_setPayload (mamaMsg msg, msgPayload payload, short ownsPayload);

MAMAExpDLL
mama_status
mamaMsgImpl_setMessageOwner (mamaMsg msg, short owner);

MAMAExpDLL
mama_status
mamaMsgImpl_getMessageOwner (mamaMsg msg, short* owner);

/* Build up the subject for the message, and set it in the underlying message
 * if needed (currently RV only)
 *
 * @param root A constant, predetermined prefix to avoid potential conflicts
 * with other fields. (_MD. for a normal subscription).
 * @param source The source name of the feed handler that will service the
 * request.
 * @param symbol The symbol to which to subscribe. May be null in some cases
 * like dictionary subsriptions.
 * @param modifyMessage add the required fields to the message if true.
 *     this can be inefficient on some platforms so we don't need to do
 *     it for inbound messages.
 *
 * @param msg The message.
 */
MAMAExpDLL extern mama_status
mamaMsgImpl_setSubscInfo (mamaMsg     msg,
                          const char* root,
                          const char* source,
                          const char* symbol,
                          int         modifyMessage);

MAMAExpDLL
mama_status
mamaMsgImpl_setStatus (mamaMsg msg, mamaMsgStatus status);

MAMAExpDLL
mama_status
mamaMsgImpl_getStatus (mamaMsg msg, mamaMsgStatus* status);

MAMAExpDLL
mamaMsgStatus
mamaMsgImpl_getStatusFromMsg (mamaMsg msg);

MAMAExpDLL
mama_status
mamaMsgImpl_getPayloadId (mamaMsg msg, char* id);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_MSG_H__ */
