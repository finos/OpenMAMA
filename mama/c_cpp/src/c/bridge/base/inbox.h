#ifndef MAMA_BRIDGE_BASE_INBOX_H__
#define MAMA_BRIDGE_BASE_INBOX_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <mama/integration/bridge.h>


#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/**
 * This function will return the topic on which to reply in order to reach the
 * supplied inbox.
 *
 * @param inboxBridge The inbox implementation to extract the reply subject from.
 *
 * @return const char* containing the subject on which to reply.
 */
const char*
baseBridgeMamaInboxImpl_getReplySubject (inboxBridge inbox);


#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_BASE_INBOX_H__ */
