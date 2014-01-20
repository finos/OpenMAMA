/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <string.h>
#include <wombat/wUuid.h>
#include <wombat/port.h>
#include <mama/mama.h>
#include <bridge.h>
#include "qpidbridgefunctions.h"
#include "qpiddefs.h"
#include "inbox.h"


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define                 UUID_STRING_BUF_SIZE                37


/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidInboxImpl
{
    char                            mInbox[MAX_SUBJECT_LENGTH];
    mamaSubscription                mSubscription;
    void*                           mClosure;
    mamaInboxMsgCallback            mMsgCB;
    mamaInboxErrorCallback          mErrCB;
    mamaInboxDestroyCallback        mOnInboxDestroyed;
    mamaInbox                       mParent;
} qpidInboxImpl;

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
qpidBridgeMamaInboxImpl_onMsg       (mamaSubscription    subscription,
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
qpidBridgeMamaInboxImpl_onCreate    (mamaSubscription    subscription,
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
qpidBridgeMamaInboxImpl_onDestroy   (mamaSubscription    subscription,
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
qpidBridgeMamaInboxImpl_onError     (mamaSubscription    subscription,
                                     mama_status         status,
                                     void*               platformError,
                                     const char*         subject,
                                     void*               closure);


/*=========================================================================
  =               Public interface implementation functions               =
  =========================================================================*/

mama_status
qpidBridgeMamaInbox_create          (inboxBridge*             bridge,
                                     mamaTransport            transport,
                                     mamaQueue                queue,
                                     mamaInboxMsgCallback     msgCB,
                                     mamaInboxErrorCallback   errorCB,
                                     mamaInboxDestroyCallback onInboxDestroyed,
                                     void*                    closure,
                                     mamaInbox                parent)
{
    return qpidBridgeMamaInbox_createByIndex (bridge,
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
qpidBridgeMamaInbox_createByIndex   (inboxBridge*             bridge,
                                     mamaTransport            transport,
                                     int                      tportIndex,
                                     mamaQueue                queue,
                                     mamaInboxMsgCallback     msgCB,
                                     mamaInboxErrorCallback   errorCB,
                                     mamaInboxDestroyCallback onInboxDestroyed,
                                     void*                    closure,
                                     mamaInbox                parent)
{
    qpidInboxImpl*      impl        = NULL;
    mama_status         status      = MAMA_STATUS_OK;
    mamaMsgCallbacks    cb;
    wUuid               tempUuid;
    char                uuidStringBuffer[UUID_STRING_BUF_SIZE];

    if (NULL == bridge || NULL == transport || NULL == queue || NULL == msgCB)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Allocate memory for the qpid inbox implementation */
    impl = (qpidInboxImpl*) calloc (1, sizeof (qpidInboxImpl));
    if (NULL == impl)
    {
       return MAMA_STATUS_NOMEM;
    }

    status = mamaSubscription_allocate (&impl->mSubscription);
    if (MAMA_STATUS_OK != status)
    {
       mama_log (MAMA_LOG_LEVEL_ERROR,
                 "qpidBridgeMamaInbox_createByIndex(): "
                 "Failed to allocate subscription ");
       mamaSubscription_deallocate (impl->mSubscription);
       free (impl);
       return status;
    }

    // NB: uuid_generate is very expensive, so we use cheaper uuid_generate_time
    wUuid_generate_time     (tempUuid);
    wUuid_unparse           (tempUuid, uuidStringBuffer);

    /* Create the unique topic name allocated to this inbox */
    snprintf (impl->mInbox,
              sizeof (impl->mInbox) - 1,
              "_INBOX.%s",
              uuidStringBuffer);

    /* Set the mandatory callbacks for basic subscriptions */
    cb.onCreate             = &qpidBridgeMamaInboxImpl_onCreate;
    cb.onError              = &qpidBridgeMamaInboxImpl_onError;
    cb.onMsg                = &qpidBridgeMamaInboxImpl_onMsg;
    cb.onDestroy            = &qpidBridgeMamaInboxImpl_onDestroy;

    /* These callbacks are not used by basic subscriptions */
    cb.onQuality            = NULL;
    cb.onGap                = NULL;
    cb.onRecapRequest       = NULL;

    /* Initialize the remaining members for the qpid inbox implementation */
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
                 "qpidBridgeMamaInbox_createByIndex(): "
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
qpidBridgeMamaInbox_destroy (inboxBridge inbox)
{
    qpidInboxImpl* impl = (qpidInboxImpl*) inbox;

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
qpidBridgeMamaInboxImpl_getReplySubject (inboxBridge inbox)
{
    qpidInboxImpl* impl = (qpidInboxImpl*) inbox;
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
qpidBridgeMamaInboxImpl_onMsg (mamaSubscription    subscription,
                               mamaMsg             msg,
                               void*               closure,
                               void*               itemClosure)
{
    qpidInboxImpl* impl = (qpidInboxImpl*) closure;
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
qpidBridgeMamaInboxImpl_onCreate (mamaSubscription    subscription,
                                  void*               closure)
{
}

/* Calls the implementation's destroy callback on execution */
static void MAMACALLTYPE
qpidBridgeMamaInboxImpl_onDestroy (mamaSubscription    subscription,
                                   void*               closure)
{
    /* The closure provided is the qpid inbox implementation */
    qpidInboxImpl* impl = (qpidInboxImpl*) closure;
    if (NULL == impl)
    {
        return;
    }

    /* Call the qpid inbox destroy callback if defined */
    if (NULL != impl->mOnInboxDestroyed)
    {
        (impl->mOnInboxDestroyed)(impl->mParent, impl->mClosure);
    }
}

/* Calls the implementation's error callback on execution */
static void MAMACALLTYPE
qpidBridgeMamaInboxImpl_onError (mamaSubscription    subscription,
                                 mama_status         status,
                                 void*               platformError,
                                 const char*         subject,
                                 void*               closure)
{
    /* The closure provided is the qpid inbox implementation */
    qpidInboxImpl* impl = (qpidInboxImpl*) closure;
    if (NULL == impl)
    {
        return;
    }

    /* Call the qpid inbox error callback if defined */
    if (NULL != impl->mErrCB)
    {
        (impl->mErrCB)(status, impl->mClosure);
    }
}


