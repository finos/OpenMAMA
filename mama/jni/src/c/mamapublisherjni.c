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

/******************************************************************************
* Global/Static variables
*******************************************************************************/
static  jfieldID    publisherPointerFieldId_g   =   NULL;

extern jfieldID     transportPointerFieldId_g;
extern jfieldID     messagePointerFieldId_g;
extern jfieldID     inboxPointerFieldId_g;

static  jmethodID   sendCallbackMethod_g         =   NULL;


extern  JavaVM*     javaVM_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/

static void MAMACALLTYPE sendCompleteCB (mamaPublisher publisher,
                                         mamaMsg       msg,
                                         mama_status   status,
                                         void*         closure);

        
/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaPublisher
 * Method:    _create
 * Signature: (Lcom/wombat/mama/Transport;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPublisher__1create
  (JNIEnv* env, jobject this, jobject transport, jstring topic)
{
    mamaPublisher   cPublisher          =   NULL;
    mamaTransport   cTransport          =   NULL;
    const char*     cTopic              =   NULL;
    jlong           transportPointer    =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
  
    /*Get the transport pointer*/
    assert(transport!=NULL);
    transportPointer = (*env)->GetLongField(env, transport,
            transportPointerFieldId_g);
    cTransport = CAST_JLONG_TO_POINTER(mamaTransport, transportPointer);
    assert(transportPointer!=0);

    /*Get the char* from the jstring*/
    if(NULL!=topic)
    {
         cTopic = (*env)->GetStringUTFChars(env,topic,0);
         if(!cTopic)return;
    }
    
    if(MAMA_STATUS_OK!=(mamaPublisher_create(
                    &cPublisher,
                    cTransport,
                    cTopic,
                    NULL,
                    NULL)))
    {
        if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env,this,publisherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPublisher));
        
    if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
    
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

    if(MAMA_STATUS_OK!=(mamaPublisher_send(
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

	if(MAMA_STATUS_OK!=(mamaPublisher_sendWithThrottle(
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

    if(MAMA_STATUS_OK!=(mamaPublisher_sendWithThrottle(
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

    if(MAMA_STATUS_OK!=(mamaPublisher_sendReplyToInbox(
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
    
     if(MAMA_STATUS_OK!=(mamaPublisher_sendFromInbox(
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
    
     if(MAMA_STATUS_OK!=(mamaPublisher_sendFromInboxWithThrottle(
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

     if(MAMA_STATUS_OK!=(mamaPublisher_sendFromInboxWithThrottle(
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

    if(closure)
        (*env)->DeleteGlobalRef(env,closure);

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

    publisherPointerFieldId_g = (*env)->GetFieldID(env,
            class, "publisherPointer_i", UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!publisherPointerFieldId_g) return;/*Exception auto thrown*/

    /* get our callback class */
    sendCallbackClass = (*env)->FindClass(env,
                "com/wombat/mama/MamaThrottleCallback");

    /* get our method */
    sendCallbackMethod_g = (*env)->GetMethodID(env, sendCallbackClass,
        "onThrottledSendComplete", "()V");

    return;
}

