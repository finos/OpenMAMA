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
* MamaDateTime.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaDateTime.h"

/******************************************************************************
* Global/Static variables
*******************************************************************************/
#define MAX_DATE_TIME_STR_LEN (10 + 1 + 15 + 1)
jfieldID dateTimePointerFieldId_g   =   NULL;
jfieldID tzFieldObjectFieldId_g     =   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/
void timeZone_set   (JNIEnv* env, jlong pTimeZone, jstring timeZoneValue);
jlong 	createTimeZone (JNIEnv* env, jobject this);

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    isEmpty
 * Signature: ()B
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaDateTime_isEmpty
  (JNIEnv* env, jobject this)
{   
    jlong pDateTime   = 0;
    
    pDateTime = (*env)->GetLongField (env,this, dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0);
    
    return (jboolean) mamaDateTime_empty(
                        CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime));
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    setEpochTimeF64
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_setEpochTimeF64
  (JNIEnv* env, jobject this, jdouble secondsSinceEpoch)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,
		"Null parameter, MamaDateTime may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setEpochTimeF64(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_f64_t) secondsSinceEpoch)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setEpochTimeF64().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    setEpochTimeMilliseconds
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_setEpochTimeMilliseconds
  (JNIEnv* env, jobject this, jlong millisecondsSinceEpoch)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaDateTime_setEpochTimeMilliseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_u64_t) millisecondsSinceEpoch)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setEpochTimeMilliseconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    setEpochTimeMicroSeconds
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_setEpochTimeMicroseconds
  (JNIEnv* env, jobject this, jlong microsecondsSinceEpoch)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setEpochTimeMicroseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_u64_t) microsecondsSinceEpoch)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setEpochTimeMicroseconds().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    setToNow
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_setToNow
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setToNow(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setToNow().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    setDate
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_setDate
  (JNIEnv* env, jobject this, jint year, jint month, jint day)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setDate(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_u32_t) year, 
                            (mama_u32_t) month, 
                            (mama_u32_t) day)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setDate().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    copyTime
 * Signature: (Lcom/wombat/mama/MamaDateTime;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_copyTime
  (JNIEnv* env, jobject this, jobject other)
{
    jlong       pDateTime       = 0;
    jlong       pDateTimeToCopy = 0;
    mama_status status          = MAMA_STATUS_OK;
    char        errorString     [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    pDateTimeToCopy = (*env)->GetLongField(env,other,dateTimePointerFieldId_g);   
    
    if((pDateTimeToCopy == 0) ||
        MAMA_STATUS_OK!=(status=mamaDateTime_copyTime(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTimeToCopy))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.copyTime().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    copyDate
 * Signature: (Lcom/wombat/mama/MamaDateTime)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_copyDate
  (JNIEnv* env, jobject this, jobject other)
{
    jlong       pDateTime       = 0;
    jlong       pDateTimeToCopy = 0;
    mama_status status          = MAMA_STATUS_OK;
    char        errorString     [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    pDateTimeToCopy = (*env)->GetLongField(env,other,dateTimePointerFieldId_g);   
    
    if((pDateTimeToCopy == 0) || 
        MAMA_STATUS_OK!=(status=mamaDateTime_copyDate(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTimeToCopy))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.copyTime().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    clear
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_clear
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_clear(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.clear().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }    
}
 
/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    clearTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_clearTime
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_clearTime(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.clearTime().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }    
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    clearDate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_clearDate
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_clearDate(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.clearDate().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }    
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    addSeconds
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_addSeconds__D
  (JNIEnv* env, jobject this, jdouble seconds)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_addSeconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_f64_t) seconds)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.addSeconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }    
} 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    addSeconds
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_addSeconds__J
  (JNIEnv* env, jobject this, jlong seconds)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_addWholeSeconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_i32_t) seconds)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.addSeconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }     
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    addMicroSeconds
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_addMicroSeconds
  (JNIEnv* env, jobject this, jlong microSeconds)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_addMicroseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_i32_t) microSeconds)))
    {
         utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.addMicroSeconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }     
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getEpochTimeMicroseconds
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getEpochTimeMicroseconds
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u64_t  epochTime   = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getEpochTimeMicroseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), &epochTime)))
    {
         utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getEpochTimeMicroseconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jlong) epochTime;
 }

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getEpochTimeMilliseconds
 * Signature: (J)V
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getEpochTimeMilliseconds
  (JNIEnv* env, jobject this)
 {
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u64_t  epochTime   = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getEpochTimeMilliseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), &epochTime)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getEpochTimeMilliseconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
     }
    return (jlong) epochTime;
 } 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getEpochTimeSeconds
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaDateTime_getEpochTimeSeconds
  (JNIEnv* env, jobject this)
 {
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_f64_t  epochTime   = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getEpochTimeSeconds (
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &epochTime)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getEpochTimeSeconds().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jdouble) epochTime;
 } 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getEpochTimeSecondsWithCheck
 * Signature: (D)V
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaDateTime_getEpochTimeSecondsWithCheck
  (JNIEnv* env, jobject this)
 {
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_f64_t  epochTime   = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getEpochTimeSecondsWithCheck (
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &epochTime)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getEpochTimeSecondsWithCheck().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jdouble) epochTime;
 } 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    hasDate
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaDateTime_hasDate 
    (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_bool_t result      = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    pDateTime = (*env)->GetLongField(env, this, dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,
        "Null parameter, MamaDateTime may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaDateTime_hasDate (
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            &result)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.hasDate().",
                status);
        utils_throwExceptionForMamaStatus (env, status, errorString);
        return 0;
    }

    return (jboolean) result;
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    hasTime
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaDateTime_hasTime
    (JNIEnv* env, jobject this)
{ 
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_bool_t result      = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
  
    pDateTime = (*env)->GetLongField(env, this, dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,
        "Null parameter, MamaDateTime may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaDateTime_hasTime (
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            &result)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.hasTime().",
                status);
        utils_throwExceptionForMamaStatus (env, status, errorString);
        return 0;
    }

    return (jboolean) result;
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getAsString
 * Signature: ()Ljava/lang/String
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaDateTime_getAsString
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        ret_c       [MAX_DATE_TIME_STR_LEN+1];
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", NULL) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getAsString(
                            CAST_JLONG_TO_POINTER(mamaDateTime,pDateTime), 
                            ret_c, 
                            MAX_DATE_TIME_STR_LEN)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getAsString().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }
    return (*env)->NewStringUTF (env, ret_c);
}  

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getTimeAsString
 * Signature: ()Ljava/lang/String
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaDateTime_getTimeAsString
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        ret_c       [MAX_DATE_TIME_STR_LEN+1];
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", NULL) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getTimeAsString(
                            CAST_JLONG_TO_POINTER(mamaDateTime,pDateTime), 
                            ret_c, 
                            MAX_DATE_TIME_STR_LEN)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getTimeAsString().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }
    return (*env)->NewStringUTF (env, ret_c);
} 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getDateAsString
 * Signature: ()Ljava/lang/String
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaDateTime_getDateAsString
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        ret_c       [MAX_DATE_TIME_STR_LEN+1];
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", NULL) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getDateAsString(
                            CAST_JLONG_TO_POINTER(mamaDateTime,pDateTime), 
                            ret_c, 
                            MAX_DATE_TIME_STR_LEN)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getDateAsString().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return NULL;
    }
    return (*env)->NewStringUTF(env, ret_c);
} 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getYear
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getYear
  (JNIEnv* env, jobject this)
 {
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  year        = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getYear(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &year)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getYear().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jlong) year;
 } 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    clearMonth
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getMonth
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  month       = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getMonth(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &month)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getMonth().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jlong) month;
 } 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    clearTime
 * Signature: ()J
 */
 JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getDay
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  day         = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getDay(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &day)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getDay().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jlong) day;
}   

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getHour
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getHour
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  hour        = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getHour(
                            CAST_JLONG_TO_POINTER (mamaDateTime, pDateTime), 
                            &hour)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getHour().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jlong) hour;
}   

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getMinute
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getMinute
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  minute      = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getMinute(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &minute)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getMinute().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jlong) minute;
}    

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getSecond
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getSecond
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  second      = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getSecond(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &second)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getSecond().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jlong) second;
} 
 
/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    getMicroSecond
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_getMicrosecond
   (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u32_t  microSecond = 0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_getMicrosecond(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            &microSecond)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getMicroSecond().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return 0;
    }       
    return (jlong) microSecond;
} 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    diffSeconds
 * Signature: (Lcom/wombat/mama/MamaDateTime;Lcom/wombat/mama/MamaDateTime;)D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaDateTime_diffSeconds
  (JNIEnv* env, jclass this, jobject time1, jobject time2)
{
    jlong       pTime1      = 0;
    jlong       pTime2      = 0; 
    mama_f64_t  result      = 0.0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pTime1 = (*env)->GetLongField(env,time1,dateTimePointerFieldId_g);
    pTime2 = (*env)->GetLongField(env,time2,dateTimePointerFieldId_g);
    
    if((0 == pTime1) ||
       (0 == pTime2) || 
       MAMA_STATUS_OK!=(status=mamaDateTime_diffSeconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime1), 
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime2), 
                            &result)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.diffSeconds().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jdouble) result;
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    diffSecondsSameDay
 * Signature: (Lcom/wombat/mama/MamaDateTime;Lcom/wombat/mama/MamaDateTime;)D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaDateTime_diffSecondsSameDay
  (JNIEnv* env, jclass this, jobject time1, jobject time2)
{
    jlong       pTime1      = 0;
    jlong       pTime2      = 0; 
    mama_f64_t  result      = 0.0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pTime1 = (*env)->GetLongField (env,time1,dateTimePointerFieldId_g);
    pTime2 = (*env)->GetLongField (env,time2,dateTimePointerFieldId_g);
    
    if((0 == pTime1) ||
       (0 == pTime2) || 
       MAMA_STATUS_OK!=(status=mamaDateTime_diffSecondsSameDay(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime1), 
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime2), 
                            &result)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.diffSecondsSameDay().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jdouble) result;
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    diffMicroSeconds
 * Signature: (Lcom/wombat/mama/MamaDateTime;Lcom/wombat/mama/MamaDateTime;)J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaDateTime_diffMicroseconds
  (JNIEnv* env, jclass this, jobject time1, jobject time2)
{
    jlong       pTime1      = 0;
    jlong       pTime2      = 0; 
    mama_i64_t  result      = 0.0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pTime1 = (*env)->GetLongField (env,time1,dateTimePointerFieldId_g);
    pTime2 = (*env)->GetLongField (env,time2,dateTimePointerFieldId_g);
    
    if((0 == pTime1) ||
       (0 == pTime2) || 
       MAMA_STATUS_OK!=(status=mamaDateTime_diffMicroseconds(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime1), 
                            CAST_JLONG_TO_POINTER (mamaDateTime,pTime2), 
                            &result)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.diffMicroSeconds().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jdouble) result;   

} 

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_destroy 
  (JNIEnv* env, jobject this)
{
    jlong       pDateTime   = 0;
    jlong       pTimeZone   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") 
    
    pTimeZone = (*env)->GetLongField (env,this,tzFieldObjectFieldId_g);
    if (0 != pTimeZone)
    {
        if(MAMA_STATUS_OK!=(status=mamaTimeZone_destroy(
                            CAST_JLONG_TO_POINTER (mamaTimeZone, pTimeZone))))
        {
                utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "Error calling MamaDateTime.destroy().",
                    status);
            utils_throwExceptionForMamaStatus (env,status,errorString);
            return;
        }              
        (*env)->SetLongField(env, this,
                         tzFieldObjectFieldId_g,
                         0);
    }
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_destroy(
                            CAST_JLONG_TO_POINTER (mamaDateTime, pDateTime))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.destroy().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
        return;
    }
    (*env)->SetLongField(env, this,
                 dateTimePointerFieldId_g,
                 0);
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    copy
 * Signature: (Lcom/wombat/mama/MamaDateTime;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_copy
  (JNIEnv* env, jobject this, jobject other)
{
    jlong       pDateTime       = 0;
    jlong       pDateTimeToCopy = 0;
    mama_status status          = MAMA_STATUS_OK;
    char        errorString     [UTILS_MAX_ERROR_STRING_LENGTH];    
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    pDateTimeToCopy = (*env)->GetLongField(env,other,dateTimePointerFieldId_g);   

    if((pDateTimeToCopy == 0) || 
        MAMA_STATUS_OK!=(status=mamaDateTime_copy(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTimeToCopy))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.copy().",
                status);
        utils_throwExceptionForMamaStatus (env,status,errorString);
    }
}   

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _setEpochTime
 * Signature: (JJS)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1setEpochTime
  (JNIEnv* env, jobject this, jlong seconds, jlong microseconds, jshort precision)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setEpochTime(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mama_u32_t) seconds, 
                            (mama_u32_t) microseconds, 
                            (mamaDateTimePrecision) precision)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setEpochTime().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _setPrecision
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1setPrecision
  (JNIEnv* env, jobject this, jshort precision)
{
    jlong       pDateTime   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if(MAMA_STATUS_OK!=(status=mamaDateTime_setPrecision(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            (mamaDateTimePrecision) precision)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setPrecision().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _setFromString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1setFromString
  (JNIEnv* env, jobject this, jstring str, jstring timeZone)
{
    jlong       pDateTime   = 0;
    jlong       pTimeZone   = 0;
    const char* c_Str       = NULL;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if (NULL != timeZone)
    {    
        pTimeZone = (*env)->GetLongField (env,this,tzFieldObjectFieldId_g);
        if (0 == pTimeZone)
        {
            pTimeZone = createTimeZone (env, this);
            assert (0!=pTimeZone);
        }
        timeZone_set (env, pTimeZone, timeZone);
    }
        
    c_Str = (*env)->GetStringUTFChars(env,str,0);
  
    if (!c_Str ||
        MAMA_STATUS_OK!=(status=mamaDateTime_setFromStringWithTz(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime), 
                            c_Str,
                            timeZone 
                            ? CAST_JLONG_TO_POINTER (mamaTimeZone, pTimeZone) 
                            : NULL)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setFromString().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _setToMidnightToday
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1setToMidnightToday
  (JNIEnv* env, jobject this, jstring timeZone)
{
    jlong       pDateTime   = 0;
    jlong       pTimeZone   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if (NULL != timeZone)
    {    
        pTimeZone = (*env)->GetLongField (env,this,tzFieldObjectFieldId_g);
        if (0 == pTimeZone)
        {
            pTimeZone = createTimeZone (env, this);
            assert (0!=pTimeZone);
        }
        timeZone_set (env, pTimeZone, timeZone);
    }
          
    if (MAMA_STATUS_OK!=(status=mamaDateTime_setToMidnightToday(
                            CAST_JLONG_TO_POINTER (mamaDateTime, pDateTime),
                            timeZone 
                            ? CAST_JLONG_TO_POINTER (mamaTimeZone, pTimeZone)
                            : NULL)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setToMidnightToday().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}  

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _setTime
 * Signature: (IIIJSLjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1setTime (JNIEnv* env, jobject this, jint hour, jint minute, 
                                                    jint second, jlong microsecond, jshort precision, jstring timeZone)
{
    jlong       pDateTime   = 0;
    jlong       pTimeZone   = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.") ;
    
    if (NULL != timeZone)
    {    
        pTimeZone = (*env)->GetLongField (env,this,tzFieldObjectFieldId_g);
        if (0 == pTimeZone)
        {
            pTimeZone = createTimeZone (env, this);
            assert (0!=pTimeZone);
        }
        timeZone_set (env, pTimeZone, timeZone);
    }
          
    if (MAMA_STATUS_OK!=(status=mamaDateTime_setTimeWithPrecisionAndTz(
                            CAST_JLONG_TO_POINTER (mamaDateTime,pDateTime),
                            (mama_u32_t) hour,
                            (mama_u32_t) minute,
                            (mama_u32_t) second,
                            (mama_u32_t) microsecond,
                            (mamaDateTimePrecision) precision,                                                    
                            timeZone 
                               ? CAST_JLONG_TO_POINTER (mamaTimeZone,pTimeZone)
                               : NULL)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.setToMidnightToday().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}  

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _getDayOfWeek
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaDateTime__1getDayOfWeek
  (JNIEnv* env, jobject this)       
{
    jlong           pDateTime   = 0;
    mama_status     status      = MAMA_STATUS_OK;
    mamaDayOfWeek   day         = 0;
    char            errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pDateTime = (*env)->GetLongField (env,this,dateTimePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pDateTime,  
		"Null parameter, MamaDateTime may have already been destroyed.", 0) ;
    
    if (MAMA_STATUS_OK!=(status=mamaDateTime_getDayOfWeek(
                            CAST_JLONG_TO_POINTER (mamaDateTime, pDateTime), 
                            &day)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaDateTime.getHour().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return 0;
    }       
    return (jshort) day;
}   
  
     
/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    _create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime__1create
  (JNIEnv* env, jobject this)
{
    mamaDateTime    cDateTime  = NULL;
    mamaTimeZone    cTimeZone  = NULL;
    mama_status     status     = MAMA_STATUS_OK;
    char errorString           [UTILS_MAX_ERROR_STRING_LENGTH];
    
    if (MAMA_STATUS_OK!=(status=mamaDateTime_create(&cDateTime)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create mamaDateTime().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return ;
    }
  
    /*If we get this far we have a valid dateTime pointer to set*/
    (*env)->SetLongField(env, this,
                         dateTimePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cDateTime));                 
}


void timeZone_set (JNIEnv* env, jlong pTimeZone, jstring timeZoneValue)
{
    mama_status status      = MAMA_STATUS_OK;
    const char* c_value     = NULL;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    c_value = (*env)->GetStringUTFChars(env,timeZoneValue,0);    
    if (!c_value ||
       MAMA_STATUS_OK!=(status=mamaTimeZone_set(
                            CAST_JLONG_TO_POINTER (mamaTimeZone,pTimeZone), 
                            c_value)))
    {
        if (c_value) (*env)->ReleaseStringUTFChars(env,timeZoneValue,c_value);           
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaTimeZone.set().",
                status);
                
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }   
    
    /*Tidy up*/
    if(c_value)(*env)->ReleaseStringUTFChars(env,timeZoneValue,c_value);           
}
    
jlong createTimeZone (JNIEnv* env, jobject this)
{
    mamaTimeZone    cTimeZone  = NULL;
	jlong			jTimeZone  = 0;
    mama_status     status     = MAMA_STATUS_OK;
    char errorString           [UTILS_MAX_ERROR_STRING_LENGTH];
    
    if (MAMA_STATUS_OK!=(status=mamaTimeZone_create(&cTimeZone)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create mamaTimeZone().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
  
	/* Cast the cTimeZone to a jlong. */
	jTimeZone = CAST_POINTER_TO_JLONG(cTimeZone);
  
    /*If we get this far we have a valid timeZome pointer to set*/
    (*env)->SetLongField(env, this,
                         tzFieldObjectFieldId_g,
                         jTimeZone);
						 
	return jTimeZone;
}

/*
 * Class:     com_wombat_mama_MamaDateTime
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDateTime_initIDs
  (JNIEnv* env, jclass class)
{
    dateTimePointerFieldId_g  = (*env)->GetFieldID(
            env, class,"dateTimePointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!dateTimePointerFieldId_g) return;/*Exception auto thrown*/
    
    tzFieldObjectFieldId_g  = (*env)->GetFieldID(
            env, class,"timeZonePointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);  
    if (!tzFieldObjectFieldId_g); return;/*Exception auto thrown*/               
}
