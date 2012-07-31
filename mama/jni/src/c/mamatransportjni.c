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
* MamaTransport.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaTransport.h"

/* These headers are required for the mamaConnection object, they must be included
 * to avoid crashes in JNI when the equivalent MamaConnection object is allocated.
 */
#include "mamajni/com_wombat_mama_MamaConnection.h"
#include "mama/conflation/connection.h"

/******************************************************************************
* Local data structures
*******************************************************************************/

/* This structure contains Java object references to callback objects that
 * receive transport events.
 */
typedef struct transportListenerClosure
{
    /* The callback object for the main transport listener. */
    jobject mListenerCallback;

    /* The callback object for topic events. */
    jobject mTopicCallback;

    /* The java connection object is used when processing transport callbacks. */
    jobject mConnection;

} transportListenerClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/

/*Defined in mamajni.c*/
extern JavaVM* javaVM_g;

/*Field in MamaTransport.java which holds pointer to C transport structure*/ 
jfieldID            transportPointerFieldId_g           =   NULL;

/*Field in MamaTransport.java which holds the pointer to the closure*/
static jfieldID     transportClosureFieldId_g           =   NULL;
jmethodID           connectionConstructorId_g           =   NULL;
jmethodID           connectionDefaultConstructorId_g    =   NULL;
jclass              connectionClass_g                   =   NULL;
extern jfieldID     connectionPointerFieldId_g;

/*Method ids for the MamaTransportListener interface callback*/
static jmethodID    transportListenerOnReconnectId_g    =   NULL;
static jmethodID    transportListenerOnDisconnectId_g   =   NULL;
static jmethodID    transportListenerOnQualityId_g      =   NULL;
static jmethodID    transportListenerOnAcceptId_g                   =   NULL;
static jmethodID    transportListenerOnAcceptReconnectId_g          =   NULL;
static jmethodID    transportListenerOnPublisherDisconnectId_g      =   NULL;
static jmethodID    transportListenerOnConnectId_g                  =   NULL;
static jmethodID    transportListenerOnNamingServiceConnectId_g     =   NULL;
static jmethodID    transportListenerOnNamingServiceDisconnectId_g  =   NULL;

/*Method ids for the MamaTransportTopicListener callback*/
static jmethodID    transportTopicListenerOnTopicSubscribeId_g    =   NULL;
static jmethodID    transportTopicListenerOnTopicUnsubscribeId_g  =   NULL;

/* Pointer field of the bridge */
extern  jfieldID    bridgePointerFieldId_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/
/*Callback function for transport related events.*/
static void
MAMACALLTYPE
mamaTransportListenerCB( mamaTransport        tport,
                         mamaTransportEvent   event,
                         short                cause,
                         const void*          platformInfo,
                         void*                closure);

static void
MAMACALLTYPE
mamaTransportTopicListenerCB( mamaTransport        tport,
                              mamaTransportEvent   event,
                              short                cause,
                              const void*          platformInfo,
                              void*                closure);

/**
 * This function will obtain a mamaConnection pointer from the supplied platform info and
 * set it inside the re-usable Java MamaConnection object.
 *
 * @param[in] env The java environment object.
 * @param[in] platformInfo Casted mamaConnection pointer.
 * @param[in] transportClosure The closure.
 *
 * @returns Java MamaConnection object or NULL if there is no connection information in the platformInfo.
 */
jobject mamaTransportImpl_getConnectionObject(JNIEnv *env, const void *platformInfo, transportListenerClosure *transportClosure);

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    _create
 * Signature: (Lcom/wombat/mama/MamaBridge;)V
 */
JNIEXPORT void JNICALL 
Java_com_wombat_mama_MamaTransport__1create__Lcom_wombat_mama_MamaBridge_2
  (JNIEnv* env, jobject this, jobject bridge)
{
    Java_com_wombat_mama_MamaTransport__1create__Ljava_lang_String_2Lcom_wombat_mama_MamaBridge_2(
            env, this, NULL, bridge);
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    _create
 * Signature: (Ljava/lang/String;Lcom/wombat/mama/MamaBridge;)V
 */
JNIEXPORT void JNICALL 
Java_com_wombat_mama_MamaTransport__1create__Ljava_lang_String_2Lcom_wombat_mama_MamaBridge_2
  (JNIEnv* env, jobject this, jstring name, jobject bridge)
{
    jlong           transportPointer    =   0;
    const char*     transName           =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong  bridgeFieldPointer = 
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);      
    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.");

    if(name)
    {
        transName = (*env)->GetStringUTFChars(env,name,0);
        if(!transName)return;
    }

    if (MAMA_STATUS_OK!=(status=mamaTransport_create(
            CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
            transName,
            CAST_JLONG_TO_POINTER(mamaBridge,bridgeFieldPointer))))
    {
        if(transName)(*env)->ReleaseStringUTFChars(env,name, transName);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaTransport_create() Failed.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Tidy up all local refs*/
    if(transName)(*env)->ReleaseStringUTFChars(env,name, transName);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_destroy(JNIEnv* env, jobject this)
{
    /* Ensure the objects are valid. */
    if((NULL != env) && (NULL != this))
    {
        /* Get the transport object from the parent java class. */
        jlong transportPointer = 0;
        transportPointer = (*env)->GetLongField(env, this, transportPointerFieldId_g);
        if(0 != transportPointer)
        {
            /* Cast to a MAMA transport. */
            mamaTransport transport = (mamaTransport)transportPointer;

            /* Destroy the transport first, note that this may cause events to be fired on the listener
             * and the topic listener.
             * Due to a MAMA problem these can't be unregistered first.
             */
            mamaTransport_destroy(transport);
            {
                /* Get the closure object from the parent java class. */
                jlong closurePointer = 0;
                closurePointer = (*env)->GetLongField(env, this, transportClosureFieldId_g);
                if(0 != closurePointer)
                {
                    /* Cast to a closure object. */
                    transportListenerClosure *closure = (transportListenerClosure *)closurePointer;
                
                    /* The callbacks cannot be removed as the transport has been destroyed, instead simply
                     * delete the global references and allow the Java objects to be garbage collected.
                     */         
                    /* The listener callback. */
                    if(NULL != closure->mListenerCallback)
                    {
                        (*env)->DeleteGlobalRef(env, closure->mListenerCallback);
                        closure->mListenerCallback = NULL;
                    }

                    /* The topic callback. */
                    if(NULL != closure->mTopicCallback)
                    {
                        (*env)->DeleteGlobalRef(env, closure->mTopicCallback);
                        closure->mTopicCallback = NULL;
                    } 
    
                    /* Delete the connection object reference, this will allow the Java object to be garbage collected. */
                    if(NULL != closure->mConnection)
                    {
                        (*env)->DeleteGlobalRef(env, closure->mConnection);
                        closure->mConnection = NULL;
                    }

                    /* Delete the closure structure itself. */
                    free(closure);
                }
            }

            /* Reset the pointers stored in the parent Java class. */
            (*env)->SetLongField(env, this, transportClosureFieldId_g, 0);
            (*env)->SetLongField(env, this, transportPointerFieldId_g, 0);
        }
    }
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    getOutboundThrottle
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaTransport_getOutboundThrottle
  (JNIEnv* env, jobject this, jint throttleInstance)
{
    mama_status status              =   MAMA_STATUS_OK;
    double      c_outboundThrottle  =   0.0;
    jlong       pointerValue        =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    pointerValue = (*env)->GetLongField(env,this,transportPointerFieldId_g);

    if(MAMA_STATUS_OK != (status = 
        mamaTransport_getOutboundThrottle(
            CAST_JLONG_TO_POINTER(mamaTransport,pointerValue),
            (mamaThrottleInstance)throttleInstance,
            &c_outboundThrottle)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get outbound throttle.",
                status);
        utils_throwMamaException(env,errorString);
        return 0.0;
    }
    
    return (jdouble)c_outboundThrottle;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    setOutboundThrottle
 * Signature: (ID)
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_setOutboundThrottle
  (JNIEnv* env, jobject this, jint throttleInstance, jdouble outboundThrottle)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       pointerValue    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    pointerValue = (*env)->GetLongField(env,this,transportPointerFieldId_g);

    if(0==pointerValue)
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Transport not yet initialised!",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }
   
    if(MAMA_STATUS_OK!=(status=mamaTransport_setOutboundThrottle(
                        CAST_JLONG_TO_POINTER(mamaTransport,pointerValue),
                        (mamaThrottleInstance)throttleInstance,
                        (double)outboundThrottle)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to set outbound throttle.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    } 
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    setDescription
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_setDescription
  (JNIEnv* env, jobject this, jstring description)
{
    jlong           transportPointer    =   0;
    const char*     c_description       =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.");

    if(description)
    {
        c_description = (*env)->GetStringUTFChars(env,description,0);
        if(!c_description)return;
    }
    else
    {
        return;
    }

    if (MAMA_STATUS_OK!=(status=mamaTransport_setDescription(
         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),c_description)))
    {
        if(c_description)(*env)->ReleaseStringUTFChars(env,
                description, c_description);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaTransport_setDescription() Failed.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Tidy up all local refs*/
    if(c_description)(*env)->ReleaseStringUTFChars(
            env,description, c_description);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    getDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaTransport_getDescription
  (JNIEnv* env, jobject this)
{
    jlong           transportPointer    =   0;
    const char*     c_description       =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.", NULL);

    if (MAMA_STATUS_OK!=(status=mamaTransport_getDescription(
         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),&c_description)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaTransport_setDescription() Failed.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, c_description);
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    setInvokeQualityForAllSubscs
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_setInvokeQualityForAllSubscs
  (JNIEnv* env, jobject this, jboolean invokeQualityForAllSubscs)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           transportPointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.");

    if (MAMA_STATUS_OK!=(status=mamaTransport_setInvokeQualityForAllSubscs (
                         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                         (int)invokeQualityForAllSubscs)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set invoke quality for all subscs for mamaTransport",
                status);
        utils_throwWombatException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    getInvokeQualityForAllSubscs
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaTransport_getInvokeQualityForAllSubscs
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           transportPointer    =   0;
    int             invokeQualityForAllSubscs;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.", 0);

    if (MAMA_STATUS_OK!=(status=mamaTransport_getInvokeQualityForAllSubscs (
                         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                         &invokeQualityForAllSubscs)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get invoke quality for all subscs for mamaTransport",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jboolean)invokeQualityForAllSubscs;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    getQuality
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaTransport_getQuality
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           transportPointer    =   0;
    mamaQuality     quality;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.", 0);

    if (MAMA_STATUS_OK!=(status=mamaTransport_getQuality (
                         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                         &quality)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get quality for mamaTransport",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jshort)quality;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_initIDs
  (JNIEnv* env, jclass class)
{
    jclass mamaTransportListenerClass   = NULL;
    jclass mamaTransportTopicListenerClass   = NULL;
    
    /* Get the Transport related method and field Id's */
    transportPointerFieldId_g = (*env)->GetFieldID(env,
             class, "transportPointer_i",
             UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!transportPointerFieldId_g) return;/*Exception auto thrown*/
    
    transportClosureFieldId_g = (*env)->GetFieldID(env,
             class, "closurePointer_i",
             UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if(!transportClosureFieldId_g) return;

    connectionClass_g = (*env)->FindClass(env,
                                          "com/wombat/mama/MamaConnection");
    if (!connectionClass_g) return;

    connectionConstructorId_g = (*env)->GetMethodID (env, connectionClass_g,
                                                     "<init>", 
    "(Lcom/wombat/mama/MamaTransport;Ljava/lang/String;IIIIILjava/lang/String;Ljava/lang/String;)V");
    
    if (connectionConstructorId_g == NULL)
    {
        return;
    }

    connectionDefaultConstructorId_g = (*env)->GetMethodID (env, connectionClass_g, "<init>", "()V");
    if (connectionDefaultConstructorId_g == NULL)
    {
        return;
    }

    connectionPointerFieldId_g = (*env)->GetFieldID (env, connectionClass_g,
                                                     "connectionPointer_i", 
                                                     UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    /* Get the transport listener method and field Id's */

   mamaTransportListenerClass = (*env)->FindClass(env,
                             UTILS_TRANSPORT_LISTENER_CLASS_NAME);
   if(!mamaTransportListenerClass) return;

   transportListenerOnReconnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onReconnect",
                        UTILS_TL_ON_RECONNECT_PROTOTYPE);
   if(!transportListenerOnReconnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
       return; 
   }
   
   transportListenerOnDisconnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onDisconnect",
                        UTILS_TL_ON_DISCONNECT_PROTOTYPE);
   if(!transportListenerOnDisconnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }
   
   transportListenerOnQualityId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onQuality",
                        UTILS_TL_ON_QUALITY_PROTOTYPE);
   if(!transportListenerOnQualityId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnConnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onConnect",
                        UTILS_TL_ON_CONNECT_PROTOTYPE);
   if(!transportListenerOnConnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnAcceptId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onAccept",
                        UTILS_TL_ON_ACCEPT_PROTOTYPE);
   if(!transportListenerOnAcceptId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnAcceptReconnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onAcceptReconnect",
                        UTILS_TL_ON_ACCEPT_RECONNECT_PROTOTYPE);
   if(!transportListenerOnAcceptReconnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnPublisherDisconnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onPublisherDisconnect",
                        UTILS_TL_ON_PUBLISHER_DISCONNECT_PROTOTYPE);
   if(!transportListenerOnPublisherDisconnectId_g)
   {   
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnNamingServiceConnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onNamingServiceConnect",
                        UTILS_TL_ON_NAMING_SERVICE_CONNECT_PROTOTYPE);
   if(!transportListenerOnNamingServiceConnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   transportListenerOnNamingServiceDisconnectId_g = (*env)->GetMethodID(env,
                        mamaTransportListenerClass,"onNamingServiceDisconnect",
                        UTILS_TL_ON_NAMING_SERVICE_DISCONNECT_PROTOTYPE);
   if(!transportListenerOnNamingServiceDisconnectId_g)
   {
        (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
        return;
   }

   /* Get the transport topic listener method and field Id's */

   mamaTransportTopicListenerClass = (*env)->FindClass(env,
                             UTILS_TRANSPORT_TOPIC_LISTENER_CLASS_NAME);
   if(!mamaTransportTopicListenerClass) return;

    transportTopicListenerOnTopicSubscribeId_g = (*env)->GetMethodID(env,
                        mamaTransportTopicListenerClass,"onTopicSubscribe",
                        UTILS_TTL_ON_TOPIC_SUBSCRIBE);
    if(!transportTopicListenerOnTopicSubscribeId_g)
    {   
        (*env)->DeleteLocalRef(env, mamaTransportTopicListenerClass);
        return;
    }

    transportTopicListenerOnTopicUnsubscribeId_g = (*env)->GetMethodID(env,
                        mamaTransportTopicListenerClass,"onTopicUnsubscribe",
                        UTILS_TTL_ON_TOPIC_UNSUBSCRIBE);
    if(!transportTopicListenerOnTopicUnsubscribeId_g)
    {   
        (*env)->DeleteLocalRef(env, mamaTransportTopicListenerClass);
        return;
    }

   (*env)->DeleteLocalRef(env, mamaTransportListenerClass);
   (*env)->DeleteLocalRef(env, mamaTransportListenerClass); 
   (*env)->DeleteLocalRef(env, mamaTransportTopicListenerClass);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    nativeAddListener
 * Signature: (Lcom/wombat/mama/MamaTransport.InternalTransportListenr;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_nativeAddListener(JNIEnv *env, jobject this, jobject listener)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != env) && (NULL != this))
    {
        /* Get the transport object from the parent java class. */
        jlong transportPointer = 0;
        transportPointer = (*env)->GetLongField(env, this, transportPointerFieldId_g);
        if(0 != transportPointer)
        {
            /* Get the closure object from the parent java class. */
            jlong closurePointer = 0;
            closurePointer = (*env)->GetLongField(env, this, transportClosureFieldId_g);
            if(0 != closurePointer)
            {
                /* Cast these to the appropriate types. */
                transportListenerClosure *closure = (transportListenerClosure *)closurePointer;
                mamaTransport transport = (mamaTransport)transportPointer;

                /* Create a global reference to the new callback */
                ret = MAMA_STATUS_NOMEM;
                closure->mListenerCallback = (*env)->NewGlobalRef(env, listener);
				if(NULL != closure->mListenerCallback)
				{
					/* Install the transport callback. */
                    ret = mamaTransport_setTransportCallback(transport, (mamaTransportCB)mamaTransportListenerCB, closure);
				}
            }
        }
    }

    /* Throw an exception if something went wrong. */
    if(MAMA_STATUS_OK != ret)
	{
		char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
		utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to add transport listener.",
                ret);
        utils_throwMamaException(env, errorString);
	}
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    nativeAddTopicListener
 * Signature: (Lcom/wombat/mama/MamaTransport.InternalTopicListener;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_nativeAddTopicListener(JNIEnv *env, jobject this, jobject topicListener)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_NULL_ARG;
    if((NULL != env) && (NULL != this))
    {
        /* Get the transport object from the parent java class. */
        jlong transportPointer = 0;
        transportPointer = (*env)->GetLongField(env, this, transportPointerFieldId_g);
        if(0 != transportPointer)
        {
            /* Get the closure object from the parent java class. */
            jlong closurePointer = 0;
            closurePointer = (*env)->GetLongField(env, this, transportClosureFieldId_g);
            if(0 != closurePointer)
            {
                /* Cast these to the appropriate types. */
                transportListenerClosure *closure = (transportListenerClosure *)closurePointer;
                mamaTransport transport = (mamaTransport)transportPointer;

                /* Create a global reference to the new callback */
                ret = MAMA_STATUS_NOMEM;
                closure->mTopicCallback = (*env)->NewGlobalRef(env, topicListener);
				if(NULL != closure->mTopicCallback)
				{
					/* Install the transport topic callback. */
					ret = mamaTransport_setTransportTopicCallback(transport, (mamaTransportCB)mamaTransportTopicListenerCB, (void *)closure);
				}
            }
        }
    }

    /* Throw an exception if something went wrong. */
    if(MAMA_STATUS_OK != ret)
	{
		char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
		utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to add transport topic listener.",
                ret);
        utils_throwMamaException(env, errorString);
	}
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    allocateTransport
 * Signature: (Lcom/wombat/mama/MamaConnection;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_allocateTransport(JNIEnv* env, jobject this, jobject connection)
{    
    /* Allocate the transport */
    mamaTransport transport = NULL;
    mama_status ret = mamaTransport_allocate(&transport);
    if(MAMA_STATUS_OK == ret)
    {
        /* Allocate the closure structure. */        
        transportListenerClosure *closureImpl = (transportListenerClosure *)calloc( 1, sizeof(transportListenerClosure));
        ret = MAMA_STATUS_NOMEM;
        if(NULL != closureImpl)
        {
            /* Create a global reference to the connection object to prevent it being collected. */
            closureImpl->mConnection = (*env)->NewGlobalRef(env, connection);
            if(NULL != closureImpl->mConnection)
            {
                /* Function has succeeded. */
                ret = MAMA_STATUS_OK;
            }
                
            /* Set the value of the closure pointer in the parent java class, this is done regardless of whether
             * the function fails so that the appropriate clean-up will be performed in destroy. 
             */
            (*env)->SetLongField(env, this, transportClosureFieldId_g, CAST_POINTER_TO_JLONG(closureImpl));
        }

        /* Set the value of the transport pointer in the parent java class, this is done regardless of whether
         * the function fails so that the appropriate clean-up will be performed in destroy. */
        (*env)->SetLongField(env, this, transportPointerFieldId_g, CAST_POINTER_TO_JLONG(transport));
    }

    /* If something went wrong then the transport must be freed. */
    if(ret != MAMA_STATUS_OK)
    {
        /* Build and print error message */
        char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
        utils_buildErrorStringForStatus(errorString, UTILS_MAX_ERROR_STRING_LENGTH, "Failed to allocate memory for transport.", ret);

        /* Destroy the transport. */
        Java_com_wombat_mama_MamaTransport_destroy(env, this);

        /* Throw an exception. */
        utils_throwMamaException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    requestConflaction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_requestConflation
  (JNIEnv* env, jobject this)
{
    mama_status status              =  MAMA_STATUS_OK;
    jlong       pointerValue        =  0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    pointerValue = (*env)->GetLongField(env,this,transportPointerFieldId_g);

    if(MAMA_STATUS_OK != (status =
                mamaTransport_requestConflation(
                    CAST_JLONG_TO_POINTER(mamaTransport,pointerValue))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to request conflation for this transport.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    requestEndConflaction
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_requestEndConflation
  (JNIEnv* env, jobject this)
{
    mama_status status              =  MAMA_STATUS_OK;
    jlong       pointerValue        =  0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    pointerValue = (*env)->GetLongField(env,this,transportPointerFieldId_g);

    if(MAMA_STATUS_OK != (status =
                mamaTransport_requestEndConflation(
                    CAST_JLONG_TO_POINTER(mamaTransport,pointerValue))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to request end of conflation for this transport.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaTransport_getName
  (JNIEnv* env, jobject this)
{
    const char*     name_c          =  NULL;
    mama_status status              =  MAMA_STATUS_OK;
    jlong       transportPointer    =  0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(transportPointer,  
        "Null parameter, MamaTransport may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK != (status =
                mamaTransport_getName(
                    CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                    &name_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get name for this transport.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    return  (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaTransport
 * Method:    setName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTransport_setName
  (JNIEnv* env, jobject this, jstring name)
{
    const char*     name_c              =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           transportPointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    transportPointer = (*env)->GetLongField(env,this,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(transportPointer,  
        "Null parameter, MamaTransport may have already been destroyed.");

    if(name)
    {
        name_c = (*env)->GetStringUTFChars(env,name,0);
        if(!name_c) return;/*Exception thrown*/
    }

    if (MAMA_STATUS_OK!=(status=mamaTransport_setName (
                         CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                         name_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set name for mamaTransport",
                status);
        utils_throwWombatException(env,errorString);
    }

    /*Tidy up all local refs*/
    if(name_c)
        (*env)->ReleaseStringUTFChars( env,name, name_c);

    return;
}

void MAMACALLTYPE mamaTransportListenerCB( mamaTransport      tport,
                           mamaTransportEvent transportEvent,
                           short              cause,
                           const void*        platformInfo,
                           void*              closure )
{
    /* Get the impl from the closure. */
    transportListenerClosure *closureImpl = (transportListenerClosure *)closure;
    if((NULL != closureImpl) && (NULL != closureImpl->mListenerCallback))
    {
        /* Get the java environment object. */
        JNIEnv* env = mamaJniUtils_attachNativeThread(javaVM_g);        
        if(NULL != env)
        {
            /* If the platform info is valid then this will be passed back up to the callback object
             * a java MamaConnection object. 
             * Note that NULL will be returned if the platform info is not valid.
             */
            jobject connection = mamaTransportImpl_getConnectionObject(env, platformInfo, closureImpl);

            /* Check the type of transport event. */
            switch (transportEvent)
            {
                case MAMA_TRANSPORT_RECONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnReconnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_DISCONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnDisconnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_QUALITY:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnQualityId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_CONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnConnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_ACCEPT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnAcceptId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_ACCEPT_RECONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnAcceptReconnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_PUBLISHER_DISCONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnPublisherDisconnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_NAMING_SERVICE_CONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnNamingServiceConnectId_g,cause,connection);
                    break;
                case MAMA_TRANSPORT_NAMING_SERVICE_DISCONNECT:
                    (*env)->CallVoidMethod(env,closureImpl->mListenerCallback, transportListenerOnNamingServiceDisconnectId_g,cause,connection);
                    break;
            }             

            /* Release the environment pointer. */
            mamaJniUtils_detachNativeThread(javaVM_g);
        }
    }
}

void MAMACALLTYPE mamaTransportTopicListenerCB( mamaTransport      tport,
                             mamaTransportEvent transportEvent,
                             short              cause,
                             const void*        platformInfo,
                             void*              closure )
{
    /* Get the impl from the closure. */
    transportListenerClosure *closureImpl = (transportListenerClosure *)closure;
    if((NULL != closureImpl) && (NULL != closureImpl->mTopicCallback))
    {
        /* Get the java environment object. */
        JNIEnv* env = mamaJniUtils_attachNativeThread(javaVM_g);
        if(NULL != env)
        {
            /* If the platform info is valid then this will be passed back up to the callback object
             * a java MamaConnection object. 
             * Note that NULL will be returned if the platform info is not valid.
             */
            jobject connection = mamaTransportImpl_getConnectionObject(env, platformInfo, closureImpl);

            /* Check the type of transport event. */
            switch (transportEvent)
            {
                case MAMA_TRANSPORT_TOPIC_SUBSCRIBED:
                    (*env)->CallVoidMethod( env, closureImpl->mTopicCallback, transportTopicListenerOnTopicSubscribeId_g, cause, connection);
                    break;
                case MAMA_TRANSPORT_TOPIC_UNSUBSRCIBED:
                    (*env)->CallVoidMethod( env, closureImpl->mTopicCallback, transportTopicListenerOnTopicUnsubscribeId_g, cause, connection);
                    break;
            }        

            /* Release the environment pointer. */
            mamaJniUtils_detachNativeThread(javaVM_g);
        }
    }
}

/******************************************************************************
* Local Function Implementations
*******************************************************************************/

jobject mamaTransportImpl_getConnectionObject(JNIEnv *env, const void *platformInfo, transportListenerClosure *transportClosure)
{
    /* Returns. */
    jobject ret = NULL;

    /* Only create an object if the platform info is valid. */
    if(NULL != platformInfo)
    {
        /* Cast the platform info to a C connection object. */
        mamaConnection *cConnection = (mamaConnection *)platformInfo;

        /* Save the pointer to the C connection object inside the re-usable Java connection object. */
        (*env)->SetLongField(env, transportClosure->mConnection, connectionPointerFieldId_g, CAST_POINTER_TO_JLONG(*cConnection));

        /* Return a reference to the re-usable object. */
        ret = transportClosure->mConnection;
    }

    return ret;
}

