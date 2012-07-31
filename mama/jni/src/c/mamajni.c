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
* Implementation for each of the native methods defined in the Mama.java 
* source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_Mama.h"

#include <errno.h>

/******************************************************************************
* Global/Static variables
*******************************************************************************/

/* Global reference to the JVM  - used for callbacks */
JavaVM* javaVM_g    =   NULL;

/*Fields and methods defined in other files.*/
extern  jmethodID   bridgeConstructorId_g;
extern  jfieldID    bridgePointerFieldId_g;
extern  jmethodID   payloadBridgeConstructorId_g;
extern  jfieldID    payloadBridgePointerFieldId_g;
extern  jmethodID   queueConstructorId_g;
extern  jfieldID    queuePointerFieldId_g;

static  jmethodID   onLogId_g						=   NULL;
static  jmethodID   onLogSizeExceededId_g			=   NULL;
static  jmethodID   onStartBackgroundCompleted_g	=   NULL;
static  jobject     mLogSizeCallback				=   NULL;
static  jobject     mLogCallback					=   NULL;
static  jobject     mStartBackgroundCallback		=   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/
static  void        logSizeCallback ();
static	void MAMACALLTYPE	onMamaLog(MamaLogLevel level, const char *format, va_list argumentList);
static	void MAMACALLTYPE	startBackgroundCallback(int status);

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
   This function is called by the JVM JNI environment the first time the
   library containing this file is loaded.
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved)
{
    /*
        We cache the jvm pointer for future use in asynchronous callbacks. 
    */
    javaVM_g = jvm;

    /*As per the spec*/ 
    return JNI_VERSION_1_2;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    loadBridge
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_Mama_loadBridge
  (JNIEnv* env, jclass class, jstring middleware, jstring path)
{
    char        errorString     [UTILS_MAX_ERROR_STRING_LENGTH];
    mama_status status          = MAMA_STATUS_OK;
    const char* middleware_c    = NULL;
    mamaBridge  bridge          = NULL;
    jobject     result          = NULL;
    const char* path_c          = NULL;
    jclass      bridgeClass     =
                    (*env)->FindClass(env,"com/wombat/mama/MamaBridge");
    if (middleware)
    {
        middleware_c = (*env)->GetStringUTFChars (env,middleware,0);
    }

    if (path)
    {
        path_c = (*env)->GetStringUTFChars (env,path,0);
    }

    if (!middleware_c) return NULL;/*Exception auto thrown*/

    if (MAMA_STATUS_OK !=
       (status = mama_loadBridgeWithPath (&bridge, middleware_c, path_c)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "loadBridge(): Failed to load mama bridge.",
                            status);
       if (middleware_c) (*env)->ReleaseStringUTFChars(env,
                                                    middleware,
                                                    middleware_c);
       if (path_c) (*env)->ReleaseStringUTFChars(env,
                                              path,
                                              path_c);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    result = (*env)->NewObject(env, bridgeClass,
                               bridgeConstructorId_g, JNI_FALSE);
    (*env)->SetLongField(env, result, bridgePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(bridge));

    /*Release any strings created*/
    if (middleware_c) (*env)->ReleaseStringUTFChars(env,
                                                    middleware,
                                                    middleware_c);
    if (path_c) (*env)->ReleaseStringUTFChars(env,
                                              path,
                                              path_c);    

    (*env)->DeleteLocalRef(env, bridgeClass);
    return result;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    loadPayloadBridge
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_Mama_loadPayloadBridge
  (JNIEnv* env, jclass class, jstring name)
{
    char                errorString        [UTILS_MAX_ERROR_STRING_LENGTH];
    mama_status         status             = MAMA_STATUS_OK;
    const char*         name_c             = NULL;
    mamaPayloadBridge   payloadBridge      = NULL;
    jobject             result             = NULL;
    jclass              payloadBridgeClass =
                    (*env)->FindClass(env,"com/wombat/mama/MamaPayloadBridge");
    if (name)
    {
        name_c = (*env)->GetStringUTFChars (env,name,0);
    }

    if (!name_c) return NULL;/*Exception auto thrown*/

    if (MAMA_STATUS_OK !=
       (status = mama_loadPayloadBridge (&payloadBridge, name_c)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "Mama.loadPayloadBridge(): Failed to load payload bridge.",
                            status);
       if (name_c) (*env)->ReleaseStringUTFChars(env,
                                                 name,
                                                 name_c);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    result = (*env)->NewObject(env, payloadBridgeClass,
                               payloadBridgeConstructorId_g);
    (*env)->SetLongField(env, result, payloadBridgePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(payloadBridge));

    /*Release any strings created*/
    if (name_c) (*env)->ReleaseStringUTFChars(env,
                                              name,
                                              name_c);

    (*env)->DeleteLocalRef(env, payloadBridgeClass);
    return result;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    loadBridge
 * Signature: (Lcom/wombat/mama/MamaBridge;)Lcom/wombat/mama/MamaQueue;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_Mama_getDefaultQueue
  (JNIEnv* env, jclass class, jobject bridge)
{
    char        errorString     [UTILS_MAX_ERROR_STRING_LENGTH];
    mama_status status          = MAMA_STATUS_OK;
    jobject     result          = NULL;
    mamaQueue   queue           = NULL;
    jclass      queueClass      =
                    (*env)->FindClass(env,"com/wombat/mama/MamaQueue");
    jlong  bridgeFieldPointer =
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);
    assert(0!=bridgeFieldPointer);

    if (MAMA_STATUS_OK != (status = mama_getDefaultEventQueue (
            CAST_JLONG_TO_POINTER (mamaBridge,bridgeFieldPointer), &queue)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "getDefaultQueue(): Failed to get default queue",
                            status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    result = (*env)->NewObject(env, queueClass,
                               queueConstructorId_g, JNI_FALSE);
    (*env)->SetLongField(env, result, queuePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(queue));

    (*env)->DeleteLocalRef(env, queueClass);
    return result;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    open
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_open__(JNIEnv* env, jclass class)
{
    /* Call the overload with NULL arugments. */
    Java_com_wombat_mama_Mama_open__Ljava_lang_String_2Ljava_lang_String_2(env, class, NULL, NULL);
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    open
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_open__Ljava_lang_String_2Ljava_lang_String_2(JNIEnv* env, jclass class, jstring path, jstring filename)
{
    /* Errors. */
    mama_status ret = MAMA_STATUS_OK;

    /* The mama_open call can now be made multiple times with a reference count
     * being held. Initialisation should only be performed the first time.
     */
    /* Convert the strings to ANSI. */
    char *convertedFile = NULL;
    char *convertedPath = NULL;
    if(NULL != filename)
    {
        convertedFile = (*env)->GetStringUTFChars(env, filename, 0);
    }
    if(NULL != path)
    {
        convertedPath = (*env)->GetStringUTFChars(env, path, 0);
    }

    /* Call the C layer. */
    ret = mama_openWithProperties(convertedPath, convertedFile);

    /* Free the strings. */
    if(NULL != convertedFile)
    {
        (*env)->ReleaseStringUTFChars(env, filename, convertedFile);
    }

    if(NULL != convertedPath)
    {
        (*env)->ReleaseStringUTFChars(env, path, convertedPath);
    }

    /* The status return must be converted into an exception. */
    if(MAMA_STATUS_OK != ret)
    {
        /* Convert the error code into an exception. */
        char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "open(): Failed to open Mama.",
                            ret);

        /* Throw the exception. */
        utils_throwMamaException(env, errorString);
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    start
 * Signature: (Lcom/wombat/mama/MamaBridge;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_start
  (JNIEnv* env , jclass class, jobject bridge)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong  bridgeFieldPointer =
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);
    assert(0!=bridgeFieldPointer);

    if (MAMA_STATUS_OK != (status =
       mama_start (CAST_JLONG_TO_POINTER (mamaBridge,bridgeFieldPointer))))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "start(): Failed to start Mama.",
                            status);
        utils_throwMamaException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    startBackground
 * Signature: (Lcom/wombat/mama/MamaBridge;Lcom/wombat/mama/MamaStartBackgroundCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_startBackground
  (JNIEnv* env, jclass class, jobject bridge, jobject callback)
{
	mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

	/* Get the bridge pointer. */
    jlong  bridgeFieldPointer =
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);
    assert(0!=bridgeFieldPointer);

	/* Save the callback object. */
	mStartBackgroundCallback = (*env)->NewGlobalRef (env, callback);

	/* Call the C mama_startBackground function. */
    if (MAMA_STATUS_OK != (status =
       mama_startBackground (CAST_JLONG_TO_POINTER (mamaBridge,bridgeFieldPointer), startBackgroundCallback )))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "startBackground(): Failed to start Mama.",
                            status);
        utils_throwMamaException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    stop
 * Signature: (Lcom/wombat/mama/MamaBridge;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_stop
  (JNIEnv* env , jclass class, jobject bridge)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong  bridgeFieldPointer =
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);
    assert(0!=bridgeFieldPointer);

    if (MAMA_STATUS_OK != (status =
        mama_stop (CAST_JLONG_TO_POINTER (mamaBridge,bridgeFieldPointer))))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "stop(): Failed to stop Mama.",
                            status);

        utils_throwMamaException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_close
  (JNIEnv* env , jclass class)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_close()))
    { 
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "close(): Failed to close Mama.",
                            status);

        utils_throwMamaException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setProperty
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setProperty
  (JNIEnv* env, jclass class, jstring name, jstring value)
{
    mama_status status = MAMA_STATUS_OK;
    const char* c_name = NULL;
    const char* c_value = NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (name)
    {
      c_name = (*env)->GetStringUTFChars (env,name,0);
      if (!c_name) return;/*Exception auto thrown*/
    }

    if (value)
    {
        c_value = (*env)->GetStringUTFChars (env,value,0);
        if (!c_value)
        {
            (*env)->ReleaseStringUTFChars(env, name, c_name);
            return;/*Exception auto thrown*/
        }
    }

    if (MAMA_STATUS_OK!=(status=mama_setProperty (c_name, c_value)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "setProperty(): Failed to setProperty.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    if (c_name)(*env)->ReleaseStringUTFChars(env, name, c_name);
    if (c_value)(*env)->ReleaseStringUTFChars(env, value, c_value);

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    logToFile
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_Mama_logToFile
  (JNIEnv* env, jclass class, jstring file, jint mamalevel)
{
    const char* filename_c = NULL;

    if (file)
    {
        filename_c = (*env)->GetStringUTFChars (env,file,0);

        if (filename_c)
        {
            if (MAMA_STATUS_OK != mama_logToFile(filename_c, (int)mamalevel))
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }

        (*env)->ReleaseStringUTFChars(env,file, filename_c);
    }
    else
    {
        return -1;
    }

    return MAMA_STATUS_OK;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    _disableLogging
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_Mama__1disableLoggingJni
  (JNIEnv* env, jclass class)
{
    mama_status status = MAMA_STATUS_OK;
    long closurePointer = 0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_disableLogging()))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_disableLogging(): Failed to disableLogging.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    if(mLogSizeCallback)
    {
        (*env)->DeleteGlobalRef(env,mLogSizeCallback);
        mLogSizeCallback = NULL;
    }

    return status;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    _getLogLevel
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_Mama__1getLogLevel
  (JNIEnv* env, jclass class)
{
    return mama_getLogLevel();
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setLogSize
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setLogSize
  (JNIEnv* env, jclass class, jlong size)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_setLogSize((unsigned long)size)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_setLogSize(size): Failed to setLogSize.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setMamaLogLevel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setMamaLogLevel
  (JNIEnv* env, jclass class, jint mamalevel)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_setLogLevel((int)mamalevel)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_setLogLevel(mamalevel): Failed to setLogLevel.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setNumLogFiles
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setNumLogFiles
  (JNIEnv* env, jclass class, jint numFiles)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_setNumLogFiles(numFiles)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_setNumLogFiles(numFiles): Failed to setNumLogFiles.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setLogFilePolicy
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setLogFilePolicy
  (JNIEnv* env, jclass class, jint policy)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK != (status = mama_setLogFilePolicy((int)policy)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_setLogFilePolicy(policy): Failed to setLogFilePolicy.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setAppendToLogFile
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setAppendToLogFile
  (JNIEnv* env, jclass class, jboolean append)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    int append_c = 0;

    if(JNI_TRUE == append)
    {
        append_c = 1;
    }

    if (MAMA_STATUS_OK != (status = mama_setAppendToLogFile(append_c)))
    {
        utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "mama_setAppendToLogFile(append): Failed to setAppendToLogFile.",
                            status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    loggingToFile
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_Mama_loggingToFile
  (JNIEnv* env, jclass class)
{
    if (0 == mama_loggingToFile())
    {
        return JNI_FALSE;
    }
    else
    {
        return JNI_TRUE;
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setLogSizeCb
 * Signature: (Lcom/wombat/mama/MamaLogFileCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setLogSizeCb
  (JNIEnv* env, jclass class, jobject callback)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    mLogSizeCallback = (*env)->NewGlobalRef (env, callback);

    if (MAMA_STATUS_OK != (status = mama_setLogSizeCb(logSizeCallback)))
    {
        (*env)->DeleteGlobalRef(env,mLogSizeCallback);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to set log size callback.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    return;
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setApplicationName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setApplicationName
(JNIEnv * env, jclass class, jstring applicationName)
{
    const char* applicationName_c = NULL;
    mama_status status            = MAMA_STATUS_OK;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];


    if (applicationName)
    {
        applicationName_c = (*env)->GetStringUTFChars (env,applicationName,0);
        if (!applicationName_c) return;/*Exception auto thrown*/
    }

    if (MAMA_STATUS_OK!=(status=mama_setApplicationName(
                             applicationName_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "setApplicationName(): Failed to set application name.",
            status);
        utils_throwMamaException(env,errorString);
    }

    /*Release any strings created*/
    if (applicationName_c) (*env)->ReleaseStringUTFChars(env,
                                                         applicationName,
                                                         applicationName_c);
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    setApplicationClassName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setApplicationClassName
(JNIEnv * env, jclass class, jstring applicationClassName)
{
    const char* applicationClassName_c = NULL;
    mama_status status                 =   MAMA_STATUS_OK;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (applicationClassName)
    {
        applicationClassName_c = (*env)->GetStringUTFChars (env,
                                                            applicationClassName,0);
        if (!applicationClassName_c) return;/*Exception auto thrown*/
    }

    if (MAMA_STATUS_OK!=(status=mama_setApplicationClassName(
                             applicationClassName_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "setApplicationClassName(): Failed to set class name.",
            status);
        utils_throwMamaException(env,errorString);
    }

    /*Release any strings created*/
    if (applicationClassName_c) (*env)->ReleaseStringUTFChars(env, 
                                                              applicationClassName,
                                                              applicationClassName_c);
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getApplicationName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getApplicationName
  (JNIEnv * env, jclass class)
{
    const char*     applicationName    =   NULL;
    mama_status     status             =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    status=mama_getApplicationName(&applicationName);
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, applicationName);
        case MAMA_STATUS_NULL_ARG:
            return NULL;
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mama_getApplicationName() Failed.",
                status);
            utils_throwMamaException(env,errorString);
        return NULL;
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getApplicationClassName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getApplicationClassName
  (JNIEnv *env, jclass class)
{
    const char*     applicationClassName    =   NULL;
    mama_status     status                  =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    status=mama_getApplicationClassName(&applicationClassName);
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, applicationClassName);
        case MAMA_STATUS_NULL_ARG:
            return NULL;
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mama_getApplicationName() Failed.",
                status);
            utils_throwMamaException(env,errorString);
        return NULL;
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getCVersion
 * Signature: (Lcom/wombat/mama/MamaBridge;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getCVersion
    (JNIEnv *env, jclass class, jobject bridge)
{
    jlong  bridgeFieldPointer =
        (*env)->GetLongField(env,bridge,bridgePointerFieldId_g);
    const char* version =
        mama_getVersion (CAST_JLONG_TO_POINTER (mamaBridge,
                                                bridgeFieldPointer));
    return (*env)->NewStringUTF(env,version);
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getUserName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getCUserName
  (JNIEnv *env, jclass class)
{
     const char*     userName    =   NULL;
     mama_status     status      =   MAMA_STATUS_OK;
     char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

     status=mama_getUserName(&userName);
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, userName);
        case MAMA_STATUS_NULL_ARG:
            return NULL;
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mama_getUserName() Failed.",
                status);
            utils_throwMamaException(env,errorString);
        return NULL;
    }
 }

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getCHostName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getCHostName
  (JNIEnv *env, jclass class)
{
     const char*     hostName    =   NULL;
     mama_status     status      =   MAMA_STATUS_OK;
     char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

     status=mama_getHostName(&hostName);
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, hostName);
        case MAMA_STATUS_NULL_ARG:
            return NULL;
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mama_getHostName(): Failed.",
                status);
            utils_throwMamaException(env,errorString);
        return NULL;
    }
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getProperty
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_Mama_getProperty
  (JNIEnv* env, jclass class, jstring name)
{
    mama_status status = MAMA_STATUS_OK;
    const char* c_name = NULL;
    const char* c_retVal = NULL;

    if (name)
    {
      c_name = (*env)->GetStringUTFChars(env,name,0);
      if (!c_name) return; /* Exception auto thrown */
    }

    c_retVal=mama_getProperty(c_name);
    if (NULL==c_retVal)
      return NULL;
    else
      return (*env)->NewStringUTF(env, c_retVal);
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    getLastErrorCode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_Mama_getLastErrorCode
  (JNIEnv* env, jclass class)
{
    return mama_getLastErrorCode();
}

/*
 * Class:     com_wombat_mama_Mama
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_initIDs
  (JNIEnv* env, jclass class)
{
    jclass mamaLogFileCallbackClass = NULL;
	jclass mamaLogCallbackClass = NULL;
	jclass mamaStartBackgroundCallbackClass = NULL;

    mamaLogFileCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaLogFileCallback");

    if (!mamaLogFileCallbackClass)
        return; /*Exception auto thrown*/

    onLogSizeExceededId_g = (*env)->GetMethodID(
                        env,
                        mamaLogFileCallbackClass,
                        "onLogSizeExceeded",
                        "()V");

    /*Tidy up local refs*/
    (*env)->DeleteLocalRef(env, mamaLogFileCallbackClass);


	/* The start background callback. */
	/* Find the callback class. */
	mamaStartBackgroundCallbackClass = (*env)->FindClass(env, "com/wombat/mama/MamaStartBackgroundCallback");
	if(mamaStartBackgroundCallbackClass != NULL)
	{
		/* Get the Id of its callback method. */
		onStartBackgroundCompleted_g = (*env)->GetMethodID(
						env,
						mamaStartBackgroundCallbackClass,
						"onStartComplete",
						"(I)V");
	}

	/*Tidy up local refs*/
	(*env)->DeleteLocalRef(env, mamaStartBackgroundCallbackClass);	

	/* The onLog callback. */
	/* Find the callback class. */
	mamaLogCallbackClass = (*env)->FindClass(env, "com/wombat/mama/Mama$LogCallbackWrapper");
	if(mamaLogCallbackClass != NULL)
	{
		/* Get the Id of its callback method. */
		onLogId_g = (*env)->GetMethodID(
			env,
			mamaLogCallbackClass,
			"onNativeLogCallback",			
			"(ILjava/lang/String;)V");
	}

	/*Tidy up local refs*/
	(*env)->DeleteLocalRef(env, mamaLogCallbackClass);
}

void logSizeCallback()
{
    JNIEnv* env = NULL;
    env = utils_getENV(javaVM_g);
    if (!env)
        return;/*Can't throw exception without JNIEnv*/

    if (mLogSizeCallback)
    {
        (*env)->CallVoidMethod(env,
                               mLogSizeCallback,
                               onLogSizeExceededId_g);
    }

    return;
}

JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_log
  (JNIEnv* env, jclass class, jint logLevel, jstring message)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
	const char* cMessage = NULL;

	if(message != NULL)
    {
		/* Convert the jstring to a c string. */
		cMessage = (*env)->GetStringUTFChars(env, message, 0);
		if(!cMessage) 
		{
			return;/*Exception auto thrown*/
		}

		/* Call the mama function. */
		mama_log(logLevel, cMessage);

		/* Release the C string. */
		(*env)->ReleaseStringUTFChars(env, message, cMessage);
    }
}

JNIEXPORT void JNICALL Java_com_wombat_mama_Mama_setNativeLogCallback
  (JNIEnv* env, jclass class, jobject callback)
{
    mama_status status = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    mLogCallback = (*env)->NewGlobalRef (env, callback);

    if (MAMA_STATUS_OK != (status = mama_setLogCallback(onMamaLog)))
    {
        (*env)->DeleteGlobalRef(env,mLogCallback);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to set log callback.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    return;
}

/*  Description :   This function receives the mama log messages when setLogCallback is
 *					invoked. It will format the arguments into a single string and send
 *					it onto the java.
 *  Arguments   :   level           [I] The logging level.
 *                  format          [I] Used to format the variable argument list.
 *                  argumentList    [I] Variable list of arguments to include in the log.
 */
void MAMACALLTYPE onMamaLog(MamaLogLevel level, const char *format, va_list argumentList)
{
	/* Get the java environment. */
	JNIEnv *env = utils_getENV(javaVM_g);
	if(env != NULL)
	{    
		/* Only continue if the callback has been supplied. */
		if(mLogCallback != NULL)
		{
			/* Format the message as a string. */
			char buffer[4096] = "";

			/* Format the message into the buffer. */
			vsnprintf(buffer, (sizeof(buffer) - 1), format, argumentList);
		
			/* Add a null terminator. */
			strcat(buffer, "\0");
			{
				/* Convert the character pointer to a java string. */
				jstring javaBuffer = (*env)->NewStringUTF(env, buffer);	

				/* Create a new object array. */
				(*env)->CallVoidMethod(env, mLogCallback, onLogId_g, level, javaBuffer);			
			}
		}
	}

    return;
}

void MAMACALLTYPE startBackgroundCallback(int status)
{
	JNIEnv* env = NULL;
    env = utils_getENV(javaVM_g);
    if (!env)
        return;/*Can't throw exception without JNIEnv*/

    if (mStartBackgroundCallback)
    {
		/* Invoke the method with the status code, note that CallVoidMethod expects
		 * a void return type.
		 */
        (*env)->CallVoidMethod(env,
                               mStartBackgroundCallback,
                               onStartBackgroundCompleted_g,
							   status);
    }    
}
