/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <string.h>
#include <wombat/wUuid.h>
#include <wombat/port.h>
#include <mama/mama.h>
#include <bridge.h>
#include "basedefs.h"
#include <mama/integration/bridge/base.h>


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define                 UUID_STRING_BUF_SIZE                37


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct baseInboxImpl
{
    char                            mInbox[MAX_SUBJECT_LENGTH];
    mamaSubscription                mSubscription;
    void*                           mClosure;
    mamaInboxMsgCallback            mMsgCB;
    mamaInboxErrorCallback          mErrCB;
    mamaInboxDestroyCallback        mOnInboxDestroyed;
    mamaInbox                       mParent;
} baseInboxImpl;

/*=========================================================================
  =                  Private implementation prototypes                    =
  =========================================================================*/

/**
 * This is the onMsg callback to call when a message is received for this inbox.
 * This will in turn relay the message to the mamaInboxMsgCallback callback
 * provided on inbox creation.
 *
 * @param subscription The MAMA subscription originating this callback.
 * @param msg          The message received.
 * @param closure      The closure passed to the mamaSubscription_create
 *                     function (in this case, the inbox impl).
 * @param itemClosure  The item closure for the subscription can be set with
 *                     mamaSubscription_setItemClosure (not used in this case).
 */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onMsg       (mamaSubscription    subscription,
                                     mamaMsg             msg,
                                     void*               closure,
                                     void*               itemClosure);

/**
 * This is the onCreate callback to call when the inbox subscription is created.
 * This currently does nothing but needs to be specified for the subscription
 * callbacks.
 *
 * @param subscription The MAMA subscription originating this callback.
 * @param closure      The closure passed to the mamaSubscription_create
 *                     function (in this case, the inbox impl).
 */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onCreate    (mamaSubscription    subscription,
                                     void*               closure);

/**
 * This is the onDestroy callback to call when the inbox subscription is
 * destroyed. This will relay this destroy request to the mamaInboxDestroy
 * callback provided on inbox creation when hit.
 *
 * @param subscription The MAMA subscription originating this callback.
 * @param closure      The closure passed to the mamaSubscription_create
 *                     function (in this case, the inbox impl).
 */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onDestroy   (mamaSubscription    subscription,
                                     void*               closure);

/**
 * This is the onError callback to call when the inbox subscription receives
 * an error. This will relay this error to the mamaInboxErrorCallback callback
 * provided on inbox creation when hit.
 *
 * @param subscription  The MAMA subscription originating this callback.
 * @param status        The error code encountered.
 * @param platformError Third party, platform specific messaging error.
 * @param subject       The subject if NOT_ENTITLED encountered.
 * @param closure       The closure passed to the mamaSubscription_create
 *                      function (in this case, the inbox impl).
 */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onError     (mamaSubscription    subscription,
                                     mama_status         status,
                                     void*               platformError,
                                     const char*         subject,
                                     void*               closure);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
baseBridgeMamaInbox_create          (inboxBridge*             bridge,
                                     mamaTransport            transport,
                                     mamaQueue                queue,
                                     mamaInboxMsgCallback     msgCB,
                                     mamaInboxErrorCallback   errorCB,
                                     mamaInboxDestroyCallback onInboxDestroyed,
                                     void*                    closure,
                                     mamaInbox                parent)
{
    return baseBridgeMamaInbox_createByIndex (bridge,
                                              transport,
                                              0,
                                              queue,
                                              msgCB,
                                              errorCB,
                                              onInboxDestroyed,
                                              closure,
                                              parent);
}

mama_status
baseBridgeMamaInbox_createByIndex   (inboxBridge*             bridge,
                                     mamaTransport            transport,
                                     int                      tportIndex,
                                     mamaQueue                queue,
                                     mamaInboxMsgCallback     msgCB,
                                     mamaInboxErrorCallback   errorCB,
                                     mamaInboxDestroyCallback onInboxDestroyed,
                                     void*                    closure,
                                     mamaInbox                parent)
{
    baseInboxImpl*      impl        = NULL;
    mama_status         status      = MAMA_STATUS_OK;
    mamaMsgCallbacks    cb;
    wUuid               tempUuid;
    char                uuidStringBuffer[UUID_STRING_BUF_SIZE];

    if (NULL == bridge || NULL == transport || NULL == queue || NULL == msgCB)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Allocate memory for the inbox implementation */
    impl = (baseInboxImpl*) calloc (1, sizeof (baseInboxImpl));
    if (NULL == impl)
    {
       return MAMA_STATUS_NOMEM;
    }

    status = mamaSubscription_allocate (&impl->mSubscription);
    if (MAMA_STATUS_OK != status)
    {
       mama_log (MAMA_LOG_LEVEL_ERROR,
                 "baseBridgeMamaInbox_createByIndex(): "
                 "Failed to allocate subscription ");
       mamaSubscription_deallocate (impl->mSubscription);
       free (impl);
       return status;
    }

    // NB: uuid_generate is very expensive, so we use cheaper uuid_generate_time
    wUuid_clear             (tempUuid);
    wUuid_generate_time     (tempUuid);
    wUuid_unparse           (tempUuid, uuidStringBuffer);

    /* Create the unique topic name allocated to this inbox */
    snprintf (impl->mInbox,
              sizeof (impl->mInbox) - 1,
              "_INBOX.%s",
              uuidStringBuffer);

    /* Set the mandatory callbacks for basic subscriptions */
    cb.onCreate             = &baseBridgeMamaInboxImpl_onCreate;
    cb.onError              = &baseBridgeMamaInboxImpl_onError;
    cb.onMsg                = &baseBridgeMamaInboxImpl_onMsg;
    cb.onDestroy            = &baseBridgeMamaInboxImpl_onDestroy;

    /* These callbacks are not used by basic subscriptions */
    cb.onQuality            = NULL;
    cb.onGap                = NULL;
    cb.onRecapRequest       = NULL;

    /* Initialize the remaining members for the base inbox implementation */
    impl->mClosure          = closure;
    impl->mMsgCB            = msgCB;
    impl->mErrCB            = errorCB;
    impl->mParent           = parent;
    impl->mOnInboxDestroyed = onInboxDestroyed;

    /* Subscribe to the inbox topic name */
    status = mamaSubscription_createBasic (impl->mSubscription,
                                           transport,
                                           queue,
                                           &cb,
                                           impl->mInbox,
                                           impl);
    if (MAMA_STATUS_OK != status)
    {
       mama_log (MAMA_LOG_LEVEL_ERROR,
                 "baseBridgeMamaInbox_createByIndex(): "
                 "Failed to create subscription ");
       mamaSubscription_deallocate (impl->mSubscription);
       free (impl);
       return status;
    }

    /* Populate the bridge with the newly created implementation */
    *bridge = (inboxBridge) impl;

    return MAMA_STATUS_OK;
}

mama_status
baseBridgeMamaInbox_destroy (inboxBridge inbox)
{
    baseInboxImpl* impl = (baseInboxImpl*) inbox;

    if (NULL != impl)
    {
        mamaSubscription_destroy    (impl->mSubscription);
        mamaSubscription_deallocate (impl->mSubscription);
        free (impl);
    }
    else
    {
        return MAMA_STATUS_NULL_ARG;
    }
    return MAMA_STATUS_OK;
}


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

const char*
baseBridgeMamaInboxImpl_getReplySubject (inboxBridge inbox)
{
    baseInboxImpl* impl = (baseInboxImpl*) inbox;
    if (NULL == impl)
    {
        return NULL;
    }
    return impl->mInbox;
}


/*=========================================================================
  =                  Private implementation functions                     =
  =========================================================================*/

/* Inbox bridge callbacks */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onMsg (mamaSubscription    subscription,
                               mamaMsg             msg,
                               void*               closure,
                               void*               itemClosure)
{
    baseInboxImpl* impl = (baseInboxImpl*) closure;
    if (NULL == impl)
    {
        return;
    }

    /* If a message callback is defined, call it */
    if (NULL != impl->mMsgCB)
    {
        (impl->mMsgCB)(msg, impl->mClosure);
    }
}

/* No additional processing is required on inbox creation */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onCreate (mamaSubscription    subscription,
                                  void*               closure)
{
}

/* Calls the implementation's destroy callback on execution */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onDestroy (mamaSubscription    subscription,
                                   void*               closure)
{
    /* The closure provided is the inbox implementation */
    baseInboxImpl* impl = (baseInboxImpl*) closure;
    if (NULL == impl)
    {
        return;
    }

    /* Call the inbox destroy callback if defined */
    if (NULL != impl->mOnInboxDestroyed)
    {
        (impl->mOnInboxDestroyed)(impl->mParent, impl->mClosure);
    }
}

/* Calls the implementation's error callback on execution */
static void MAMACALLTYPE
baseBridgeMamaInboxImpl_onError (mamaSubscription    subscription,
                                 mama_status         status,
                                 void*               platformError,
                                 const char*         subject,
                                 void*               closure)
{
    /* The closure provided is the inbox implementation */
    baseInboxImpl* impl = (baseInboxImpl*) closure;
    if (NULL == impl)
    {
        return;
    }

    /* Call the inbox error callback if defined */
    if (NULL != impl->mErrCB)
    {
        (impl->mErrCB)(status, impl->mClosure);
    }
}


