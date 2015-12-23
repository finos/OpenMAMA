/* $Id:
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
 * MamaPublisher.java source file.
 */

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaPublisher.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/

/* Used as a closure to the underlying enqueueEvent call */
typedef struct sendMsgCallback_
{
    /*The Java MamaQueueEnqueueCallback instance. */
    jobject mClientJavaCallback;

    /* Reference to the client suppliend Java object closure */
    jobject mClientClosure;
} sendMsgCallbackClosure;

typedef struct pubCallbackClosure
{
    /* The callback object for the publisher. This is stored as a
       global reference and must be deleted when the publisher is deleted
     */
    jobject mClientCb;
    
    /* The java publisher object. Stored as a global reference
       and deleted when the publisher is destroyed.
     */
    jobject  mPublisher;
} pubCallbackClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
static jfieldID     publisherPointerFieldId_g          = NULL;
static jfieldID     mamaTransportObjectFieldId_g       = NULL;

extern jfieldID     transportPointerFieldId_g;
extern jfieldID     queuePointerFieldId_g;
extern jfieldID     messagePointerFieldId_g;
extern jfieldID     inboxPointerFieldId_g;

static jmethodID    sendCallbackMethod_g               = NULL;

static jmethodID    publisherCallbackMethoOnCreate_g   = NULL;
static jmethodID    publisherCallbackMethoOnDestroy_g  = NULL;
static jmethodID    publisherCallbackMethoOnError_g    = NULL;

extern  JavaVM*     javaVM_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/

static void MAMACALLTYPE sendCompleteCB (mamaPublisher publisher,
                                         mamaMsg       msg,
                                         mama_status   status,
                                         void*         closure);

/** Publisher event callback methods
 */
static void publisherOnCreateCb (mamaPublisher publisher, void* closure)
{
    JNIEnv* env = utils_getENV(javaVM_g);
    pubCallbackClosure* closureImpl = (pubCallbackClosure*) closure;

    /* Need to set this here since the create call has not yet done it */
    (*env)->SetLongField(env, closureImpl->mPublisher,
                         publisherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(publisher));

    /* Invoke the onCreate() callback method */
    (*env)->CallVoidMethod(env, closureImpl->mClientCb,
                           publisherCallbackMethoOnCreate_g,
                           closureImpl->mPublisher);
}

static void publisherOnDestroyCb (mamaPublisher publisher, void* closure)
{
    JNIEnv* env = utils_getENV(javaVM_g);
    pubCallbackClosure* closureImpl = (pubCallbackClosure*) closure;

    /* Invoke the onDestroy() callback method */
    (*env)->CallVoidMethod(env, closureImpl->mClientCb,
                           publisherCallbackMethoOnDestroy_g,
                           closureImpl->mPublisher);

    /* Release the Java objects for JVM GC */
    (*env)->DeleteGlobalRef(env, closureImpl->mPublisher);
    (*env)->DeleteGlobalRef(env, closureImpl->mClientCb);
    free((void*) closureImpl);
}

static void publisherOnErrorCb (mamaPublisher publisher,
                                mama_status status,
                                const char*   info,
                                void* closure)
{
    JNIEnv* env = utils_getENV(javaVM_g);
    pubCallbackClosure* closureImpl = (pubCallbackClosure*) closure;

    /* Invoke the onError() callback method */
    jstring jmsg = (*env)->NewStringUTF(env, info);
    (*env)->CallVoidMethod(env, closureImpl->mClientCb,
                           publisherCallbackMethoOnError_g,
                           closureImpl->mPublisher,
                           status,
                           jmsg);
     (*env)->DeleteLocalRef(env, jmsg);        /* delete this since this thread is not from the JVM */
}
        
/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _getTransport
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1getTransport
(JNIEnv * env, jobject this)
{
    jlong           publisherPointer    = 0;
    mamaTransport   c_result            = NULL;
    jobject         result              = NULL;
    mama_status     status              = MAMA_STATUS_OK;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    assert(publisherPointerFieldId_g!=NULL);
    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(publisherPointer,
        "Null parameter, MamaPublisher may have already been destroyed.");

    if (MAMA_STATUS_OK!=(mamaPublisher_getTransport(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    &c_result)))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get mamaTransport from publisher.", status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }

    /*re-use the existing field object */
    if (c_result)
    {
        result = (*env)->GetObjectField(env,this,
                                        mamaTransportObjectFieldId_g);
        assert(NULL!=result);/*throws an exception*/

        (*env)->SetLongField(env, result, transportPointerFieldId_g,
                             CAST_POINTER_TO_JLONG(c_result));
    }
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _create
 * Signature: (Lcom/wombat/mama/MamaTransport;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1create
  (JNIEnv* env, jobject this, jobject transport, jobject queue, jstring topic, jstring source, jobject callback)
{
    mamaPublisher   cPublisher          =   NULL;
    mamaTransport   cTransport          =   NULL;
    mamaQueue       cQueue              =   NULL;
    const char*     cSource             =   NULL;
    const char*     cTopic              =   NULL;
    jlong           transportPointer    =   0;
    jlong           queuePointer        =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    /* Get the transport pointer */
    assert(transport!=NULL);
    transportPointer = (*env)->GetLongField(env, transport,
            transportPointerFieldId_g);
    assert(transportPointer!=0);
    cTransport = CAST_JLONG_TO_POINTER(mamaTransport, transportPointer);

    /* Get the queue pointer (may be null) */
    if (NULL != queue)
    {
        queuePointer = (*env)->GetLongField(env, queue,
            queuePointerFieldId_g);
        cQueue = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /* Get the char* from the jstring */
    if (NULL!=topic)
    {
         cTopic = (*env)->GetStringUTFChars(env,topic,0);
         if (!cTopic)return;
    }
    if (NULL!=source)
    {
         cSource = (*env)->GetStringUTFChars(env,source,0);
         if (!cSource)return;
    }

    if (NULL != callback)
    {
        mamaPublisherCallbacks* cb = NULL;
        pubCallbackClosure* closureImpl = (pubCallbackClosure*) calloc(1, sizeof(pubCallbackClosure));
        if (!closureImpl)
        {
            utils_throwMamaException(env,"MamaPublisher_create: Could not allocate.");
            return;
        }
        closureImpl->mPublisher = (*env)->NewGlobalRef(env, this);
        closureImpl->mClientCb  = (*env)->NewGlobalRef(env, callback);

        mamaPublisherCallbacks_allocate(&cb);
        cb->onCreate = publisherOnCreateCb;
        cb->onDestroy = publisherOnDestroyCb;
        cb->onError = publisherOnErrorCb;

        status = mamaPublisher_createWithCallbacks(
                    &cPublisher,
                    cTransport,
                    cQueue, 
                    cTopic,
                    cSource,
                    NULL,            // root
                    cb,
                    (void*) closureImpl);    // closure
        mamaPublisherCallbacks_deallocate(cb);
    }
    else
    {
        status = mamaPublisher_create(
                    &cPublisher,
                    cTransport,
                    cTopic,
                    cSource,
                    NULL);
    }


    if (MAMA_STATUS_OK != status)
    {
        if (cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        if (cSource)(*env)->ReleaseStringUTFChars(env,source, cSource);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env,this,publisherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPublisher));
        
    if (cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
    if (cSource)(*env)->ReleaseStringUTFChars(env,source, cSource);
    
    return;
}
 
/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _send
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1send
  (JNIEnv* env, jobject this, jobject msg)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);

    if (MAMA_STATUS_OK!=(mamaPublisher_send(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer))))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from publisher.", status);
        utils_throwMamaException(env,errorString);
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendWithThrottle
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendWithThrottle
 (JNIEnv* env, jobject this, jobject msg)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);

    if (MAMA_STATUS_OK!=(mamaPublisher_sendWithThrottle(
                         CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                         CAST_JLONG_TO_POINTER(mamaMsg,messagePointer),
                         NULL,
                         NULL)))
    {
          utils_buildErrorStringForStatus(
                    errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                    "Failed to send from publisher.", status);
         utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendWithThrottleWithCallback
 * Signature: (Lcom/wombat/mama/Msg;Lcom/wombat/mama/MamaThrottleCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendWithThrottleWithCallback
  (JNIEnv* env, jobject this, jobject msg, jobject callback)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    sendMsgCallbackClosure*    closureData         = NULL;
    mama_status     status              =   MAMA_STATUS_OK;

    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    closureData = (sendMsgCallbackClosure*) calloc (1, sizeof (sendMsgCallbackClosure));
    if (!closureData)
    {
        utils_throwMamaException(env,"sendWithThrottleWithCallback(): Could not allocate.");
        return;
    }

    closureData->mClientJavaCallback = (*env)->NewGlobalRef(env,callback);
    closureData->mClientClosure      = (*env)->NewGlobalRef(env,msg);

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);

    if (MAMA_STATUS_OK!=(mamaPublisher_sendWithThrottle(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer),
                    &sendCompleteCB,
                    closureData)))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from publisher.", status);
        utils_throwMamaException(env,errorString);
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendReplyToInbox
 * Signature: (Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendReplyToInbox
  (JNIEnv* env, jobject this, jobject request, jobject reply)
{
    jlong           requestMessagePointer       =   0;
    jlong           replyMessagePointer         =   0;
    jlong           publisherPointer            =   0;
    mama_status     status                      =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    requestMessagePointer = (*env)->GetLongField(env,request,messagePointerFieldId_g);
    assert(requestMessagePointer!=0);
    replyMessagePointer = (*env)->GetLongField(env,reply,messagePointerFieldId_g);
    assert(replyMessagePointer!=0);

    if (MAMA_STATUS_OK!=(mamaPublisher_sendReplyToInbox(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,requestMessagePointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,replyMessagePointer))))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from publisher.", status);
        utils_throwMamaException(env,errorString);
    }
    return;
}
 
/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendFromInbox
 * Signature: (Lcom/wombat/mama/MamaInbox;Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendFromInbox
  (JNIEnv* env, jobject this, jobject inbox, jobject msg)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    jlong           inboxPointer        =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);
    
    inboxPointer = (*env)->GetLongField(env,inbox,inboxPointerFieldId_g);
    assert(inboxPointer!=0);
    
     if (MAMA_STATUS_OK!=(mamaPublisher_sendFromInbox(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaInbox,inboxPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer))))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from inbox.", status);
        utils_throwMamaException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendFromInboxWithThrottle
 * Signature: (Lcom/wombat/mama/MamaInbox;Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendFromInboxWithThrottle
  (JNIEnv* env, jobject this, jobject inbox, jobject msg)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    jlong           inboxPointer        =   0;
    sendMsgCallbackClosure*    closureData         = NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    closureData = (sendMsgCallbackClosure*) calloc (1, sizeof (sendMsgCallbackClosure));
    if (!closureData)
    {
        utils_throwMamaException(env,"sendWithThrottleWithCallback(): Could not allocate.");
        return;
    }

    closureData->mClientClosure      = (*env)->NewGlobalRef(env,msg);

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);
    
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);
    
    inboxPointer = (*env)->GetLongField(env,inbox,inboxPointerFieldId_g);
    assert(inboxPointer!=0);
    
     if (MAMA_STATUS_OK!=(mamaPublisher_sendFromInboxWithThrottle(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaInbox,inboxPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer),
                    sendCompleteCB,
                    closureData)))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from inbox with throttle.", status);
        utils_throwMamaException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _sendFromInboxWithThrottleWithCallback
 * Signature: (Lcom/wombat/mama/MamaInbox;Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaThrottleCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1sendFromInboxWithThrottleWithCallback
  (JNIEnv* env, jobject this, jobject inbox, jobject msg, jobject callback)
{
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    jlong           inboxPointer        =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    sendMsgCallbackClosure*    closureData         = NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    closureData = (sendMsgCallbackClosure*) calloc (1, sizeof (sendMsgCallbackClosure));
    if (!closureData)
    {
        utils_throwMamaException(env,"sendWithThrottleWithCallback(): Could not allocate.");
        return;
    }

    closureData->mClientJavaCallback = (*env)->NewGlobalRef(env,callback);
    closureData->mClientClosure      = (*env)->NewGlobalRef(env,msg);

    publisherPointer = (*env)->GetLongField(env,this,publisherPointerFieldId_g);
    assert(publisherPointer!=0);

    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);

    inboxPointer = (*env)->GetLongField(env,inbox,inboxPointerFieldId_g);
    assert(inboxPointer!=0);

     if (MAMA_STATUS_OK!=(mamaPublisher_sendFromInboxWithThrottle(
                    CAST_JLONG_TO_POINTER(mamaPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaInbox,inboxPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer),
                    sendCompleteCB,
                    closureData)))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from inbox with throttle.", status);
        utils_throwMamaException(env,errorString);
    }
    return;
}
  
static void MAMACALLTYPE sendCompleteCB (mamaPublisher publisher,
                                         mamaMsg       msg,
                                         mama_status   status,
                                         void*         closure)
{
    JNIEnv*              env         = NULL;
    sendMsgCallbackClosure* closureData = (sendMsgCallbackClosure*)closure;

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);

    if (closureData && (closureData->mClientJavaCallback))
        (*env)->CallVoidMethod(env, closureData->mClientJavaCallback, sendCallbackMethod_g);

    if (closureData && (closureData->mClientJavaCallback))
        (*env)->DeleteGlobalRef(env,closureData->mClientJavaCallback);

    if (closure)
        free((void*) closureData);
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    getRoot
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaPublisher_getRoot
  (JNIEnv* env, jobject this)
{
    const char* root = NULL;
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
	jstring jmsg;

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(publisherPointer,
        "MamaPublisher.getRoot: Null parameter, publisher may have been destroyed.", NULL);
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_getRoot(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer),
                    &root)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Root for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return NULL;
    }

    jmsg = (*env)->NewStringUTF(env, root);
    (*env)->DeleteLocalRef(env, jmsg);
    return jmsg;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    getSource
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaPublisher_getSource
  (JNIEnv* env, jobject this)
{
    const char* source = NULL;
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
	jstring jmsg;

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(publisherPointer,
        "MamaPublisher.getSource: Null parameter, publisher may have been destroyed.", NULL);
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_getSource(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer),
                    &source)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Source for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return NULL;
    }

    jmsg = (*env)->NewStringUTF(env, source);
    (*env)->DeleteLocalRef(env, jmsg);
    return jmsg;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    getSymbol
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaPublisher_getSymbol
  (JNIEnv* env, jobject this)
{
    const char* symbol = NULL;
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
	jstring jmsg;

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(publisherPointer,
        "MamaPublisher.getSymbol: Null parameter, publisher may have been destroyed.", NULL);
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_getSymbol(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer),
                    &symbol)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Symbol for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return NULL;
    }

    jmsg = (*env)->NewStringUTF(env, symbol);
    (*env)->DeleteLocalRef(env, jmsg);
    return jmsg;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    getState
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaPublisher_getState
  (JNIEnv* env, jobject this)
{
    mamaPublisherState state = MAMA_PUBLISHER_UNKNOWN;
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(publisherPointer,
        "MamaPublisher.getState: Null parameter, publisher may have been destroyed.", NULL);
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_getState(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer),
                    &state)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get State for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return (jshort) MAMA_PUBLISHER_UNKNOWN;
    }

    return (jshort) state;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    stringForState
 * Signature: (S)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaPublisher_stringForState
  (JNIEnv* env, jobject this, jshort state)
{
    const char* stateString = NULL;
	jstring jmsg;

    stateString = mamaPublisher_stringForState((mamaPublisherState) state);

    jmsg = (*env)->NewStringUTF(env, stateString);
    (*env)->DeleteLocalRef(env, jmsg);
    return jmsg;
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher_destroy
  (JNIEnv* env, jclass this)
{
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(publisherPointer,
        "MamaPublisher.destroy: Null parameter, publisher may have been destroyed.");
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_destroy(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not call destroy for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return;
    }
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    destroyEx
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher_destroyEx
  (JNIEnv* env, jclass this)
{
    mama_status status = MAMA_STATUS_OK;
    jlong publisherPointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env, this, publisherPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(publisherPointer,
        "MamaPublisher.destroyEx: Null parameter, publisher may have been destroyed.");
    
    if (MAMA_STATUS_OK != (status = mamaPublisher_destroyEx(
                    CAST_JLONG_TO_POINTER(mamaPublisher, publisherPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not call destroyEx for mamaPublisher.",
                status);
        utils_throwWombatException(env, errorString);
        return;
    }
}

/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher_initIDs
  (JNIEnv* env, jclass class)
{
    jclass sendCallbackClass = NULL;
    jclass publisherCallbackClass = NULL;

    /* -----------------*/
    /* Publisher object */
    publisherPointerFieldId_g = (*env)->GetFieldID(env,
            class, "publisherPointer_i", UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!publisherPointerFieldId_g) return;/*Exception auto thrown*/

    /* ------------------------*/
    /*The MamaTransport object.*/
    mamaTransportObjectFieldId_g = (*env)->GetFieldID(env,
                              class,"mamaTransport_i",
                              "Lcom/wombat/mama/MamaTransport;");
    if (!mamaTransportObjectFieldId_g) return;

    /* ---------------------------------*/
    /* get our publisher callback class */
    publisherCallbackClass = (*env)->FindClass(env, "com/wombat/mama/MamaPublisherCallback");

    /* get our methods */
    publisherCallbackMethoOnCreate_g = (*env)->GetMethodID(env, publisherCallbackClass,
        "onCreate", "(Lcom/wombat/mama/MamaPublisher;)V");
    publisherCallbackMethoOnDestroy_g = (*env)->GetMethodID(env, publisherCallbackClass,
        "onDestroy", "(Lcom/wombat/mama/MamaPublisher;)V");
    publisherCallbackMethoOnError_g = (*env)->GetMethodID(env, publisherCallbackClass,
        "onError", "(Lcom/wombat/mama/MamaPublisher;SLjava/lang/String;)V");

    /* ----------------------------*/
    /* get our send callback class */
    sendCallbackClass = (*env)->FindClass(env,
                "com/wombat/mama/MamaThrottleCallback");

    /* get our method */
    sendCallbackMethod_g = (*env)->GetMethodID(env, sendCallbackClass,
        "onThrottledSendComplete", "()V");

    (*env)->DeleteLocalRef(env, sendCallbackClass);
    (*env)->DeleteLocalRef(env, publisherCallbackClass);

    return;
}

