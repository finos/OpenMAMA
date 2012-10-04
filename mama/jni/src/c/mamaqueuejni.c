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
* MamaQueue.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaQueue.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/

/* Used as a closure to the underlying registering of the C monitor callbacks.
   Needed to maintain global references to a number of Java objects during the
   lifetime of the MamaQueue which is being monitored.*/
typedef struct monitorCallbackClosure_
{
    /*The Java MamaQueueMonitorCallback instance.
     Stored as a global reference. Destroyed when queue is destroyed*/ 
    jobject mClientJavaCallback;

    /*Global reference to the Java MamaQueue instance.
     Stored as a global reference. Destroyed when the queue is destroyed.*/
    jobject mJavaQueue;
} monitorCallbackClosure;

/* Used as a closure to the underlying enqueueEvent call */
typedef struct enqueueEventClosure_
{
    /*The Java MamaQueueEnqueueCallback instance. */
    jobject mClientJavaCallback;

    /*Global reference to the Java MamaQueue instance.
     Stored as a global reference. Destroyed when the queue is destroyed.*/
    jobject mJavaQueue;

    /* Reference to the client suppliend Java object closure */
    jobject mClientClosure;
} enqueueEventClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern  JavaVM*     javaVM_g;

/*Global as also needed in mamadispatcherjni.c*/
jfieldID    queuePointerFieldId_g   =   NULL;

/*Global as also needed in mamajni.c*/
jmethodID   queueConstructorId_g    =   NULL;
/*Stores the C closure to the monitor callbacks*/
static jfieldID    monitorClosureFieldId_g   =   NULL;

/*Callback methods on the MamaQueueMonitorCallback class*/
static  jmethodID   queueCallbackonLow_g         =   NULL;
static  jmethodID   queueCallbackonHigh_g        =   NULL;

/*Callback methods on the MamaQueueEnqueueCallback class*/
static  jmethodID   queueEventCallback_g        =   NULL;

/* Pointer field of the bridge */
extern  jfieldID    bridgePointerFieldId_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/
/*Callback for event queue high watermark monitoring.*/
static void MAMACALLTYPE highWaterMarkCallback (mamaQueue     queue,
                                                size_t        size,
                                                void*         closure);
                                                                                                
/*Callback for event queue low watermark monitoring.*/
static void MAMACALLTYPE lowWaterMarkCallback (mamaQueue     queue,
                                               size_t        size,
                                               void*         closure);

static void MAMACALLTYPE queueEventCallBack (mamaQueue     queue,
                                             void*         closure);

void mamaQueuePerformCleanup(JNIEnv *env, jobject this);

/******************************************************************************
* Public function implementation
*******************************************************************************/

void mamaQueuePerformCleanup(JNIEnv *env, jobject this)
{
    /*Destroy the globals we have used for the queue monitor callback
      if it has been set at any point*/
     jlong monitorClosurePointer = (*env)->GetLongField(env,this,monitorClosureFieldId_g);

     /*Will have the default 0 value if a monitor was not set.*/
     if (monitorClosurePointer!=0)
     {
        monitorCallbackClosure* closure = CAST_JLONG_TO_POINTER(monitorCallbackClosure*,monitorClosurePointer);

        if(closure->mClientJavaCallback)
        {
            (*env)->DeleteGlobalRef(env,closure->mClientJavaCallback);
            closure->mClientJavaCallback = NULL;
        }

        if(closure->mJavaQueue)
        {
            (*env)->DeleteGlobalRef(env,closure->mJavaQueue);
            closure->mJavaQueue = NULL;
        }

        free (closure);
        (*env)->SetLongField(env, this, monitorClosureFieldId_g, 0);
     }

     (*env)->SetLongField(env, this,
         queuePointerFieldId_g,
         0);
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_destroy(JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    jlong           monitorClosurePointer = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer, 
    "MamaQueue.destroy(): Null parameter, MamaQueue may have already been destroyed."); 
    
   
    if(MAMA_STATUS_OK!=(status=mamaQueue_destroy(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_destroy() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    
    mamaQueuePerformCleanup(env, this);

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    destroyWait
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_destroyWait(JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    jlong           monitorClosurePointer = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer, 
    "MamaQueue.destroyWait(): Null parameter, MamaQueue may have already been destroyed."); 
    
   
    if(MAMA_STATUS_OK!=(status=mamaQueue_destroyWait(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_destroyWait() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    
    mamaQueuePerformCleanup(env, this);

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    destroyTimedWait
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_destroyTimedWait(JNIEnv* env, jobject this, jlong timeout)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    jlong           monitorClosurePointer = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer, 
    "MamaQueue.destroyTimedWait(): Null parameter, MamaQueue may have already been destroyed."); 
    
   
    if(MAMA_STATUS_OK!=(status=mamaQueue_destroyTimedWait(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer), timeout)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_destroyTimedWait() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    
    mamaQueuePerformCleanup(env, this);

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    dispatch
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_dispatch
  (JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaQueue_dispatch(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_dispatch() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    stopDispatch
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_stopDispatch
  (JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaQueue_stopDispatch(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_stopDispatch() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    enqueueEvent
 * Signature: (Lcom/wombat/mama/MamaEnqueueEventCallback;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_enqueueEvent
  (JNIEnv* env, jobject this, jobject callback, jobject closure)
{
    mama_status             status              = MAMA_STATUS_OK;
    jlong                   queuePointer        = 0;
    enqueueEventClosure*    closureData         = NULL;
    char                    errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
        "Null parameter, MamaQueue may have already been destroyed.");

    closureData = (enqueueEventClosure*) calloc (1, sizeof (enqueueEventClosure));
    if (!closureData)
    {
        utils_throwMamaException(env,"enqueueEvent(): Could not allocate.");
        return;
    }

    /*Store the queue, callback and use closure*/
    closureData->mJavaQueue          = (*env)->NewGlobalRef(env,this);
    closureData->mClientJavaCallback = (*env)->NewGlobalRef(env,callback);
    closureData->mClientClosure      = (*env)->NewGlobalRef(env,closure);

    if (MAMA_STATUS_OK!=
       (status = mamaQueue_enqueueEvent(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               &queueEventCallBack,
                               closureData)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_enqueueEvent() ",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    setQueueMonitorCallback
 * Signature: (Lcom/wombat/mama/MamaQueueMonitorCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_setQueueMonitorCallback
  (JNIEnv* env, jobject this, jobject callback)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    monitorCallbackClosure* closureData = NULL;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    mamaQueueMonitorCallbacks queueCallbacks;

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    closureData = (monitorCallbackClosure*) calloc (1, sizeof
            (monitorCallbackClosure));
    if (!closureData)
    {
        utils_throwMamaException(env,"setQueueMonitorCallback():"
                        " Could not allocate.");
        return;
    }

    /*Store the queue and the callback as globals in the callback*/
    closureData->mJavaQueue = (*env)->NewGlobalRef(env,this);
    closureData->mClientJavaCallback = (*env)->NewGlobalRef(env,callback);

    queueCallbacks.onQueueHighWatermarkExceeded = highWaterMarkCallback;
    queueCallbacks.onQueueLowWatermark = lowWaterMarkCallback;

    if(MAMA_STATUS_OK!=(status=mamaQueue_setQueueMonitorCallbacks(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               &queueCallbacks,
                               closureData)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_setQueueMonitorCallbacks() ",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Set the pointer for the C queue in the Java queue object*/
    (*env)->SetLongField(env, 
                         this, 
                         monitorClosureFieldId_g,
                         CAST_POINTER_TO_JLONG(closureData));

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    setHighWatermark
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_setHighWatermark
  (JNIEnv* env, jobject this, jlong highWatermark)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaQueue_setHighWatermark(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               (size_t)highWatermark)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_setHighWatermark() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    getHighWatermark
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaQueue_getHighwatermark
  (JNIEnv* env, jobject this)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    size_t          highWatermark   = 0;

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaQueue_getHighWatermark(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               &highWatermark)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_getHighWatermark() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return (jlong)highWatermark;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    setLowWatermark
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_setLowWatermark
  (JNIEnv* env, jobject this, jlong lowWatermark)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaQueue_setLowWatermark(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               (size_t)lowWatermark)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_setLowWatermark() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    getLowWatermark
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaQueue_getLowWatermark
  (JNIEnv* env, jobject this)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    = 0;
    size_t          lowWatermark    = 0; 
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaQueue_getLowWatermark(
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                               &lowWatermark)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_getLowWatermark() ",
                status);
        utils_throwMamaException(env,errorString);
    }
    return (jlong)lowWatermark;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    setQueueName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_setQueueName
  (JNIEnv* env, jobject this, jstring name)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    = 0;
    const char*     queueName       = NULL;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.");

    if (name)
    {
        queueName = (*env)->GetStringUTFChars(env,name,0);
        if(!queueName)return;

        if(MAMA_STATUS_OK!=(status=mamaQueue_setQueueName(
                        CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                        queueName)))
        {
            utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "mamaQueue_setQueueName() failed.",
                    status);
            utils_throwMamaException(env,errorString);
        }

        (*env)->ReleaseStringUTFChars(env,name, queueName);
    }

    return;
}
                                                                                                
/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    getQueueName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaQueue_getQueueName
  (JNIEnv* env, jobject this)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    = 0;
    const char*     queueName       = NULL;
    jstring         retValue        = NULL;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaQueue_getQueueName(
                    CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                    &queueName)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_getQueueName() failed.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    if (queueName)
    {
        retValue = (*env)->NewStringUTF(env, queueName);
    }

    return retValue;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    getQueueBridgeName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaQueue_getQueueBridgeName
  (JNIEnv* env, jobject this)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    = 0;
    const char*     queueBridgeName = NULL;
    jstring         retValue        = NULL;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(queuePointer,
        "Null parameter, MamaQueue may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaQueue_getQueueBridgeName(
                    CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                    &queueBridgeName)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaQueue_getQueueBridgeName() failed.",
            status);
        utils_throwMamaException(env,errorString);
        return;
    }

    if (queueBridgeName)
    {
        retValue = (*env)->NewStringUTF(env,queueBridgeName);
    }

    return retValue;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    getEventCount
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaQueue_getEventCount
  (JNIEnv* env, jobject this)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           queuePointer    = 0;
    size_t          eventCount      = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,this,queuePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(queuePointer,  
		"Null parameter, MamaQueue may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaQueue_getEventCount(
                    CAST_JLONG_TO_POINTER(mamaQueue,queuePointer),
                    &eventCount)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaQueue_getEventCount () failed.",
                status);
        utils_throwMamaException(env,errorString);
    }
    
    return (jlong)eventCount;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    create
 * Signature: (Lcom/wombat/mama/MamaBridge;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_create
  (JNIEnv* env, jobject this, jobject bridge)
{
    mamaQueue       queue   =   NULL;
    mama_status     status  =   MAMA_STATUS_OK;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong  bridgeFieldPointer = 
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);             
    assert(0!=bridgeFieldPointer); 
    
    if(MAMA_STATUS_OK!=(status=mamaQueue_create(
                &queue, CAST_JLONG_TO_POINTER(mamaBridge,bridgeFieldPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create timer.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Set the pointer for the C queue in the Java queue object*/
    (*env)->SetLongField(env, this, queuePointerFieldId_g,
                                   CAST_POINTER_TO_JLONG(queue));
     
    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    createUsingNative
 * Signature: (Lcom/wombat/mama/MamaBridge;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_createUsingNative
  (JNIEnv* env, jobject this, jobject bridge, jobject nativeQueue)
{
    mama_status     status  =   MAMA_STATUS_NOT_IMPLEMENTED;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Creating MamaQueue using the native Queue is not supported in JNI.",
            status);
    utils_throwMamaException(env,errorString);
    return;
}

/*
 * Class:     com_wombat_mama_MamaQueue
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaQueue_initIDs
  (JNIEnv* env, jclass class)
{
    jclass   queueMonitorCallbackClass  =   NULL;
    jclass   ququeEventCallbackClass    =   NULL;
    
    int i=0;
    queuePointerFieldId_g = (*env)->GetFieldID(env,
                                class,"queuePointer_i",
                                UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    monitorClosureFieldId_g = (*env)->GetFieldID(env,
                                class,"monitorClosure_i",
                                UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    /*Get a reference to the subscription Callback class*/
    queueMonitorCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaQueueMonitorCallback");
    
    if (!queueMonitorCallbackClass) return;/*Exception auto thrown*/
    /*MamaQueueMonitorCallback.onHighWatermarkExceeded()*/
    queueCallbackonHigh_g = (*env)->GetMethodID(env, queueMonitorCallbackClass,
            "onHighWatermarkExceeded", "(Lcom/wombat/mama/MamaQueue;J)V" );
    
    if (!queueCallbackonHigh_g)
    {
        (*env)->DeleteLocalRef(env, queueMonitorCallbackClass);
        return;/*Exception auto thrown*/
    }
    /*MamaQueueMonitorCallback.onLowWatermark()*/
    queueCallbackonLow_g = (*env)->GetMethodID(env, queueMonitorCallbackClass,
            "onLowWatermark", "(Lcom/wombat/mama/MamaQueue;J)V" );
    
    if (!queueCallbackonLow_g)
    {
        (*env)->DeleteLocalRef(env, queueMonitorCallbackClass);
        return;/*Exception auto thrown*/
    }

    ququeEventCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaQueueEventCallback");
    
    /*MamaQueueEnqueueCallback.onEventEnqueue()*/
    queueEventCallback_g = (*env)->GetMethodID(env, ququeEventCallbackClass,
            "onEvent", "(Lcom/wombat/mama/MamaQueue;Ljava/lang/Object;)V");

    if (!queueEventCallback_g)
    {
        (*env)->DeleteLocalRef(env, ququeEventCallbackClass);
        return;/*Exception auto thrown*/
    }
    
    queueConstructorId_g = (*env)->GetMethodID(env, class,
                             "<init>", "()V" );           
    return;
}

void MAMACALLTYPE highWaterMarkCallback (mamaQueue     queue,
                                         size_t        size,
                                         void*         closure)
{
    JNIEnv*                 env             =   NULL;
    monitorCallbackClosure* closureData     = 
        (monitorCallbackClosure*)closure;

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);

    if (!env) return; /*Details logged in utils_getENV()*/
    
    if (!closureData) 
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "highWaterMarkCallback(): NULL "
                                       "Closure!!");
        return;
    }

    if (!closureData->mClientJavaCallback)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "highWaterMarkCallback() NULL "
                  "callback in closure.");
        return;
    }

    if (!closureData->mJavaQueue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "highWaterMarkCallback() NULL "
                  "Java queue in closure.");
        return;
    }

    /*Call the onHighWatermarkExceeded Java callback*/
    (*env)->CallVoidMethod(env, 
                           closureData->mClientJavaCallback,
                           queueCallbackonHigh_g,
                           closureData->mJavaQueue,
                           (jlong)size);

    return;
}
                                                                                                
void MAMACALLTYPE lowWaterMarkCallback (mamaQueue     queue,
                                        size_t        size,
                                        void*         closure)
{
    JNIEnv*                 env             =   NULL;
    monitorCallbackClosure* closureData     = 
        (monitorCallbackClosure*)closure;

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);

    if (!env) return; /*Details logged in utils_getENV()*/
    
    if (!closureData) 
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "lowWaterMarkCallback(): NULL "
                                       "Closure");
        return;
    }

    if (!closureData->mClientJavaCallback)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "lowWaterMarkCallback() NULL "
                  "callback in closure.");
        return;
    }

    if (!closureData->mJavaQueue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL, "lowWaterMarkCallback() NULL "
                  "Java queue in closure.");
        return;
    }

    /*Call the onHighWatermarkExceeded Java callback*/
    (*env)->CallVoidMethod(env, 
                           closureData->mClientJavaCallback,
                           queueCallbackonLow_g,
                           closureData->mJavaQueue,
                           (jlong)size);
    return;
}


void MAMACALLTYPE queueEventCallBack (mamaQueue     queue,
                                      void*         closure)
{
    JNIEnv*              env         = NULL;
    enqueueEventClosure* closureData = (enqueueEventClosure*)closure;

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);

    if (!env) return; /*Details logged in utils_getENV()*/
    
    if (!closureData) 
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "enqueueEventCallBack(): NULL Closure");
        return;
    }

    if (!closureData->mClientJavaCallback)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "enqueueEventCallBack() NULL callback in closure.");
        return;
    }

    if (!closureData->mJavaQueue)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                 "enqueueEventCallBack() NULL Java queue in closure.");
        return;
    }
    /*Call the onHighWatermarkExceeded Java callback*/
    (*env)->CallVoidMethod(env, 
                           closureData->mClientJavaCallback,
                           queueEventCallback_g,
                           closureData->mJavaQueue,
                           closureData->mClientClosure);

    /* Clean up */
    (*env)->DeleteGlobalRef(env,closureData->mClientJavaCallback);
    (*env)->DeleteGlobalRef(env,closureData->mJavaQueue);
    (*env)->DeleteGlobalRef(env,closureData->mClientClosure);

    free (closureData);
      
    return;
}
