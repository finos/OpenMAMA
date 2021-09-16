/*=========================================================================
 =                             Includes                                  =
 =========================================================================*/

#include <stdint.h>
#include <string.h>

#include "bridgefunctions.h"
#include <mama/mama.h>
#include <mama/inbox.h>
#include <mama/publisher.h>
#include <mama/integration/bridge.h>
#include <mama/integration/publisher.h>

typedef struct implPublisherBridge
{
    transportBridge         mTransport;
    const char*             mTopic;
    const char*             mSource;
    const char*             mRoot;
    msgBridge               mMamaBridgeMsg;
    mamaPublisher           mParent;
    mamaPublisherCallbacks  mCallbacks;
    void*                   mCallbackClosure;
} implPublisherBridge;

/*=========================================================================
 =               Public interface implementation functions               =
 =========================================================================*/

mama_status
ompbBridgeMamaPublisher_createByIndex (publisherBridge*     result,
                                       mamaTransport        tport,
                                       int                  tportIndex,
                                       const char*          topic,
                                       const char*          source,
                                       const char*          root,
                                       mamaPublisher        parent)
{
    implPublisherBridge* impl = (implPublisherBridge*) calloc (1, sizeof(implPublisherBridge));

    if (NULL == impl)
    {
        return MAMA_STATUS_NOMEM;
    }

    mamaTransport_getBridgeTransport (tport, &impl->mTransport);

    if (NULL != source)
    {
        impl->mSource = strdup(source);
    }

    if (NULL != topic)
    {
        impl->mTopic = strdup(topic);
    }

    if (NULL != root)
    {
        impl->mRoot = strdup(root);
    }

    impl->mParent = parent;

    *result = (publisherBridge)impl;

    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_destroy (publisherBridge publisher)
{
    mamaPublisherCallbacks  callbacks;
    mamaPublisher           parent  = NULL;
    void*                   closure = NULL;
    implPublisherBridge*    impl    = (implPublisherBridge*) publisher;

    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    free ((void*)impl->mRoot);
    free ((void*)impl->mSource);
    free ((void*)impl->mTopic);

    /* Take a copy of the callbacks - we'll need those */
    callbacks = impl->mCallbacks;
    parent    = impl->mParent;
    closure   = impl->mCallbackClosure;

    if (NULL != callbacks.onDestroy)
    {
        (*callbacks.onDestroy)(parent, closure);
    }

    free (impl);

    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_send (publisherBridge publisher, mamaMsg msg)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_sendReplyToInbox (publisherBridge   publisher,
                                          mamaMsg           request,
                                          mamaMsg           reply)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_sendReplyToInboxHandle (publisherBridge     publisher,
                                                void*               inbox,
                                                mamaMsg             reply)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_sendFromInboxByIndex (publisherBridge   publisher,
                                              int               tportIndex,
                                              mamaInbox         inbox,
                                              mamaMsg           msg)
{
    return MAMA_STATUS_OK;
}

mama_status
ompbBridgeMamaPublisher_sendFromInbox (publisherBridge  publisher,
                                       mamaInbox        inbox,
                                       mamaMsg          msg)
{
    return ompbBridgeMamaPublisher_sendFromInboxByIndex (publisher,
                                                             0,
                                                             inbox,
                                                             msg);
}

mama_status
ompbBridgeMamaPublisher_setUserCallbacks (publisherBridge         publisher,
                                          mamaQueue               queue,
                                          mamaPublisherCallbacks* cb,
                                          void*                   closure)
{
    implPublisherBridge* impl = (implPublisherBridge*) publisher;

    if (NULL == impl || NULL == cb)
    {
        return MAMA_STATUS_NULL_ARG;
    }
    /* Take a copy of the callbacks */
    impl->mCallbacks = *cb;
    impl->mCallbackClosure = closure;

    return MAMA_STATUS_OK;
}
