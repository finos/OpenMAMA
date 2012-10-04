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
* MamaSubscription.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaSubscription.h"
#include "mamajni/dictionaryimpl.h"
#include "subscriptioncommon.h"

#include <assert.h>

/******************************************************************************
* Global/Static variables (NB - These are all read only after initialisation)
*******************************************************************************/

/*The controlling JVM - in mamajni.c*/
extern  JavaVM*     javaVM_g;

/*ID's for fields/methods we don't initialise here */
extern  jmethodID   dictionaryConstructorId_g; /*Defined in mamadictionaryjni.c*/

extern  jfieldID    messagePointerFieldId_g; /*Defined in mamamsgjni.c*/
extern  jfieldID    sourcePointerFieldId_g; /*Defined in mamasourcejni.c*/
extern  jfieldID    dictionaryPointerFieldId_g; /*Defined in mamadictionaryjni.c*/
extern  jfieldID    dictionaryClosurePointerFieldId_g; /*Defined in mamadictionaryjni.c*/

extern jfieldID    queuePointerFieldId_g;/*Defined in mamaqueuejni.c*/

/*Cached Id's for fields/methods required here*/
static  jfieldID    subscriptionPointerFieldId_g    =   NULL;
static  jmethodID   subCallbackonCreateId_g         =   NULL;
static  jmethodID   subCallbackonMsgId_g            =   NULL;
static  jmethodID   subCallbackonErrorId_g          =   NULL;
static  jmethodID   subCallbackonQualityId_g        =   NULL;
static  jmethodID   subCallbackonGapId_g            =   NULL;
static  jmethodID   subCallbackonRecapId_g          =   NULL;
static  jmethodID   subCallbackonDestroyId_g        =   NULL;

/*The following method ids belong to the MamaDictionaryCallback. */
static  jmethodID   dictCallbackonTimeout_g     =   NULL;
static  jmethodID   dictCallbackonError_g       =   NULL;
static  jmethodID   dictCallbackonComplete_g    =   NULL;

/*mamasource declaration*/
extern  jfieldID     sourcePointerFieldId_g; /*defined in mamasourcejni.c*/


/****************************************************************************
* Local function declarations
*******************************************************************************/
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

/*Invoked to indicate a data quality event*/
static void MAMACALLTYPE subscriptionQualityCB (mamaSubscription    subscription,
                       mamaQuality         quality,
                       const char*         symbol,
                       short               cause,
                       const void*         platformInfo,
                       void*               closure);    
  
/*Invoked when a subscription is first created*/
static void MAMACALLTYPE subscriptionGapCB (mamaSubscription    subscription,
                   void*               closure);

/*Invoked to indicate a data quality event*/
static void MAMACALLTYPE subscriptionRecapCB (mamaSubscription    subscription,
                     void*               closure); 
                                
/*Invoked to indicate a subscription destroyed event*/
static void MAMACALLTYPE subscriptionDestroyCB (mamaSubscription    subscription,
                     void*               closure); 

                                
/*The following three are callback functions for dictionary */
static void MAMACALLTYPE dictTimeoutCB( mamaDictionary dict, void *closure );
static void MAMACALLTYPE dictErrorCB( mamaDictionary dict, const char *errMsg, void *closure );
static void MAMACALLTYPE dictCompleteCB( mamaDictionary dict, void *closure );

/******************************************************************************
 * Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setRequiresInitial
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaSubscription_setRequiresInitial
  (JNIEnv* env, jobject this, jboolean requiresInitial)
{   
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaSubscription.setRequiresInitial(): Null parameter, subscription may have already been destroyed.");

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_setRequiresInitial(
                    CAST_JLONG_TO_POINTER(
                        mamaSubscription,subscriptionPointer),
                    (int)requiresInitial)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set requires initial for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);   
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    initIDs
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_wombat_mama_MamaSubscription_getRequiresInitital
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    int             requiresInitial     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getRequiresInitital(): Null parameter, subscription may have been destroyed.", 0);

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getRequiresInitial(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &requiresInitial)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get requires initial for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);   
    }
    
    return (jboolean)requiresInitial;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getReceivedInitial
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL
Java_com_wombat_mama_MamaSubscription_getReceivedInitial
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    int             receivedInitial     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getReceivedInitial(): Null parameter, subscription may have been destroyed.", 0);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getReceivedInitial(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &receivedInitial)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get received-initial for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jboolean)receivedInitial;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    activate
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaSubscription_activate
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer,
    "MamaSubscription.activate(): Null parameter, subscription may have been destroyed.");

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_activate(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not activate subscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    deactivate
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaSubscription_deactivate
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer,
    "MamaSubscription.deactivate(): Null parameter, subscription may have been destroyed.");

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_deactivate(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer))))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not deactivate subscription.",
            status);
        utils_throwWombatException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getSymbol
 * Signature: ()Ljava/lang/String
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSubscription_getSymbol
  (JNIEnv* env, jobject this)
{
    const char*     retVal_c            =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getSymbol(): Null parameter, subscription may have been destroyed.", NULL);

    assert(0!=subscriptionPointer);

    if(MAMA_STATUS_OK!=(status=mamaSubscription_getSymbol(
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
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getSubscSource
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSubscription_getSubscSource
  (JNIEnv* env, jobject this)
{
    const char*     retVal_c            =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getSubscSource(): Null parameter, subscription may have been destroyed.", NULL);

    assert(0!=subscriptionPointer);

    if(MAMA_STATUS_OK!=(status=mamaSubscription_getSource(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Symbol for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
        return;
    }

    return (*env)->NewStringUTF(env, retVal_c);
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setTimeout
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setTimeout
  (JNIEnv* env, jobject this, jdouble timeout)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer,
    "MamaSubscription.setTimeout(): Null parameter, subcription may have been destroyed.");
    
    assert(0!=subscriptionPointer);
    
    if(MAMA_STATUS_OK!=(status=mamaSubscription_setTimeout
                (CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                 (double)timeout)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set timeout for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getTimeout
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaSubscription_getTimeout
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    double          timeout             =   0.0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getTimeout(): Null parameter, subcription may have been destroyed.", 0);
    assert(0!=subscriptionPointer);

    if(MAMA_STATUS_OK!=(status=mamaSubscription_getTimeout(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &timeout)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get timeout for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);       
    }

    return (jdouble)timeout;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setServiceLevel
 * Signature: (SJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setServiceLevel
  (JNIEnv* env, jobject this, jshort serviceLevel, jlong serviceLevelOpt)
{
    mama_status          status              =   MAMA_STATUS_OK;
    jlong                subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer,
    "MamaSubscription.setServiceLevel(): Null parameter, subcription may have been destroyed.");
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_setServiceLevel(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    (short)serviceLevel,
                    (long)serviceLevelOpt)))
    {
        utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set service level for mamaSubscription.",
                status);
        utils_throwWombatException (env,errorString);
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getServiceLevel
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaSubscription_getServiceLevel
  (JNIEnv* env, jobject this)
{
    mama_status          status              =   MAMA_STATUS_OK;
    jlong                subscriptionPointer =   0;
    mamaServiceLevel     serviceLevel        =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer,
    "MamaSubscription.getServiceLevel(): Null parameter, subcription may have been destroyed.", 0);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getServiceLevel(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &serviceLevel)))
    {
        utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get service level for mamaSubscription.",
                status);
        utils_throwWombatException (env,errorString);
    }
    
    return (jshort)serviceLevel;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getServiceLevelOptions
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_com_wombat_mama_MamaSubscription_getServiceLevelOptions
  (JNIEnv* env, jobject this)
{
    mama_status          status              =   MAMA_STATUS_OK;
    jlong                subscriptionPointer =   0;
    long                 serviceLevelOpt     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaSubscription.getServiceLevelOptions(): Null parameter, subcription may have been destroyed.", 0);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getServiceLevelOpt(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &serviceLevelOpt)))
    {
        utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get service level options for mamaSubscription.",
                status);
        utils_throwWombatException (env,errorString);
    }
    
    return (jlong)serviceLevelOpt;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    isValid
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_com_wombat_mama_MamaSubscription_isValid
  (JNIEnv* env, jobject this)
{
    jlong           subscriptionPointer =   0;
    int             isValid     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,
                                               subscriptionPointerFieldId_g);

    /* if the subscription pointer has been destroyed, it's pretty inValid */
    if (0==subscriptionPointer)
        return (jboolean)0;

    isValid=mamaSubscription_isValid(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer));

    return (jboolean)isValid;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setRetries
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setRetries
  (JNIEnv* env, jobject this, jint retries)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaSubscription.setRetries(): Null parameter, subcription may have been destroyed.");
    
    assert(0!=subscriptionPointer);
    
    if(MAMA_STATUS_OK!=(status=mamaSubscription_setRetries
                (CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                 (int)retries)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set retries for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getRetries
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSubscription_getRetries
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    int             retries             =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaSubscription.getRetries(): Null parameter, subcription may have been destroyed.", 0);

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getRetries(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &retries)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get retries for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jint)retries;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_destroy
  (JNIEnv* env, jobject this)
{
    jlong           subscriptionPointer =   0;

    subscriptionPointer =
            (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaSubscription.destroy(): Null parameter, subscription may have already been destroyed."); 

    assert(0!=subscriptionPointer);

    subCommon_destroy(env,this,subscriptionPointerFieldId_g);
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    destroyEx
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_destroyEx
  (JNIEnv* env, jobject this)
{
    jlong           subscriptionPointer =   0;

    subscriptionPointer =
            (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaSubscription.destroy(): Null parameter, subscription may have already been destroyed."); 

    assert(0!=subscriptionPointer);

    subCommon_destroyEx(env,this,subscriptionPointerFieldId_g);
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setRecoverGap
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setRecoverGaps
    (JNIEnv* env, jobject this, jboolean recoverGaps)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

     MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer,
    "MamaSubscription.setRecoverGaps(): Null parameter, subcription may have been destroyed.");

    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_setRecoverGaps(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    (int)recoverGaps)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set recover gaps for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getRecoverGaps
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaSubscription_getRecoverGaps
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    int             recoverGaps         =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaSubscription.getRecoverGaps(): Null parameter, subcription may have been destroyed.",0);
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getRecoverGaps(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &recoverGaps)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set retries for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jboolean)recoverGaps;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getItemClosure
 * Signature: ()Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaSubscription_getItemClosure
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    void*           itemClosure         =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaSubscription.getItemClosure(): Null parameter, subcription may have been destroyed.", NULL);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getItemClosure(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    &itemClosure)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get item closure for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    
    return (jobject)itemClosure;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setItemClosure
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setItemClosure
    (JNIEnv* env, jobject this, jobject itemClosure)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    jobject         itemClosureGlobal   =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = 
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(subscriptionPointer, 
    "MamaSubscription.setItemClosure(): Null parameter, subcription may have been destroyed.");
    
    assert(0!=subscriptionPointer);

    /*This needs to be global otherwise it will go out of scope once this cal
     returns and will be garbage collected (or at least the jobject won't be
     valid!)*/
    itemClosureGlobal = (*env)->NewGlobalRef(env,itemClosure);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_setItemClosure(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    (void*)itemClosureGlobal)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not set item closure for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    createNativeSubscription
 * Signature: (Lcom/wombat/mama/MamaSubscriptionCallback;Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaSource;Ljava/lang/String;Ljava/lang/Object;)V
 */

JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_createNativeSubscription
    (JNIEnv*    env,
     jobject    this,
     jobject    callback,
     jobject    queue,
     jobject    source,
     jstring    symbol,
     jobject    closure)
{
    mama_status         status              =   MAMA_STATUS_OK;
    const char*         c_symbol            =   NULL;
    jlong               sourcePointer       =   0;
    jlong               subscriptionPointer =   0;
    mamaQueue           queue_c             =   NULL;
    jobject             messageImpl         =   NULL;
    jlong               queuePointer        =   0;
    mamaMsgCallbacks    c_callback;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    callbackClosure* closureImpl = (callbackClosure*)calloc(1,
            sizeof(callbackClosure));
    if (!closureImpl)
    {
        utils_throwMamaException(env,"createSubscription():"
                " Could not allocate.");
        return;
    }
    
    closureImpl->mUserData      =   NULL;
    closureImpl->mClientCB      =   NULL;
    closureImpl->mSubscription  =   NULL;
    closureImpl->mMessage       =   NULL;

    /*Setup the callback structure*/
    memset(&c_callback, 0, sizeof(c_callback));
    c_callback.onCreate         = (wombat_subscriptionCreateCB)subscriptionCreateCB;
    c_callback.onError          = (wombat_subscriptionErrorCB)subscriptionErrorCB;
    c_callback.onMsg            = (wombat_subscriptionOnMsgCB)subscriptionMsgCB;
    c_callback.onQuality        = (wombat_subscriptionQualityCB)subscriptionQualityCB;
    c_callback.onGap            = (wombat_subscriptionGapCB)subscriptionGapCB;
    c_callback.onRecapRequest   = (wombat_subscriptionRecapCB)subscriptionRecapCB;
    c_callback.onDestroy        = (wombat_subscriptionDestroyCB)subscriptionDestroyCB;
	
    if(symbol)
    {
        c_symbol = (*env)->GetStringUTFChars(env,symbol,0);
        if (!c_symbol)
        {
            free(closureImpl);
            return;/*Exception thrown*/
        }
    }

    sourcePointer = (*env)->GetLongField(env, source,
                                            sourcePointerFieldId_g);
    
    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);

    /*If the client supplied a Java closeure add it to ours*/
    if(closure)closureImpl->mUserData = (*env)->NewGlobalRef(env,closure);

    /*Check if a queue was specified.*/
    if(queue)
    {  
        /*Get the queue pointer value from the MamaQueue java object*/
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /*Create a reuseable message object to hang off the subscription*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl)
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return;
    }/*Exception will have been thrown*/
    /*This global will be deleted when the subscription is destroyed*/
    closureImpl->mMessage  = (*env)->NewGlobalRef(env,messageImpl);

    /*Get the C pointer for the already allocated subscription*/
    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    /*Add the Java Subscription to the closure - we need it in the 
     async callbacks so it can be passed to the Java callback*/
    closureImpl->mSubscription = (*env)->NewGlobalRef(env,this);
        
    if(MAMA_STATUS_OK != (status = mamaSubscription_create(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer), 
                    queue_c,
                    &c_callback,
                    CAST_JLONG_TO_POINTER(mamaSource,sourcePointer),
                    c_symbol,
                    closureImpl)))    
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        (*env)->DeleteGlobalRef(env,this);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create subscription.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Tidy up resources*/
    if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setupNativeSubscription
 * Signature: (Lcom/wombat/mama/MamaSubscriptionCallback;Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaSource;Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setupNativeSubscription
    (JNIEnv*    env,
     jobject    this,
     jobject    callback,
     jobject    queue,
     jobject    source,
     jstring    symbol,
     jobject    closure)
{
    mama_status         status              =   MAMA_STATUS_OK;
    const char*         c_symbol            =   NULL;
    jlong               sourcePointer       =   0;
    jlong               subscriptionPointer =   0;
    mamaQueue           queue_c             =   NULL;
    jobject             messageImpl         =   NULL;
    jlong               queuePointer        =   0;
    mamaMsgCallbacks    c_callback;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    callbackClosure* closureImpl = (callbackClosure*)calloc(1,
            sizeof(callbackClosure));
    if (!closureImpl)
    {
        utils_throwMamaException(env,"createSubscription():"
                " Could not allocate.");
        return;
    }
    
    closureImpl->mUserData      =   NULL;
    closureImpl->mClientCB      =   NULL;
    closureImpl->mSubscription  =   NULL;
    closureImpl->mMessage       =   NULL;

    /*Setup the callback structure*/
    memset(&c_callback, 0, sizeof(c_callback));
    c_callback.onCreate         = (wombat_subscriptionCreateCB)subscriptionCreateCB;
    c_callback.onError          = (wombat_subscriptionErrorCB)subscriptionErrorCB;
    c_callback.onMsg            = (wombat_subscriptionOnMsgCB)subscriptionMsgCB;
    c_callback.onQuality        = (wombat_subscriptionQualityCB)subscriptionQualityCB;
    c_callback.onGap            = (wombat_subscriptionGapCB)subscriptionGapCB;
    c_callback.onRecapRequest   = (wombat_subscriptionRecapCB)subscriptionRecapCB;
    c_callback.onDestroy        = (wombat_subscriptionDestroyCB)subscriptionDestroyCB;
	
    if(symbol)
    {
        c_symbol = (*env)->GetStringUTFChars(env,symbol,0);
        if (!c_symbol)
        {
            free(closureImpl);
            return;/*Exception thrown*/
        }
    }

    sourcePointer = (*env)->GetLongField(env, source,
                                            sourcePointerFieldId_g);
    
    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);

    /*If the client supplied a Java closeure add it to ours*/
    if(closure)closureImpl->mUserData = (*env)->NewGlobalRef(env,closure);

    /*Check if a queue was specified.*/
    if(queue)
    {  
        /*Get the queue pointer value from the MamaQueue java object*/
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /*Create a reuseable message object to hang off the subscription*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl)
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return;
    }/*Exception will have been thrown*/
    /*This global will be deleted when the subscription is destroyed*/
    closureImpl->mMessage  = (*env)->NewGlobalRef(env,messageImpl);

    /*Get the C pointer for the already allocated subscription*/
    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    /*Add the Java Subscription to the closure - we need it in the 
     async callbacks so it can be passed to the Java callback*/
    closureImpl->mSubscription = (*env)->NewGlobalRef(env,this);
        
    if(MAMA_STATUS_OK != (status = mamaSubscription_setup(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer), 
                    queue_c,
                    &c_callback,
                    CAST_JLONG_TO_POINTER(mamaSource,sourcePointer),
                    c_symbol,
                    closureImpl)))    
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        (*env)->DeleteGlobalRef(env,this);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create subscription.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Tidy up resources*/
    if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    createNativeDictionarySubscription
 * Signature: (Lcom/wombat/mama/MamaSubscriptionCallback;Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaSource;DI)Lcom/wombat/mama/MamaDictionary;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaSubscription_createNativeDictionarySubscription
  (JNIEnv* env,         jobject this,
   jobject callback,    jobject queue,
   jobject source,      jdouble timeout,     
   jint    retries)
{
    mama_status                 status              =   MAMA_STATUS_OK;
    jobject                     dictionary          =   NULL;
    mamaDictionary              c_dictionary        =   NULL;
    jclass                      dictionaryImplClass =   NULL;
    mamaQueue                   queue_c             =   NULL;
    jlong                       queuePointer        =   0;
    jlong                       sourcePointer       =   0;
    mamaDictionaryCallbackSet   dictionaryCallback;
    char                        errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    /*Used internally to gain access to various objects in async callbacks*/
    dictionaryClosure* closureImpl = (dictionaryClosure*)calloc(1,
            sizeof(dictionaryClosure));

    if(!closureImpl)
    {
        utils_throwMamaException(env,"createDictionarySubscription():"
            " Could not allocate.");
        return NULL;
    }

    dictionaryCallback.onComplete = (mamaDictionary_completeCallback)dictCompleteCB;
    dictionaryCallback.onTimeout  = (mamaDictionary_timeoutCallback)dictTimeoutCB;
    dictionaryCallback.onError    = (mamaDictionary_errorCallback)dictErrorCB;

    sourcePointer = (*env)->GetLongField(env, source,
            sourcePointerFieldId_g);

    /*Check if a queue was specified.*/
    if (queue)
    {  
        /*Get the queue pointer value from the MamaQueue java object*/
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);

    if (MAMA_STATUS_OK != (status = mama_createDictionary (&c_dictionary,
                    queue_c,
                    dictionaryCallback,
                    CAST_JLONG_TO_POINTER(mamaSource,sourcePointer),
                    (double)timeout,
                    (int)retries,
                    closureImpl)))
    {
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "createDictionary() Failed.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }
    /*Create a Java dictionary and set the pointer val*/
    dictionaryImplClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaDictionary");

    if(!dictionaryImplClass)
    {
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return NULL;/*Exception auto thrown on stack*/
    }

    dictionary = (*env)->NewObject(env, dictionaryImplClass,
            dictionaryConstructorId_g);

    if(!dictionary)
    {
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return NULL;
    }

    (*env)->SetLongField(env, dictionary, dictionaryPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(c_dictionary));

    /*Store the closure pointer so we can delete it when the dictionary is
     destroyed.*/
    (*env)->SetLongField(env, dictionary, dictionaryClosurePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(closureImpl));

    return dictionary;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_initIDs
  (JNIEnv* env, jclass class)
{
    jclass   subscriptionCallbackClass  =   NULL;
    jclass   dictionaryCallbackClass    =   NULL;
    
    subscriptionPointerFieldId_g = (*env)->GetFieldID(env,
                                 class, "subscriptionPointer_i",
                                 UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!subscriptionPointerFieldId_g) return;/*Exception auto thrown*/

    /*Get a reference to the subscription Callback class*/
    subscriptionCallbackClass = (*env)->FindClass(env, 
            "com/wombat/mama/MamaSubscriptionCallback");
    if (!subscriptionCallbackClass) return;/*Exception auto thrown*/

    /*MamaSubscriptionCallback.onCreate()*/
    subCallbackonCreateId_g = (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onCreate", "(Lcom/wombat/mama/MamaSubscription;)V" );
    if (!subCallbackonCreateId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }
    
    /*MamaSubscriptionCallback.onMsg()*/
    subCallbackonMsgId_g = (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onMsg", "(Lcom/wombat/mama/MamaSubscription;"
            "Lcom/wombat/mama/MamaMsg;)V" );
    if (!subCallbackonMsgId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

    /*MamaSubscriptionCallback.onError()*/
    subCallbackonErrorId_g = (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onError", "(Lcom/wombat/mama/MamaSubscription;"
            "SILjava/lang/String;Ljava/lang/Exception;)V" );
    if(!subCallbackonErrorId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

    /*MamaSubscriptionCallback.onQuality*/
    subCallbackonQualityId_g =  (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onQuality", 
            "(Lcom/wombat/mama/MamaSubscription;SSLjava/lang/Object;)V" );
    if(!subCallbackonQualityId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }

    /*MamaSubscriptionCallback.onQuality*/
    subCallbackonGapId_g =  (*env)->GetMethodID(env, subscriptionCallbackClass,
           "onGap", 
           "(Lcom/wombat/mama/MamaSubscription;)V" );
    if(!subCallbackonGapId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }
    
    /*MamaSubscriptionCallback.onRecap*/
    subCallbackonRecapId_g =  (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onRecapRequest", 
            "(Lcom/wombat/mama/MamaSubscription;)V" );
    if(!subCallbackonRecapId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }
    
    /* Get the destroy callback. */
    subCallbackonDestroyId_g =  (*env)->GetMethodID(env, subscriptionCallbackClass,
            "onDestroy", 
            "(Lcom/wombat/mama/MamaSubscription;)V" );
    if(!subCallbackonDestroyId_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        return;/*Exception auto thrown*/
    }
    
    /*********************************************************
     * Dictionary fields/methods
     *********************************************************/

    /*Get a reference to the dictionary Callback class*/
    dictionaryCallbackClass = (*env)->FindClass(env, "com/wombat/mama/MamaDictionaryCallback");
    if (!dictionaryCallbackClass) return;/*Exception auto thrown*/

    /*MamaDictionaryCallback.onQuality()*/
    dictCallbackonTimeout_g =  (*env)->GetMethodID(env, dictionaryCallbackClass,
            "onTimeout", "()V" );
    if(!dictCallbackonTimeout_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        (*env)->DeleteLocalRef(env, dictionaryCallbackClass);
        return;
    }

    /*MamaDictionaryCallback.onComplete()*/
    dictCallbackonComplete_g =  (*env)->GetMethodID(env, dictionaryCallbackClass,
            "onComplete", "()V" );
    if(!dictCallbackonComplete_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        (*env)->DeleteLocalRef(env, dictionaryCallbackClass);
        return;
    }

    /*MamaDictionaryCallback.onError()*/
    dictCallbackonError_g =  (*env)->GetMethodID(env, dictionaryCallbackClass,
            "onError", "(Ljava/lang/String;)V" );
    if(!dictCallbackonError_g)
    {
        (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
        (*env)->DeleteLocalRef(env, dictionaryCallbackClass);
        return;
    }

    /*Tidy up local refs*/
    (*env)->DeleteLocalRef(env, dictionaryCallbackClass);
    (*env)->DeleteLocalRef(env, subscriptionCallbackClass);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    allocateSubscription
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaSubscription_allocateSubscription(JNIEnv* env, jobject this)
{
    mamaSubscription subscription       = NULL;
    mama_status      status             = MAMA_STATUS_OK;
    char             errorString[UTILS_MAX_ERROR_STRING_LENGTH];
   
    if(MAMA_STATUS_OK!=(status = mamaSubscription_allocate(&subscription)))
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
    (*env)->SetLongField(env, this, subscriptionPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(subscription));
    
    return;
}

void MAMACALLTYPE subscriptionMsgCB (
        mamaSubscription subscription,
        mamaMsg          msg,
        void*            closure,
        void*            itemClosure)
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

void MAMACALLTYPE subscriptionCreateCB (mamaSubscription subscription,
        void *closure )
{
    subCommon_createCb (subscription,
                        closure,
                        subscriptionPointerFieldId_g,
                        subCallbackonCreateId_g);
    return;
}

void MAMACALLTYPE subscriptionGapCB (mamaSubscription subscription,
                   void *closure )
{
    subCommon_gapCb (subscription,
                     closure,
                     subscriptionPointerFieldId_g,
                     subCallbackonGapId_g);
    return;
}

void MAMACALLTYPE subscriptionDestroyCB (mamaSubscription subscription,
                   void *closure )
{
    subCommon_destroyCb (subscription,
                     closure,
                     subscriptionPointerFieldId_g,
                     subCallbackonDestroyId_g);
    return;
}

void MAMACALLTYPE subscriptionRecapCB (mamaSubscription subscription,
                     void *closure )
{
    subCommon_recapCb (subscription,
                       closure,
                       subscriptionPointerFieldId_g,
                       subCallbackonRecapId_g);
    return;
}

void MAMACALLTYPE subscriptionQualityCB (mamaSubscription    subscription,
                       mamaQuality         quality,
                       const char*         symbol,
                       short               cause,
                       const void*         platformInfo,
                       void*               closure)
{
    subCommon_qualityCb (subscription,
                         quality,
                         symbol,
                         cause,
                         platformInfo,
                         closure,
                         subCallbackonQualityId_g);
    return;
}

void MAMACALLTYPE dictTimeoutCB( mamaDictionary dict, void *closure )
{
    JNIEnv*             env             =   NULL;
    dictionaryClosure*  closureImpl     =   NULL;

    closureImpl = (dictionaryClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't invoke callback without JNIEnv*/

    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dictCallbackonTimeout_g );
    return;
}

void MAMACALLTYPE dictErrorCB( mamaDictionary dict, const char *errMsg, void *closure )
{
    JNIEnv*             env             =   NULL;
    dictionaryClosure*  closureImpl     =   NULL;

    closureImpl = (dictionaryClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't invoke callback without JNIEnv*/

    /*Invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dictCallbackonError_g,
                           (*env)->NewStringUTF(env,errMsg));

    return;
}

void MAMACALLTYPE dictCompleteCB( mamaDictionary dict, void *closure )
{
    JNIEnv*             env             =   NULL;
    dictionaryClosure*  closureImpl     =   NULL;

    closureImpl = (dictionaryClosure*)closure;
    assert(closureImpl!=NULL);
    assert(closureImpl->mClientCB!=NULL);

    /*Get the env for the current thread*/
    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/

    /*invoke the callback*/
    (*env)->CallVoidMethod(env, closureImpl->mClientCB,
                           dictCallbackonComplete_g );
    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    createSubscription
 * Signature: (Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaSubscriptionCallback;Lcom/wombat/mama/MamaSource;Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT 
void JNICALL Java_com_wombat_mama_MamaSubscription_createSubscription__Lcom_wombat_mama_MamaQueue_2Lcom_wombat_m\
ama_MamaSubscriptionCallback_2Lcom_wombat_mama_MamaSource_2Ljava_lang_String_2Ljava_lang_Object_2
     (JNIEnv * env, 
      jobject this, 
      jobject queue, 
      jobject callback, 
      jobject source,
      jstring symbol, 
      jobject closure)
{
    mama_status         status              =   MAMA_STATUS_OK;
    const char*         c_symbol            =   NULL;
    jlong               subscriptionPointer =   0;
    mamaQueue           queue_c             =   NULL;
    jobject             messageImpl         =   NULL;
    jlong               queuePointer        =   0;
    jlong               sourcePointer       =   0;
    mamaMsgCallbacks    c_callback;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    callbackClosure* closureImpl = (callbackClosure*)calloc(1,
            sizeof(callbackClosure));
    if (!closureImpl)
    {
        utils_throwMamaException(env,"createSubscription():"
                " Could not allocate.");
        return;
    }
    
    closureImpl->mUserData      =   NULL;
    closureImpl->mClientCB      =   NULL;
    closureImpl->mSubscription  =   NULL;
    closureImpl->mMessage       =   NULL;

    /*Setup the callback structure*/
    memset(&c_callback, 0, sizeof(c_callback));
    c_callback.onCreate         = 
        (wombat_subscriptionCreateCB)subscriptionCreateCB;
    c_callback.onError          = 
        (wombat_subscriptionErrorCB)subscriptionErrorCB;
    c_callback.onMsg            = 
        (wombat_subscriptionOnMsgCB)subscriptionMsgCB;
    c_callback.onQuality        = 
        (wombat_subscriptionQualityCB)subscriptionQualityCB;
    c_callback.onGap            = 
        (wombat_subscriptionGapCB)subscriptionGapCB;
    c_callback.onRecapRequest   = 
        (wombat_subscriptionRecapCB)subscriptionRecapCB;

    if(symbol)
    {
        c_symbol = (*env)->GetStringUTFChars(env,symbol,0);
        if (!c_symbol)
        {
            free(closureImpl);
            return;/*Exception thrown*/
        }
    }
    sourcePointer = (*env)->GetLongField(env,source,
                                         sourcePointerFieldId_g);
    assert (0 != sourcePointer);

    closureImpl->mClientCB = (*env)->NewGlobalRef(env, callback);

    /*If the client supplied a Java closeure add it to ours*/
    if(closure)closureImpl->mUserData = (*env)->NewGlobalRef(env,closure);

    /*Check if a queue was specified.*/
    if(queue)
    {  
        /*Get the queue pointer value from the MamaQueue java object*/
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /*Create a reuseable message object to hang off the subscription*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl)
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        free(closureImpl);
        return;
    }/*Exception will have been thrown*/
    /*This global will be deleted when the subscription is destroyed*/
    closureImpl->mMessage  = (*env)->NewGlobalRef(env,messageImpl);

    /*Get the C pointer for the already allocated subscription*/
    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    /*Add the Java Subscription to the closure - we need it in the 
     async callbacks so it can be passed to the Java callback*/
    closureImpl->mSubscription = (*env)->NewGlobalRef(env,this);
        
    if(MAMA_STATUS_OK != (status = mamaSubscription_create(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    queue_c,
                    &c_callback,
                    CAST_JLONG_TO_POINTER(mamaSource,sourcePointer),
                    c_symbol,
                    closureImpl)))
       
    {
        if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
        if(closure)(*env)->DeleteGlobalRef(env,closureImpl->mUserData);
        (*env)->DeleteGlobalRef(env,closureImpl->mClientCB);
        (*env)->DeleteGlobalRef(env,this);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create subscription.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Tidy up resources*/
    if(c_symbol)(*env)->ReleaseStringUTFChars(env,symbol, c_symbol);
    return;
}
/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    _setSubscriptionType
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription__1setSubscriptionType
(JNIEnv * env, jobject this, jshort type)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_setSubscriptionType(
                             CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                             (mamaSubscriptionType)type)))
    {
        utils_buildErrorStringForStatus (
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not set subscription type for mamaSubscription.",
            status);
        utils_throwWombatException (env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    setPreInitialCacheSize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_setPreInitialCacheSize
(JNIEnv * env, jobject this, jint value)
{
    mama_status status                  = MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
   
    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    assert(0!=subscriptionPointer);

    
    if (MAMA_STATUS_OK!=(status=mamaSubscription_setPreIntitialCacheSize(
                             CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                             (int)value)))
    {
        utils_buildErrorStringForStatus (
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not set preinitial cache size for mamaSubscription.",
            status);
        utils_throwWombatException (env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getPreInitialCacheSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSubscription_getPreInitialCacheSize
(JNIEnv * env, jobject this)
{
    mama_status status                  = MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    int cacheSize                       =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer =
        (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);
    assert(0!=subscriptionPointer);

    
    if (MAMA_STATUS_OK!=(status=mamaSubscription_getPreIntitialCacheSize(
                             CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                             &cacheSize)))
    {
        utils_buildErrorStringForStatus (
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not get pre initial cache size for mamaSubscription.",
            status);
        utils_throwWombatException (env,errorString);
    }

    return (jint)cacheSize;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    getSubscriptionState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaSubscription_getSubscriptionState
    (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           subscriptionPointer =   0;
    jint            state               =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    subscriptionPointer = (*env)->GetLongField(env,this,subscriptionPointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(subscriptionPointer, 
    "MamaSubscription.getSubscriptionState(): Null parameter, subcription may have been destroyed.", NULL);
    
    assert(0!=subscriptionPointer);

    if (MAMA_STATUS_OK!=(status=mamaSubscription_getState(
                    CAST_JLONG_TO_POINTER(mamaSubscription,subscriptionPointer),
                    (mamaSubscriptionState *)&state)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get state for mamaSubscription.",
                status);
        utils_throwWombatException(env,errorString);
    }
    
    return state;
}

/*
 * Class:     com_wombat_mama_MamaSubscription
 * Method:    deallocate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSubscription_deallocate(JNIEnv *env, jobject this)
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
                "MamaSubscription.deallocate(): Could not deallocate mamaSubscriptoin.",
                msd);

            /* Throw the exception. */
            utils_throwMamaException(env, errorString); 
        }
    }
}
