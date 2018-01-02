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

#if defined (OPENMAMA_INTEGRATION) && ! defined (OPENMAMA_INTEGRATION_MSGUTILS_H__)
#define OPENMAMA_INTEGRATION_MSGUTILS_H__

#include <mama/mama.h>

#if defined (__cplusplus)
extern "C"
{
#endif

MAMAExpDLL
mama_status
msgUtils_getIssueSymbol            (mamaMsg msg, const char** result);

MAMAExpDLL
mama_status
msgUtils_setStatus                 (mamaMsg msg, short status);

MAMAExpDLL
mama_status
msgUtils_msgTotal                  (mamaMsg msg, short* result);

MAMAExpDLL
mama_status 
msgUtils_msgNum                    (mamaMsg msg, short* result);

MAMAExpDLL
mama_status
msgUtils_msgSubscMsgType           (mamaMsg msg, short* result);

MAMAExpDLL
mama_status 
msgUtils_createSubscriptionMessage (mamaSubscription  subscription,
                                    mamaSubscMsgType  subscMsgType,
                                    mamaMsg*          msg,
                                    const char*       issueSymbol);

/**
 * Create a message requesting a  subscription to the specified subject
 *
 * @param subsc
 * @return the subscribe message.
 */
MAMAExpDLL
mama_status
msgUtils_createSubscribeMsg        (mamaSubscription subsc, mamaMsg* result);

/**
 * Create a RESUBSCRIBE message  in response to a sync request.
 * @return  the resubscribe message.
 */
MAMAExpDLL
mama_status
msgUtils_createResubscribeMessage  (mamaMsg* result);

MAMAExpDLL
mama_status
msgUtils_createRefreshMsg          (mamaSubscription subsc, mamaMsg* result);

MAMAExpDLL
mama_status
msgUtils_createTimeoutMsg          (mamaMsg* msg);

MAMAExpDLL
mama_status
msgUtils_createEndOfInitialsMsg    (mamaMsg* msg);

/**
 * Create a recovery message in when required to attempt recovery
 * of an item..
 * @return the recover request message.
 */
MAMAExpDLL
mama_status
msgUtils_createRecoveryRequestMsg  (mamaSubscription subsc, 
                                    short            reason,
                                    mamaMsg*         result, 
                                    const char*      issueSymbol);

MAMAExpDLL
mama_status
msgUtils_setSubscSubject           (mamaMsg msg, const char* sendSubject);

MAMAExpDLL
mama_status 
msgUtils_createUnsubscribeMsg      (mamaSubscription subsc, mamaMsg* msg);

MAMAExpDLL
mama_status
msgUtils_createDictionarySubscribe (mamaSubscription subscription, 
                                    mamaMsg*         msg);

MAMAExpDLL
mama_status
msgUtils_createSnapshotSubscribe   (mamaSubscription subsc, mamaMsg* msg);

MAMAExpDLL
mama_status
msgUtils_msgTotal                  (mamaMsg msg, short* result);

MAMAExpDLL
mama_status
msgUtils_msgNum                    (mamaMsg msg, short* result);

#if defined (__cplusplus)
}
#endif

#endif /* OPENMAMA_INTEGRATION_MSGUTILS_H__ */
