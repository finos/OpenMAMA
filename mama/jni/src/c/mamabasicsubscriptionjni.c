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
* Implementation for each of the native methods defined in the
* MamaBasicSubscription.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajni/com_wombat_mama_MamaBasicSubscription.h"
#include "subscriptioncommon.h"

/******************************************************************************
* Global/Static variables (NB - These are all read only after
* initialisation)
*******************************************************************************/
/*The controlling JVM - in mamajni.c*/
extern  JavaVM*     javaVM_g;

extern  jfieldID    queuePointerFieldId_g;/*Defined in mamaqueuejni.c*/
extern  jfieldID    messagePointerFieldId_g; /*Defined in mamamsgjni.c*/
extern  jfieldID    transportPointerFieldId_g; /*Defined in
                                                 mamatransportjni.c*/
static  jfieldID    subscriptionPointerFieldId_g    =   NULL;

/*For the callback methods on the subscription*/
static  jmethodID   subCallbackonCreateId_g         =   NULL;
static  jmethodID   subCallbackonMsgId_g            =   NULL;
static  jmethodID   subCallbackonErrorId_g          =   NULL;
static  jmethodID   subCallbackonDestroyId_g        =   NULL;

/*Invoked for normal updates*/
static void MAMACALLTYPE subscriptionMsgCB (
        mamaSubscription    subscription,
        mamaMsg             msg,
        void*               closure,
        void*               itemClosure);
                                                                                                                       
/*Invoked in response to errors*/
static void MAMACALLTYPE subscriptionErrorCB  (
        mamaSubscription    subscription,
        mama_status         status,
        void*               platformError,
        const char*         subject,
        void*               closure );
                                                                                                                       
/*Invoked when a subscription is first created*/
static void MAMACALLTYPE subscriptionCreateCB (
        mamaSubscription    subscription,
        void*               closure );

static void MAMACALLTYPE subscriptionDestroyCB (
    mamaSubscription subscription, 
    void *closure );

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    createNativeSubscription
 * Signature: (Lcom/wombat/mama/MamaBasicSubscriptionCallback;Lcom/wombat/mama/MamaTransport;Lcom/wombat/mama/MamaQueue;Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaBasicSubscription_createNativeSubscription
  (JNIEnv* env, jobject this, jobject callback, jobject transport,
   jobject queue, jstring topic, jobject closure)
{
    mama_status         status              =   MAMA_STATUS_OK;
    const char*         c_topic             =   NULL;
    jlong               transportPointer    =   0;
    jlong               subscriptionPointer =   0;
    mamaQueue           queue_c             =   NULL;
    jlong               queuePointer        =   0;
    jobject             messageImpl         =   NULL;
    mamaMsgCallbacks    c_callback;
    char                errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    callbackClosure*    closureImpl         = (callbackClosure*)calloc(1,
                                              sizeof(callbackClosure));
    if (!closureImpl)
    {
        utils_throwMamaException(env,"createBasicSubscription():"
                " Could not allocate.");
        return;
    }

    closureImpl->mUserData      =   NULL;
    closureImpl->mClientCB      =   NULL;
    closureImpl->mSubscription  =   NULL;
    closureImpl->mMessage       =   NULL;

    /*Setup the callback structure*/
    memset(&c_callback, 0, sizeof(c_callback));
    c_callback.onCreate   = (wombat_subscriptionCreateCB)subscriptionCreateCB;
    c_callback.onError    = (wombat_subscriptionErrorCB)subscriptionErrorCB;
    c_callback.onMsg      = (wombat_subscriptionOnMsgCB)subscriptionMsgCB;
    c_callback.onDestroy  = (wombat_subscriptionDestroyCB)subscriptionDestroyCB;
    c_callback.onQuality  = NULL;/*Not used for basic subscriptions*/

    c_topic = (*env)->GetStringUTFChars(env,topic,0);
    if (!c_topic) return;/*Exception auto thrown*/

    transportPointer = (*env)->GetLongField(env, transport,
            transportPointerFieldId_g);

    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);
    
    /*If the client supplied a Java closure add it to ours*/
    if(closure)closureImpl->mUserData = (*env)->NewGlobalRef(env,closure);

    /*Check if a queue was specified.*/
    if(queue)
    {
        /* Get the queue pointer value from the MamaQueue java object */
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /*Create a reuseable message object to hang off the subscription*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl)
    {
        if(c_topic)(*env)->ReleaseStringUTFChars(env,topic, c_topic);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return;
    }/*Exception will have been thrown*/
    /*This global will be deleted when the subscription is destroyed*/
    closureImpl->mMessage  = (*env)->NewGlobalRef(env,messageImpl);

    /*Add the Java Subscription to the closure - we need it in the 
     async callbacks so it can be passed to the Java callback*/
    closureImpl->mSubscription = (*env)->NewGlobalRef(env,this);

    subscriptionPointer = (*env)->GetLongField(env, this,
            subscriptionPointerFieldId_g);

    /*Actually create the C basic Subscription*/
    if(MAMA_STATUS_OK!=(status=mamaSubscription_createBasic(
                     CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                     CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                     queue_c,
                     &c_callback,
                     c_topic,
                     closureImpl)))
                     
    {
        if(c_topic)(*env)->ReleaseStringUTFChars(env,topic, c_topic);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        (*env)->DeleteGlobalRef(env,closureImpl->mSubscription);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create basic subscription.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }
    
    /*Tidy up*/
    if(c_topic)(*env)->ReleaseStringUTFChars(env,topic, c_topic);
    return;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaBasicSubscription_destroy
  (JNIEnv* env, jobject this)
{
    /*Delegate to utility destroy function*/
    subCommon_destroy(env,this,subscriptionPointerFieldId_g);
    return;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    allocateSubscription
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaBasicSubscription_allocateSubscription
  (JNIEnv* env, jobject this)
{
     mamaSubscription subscription       = NULL;
     mama_status      status             = MAMA_STATUS_OK;
     char             errorString[UTILS_MAX_ERROR_STRING_LENGTH];
     
     if(MAMA_STATUS_OK!=(status =
                 mamaSubscription_allocate(&subscription)))
     {
         utils_buildErrorStringForStatus(
                 errorString,
                 UTILS_MAX_ERROR_STRING_LENGTH,
                 "Could not allocate mamaSubscriptoin.",
                 status);
         utils_throwMamaException(env,errorString);
         return;
     }
     
     /*Add the subscription pointer to the Java object field*/
     (*env)->SetLongField(env, this,
                          subscriptionPointerFieldId_g,
                          CAST_POINTER_TO_JLONG(subscription));
     return;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    getSymbol
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaBasicSubscription_getSymbol
  (JNIEnv* env, jobject this)
{
    const char*      retVal_c            =   NULL;
    mama_status      status              =   MAMA_STATUS_OK;
    jlong            subscriptionPointer =   0;
    char             errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                        subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaBasicSubscription.getSymbol(): Null parameter, subscription may have been destroyed.", NULL);

    assert(0!=subscriptionPointer);

    if(MAMA_STATUS_OK!=(status=mamaSubscription_getSubscSymbol(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Symbol for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, retVal_c);
}
                

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaBasicSubscription_initIDs
  (JNIEnv* env, jclass class)
{
    jclass   basicSubscriptionCallbackClass  =   NULL;

    /*A reference to the */
    subscriptionPointerFieldId_g = (*env)->GetFieldID(env,
            class, "basicSubscriptionPointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!subscriptionPointerFieldId_g) return;/*Exception auto thrown*/

    /*Get a reference to the subscription Callback class*/
    basicSubscriptionCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaBasicSubscriptionCallback");
    if (!basicSubscriptionCallbackClass) return;/*Exception auto thrown*/

    /*MamaSubscriptionCallback.onMsg()*/
    subCallbackonMsgId_g = (*env)->GetMethodID(env,
            basicSubscriptionCallbackClass,
            "onMsg", "(Lcom/wombat/mama/MamaBasicSubscription;"
            "Lcom/wombat/mama/MamaMsg;)V" );
    if (!subCallbackonMsgId_g)
    {
        (*env)->DeleteLocalRef(env, basicSubscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

     /*MamaSubscriptionCallback.onError()*/
    subCallbackonErrorId_g = (*env)->GetMethodID(env,
            basicSubscriptionCallbackClass,
            "onError", "(Lcom/wombat/mama/MamaBasicSubscription;"
            "SILjava/lang/String;)V" );
    if(!subCallbackonErrorId_g)
    {
        (*env)->DeleteLocalRef(env, basicSubscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

    /*MamaSubscriptionCallback.onCreate()*/
    subCallbackonCreateId_g = (*env)->GetMethodID(env,
            basicSubscriptionCallbackClass,
            "onCreate",
            "(Lcom/wombat/mama/MamaBasicSubscription;)V" );
    if (!subCallbackonCreateId_g)
    {
        (*env)->DeleteLocalRef(env, basicSubscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

    /* Get the destroy callback. */
    subCallbackonDestroyId_g =  (*env)->GetMethodID(env, basicSubscriptionCallbackClass,
            "onDestroy", 
            "(Lcom/wombat/mama/MamaBasicSubscription;)V" );
    if (!subCallbackonDestroyId_g)
    {
        (*env)->DeleteLocalRef(env, basicSubscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }
    return;
}

void MAMACALLTYPE subscriptionMsgCB (
        mamaSubscription    subscription,
        mamaMsg             msg,
        void*               closure,
        void*               itemClosure)
{
    /*Delegate to common function*/
    subCommon_onMsgCb (subscription,
                       msg,
                       closure,
                       messagePointerFieldId_g,
                       subCallbackonMsgId_g);
    return;
}


void MAMACALLTYPE subscriptionErrorCB  (
        mamaSubscription    subscription,
        mama_status         status,
        void*               platformError,
        const char*         subject,
        void*               closure )
{
     subCommon_onErrorCb (subscription,
                          status,
                          platformError,
                          subject,
                          closure,
                          subCallbackonErrorId_g);
    return;      
}

void MAMACALLTYPE subscriptionCreateCB (
        mamaSubscription    subscription,
        void*               closure )
{
    subCommon_createCb (subscription,
                        closure,
                        subscriptionPointerFieldId_g,
                        subCallbackonCreateId_g);
    return;
}

void MAMACALLTYPE subscriptionDestroyCB (mamaSubscription subscription, void *closure )
{
    subCommon_destroyCb (subscription,
                     closure,
                     subscriptionPointerFieldId_g,
                     subCallbackonDestroyId_g);
    return;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    getSubscriptionState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaBasicSubscription_getSubscriptionState
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    jint            state               =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaBasicSubscription.getItemClosure(): Null parameter, subcription may have been destroyed.", NULL);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getState(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    (mamaSubscriptionState *)&state)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get item closure for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    
    return state;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    destroyEx
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaBasicSubscription_destroyEx
  (JNIEnv* env, jobject this)
{
    jlong           subscriptionPointer =   0;

    subscriptionPointer =
            (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaBasicSubscription.destroy(): Null parameter, subscription may have already been destroyed."); 

    assert(0!=subscriptionPointer);

    subCommon_destroyEx(env,this,subscriptionPointerFieldId_g);
    return;
}

/*
 * Class:     com_wombat_mama_MamaBasicSubscription
 * Method:    deallocate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaBasicSubscription_deallocate(JNIEnv *env, jobject this)
{
    /* Get the mama subscription pointer. */
    jlong subscriptionPointer =(*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    if(0 != subscriptionPointer)
    {
        /* Deallocate the mama subscription. */
        mama_status msd = mamaSubscription_deallocate(CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer));
        if(MAMA_STATUS_OK == msd)
        {
            /* Clear the subscription reference in the java class. */
            (*env)->SetLongField(env, this, subscriptionPointerFieldId_g, 0);
        }
        
        /* Otherwise throw an error. */
        else
        {
            /* Format the error string. */
            char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaBasicSubscription.deallocate(): Could not deallocate mamaSubscriptoin.",
                msd);

            /* Throw the exception. */
            utils_throwMamaException(env, errorString); 
        }
    }
}
