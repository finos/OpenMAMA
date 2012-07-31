/* $Id$
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
* mamaDQPublisher.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaDQPublisherManager.h"

#include "mama/dqpublishermanager.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/
typedef struct dqcallbackClosure
{
    /*
       The closure passed by the client. This is stored as a global reference
       and must be deleted when the subscription is deleted.
     */
    jobject mUserData;
    
    /*
       The callback object for the subscription. This is stored as a
       global reference and must be deleted when the subscription is deleted
     */
    jobject mClientCB;
    
    /*
       The java Publisher Manager object. Stored as a global reference
       and deleted when the subscription is destroyed.
     */
    jobject  mPublisherManager;
    
    /*
       The Message which is to be reused for the callbacks.
       This is more efficient than creating a new message for each.
       Global reference which gets deleted when the subscription is destroyed.
     */
    jobject mMessage;
    
} dqcallbackClosure;


/******************************************************************************
* Global/Static variables
*******************************************************************************/

jfieldID     dqPublisherManagerPointerFieldId_g;

jclass      mamaDQPublisherClass = NULL;
jclass      dqPublisherManagerCallbackClass = NULL;
jclass      mamaDQPublishTopicClass = NULL;

extern jmethodID   dqpublisherConstructorId_g;
jmethodID          dqtopicConstructorId_g = NULL;


jmethodID                  dqNewRequestId_g = NULL;
jmethodID                  dqRequestId_g = NULL;
jmethodID                  dqRefreshId_g = NULL;
jmethodID                  dqErrorId_g = NULL;
jmethodID                  dqCreateId_g = NULL;


extern   jfieldID            transportPointerFieldId_g;
extern   jfieldID            messagePointerFieldId_g;
extern   jfieldID            queuePointerFieldId_g;
extern   jfieldID            dqpublisherPointerFieldId_g;



extern   jfieldID            queuePointer;
extern  JavaVM*     javaVM_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/

static void MAMACALLTYPE mamaDQPublisherManagerOnCreateCb (mamaDQPublisherManager manager)
{
	JNIEnv*             env             =   NULL;
    dqcallbackClosure*    closureImpl     =   (dqcallbackClosure*)mamaDQPublisherManager_getClosure(manager);
    
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mPublisherManager!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/   
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dqCreateId_g,
                           closureImpl->mPublisherManager);
                           
    utils_printAndClearExceptionFromStack (env,"mamaDQPublisherManagerOnCreateCb");
}

static void MAMACALLTYPE mamaDQPublisherManagerOnErrorCb (mamaDQPublisherManager manager,
                                                          mama_status            status,
                                                          const char*            errortxt,
                                                          mamaMsg                msg)
{
    JNIEnv*             env             =   NULL;
    dqcallbackClosure*    closureImpl     =   (dqcallbackClosure*)mamaDQPublisherManager_getClosure(manager);
    jstring jErrorTxt;
    
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mPublisherManager!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/
    
    (*env)->SetLongField(env, closureImpl->mMessage,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(msg));
    
    jErrorTxt = (*env)->NewStringUTF(env, errortxt);
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dqErrorId_g,
                           closureImpl->mPublisherManager,
                           status,
                           jErrorTxt,
                           closureImpl->mMessage);

    /* 
       Need to check if any exceptions were propagated here.
       If we don't the exceptions could actually fill the stack!!
    */
    utils_printAndClearExceptionFromStack (env,"mamaDQPublisherManagerOnErrorCb");
}

static void MAMACALLTYPE mamaDQPublisherManagerOnNewRequestCb (mamaDQPublisherManager manager,
                                                               const char*            symbol,
                                                               short                  subType,
                                                               short                  msgType,
                                                               mamaMsg                msg)
{
    JNIEnv*             env             =   NULL;
    dqcallbackClosure*    closureImpl     =   (dqcallbackClosure*)mamaDQPublisherManager_getClosure(manager);
    jstring jSymbol;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mPublisherManager!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/
    
    (*env)->SetLongField(env, closureImpl->mMessage,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(msg));
    
    jSymbol = (*env)->NewStringUTF(env, symbol);
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dqNewRequestId_g,
                           closureImpl->mPublisherManager,
                           jSymbol,
                           subType,
                           msgType,
                           closureImpl->mMessage);

    /* 
       Need to check if any exceptions were propagated here.
       If we don't the exceptions could actually fill the stack!!
    */
    utils_printAndClearExceptionFromStack (env,"onMsg");
}


static void MAMACALLTYPE mamaDQPublisherManagerOnRequestCb (mamaDQPublisherManager manager,
                                                            mamaPublishTopic*      publishTopicInfo,
                                                            short                  subType,
                                                            short                  msgType,
                                                            mamaMsg                msg)
{
    JNIEnv*             env             =   NULL;
    dqcallbackClosure*    closureImpl     =   (dqcallbackClosure*)mamaDQPublisherManager_getClosure(manager);
    
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mPublisherManager!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/
    
    (*env)->SetLongField(env, closureImpl->mMessage,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(msg)); 
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dqRequestId_g,
                           closureImpl->mPublisherManager,
                           publishTopicInfo->cache,
                           subType,
                           msgType,
                           closureImpl->mMessage);

    /* 
       Need to check if any exceptions were propagated here.
       If we don't the exceptions could actually fill the stack!!
    */
    utils_printAndClearExceptionFromStack (env,"onMsg");
}


static void MAMACALLTYPE mamaDQPublisherManagerOnRefreshCb (mamaDQPublisherManager  publisherManager,
                                                            mamaPublishTopic*       publishTopicInfo,
                                                            short                   subType,
                                                            short                   msgType,
                                                            mamaMsg                 msg)
{ 
    JNIEnv*             env             =   NULL;
    dqcallbackClosure*    closureImpl     =   (dqcallbackClosure*)mamaDQPublisherManager_getClosure(publisherManager);
    
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);
    assert(closureImpl->mMessage!=NULL);
    assert(closureImpl->mPublisherManager!=NULL);
    
    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/
    
    (*env)->SetLongField(env, closureImpl->mMessage,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(msg)); 
    
    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dqRefreshId_g,
                           closureImpl->mPublisherManager,
                           publishTopicInfo->cache,
                           subType,
                           msgType,
                           closureImpl->mMessage);

    /* 
       Need to check if any exceptions were propagated here.
       If we don't the exceptions could actually fill the stack!!
    */
    utils_printAndClearExceptionFromStack (env,"onMsg");
}
       
       
       

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _create
 * Signature: (Lcom/wombat/mama/MamaTransport;Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaDQPublisherManagerCallback;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1create
  (JNIEnv *env, jobject this, jobject transport, jobject queue, jobject callback, jstring topic, jstring root)
  {
    mamaDQPublisherManager   cPublisher =   NULL;
    mamaTransport   cTransport          =   NULL;
    mamaQueue       cQueue              =   NULL;
    const char*     cTopic              =   NULL;
    const char*     cRoot               =   NULL;
    jlong           transportPointer    =   0;
    jlong           queuePointer        =   0;
    jobject         messageImpl         =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    mamaDQPublisherManagerCallbacks managerCallback;
      dqcallbackClosure* closureImpl = NULL;
    /*Get the transport pointer*/
    assert(transport!=NULL);
    transportPointer = (*env)->GetLongField(env, transport,
            transportPointerFieldId_g);
    cTransport = CAST_JLONG_TO_POINTER(mamaTransport, transportPointer);
    assert(transportPointer!=0);

    /* Get the queue pointer */
    assert(queue!=NULL);
    queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
    cQueue = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    assert(queuePointer!=0);

    closureImpl = (dqcallbackClosure*)calloc(1,
            sizeof(dqcallbackClosure));
    if (!closureImpl)
    {
        utils_throwMamaException(env,"createSubscription():"
                " Could not allocate.");
        return;
    }
    
    closureImpl->mUserData      =   NULL;
    closureImpl->mClientCB      =   NULL;
    closureImpl->mPublisherManager  =   NULL;
    closureImpl->mMessage       =   NULL;
    
    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);
    
    /*Create a reuseable message object to hang off the subscription*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl)
    {
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return;
    }/*Exception will have been thrown*/
    /*This global will be deleted when the subscription is destroyed*/
    closureImpl->mMessage  = (*env)->NewGlobalRef(env,messageImpl);
    

    /*Get the char* from the jstring*/
    if(NULL!=topic)
    {
         cTopic = (*env)->GetStringUTFChars(env,topic,0);
         if(!cTopic)return;
    }
    if(NULL!=root)
    {
         cRoot= (*env)->GetStringUTFChars(env,root,0);
         if(!cRoot)return;
    }
    
    if(MAMA_STATUS_OK!=(mamaDQPublisherManager_allocate(
                    &cPublisher)))
    {
        if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }
             
	/*Add the Java Subscription to the closure - we need it in the 
      async callbacks so it can be passed to the Java callback*/
    closureImpl->mPublisherManager = (*env)->NewGlobalRef(env,this);
             
    managerCallback.onCreate =  mamaDQPublisherManagerOnCreateCb;
    managerCallback.onError =  mamaDQPublisherManagerOnErrorCb;
    managerCallback.onNewRequest =  mamaDQPublisherManagerOnNewRequestCb;
    managerCallback.onRequest =  mamaDQPublisherManagerOnRequestCb;
    managerCallback.onRefresh =  mamaDQPublisherManagerOnRefreshCb;

    
    if(MAMA_STATUS_OK!=(mamaDQPublisherManager_create(
                    cPublisher,
                    cTransport,
                    cQueue,
                    &managerCallback,
                    cTopic,
                    cRoot,
                    closureImpl)))
    {
        if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env,this,dqPublisherManagerPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPublisher));
        
    if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
    
    return;
  }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1destroy
  (JNIEnv *env, jobject this)
 {
    jlong           publisherPointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
     mamaDQPublisherManager_destroy(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer));
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _addPublisher
 * Signature: (Ljava/lang/String;Lcom/wombat/mama/MamaDQPublisher;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1addPublisher
  (JNIEnv *env, jobject this, jstring symbol, jobject publisher, jobject cache)
 {
 
     jlong           publisherPointer    =   0;
     jlong           subpublisherPointer    =   0;
     const char*     cSymbol              =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jobject topic = NULL;
    
    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
    subpublisherPointer = (*env)->GetLongField(env,publisher,dqpublisherPointerFieldId_g);
    assert(subpublisherPointer!=0);
    
	if(NULL!=symbol)
    {
         cSymbol= (*env)->GetStringUTFChars(env,symbol,0);
         if(!cSymbol)
			return;
    }
	
    
    topic = (*env)->NewObject(env, mamaDQPublishTopicClass,
                               dqtopicConstructorId_g, symbol,publisher, cache);
                               
                               
    
    mamaDQPublisherManager_addPublisher(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
					cSymbol,
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,subpublisherPointer),
                    (*env)->NewGlobalRef(env,topic));
                    
                    
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _createPublisher
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)Lcom/wombat/mama/MamaDQPublisher;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1createPublisher
  (JNIEnv *env, jobject this, jstring symbol, jobject pub, jobject cache)
 {
    jlong           publisherPointer    = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    mamaDQPublisher   cPublisher        = NULL;
	jobject      result                 = NULL;
	const char*     cSymbol             = NULL;
    mama_status     status              = MAMA_STATUS_OK;
   
	if(NULL!=symbol)
    {
         cSymbol= (*env)->GetStringUTFChars(env,symbol,0);
         if(!cSymbol)
			return result;
    }

    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
      

    if(MAMA_STATUS_OK != (status = mamaDQPublisherManager_createPublisher(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
                    cSymbol,
                    (*env)->NewGlobalRef(env,cache),
					&cPublisher)))
    {
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create subscription.",
                status);
        utils_throwMamaException(env,errorString);
    }
                 
                               
    (*env)->SetLongField(env, pub, dqpublisherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPublisher));                
                    
    return result; 
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _setStatus
 * Signature: (Lcom/wombat/mama/MamaMsgStatus;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1setStatus
  (JNIEnv *env, jobject this, jint status)
  {
      jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);  
    
    mamaDQPublisher_setStatus(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
                    status);
                    
    return;
  }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _removePublisher
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1removePublisher
  (JNIEnv *env, jobject this, jstring symbol)
  {
    jlong           publisherPointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    const char* cSymbol;
    mamaDQPublisher   cPublisher = NULL;
    
    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
  	if(NULL!=symbol)
    {
         cSymbol= (*env)->GetStringUTFChars(env,symbol,0);
         if(!cSymbol)
			return;
    }
    
    
     mamaDQPublisherManager_removePublisher(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer), cSymbol, &cPublisher );
                    
    return NULL;
  }
 
/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _destroyPublisher
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1destroyPublisher
  (JNIEnv *env, jobject this, jstring symbol)
  {
    jlong           publisherPointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    const char* cSymbol;
    
    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
  	if(NULL!=symbol)
    {
         cSymbol= (*env)->GetStringUTFChars(env,symbol,0);
         if(!cSymbol)
			return;
    }
    
    
     mamaDQPublisherManager_destroyPublisher(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer), cSymbol );
                    
    return;
  }
  
/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _setSeqNum
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1setSeqNum
  (JNIEnv *env, jobject this, jlong seqnum)
  
   {
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisher_setSeqNum(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
                    seqnum);
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _setSenderId
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1setSenderId
  (JNIEnv *env, jobject this, jlong senderId)

   {
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisher_setSenderId(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
                    (uint64_t)senderId);
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _sendSyncRequest
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1sendSyncRequest
  (JNIEnv *env, jobject this, jint nummsg , jdouble  delay , jdouble duration)
 {
     jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisherManager_sendSyncRequest(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer),
                    (mama_u16_t)nummsg,
					(mama_f64_t) delay,
					(mama_f64_t) duration);
                    
    return;
 }

 /*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    _sendNoSubscribers
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager__1sendNoSubscribers
  (JNIEnv *env, jobject this, jstring symbol)
 {
    jlong           publisherPointer            = 0;
    const char*     cSymbol                     = NULL;
    
    publisherPointer = (*env)->GetLongField(env,this,dqPublisherManagerPointerFieldId_g);
    assert(publisherPointer!=0);
    
 	if(NULL!=symbol)
    {
         cSymbol= (*env)->GetStringUTFChars(env,symbol,0);
         if(!cSymbol)
			return;
    }
    mamaDQPublisherManager_sendNoSubscribers(
                    CAST_JLONG_TO_POINTER(mamaDQPublisherManager,publisherPointer), cSymbol);
                    
    return;
 }
 
/*
 * Class:     com_wombat_mama_MamaDQPublisherManager
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisherManager_initIDs
  (JNIEnv *env , jclass class)
{

    dqPublisherManagerPointerFieldId_g = (*env)->GetFieldID(env,
            class, "dqpublisherManagerPointer_i", UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!dqPublisherManagerPointerFieldId_g) return;/*Exception auto thrown*/
 
  /*Get a reference to the subscription Callback class*/
    dqPublisherManagerCallbackClass = (*env)->FindClass(env, 
            "com/wombat/mama/MamaDQPublisherManagerCallback");
    if (!dqPublisherManagerCallbackClass) return;/*Exception auto thrown*/
    
      /*Get a reference to the subscription Callback class*/
    mamaDQPublishTopicClass = (*env)->FindClass(env, 
            "com/wombat/mama/MamaDQPublisherManager$MamaPublishTopic");
    if (!mamaDQPublishTopicClass) return;/*Exception auto thrown*/
    
	mamaDQPublisherClass = (*env)->FindClass(env,
                                 "com/wombat/mama/MamaDQPublisher");
	if (!mamaDQPublisherClass) return;/*Exception auto thrown*/

    /*Callback.onNewRequest()*/
    dqNewRequestId_g = (*env)->GetMethodID(env, dqPublisherManagerCallbackClass,
            "onNewRequest", "(Lcom/wombat/mama/MamaDQPublisherManager;Ljava/lang/String;SSLcom/wombat/mama/MamaMsg;)V" );
      
	/*Callback.onRequest()*/
    dqRequestId_g = (*env)->GetMethodID(env, dqPublisherManagerCallbackClass,
            "onRequest", "(Lcom/wombat/mama/MamaDQPublisherManager;Lcom/wombat/mama/MamaDQPublisherManager$MamaPublishTopic;SSLcom/wombat/mama/MamaMsg;)V" );

	/*Callback.onRefresh()*/
    dqRefreshId_g = (*env)->GetMethodID(env, dqPublisherManagerCallbackClass,
            "onRefresh", "(Lcom/wombat/mama/MamaDQPublisherManager;Lcom/wombat/mama/MamaDQPublisherManager$MamaPublishTopic;SSLcom/wombat/mama/MamaMsg;)V" );

	/*Callback.onError()*/
    dqErrorId_g = (*env)->GetMethodID(env, dqPublisherManagerCallbackClass,
            "onError", "(Lcom/wombat/mama/MamaDQPublisherManager;ILjava/lang/String;Lcom/wombat/mama/MamaMsg;)V" );

	/*Callback.onCreate()*/
    dqCreateId_g = (*env)->GetMethodID(env, dqPublisherManagerCallbackClass,
            "onCreate", "(Lcom/wombat/mama/MamaDQPublisherManager;)V" );          
    
    dqtopicConstructorId_g = (*env)->GetMethodID(env, mamaDQPublishTopicClass,
                              "<init>", "(Ljava/lang/String;Lcom/wombat/mama/MamaDQPublisher;Ljava/lang/Object;)V" );        
    if (!dqNewRequestId_g)
    {
        (*env)->DeleteLocalRef(env, dqPublisherManagerCallbackClass);
        return;/*Exception auto thrown*/
    }
                                                      
    
    return;
}

