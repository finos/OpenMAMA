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

/*
* Common functions used by both the mamasubscription and mamabasicsubscription
* implementations.
*/

/******************************************************************************
* Includes
*******************************************************************************/
#include "subscriptioncommon.h"

/******************************************************************************
* Created once on startup
*******************************************************************************/
extern  JavaVM*     javaVM_g;
extern jfieldID    messagePointerFieldId_g;

/*Used from the destroy functions of the subscriptions*/
void subCommon_destroy(JNIEnv* env, jobject subscription,
                              jfieldID    subscriptionPointerFieldId)
{
    mama_status status          =   MAMA_STATUS_OK;
    void*       closure         =   NULL;
    jlong       pointerValue    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    pointerValue =
        (*env)->GetLongField(env,subscription,subscriptionPointerFieldId);
        
    mamaSubscription_getClosure(
            CAST_JLONG_TO_POINTER(mamaSubscription,pointerValue),&closure);

    if (0!=subscriptionPointerFieldId)
    {
        /* Destroy the subscription, clean-up will be performed in the callback. */
        if (MAMA_STATUS_OK != (status = mamaSubscription_destroy
                    (CAST_JLONG_TO_POINTER(mamaSubscription,pointerValue))))
        {
            utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "Failed to destroy subscription.",
                    status);
            utils_throwMamaException(env,errorString);
        }
    }

    return;
}

/*Used from the destroy functions of the subscriptions*/
void subCommon_destroyEx(JNIEnv* env, jobject subscription,
                              jfieldID    subscriptionPointerFieldId)
{
    mama_status status          =   MAMA_STATUS_OK;
    void*       closure         =   NULL;
    jlong       pointerValue    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    pointerValue =
        (*env)->GetLongField(env,subscription,subscriptionPointerFieldId);
        
    mamaSubscription_getClosure(
            CAST_JLONG_TO_POINTER(mamaSubscription,pointerValue),&closure);

    if (0!=subscriptionPointerFieldId)
    {
        if (MAMA_STATUS_OK != (status = mamaSubscription_destroyEx
                    (CAST_JLONG_TO_POINTER(mamaSubscription,pointerValue))))
        {
            utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "Failed to destroy subscription.",
                    status);
            utils_throwMamaException(env,errorString);
        }

		/*It's different here.  DestroyEx in C actually doesn't destroy it right away.  
		 We need to wait for the callback. */
	}
}


void subCommon_createCb ( mamaSubscription    subscription,
                          void*               closure,
                          jfieldID            subscriptionPointerFieldId,
                          jmethodID           subCallbackonCreateId)
{
    JNIEnv*             env                      =   NULL;
    jobject             messageImpl              =   NULL;
    callbackClosure*    closureImpl              =   NULL;
    
    closureImpl = (callbackClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mSubscription!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    
    /*Invoke the onCreate() callback method!!*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           subCallbackonCreateId,
                           closureImpl->mSubscription);
    return;
}

                                                                                                                       
void subCommon_onErrorCb ( mamaSubscription    subscription,
                           mama_status         status,
                           void*               platformError,
                           const char*         subject,
                           void*               closure,
                           jmethodID           subCallbackonErrorId)
{
     JNIEnv*             env             =   NULL;
     callbackClosure*    closureImpl     =   NULL;
     
     closureImpl = (callbackClosure*)closure;
     assert(closureImpl!=NULL);
     assert(closureImpl->mClientCB!=NULL);
     assert(closureImpl->mSubscription!=NULL);
     
     /*Get the env for the current thread*/
     env = utils_getENV(javaVM_g);
     if (!env) return;

     (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                            subCallbackonErrorId,
                            closureImpl->mSubscription,
                            status,0,(*env)->NewStringUTF(env, subject),
                            NULL/*We have no exceptions to propagate!*/);
    return;
}
        
void subCommon_qualityCb ( mamaSubscription subscription,
                           mamaQuality      quality,
                           const char*      symbol,
                           short            cause,
                           const void*      platformInfo,
                           void*            closure,
                           jmethodID        subCallbackonQualityId)
{
     JNIEnv*             env             =   NULL;
     callbackClosure*    closureImpl     =   NULL;
     
     closureImpl = (callbackClosure*)closure;
     assert(closureImpl!=NULL);
     assert(closureImpl->mClientCB!=NULL);
     assert(closureImpl->mSubscription!=NULL);
     
     /*Get the env for the current thread*/
     env = utils_getENV(javaVM_g);
     if (!env) return;
     
     (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                            subCallbackonQualityId,
                            closureImpl->mSubscription,
                            quality,
                            cause,
                            NULL/*We have no exceptions to propagate!*/);
    return;
}

void subCommon_onMsgCb ( mamaSubscription    subscription,
                         mamaMsg             msg,
                         void*               closure,
                         jfieldID            messagePointerFieldId,
                         jmethodID           subCallbackonMsgId)
{
    JNIEnv*             env             =   NULL;
    callbackClosure*    closureImpl     =   (callbackClosure*)closure;

    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mSubscription!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv!!*/
    
    (*env)->SetLongField(env, closureImpl->mMessage,
                         messagePointerFieldId,
                         CAST_POINTER_TO_JLONG(msg));
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           subCallbackonMsgId,
                           closureImpl->mSubscription,
                           closureImpl->mMessage);

    /* 
       Need to check if any exceptions were propagated here.
       If we don't the exceptions could actually fill the stack!!
    */
    utils_printAndClearExceptionFromStack (env,"onMsg");

    return;
}

void subCommon_gapCb (mamaSubscription    subscription,
                      void*               closure,
                      jfieldID            subscriptionPointerFieldId,
                      jmethodID           subCallbackonRecapId)
{
    JNIEnv*             env                      =   NULL;
    callbackClosure*    closureImpl              =   NULL;
    
    closureImpl = (callbackClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mSubscription!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    
    /*Invoke the onGap() callback method!!*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           subCallbackonRecapId,
                           closureImpl->mSubscription);
    return;
}

void subCommon_recapCb (mamaSubscription    subscription,
                        void*               closure,
                        jfieldID            subscriptionPointerFieldId,
                        jmethodID           subCallbackonRecapId)
{
    JNIEnv*             env                      =   NULL;
    callbackClosure*    closureImpl              =   NULL;
    
    closureImpl = (callbackClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mSubscription!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    
    /*Invoke the onRecap() callback method!!*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           subCallbackonRecapId,
                           closureImpl->mSubscription);
    return;
}

void subCommon_destroyCb(mamaSubscription  subscription, 
                                      void*             closure, 
                                      jfieldID          subscriptionPointerFieldId, 
                                      jmethodID         subCallbackonDestroyId)
{
    /*Get the env for the current thread*/
    JNIEnv *env = utils_getENV(javaVM_g);
    if(NULL != env)
    {
        /* Get the closure impl. */
        callbackClosure *closureImpl = (callbackClosure*)closure;
        if(NULL != closureImpl)
        {
            /* Verify that the structure members are valid. */            
            assert(closureImpl->mClientCB!=NULL);
            assert(closureImpl->mSubscription!=NULL);

            (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                                   subCallbackonDestroyId,
                                   closureImpl->mSubscription);
                    
            {
                /* The closure must only be destroyed if the subscription has been destroyed
                 * and not de-activated.
                 */
                mamaSubscriptionState state = MAMA_SUBSCRIPTION_UNKNOWN;
                mamaSubscription_getState(subscription, &state);
                if((MAMA_SUBSCRIPTION_DESTROYED == state) || (MAMA_SUBSCRIPTION_DEALLOCATED == state))
                {
                    /* Free the references to each of the objects. */
                    if(closureImpl->mUserData)
                    {
                        (*env)->DeleteGlobalRef(env,closureImpl->mUserData);                        
                    }

                    if(closureImpl->mClientCB)
                    {
                        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);                        
                    }

                    if(closureImpl->mMessage)
                    {
                        (*env)->SetLongField(env, closureImpl->mMessage, messagePointerFieldId_g, 0);
                        (*env)->DeleteGlobalRef(env,closureImpl->mMessage);                        
                    }

                    if(closureImpl->mSubscription)
                    {
			            /* Delete the global ref. */
                        (*env)->DeleteGlobalRef(env,closureImpl->mSubscription);                        
                    }

                    /* Free the impl itself. */
                    free(closureImpl);
                }
            }
        }
    }	
}
