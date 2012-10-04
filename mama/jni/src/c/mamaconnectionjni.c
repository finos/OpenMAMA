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
* MamaConnection.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaConnection.h"
#include <mama/conflation/connection.h>
#include <assert.h>

/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern  JavaVM*     javaVM_g;

jfieldID            connectionPointerFieldId_g = NULL;
extern jfieldID     transportPointerFieldId_g;
extern jmethodID    transportConstructorId_g;

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaConnection_destroy
    (JNIEnv* env, jobject this)
{
    mama_status status                              = MAMA_STATUS_OK;
    jlong       connectionPointer                   = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    connectionPointer = (*env)->GetLongField(env,this,connectionPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(connectionPointer,
    "MamaConnection.destroy(): Null parameter, MamaFtConnection may have already been destroyed.");

    if (MAMA_STATUS_OK!=(status=mamaConnection_free(
                                    CAST_JLONG_TO_POINTER(mamaConnection,connectionPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaConnection_destroy() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    (*env)->SetLongField(env, this,
            connectionPointerFieldId_g,
            0);

    return;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    create
 * Signature: (Lcom/wombat/mama/MamaTransport;Ljava/lang/String;IIIIILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaConnection_create
    (JNIEnv* env,
     jobject this,
     jobject transport,
     jstring ipaddress,
     jint    port,
     jint    maxQueueSize,
     jint    curQueueSize,
     jint    msgCount,
     jint    bytesSent,
     jstring userName,
     jstring appName)
{
    mama_status     status           = MAMA_STATUS_OK;
    mamaConnection  connection       = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    const char*     cIpAddress       = NULL;
    const char*     cUserName        = NULL;
    const char*     cAppName         = NULL;
    jlong           transportPointer = 0;

    if (ipaddress)
    {
        cIpAddress = (*env)->GetStringUTFChars (env, ipaddress, 0);
        if (!cIpAddress)
        {
            return;
        }
    }

    if (userName)
    {
        cUserName = (*env)->GetStringUTFChars (env, userName, 0);
        if (!cUserName)
        {
            return;
        }
    }

    if (appName)
    {
        cAppName = (*env)->GetStringUTFChars (env, appName, 0);
        if (!cAppName)
        {
            return;
        }
    }

    if (transport)
    {
        transportPointer = (*env)->GetLongField (env, transport, transportPointerFieldId_g);
    }

    if (MAMA_STATUS_OK!=(status=mamaConnection_allocate(&connection,
                                                        CAST_JLONG_TO_POINTER (mamaTransport, transportPointer),
                                                        cIpAddress,
                                                        (uint16_t)port,
                                                        (uint32_t)maxQueueSize,
                                                        (uint32_t)curQueueSize,
                                                        (uint32_t)msgCount,
                                                        cUserName,
                                                        cAppName,
                                                        NULL)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaConnection_create() ",
                status);
        (*env)->ReleaseStringUTFChars (env, ipaddress, cIpAddress);
        (*env)->ReleaseStringUTFChars (env, userName, cUserName);
        (*env)->ReleaseStringUTFChars (env, appName, cAppName);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env, this, connectionPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(connection));

     (*env)->ReleaseStringUTFChars (env, ipaddress, cIpAddress);
     (*env)->ReleaseStringUTFChars (env, userName, cUserName);
     (*env)->ReleaseStringUTFChars (env, appName, cAppName);

    return;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getTransport
 * Signature: ()Lcom/wombat/mama/MamaTransport;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaConnection_getTransport
(JNIEnv * env , jobject this)
{
    mamaTransport transport  = NULL;

    jobject      result;
    jlong        connection = 0;
    mama_status status      = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    jclass       transportClass_g = (*env)->FindClass(env,
                                                      "com/wombat/mama/MamaTransport");
    if(!transportClass_g) return;
   
    transportConstructorId_g = (*env)->GetMethodID(env, transportClass_g,
                                                   "<init>", "()V" );
   
    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
   

    if(MAMA_STATUS_OK!=(status= mamaConnection_getTransport(
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &transport)))
    {
        utils_buildErrorStringForStatus(
                  errorString,
                  UTILS_MAX_ERROR_STRING_LENGTH,
                  "mamaConnection_getTransport() failed",
                  status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    if (transport)
    {
        result = (*env)->NewObject(env, transportClass_g,
                                   transportConstructorId_g);
        (*env)->SetLongField(env, result, transportPointerFieldId_g,
                             CAST_POINTER_TO_JLONG(transport));
    }
    return result;

}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getIpAddress
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaConnection_getIpAddress
(JNIEnv * env, jobject this)
{
    const char* name_c          =   NULL;
    jlong       connection      = 0;
    mama_status status          = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
   
    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaConnection_getIpAddress (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getIpAddress() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
   
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getPort
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaConnection_getPort
(JNIEnv * env, jobject this)
{
    int          retVal_c        = 0;
    jlong        connection      = 0;
    mama_status  status          = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", 0);
   
    if(MAMA_STATUS_OK!=(status=mamaConnection_getPort (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getPort() failed",
            status);
        utils_throwWombatException(env,errorString);
    }
   
    return (jint) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getUserName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaConnection_getUserName
(JNIEnv * env, jobject this)
{
    const char*  name_c     = NULL;
    jlong        connection = 0;
    mama_status  status     = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaConnection_getUserName (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getId() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getAppName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaConnection_getAppName
(JNIEnv * env, jobject this)
{
    const char* name_c      =   NULL;
    jlong       connection  = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaConnection_getAppName (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnectionSource_getAppName() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getMaxQueueSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSource_getMaxQueueSize
(JNIEnv * env, jobject this)
{
    uint32_t     retVal_c   = 0;
    jlong        connection = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", 0);
   
    if(MAMA_STATUS_OK!=(status=mamaConnection_getMaxQueueSize (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getMaxQueueSize() failed",
            status);
        utils_throwWombatException(env,errorString);
    }
   
    return (jint) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getQueueSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSource_getQueueSize
(JNIEnv * env, jobject this)
{
    uint32_t     retVal_c    = 0;
    jlong        connection  = 0;
    mama_status  status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", 0);
   
    if(MAMA_STATUS_OK!=(status=mamaConnection_getQueueSize (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getQueueSize() failed",
            status);
        utils_throwWombatException(env,errorString);
    }

    return (jint) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getMsgCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSource_getMsgCount
(JNIEnv * env, jobject this)
{
    uint32_t    retVal_c        = 0;
    jlong       connection      = 0;
    mama_status status          = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", 0);
   
    if(MAMA_STATUS_OK!=(status=mamaConnection_getMsgCount (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getMsgCount() failed",
            status);
        utils_throwWombatException(env,errorString);
    }

    return (jshort) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    getBytesSent
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaConnection_getBytesSent
(JNIEnv * env, jobject this)
{
    uint32_t     retVal_c   = 0;
    jlong        connection = 0;
    mama_status  status     = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", 0);
   
    if(MAMA_STATUS_OK!=(status=mamaConnection_getBytesSent (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_getBytesSent() failed",
            status);
        utils_throwWombatException(env,errorString);
    }

    return (jint) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    toString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaConnection_toString
(JNIEnv * env, jobject this)
{
    const char*  name_c     = NULL;
    jlong        connection = 0;
    mama_status  status     = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
    name_c = mamaConnection_toString(CAST_JLONG_TO_POINTER(mamaConnection,connection));
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    isIntercepted
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaConnection_isIntercepted
(JNIEnv * env, jobject this)
{
    const char*  name_c      = NULL;
    jlong        connection  = 0;
    mama_status  status      = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    connection = (*env)->GetLongField (env,this,connectionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(connection,
        "Null parameter, MamaConnection may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaConnection_isIntercepted (
                            CAST_JLONG_TO_POINTER(mamaConnection,connection),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaConnection_isIntercepted() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaConnection
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaConnection_initIDs
    (JNIEnv* env, jclass class)
{
    connectionPointerFieldId_g = (*env)->GetFieldID (env, class,
                                                     "connectionPointer_i",
                                                     UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    return;
}

