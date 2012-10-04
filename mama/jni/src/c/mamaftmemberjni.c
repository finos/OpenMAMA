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
* MamaFtMember.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaFtMember.h"
#include <mama/ft.h>

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/

/* Used as a closure to the underlying registering of the C ft state callbacks.
   Needed to maintain global references to a number of Java objects during the
   lifetime of the MamaFtMember.*/
typedef struct ftStateChangeCallbackClosure_
{
    /*The Java MamaFtMemberCallback instance.
     Stored as a global reference. Destroyed when ft member is destroyed*/
    jobject mClientJavaCallback;

    /*Global reference to the Java MamaFtMember instance.
     Stored as a global reference. Destroyed when the ft member is destroyed.*/
    jobject mJavaFtMember;
} ftStateChangeCallbackClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern  JavaVM*     javaVM_g;

jmethodID           ftMemberConstructorId_g               = NULL;
jfieldID            ftMemberPointerFieldId_g              = NULL;
static jfieldID     ftStateChangeCallbackClosureFieldId_g = NULL;
static jmethodID    ftCallbackOnStateChange_g             = NULL;
extern jfieldID     queuePointerFieldId_g;
extern jfieldID     transportPointerFieldId_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/
/*Callback for FT state change*/
static void MAMACALLTYPE ftStateChangeCallback (mamaFtMember ftMember,
                                                const char*  groupName,
                                                mamaFtState  state,
                                                void*        closure);

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_destroy 
    (JNIEnv* env, jobject this)
{
    mama_status status                              = MAMA_STATUS_OK;
    jlong       ftMemberPointer                     = 0;
    jlong       ftStateChangeCallbackClosurePointer = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.destroy(): Null parameter, MamaFtMember may have already been destroyed.");

    ftStateChangeCallbackClosurePointer =
        (*env)->GetLongField(env,this,ftStateChangeCallbackClosureFieldId_g);

    /*Will have the default 0 value if a monitor was not set.*/
    if (ftStateChangeCallbackClosurePointer!=0)
    {
        ftStateChangeCallbackClosure* closure =
            CAST_JLONG_TO_POINTER(ftStateChangeCallbackClosure*,ftStateChangeCallbackClosurePointer);
        
        if(closure->mClientJavaCallback)
        {
            (*env)->DeleteGlobalRef(env,closure->mClientJavaCallback);
            closure->mClientJavaCallback = NULL;
        }

        if(closure->mJavaFtMember)
        {
            (*env)->DeleteGlobalRef(env,closure->mJavaFtMember);
            closure->mJavaFtMember = NULL;
        }
        free (closure);

        (*env)->SetLongField(env, this,
         ftStateChangeCallbackClosureFieldId_g,
         0);
    }

    if (MAMA_STATUS_OK!=(status=mamaFtMember_destroy(
                                    CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_destroy() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    (*env)->SetLongField(env, this,
            ftMemberPointerFieldId_g,
            0);

    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    activate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_activate
    (JNIEnv* env, jobject this)
{
    mama_status status          = MAMA_STATUS_OK;
    jlong       ftMemberPointer = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.activate(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_activate(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_activate() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    deactivate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_deactivate
    (JNIEnv* env, jobject this)
{
    mama_status status          = MAMA_STATUS_OK;
    jlong       ftMemberPointer = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.deactivate(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_deactivate(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_deactivate() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return;
}
   
/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    isActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaFtMember_isActive
    (JNIEnv* env, jobject this)
{
    mama_status status          = MAMA_STATUS_OK;
    jlong       ftMemberPointer = 0;
    int         isActive        = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.isActive(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_isActive(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                &isActive)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_isActive() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return (jboolean)isActive;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    getGroupName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaFtMember_getGroupName
    (JNIEnv* env, jobject this)
{
    mama_status status          = MAMA_STATUS_OK;
    jlong       ftMemberPointer = 0;
    const char* groupName       = NULL;
    jstring     retValue        = NULL;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.getGroupName(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_getGroupName(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                &groupName)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_getGroupName() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    if (groupName)
    {
        retValue = (*env)->NewStringUTF(env, groupName);
    }

    return retValue;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    getWeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaFtMember_getWeight
    (JNIEnv* env, jobject this) 
{
    mama_status status          = MAMA_STATUS_OK;
    jlong       ftMemberPointer = 0;
    mama_u32_t  weight          = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.getWeight(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_getWeight(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                &weight)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_getWeight() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return (jint)weight;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    getHeartbeatInterval
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaFtMember_getHeartbeatInterval
    (JNIEnv* env, jobject this)
{
    mama_status status            = MAMA_STATUS_OK;
    jlong       ftMemberPointer   = 0;
    double      heartbeatInterval = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.getHeartbeatInterval(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_getHeartbeatInterval(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                &heartbeatInterval)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_getHeartbeatInterval() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return (jdouble)heartbeatInterval;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    getTimeoutInterval
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaFtMember_getTimeoutInterval
    (JNIEnv* env, jobject this)
{
    mama_status status           = MAMA_STATUS_OK;
    jlong       ftMemberPointer  = 0;
    double      timeoutInterval  = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.getTimeoutInterval(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_getTimeoutInterval(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                &timeoutInterval)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_getTimeoutInterval() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return (jdouble)timeoutInterval;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    setWeight
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_setWeight 
    (JNIEnv* env, jobject this, jint weight)
{
    mama_status status           = MAMA_STATUS_OK;
    jlong       ftMemberPointer  = 0;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.setWeight(): Null parameter, MamaFtMember may have already been destroyed.");

    assert(0!=ftMemberPointer);

    if (MAMA_STATUS_OK!=(status=mamaFtMember_setWeight(
                                CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                                (int)weight)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_setWeight() ",
                status);
        utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    setInstanceId
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_setInstanceId 
    (JNIEnv* env, jobject this, jstring id)
{
    mama_status status           = MAMA_STATUS_OK;
    jlong       ftMemberPointer  = 0;
    const char* instanceId       = NULL;
    char        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

     ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.getSetInstanceId(): Null parameter, MamaFtMember may have already been destroyed.");
    
    if (id)
    {
        instanceId = (*env)->GetStringUTFChars(env,id,0);
        
        if(!instanceId) return;

        if(MAMA_STATUS_OK!=(status=mamaFtMember_setInstanceId(
                        CAST_JLONG_TO_POINTER(mamaFtMember,ftMemberPointer),
                        instanceId)))
        {
            utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "mamaFtMember_setInstanceId() failed.",
                    status);
            utils_throwMamaException(env,errorString);
        }

        (*env)->ReleaseStringUTFChars(env,id,instanceId);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_create
    (JNIEnv* env,
     jobject this)
{
    mama_status     status      = MAMA_STATUS_OK;
    mamaFtMember    ftMember    = 0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (MAMA_STATUS_OK!=(status=mamaFtMember_create(&ftMember)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaFtMember_create() ",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env, this, ftMemberPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(ftMember));
    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    setupFtMember
 * Signature: (ILcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaFtMemberCallback;Lcom/wombat/mama/MamaTransport;Ljava/lang/String;IDD)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_setupFtMember 
    (JNIEnv*    env, 
     jobject    this, 
     jshort     ftType, 
     jobject    queue, 
     jobject    callback,
     jobject    transport, 
     jstring    name,
     jint       weight,
     jdouble    heartbeatInterval,
     jdouble    timeoutInterval) 
{
    mama_status                     status           = MAMA_STATUS_OK;
    jlong                           ftMemberPointer  = 0;
    jlong                           transportPointer = 0;
    jlong                           queuePointer     = 0;
    mamaTransport                   transport_c      = NULL;
    mamaQueue                       queue_c          = NULL;
    ftStateChangeCallbackClosure*   closureData      = NULL;
    const char*                     groupName        = NULL;
    char                            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    ftMemberPointer = (*env)->GetLongField(env,this,ftMemberPointerFieldId_g);
         MAMA_THROW_NULL_PARAMETER_RETURN_VOID(ftMemberPointer,
    "MamaFtMember.setup(): Null parameter, MamaFtMember may have already been destroyed.");

    if (name)
    {
        groupName = (*env)->GetStringUTFChars(env,name,0);
        
        if(!groupName) return;
    }

    closureData = (ftStateChangeCallbackClosure*) calloc (1, sizeof
            (ftStateChangeCallbackClosure));
    
    if (!closureData)
    {
        utils_throwMamaException(env,"setupFtMember():"
                        " Could not allocate.");
        return;
    }

    /* Store the ft member and the callback as globals in the callback */ 
    closureData->mJavaFtMember = (*env)->NewGlobalRef(env, this);
    closureData->mClientJavaCallback = (*env)->NewGlobalRef(env,callback);

    if (transport)
    {
        transportPointer = (*env)->GetLongField(env, transport, transportPointerFieldId_g); 
        transport_c      = CAST_JLONG_TO_POINTER(mamaTransport, transportPointer);
    }

    if (queue)
    {
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c      = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    if (MAMA_STATUS_OK!=(status=mamaFtMember_setup (
                                CAST_JLONG_TO_POINTER(mamaFtMember, ftMemberPointer),
                                (mamaFtType)ftType, 
                                queue_c,
                                ftStateChangeCallback,
                                transport_c,
                                groupName,
                                (mama_u32_t)weight,
                                (mama_f64_t)heartbeatInterval,
                                (mama_f64_t)timeoutInterval,
                                (void*)closureData)))
    {
        utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "mamaFtMember_setup() ",
                    status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env,
                         this,
                         ftStateChangeCallbackClosureFieldId_g,
                         CAST_POINTER_TO_JLONG(closureData));

    (*env)->ReleaseStringUTFChars(env, name, groupName);

    return;
}

/*
 * Class:     com_wombat_mama_MamaFtMember
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaFtMember_initIDs
    (JNIEnv* env, jclass class)
{
    jclass ftStateChangeCallbackClass = NULL;

    int i=0;

    ftMemberPointerFieldId_g = (*env)->GetFieldID(env,
                            class,"ftMemberPointer_i",
                            UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    ftStateChangeCallbackClosureFieldId_g = (*env)->GetFieldID(env,
                            class,"ftCallbackClosure_i",
                            UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    /* Get a reference to the state change callback class */
    ftStateChangeCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaFtCallback");

    if (!ftStateChangeCallbackClass) return;

    ftCallbackOnStateChange_g = (*env)->GetMethodID(env, ftStateChangeCallbackClass,
            "onFtStateChange", "(Lcom/wombat/mama/MamaFtMember;Ljava/lang/String;S)V");

    if (!ftCallbackOnStateChange_g)
    {
        (*env)->DeleteLocalRef(env, ftStateChangeCallbackClass);
    }

    ftMemberConstructorId_g = (*env)->GetMethodID(env, class,
                              "<init>", "()V");

    return;
}

void MAMACALLTYPE ftStateChangeCallback (mamaFtMember ftMember,
                            const char*  name,
                            mamaFtState  state,
                            void*        closure)
{
    JNIEnv* env                                 = NULL;
    jstring groupName                           = NULL;
    ftStateChangeCallbackClosure* closureData   = 
        (ftStateChangeCallbackClosure*)closure;    

    env = utils_getENV(javaVM_g);

    if (!env) return;

    if (name)
    {
        groupName = (*env)->NewStringUTF(env, name);
    }

    if (!closureData)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "ftStateChangeCallback(): NULL Closure");
        return;
    }

    if (!closureData->mClientJavaCallback)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "ftStateChangeCallback(): NULL Closure");
        return;
    }

    if (!closureData->mJavaFtMember)
    {
        mama_log (MAMA_LOG_LEVEL_NORMAL,
                  "ftStateChangeCallback(): NULL Closure");
        return;
    }

    /* Call the onFtStateChange Java callback */
    (*env)->CallVoidMethod(env,
                           closureData->mClientJavaCallback,
                           ftCallbackOnStateChange_g,
                           closureData->mJavaFtMember,
                           groupName,
                           state);
}
