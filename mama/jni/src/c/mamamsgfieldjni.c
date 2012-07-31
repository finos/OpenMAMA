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
* MamaMsgField.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>
                                                                                                                             
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaMsgField.h"

/*Get signed scalars from MamaMsgField*/
#define ExpandGetScalarVector(JTYPE, JFUNC, CTYPE, CFUNC)               \
do                                                                      \
{                                                                       \
    JTYPE ## Array  result          =   NULL;                           \
    jlong           msgFieldPointer =   0;                              \
    mama_status     status          =   MAMA_STATUS_OK;                 \
    mama_size_t     cResultLen      =   0;                              \
    CTYPE*          cResult         =   NULL;                           \
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];    \
                                                                        \
    msgFieldPointer = (*env)->GetLongField(env,                         \
                         this,messageFieldPointerFieldId_g);            \
    assert(0!=msgFieldPointer);                                         \
                                                                        \
    if (MAMA_STATUS_OK!=(status=mamaMsgField_getVector ## CFUNC (       \
                        CAST_JLONG_TO_POINTER(mamaMsgField,             \
                        msgFieldPointer),                               \
                        &cResult,                                       \
                        &cResultLen)))                                  \
    {                                                                   \
        utils_buildErrorStringForStatus(                                \
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,             \
                "Could not get vector " #CFUNC " for mamaMsgField.", status);      \
        utils_throwWombatException(env,errorString);                    \
        return NULL;                                                    \
    }                                                                   \
    /*If we actually have a valid result convert to Java*/              \
    if (cResultLen > 0)                                                 \
    {                                                                   \
        result = (*env)->New ## JFUNC ## Array (env, cResultLen);       \
        if (!result) return NULL; /*Exception autothrown*/              \
        (*env)->Set ## JFUNC ## ArrayRegion (env, result, 0,            \
                                    cResultLen, cResult);               \
    }                                                                   \
    return result;                                                      \
} while(0)                                                              \

/*Get unsigned vectors from the MamaMsgField*/
#define ExpandGetScalarVectorU(JTYPE, JFUNC, CTYPE, CFUNC)               \
do                                                                      \
{                                                                       \
    JTYPE ## Array  result          =   NULL;                           \
    jlong           msgFieldPointer =   0;                              \
    mama_status     status          =   MAMA_STATUS_OK;                 \
    mama_size_t     cResultLen      =   0;                              \
    CTYPE*          cResult         =   NULL;                           \
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];    \
                                                                        \
    msgFieldPointer = (*env)->GetLongField(env,                         \
                         this,messageFieldPointerFieldId_g);            \
    assert(0!=msgFieldPointer);                                         \
                                                                        \
    if (MAMA_STATUS_OK!=(status=mamaMsgField_getVector ## CFUNC (       \
                        CAST_JLONG_TO_POINTER(mamaMsgField,             \
                        msgFieldPointer),                               \
                        &cResult,                                       \
                        &cResultLen)))                                  \
    {                                                                   \
        utils_buildErrorStringForStatus(                                \
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,             \
                "Could not get vector " #CFUNC " for mamaMsgField.", status);      \
        utils_throwWombatException(env,errorString);                    \
        return NULL;                                                    \
    }                                                                   \
    /*If we actually have a valid result convert to Java*/              \
    if (cResultLen > 0)                                                 \
    {                                                                   \
        int     i       = 0;                                                \
        JTYPE*  j_array = NULL;                                             \
                                                                            \
        result = (*env)->New ## JFUNC ## Array (env, cResultLen);           \
        if (!result) return NULL;                                           \
                                                                            \
        j_array = calloc (cResultLen, sizeof (JTYPE));                      \
        if (!j_array)                                                       \
        {                                                                   \
            utils_buildErrorStringForStatus(                                \
                    errorString, UTILS_MAX_ERROR_STRING_LENGTH,             \
                    "getArray " #CFUNC " (): Could not allocate.", status); \
            utils_throwExceptionForMamaStatus(env,status,errorString);      \
            return NULL;                                                    \
        }                                                                   \
                                                                            \
        for (i=0;i<cResultLen;i++) j_array[i] = (JTYPE)cResult[i];          \
                                                                            \
        (*env)->Set ## JFUNC ## ArrayRegion (env, result, 0, cResultLen, j_array);  \
                                                                            \
        free ((void*)j_array);                                                     \
    }                                                                       \
    return result;                                                          \
} while(0)                                                                  \



/******************************************************************************
* Local data structures
*******************************************************************************/

/******************************************************************************
* Global/Static variables
*******************************************************************************/
/*Field ids for MamaMsg.java field callbacks*/
jfieldID    messageFieldPointerFieldId_g   =   NULL;
static jfieldID  pricePointerFieldId_g     =   NULL;
static jfieldID  subMsgFieldId_g    =   NULL;
static jfieldID  dateTimePointerFieldId_g  =   NULL;
static jfieldID  jMsgArrayFieldId_g        =   NULL;
static jfieldID  jMsgArraySizeFieldId_g    =   NULL;

/*Method ids for MamaMsg.java method callbacks*/
jmethodID  messageFieldConstructorId_g     =   NULL;

extern jfieldID         messagePointerFieldId_g;
extern jmethodID        messageConstructorId_g;

/*Used for creating MamaDescriptor Java Objects*/
static  jmethodID    fieldDescriptorConstructorId_s  =   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/

/*Utility function which returns a new MamaFieldDescriptor object*/
static jobject createJavaMamaFieldDescriptor(JNIEnv* env, int fid,
                                     mamaFieldType type, const char* name);
                                     
/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getFid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsgField_getFid
  (JNIEnv* env, jobject this)
{
    uint16_t    fid_c           =   0;
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(mamaMsgField_getFid(
                    CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                    &fid_c)))
    {
       utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get fid for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return (jint)fid_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsgField_getName
  (JNIEnv* env, jobject this)
{
    const char*     name_c          =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getName(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &name_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get name for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getType
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaMsgField_getType
  (JNIEnv* env, jobject this)
{
    mamaFieldType   fieldType_c         =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    jlong           msgFieldPointer     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(mamaMsgField_getType(
                    CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                    &fieldType_c)))
    {
       utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get type for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return 0;
    }

    return (jshort)fieldType_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getTypeName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsgField_getTypeName
  (JNIEnv* env, jobject this)
{
    const char*     name_c          =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getTypeName(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &name_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get name for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getBoolean
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsgField_getBoolean
  (JNIEnv* env, jobject this)
{
    mama_bool_t     retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getBool(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get short for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return retVal_c?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getI8
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_com_wombat_mama_MamaMsgField_getI8
  (JNIEnv* env, jobject this)
{
    int8_t          retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getI8(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get I8 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jbyte)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getU8
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaMsgField_getU8
  (JNIEnv* env, jobject this)
{
    uint8_t         retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getU8(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get U8 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jshort)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getChar
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_com_wombat_mama_MamaMsgField_getChar
  (JNIEnv* env, jobject this)
{
    char            retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getChar(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get char for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jchar)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getI16
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaMsgField_getI16
  (JNIEnv* env, jobject this)
{
    int16_t         retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getI16(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get I16 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jshort)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    initIDs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsgField_getU16
  (JNIEnv* env, jobject this)
{
    uint16_t        retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getU16(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get U16 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jint)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getI32
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsgField_getI32
  (JNIEnv* env, jobject this)
{
    int32_t         retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getI32(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get I32 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jint)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getU32
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsgField_getU32
  (JNIEnv* env, jobject this)
{
    uint32_t        retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getU32(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get U32 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jlong)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getI64
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsgField_getI64
  (JNIEnv* env, jobject this)
{
    int64_t         retVal_c        =   0L;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getI64(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get I64 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jlong)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getU64
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsgField_getU64
  (JNIEnv* env, jobject this)
{
    uint64_t        retVal_c        =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getU64(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get U64 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    /*We gotta truncate here!*/
    return (jlong)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getF32
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_com_wombat_mama_MamaMsgField_getF32
  (JNIEnv* env, jobject this)
{
    mama_f32_t      retVal_c        =   0.0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getF32(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get F32 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jfloat)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getF64
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaMsgField_getF64
  (JNIEnv* env, jobject this)
{
    mama_f64_t      retVal_c        =   0.0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getF64(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get F64 for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
    return (jdouble)retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsgField_getString
  (JNIEnv* env, jobject this)
{
    const char*     retVal_c        =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getString(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &retVal_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get String for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, retVal_c);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getOpaque
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_wombat_mama_MamaMsgField_getOpaque
  (JNIEnv* env, jobject this)
{
    jbyteArray      result          =   NULL;
    jlong           msgFieldPointer =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    mama_size_t     cResultLen      =   0;
    const void*     cResult         =   NULL;
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    if (MAMA_STATUS_OK!=(status=mamaMsgField_getOpaque(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &cResult,
                        &cResultLen)))
    {
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get opaque for mamaMsgField.", status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    /*If we actually have a valid result convert to Java*/
    if (cResultLen > 0)
    {
        result = (*env)->NewByteArray (env, cResultLen);
        if (!result) return NULL; /*Exception autothrown*/
        (*env)->SetByteArrayRegion (env, result, 0, cResultLen, cResult);
    }
    return result;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayI8
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayI8
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jbyte, Byte, const mama_i8_t, I8);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayU8
 * Signature: ()[S
 */
JNIEXPORT jshortArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayU8
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVectorU (jshort, Short, const mama_u8_t, U8);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayI16
 * Signature: ()[S
 */
JNIEXPORT jshortArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayI16
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jshort, Short, const mama_i16_t, I16);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayU16
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayU16
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVectorU (jint, Int, const mama_u16_t, U16);
}

JNIEXPORT jintArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayI32
  (JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jint, Int, const mama_i32_t, I32);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayU32
 * Signature: ()[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayU32
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVectorU (jlong, Long, const mama_u32_t, U32);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayI64
 * Signature: ()[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayI64
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jlong, Long, const mama_i64_t, I64);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayU64
 * Signature: ()[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayU64 
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVectorU (jlong, Long, const mama_u64_t, U64);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayF32
 * Signature: ()[F
 */
JNIEXPORT jfloatArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayF32
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jfloat, Float, const mama_f32_t, F32);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayF64
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayF64
(JNIEnv* env, jobject this)
{
    ExpandGetScalarVector (jdouble, Double, const mama_f64_t, F64);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    _getPrice
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgField__1getPrice
  (JNIEnv* env, jobject this)
{ 
    mama_status     status          =   MAMA_STATUS_OK;
    jobject         result          =   NULL;
    jlong           msgFieldPointer =   0;
    jlong           pricePointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.");
    
    pricePointer   = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    assert(0!=pricePointer);
    
    if(MAMA_STATUS_OK!=(status=mamaMsgField_getPrice(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        CAST_JLONG_TO_POINTER(mamaPrice,pricePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Price for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);  
    }
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    _getMsg
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgField__1getMsg
  (JNIEnv* env, jobject this)
{ 
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    jlong           msgPointerId    =   0;
    jobject         subMsg          = NULL;
    mamaMsg         msg             =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.");

    subMsg = (*env)->GetObjectField(env,this,subMsgFieldId_g),
    assert(0!=subMsg);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getMsg(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &msg)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get Msg for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return; 
    }
    (*env)->SetLongField (env, 
                          subMsg,
                          messagePointerFieldId_g, 
                          CAST_POINTER_TO_JLONG(msg));
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    _getDateTime
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgField__1getDateTime
  (JNIEnv* env, jobject this)
{
    mama_status     status              =   MAMA_STATUS_OK;
    jobject         result              =   NULL;
    jlong           msgFieldPointer     =   0;
    jlong           dateTimePointer     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.");
    
    dateTimePointer   = (*env)->GetLongField(env,this,dateTimePointerFieldId_g);
    assert(0!=dateTimePointer);
       
    if(MAMA_STATUS_OK!=(status=mamaMsgField_getDateTime(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        CAST_JLONG_TO_POINTER(mamaDateTime,dateTimePointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get DateTime for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getDescriptor
 * Signature: ()Lcom/wombat/mama/MamaFieldDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaMsgField_getDescriptor
  (JNIEnv* env, jobject this)
{    
    jobject             j_fieldDescriptor   =   NULL;
    jlong               msgFieldPointer     =   0;
    mamaFieldDescriptor descriptor;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);    
    
    if(MAMA_STATUS_OK != (status = mamaMsgField_getDescriptor (
                          CAST_JLONG_TO_POINTER (mamaMsgField,msgFieldPointer),
                          &descriptor)))           
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create field descriptor.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }
    
    j_fieldDescriptor = createJavaMamaFieldDescriptor ( env,
                        mamaFieldDescriptor_getFid(descriptor),
                        mamaFieldDescriptor_getType(descriptor),
                        mamaFieldDescriptor_getName(descriptor));
    /*
     If createJavaMamaFieldDescriptor returned NULL an exception will
     already be on the stack
    */ 
    return j_fieldDescriptor;   
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayString
 * Signature: ()[Ljava/lang/String
 */
JNIEXPORT jobjectArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayString
  (JNIEnv* env, jobject this)
{
    jobjectArray    result          =   NULL;
    const char*     c_name          =   NULL;
    jlong           msgFieldPointer =   0;
    const char**    tempResult      =   NULL;
    size_t          resultLength    =   0;
    int             i               =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jstring         nextString      =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jclass          stringArrayClass   =
                    (*env)->FindClass(env,"java/lang/String");
    if(!stringArrayClass) return NULL;/*Exception auto thrown*/

    assert(messagePointerFieldId_g!=NULL);
    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    assert(msgFieldPointer!=0);

    /*Get the array of char* from the C message*/
    if(MAMA_STATUS_OK!=(status=mamaMsgField_getVectorString(
                    CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                    &tempResult,&resultLength)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get string vector "
                "from mamaMsgField.",
                status);
        (*env)->DeleteLocalRef(env, stringArrayClass);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return NULL;
    }

    if(resultLength>0)
    {
        /*Create the jstring array*/
        result = (*env)->NewObjectArray(env,resultLength,stringArrayClass,NULL);
        if(!result)
        {
            (*env)->DeleteLocalRef(env, stringArrayClass);
            return NULL;
        }
        for(i=0;i<resultLength;++i)
        {
            nextString = (*env)->NewStringUTF(env, tempResult[i]);
            /*Add the new msg to the Java string array*/
            (*env)->SetObjectArrayElement(env,result,i,nextString);
        }
    }
    (*env)->DeleteLocalRef(env, stringArrayClass);
    return  result;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    getArrayMsg
 * Signature: ()[Lcom/wombat/mama/MamaMsgField;
 */
JNIEXPORT jobjectArray JNICALL Java_com_wombat_mama_MamaMsgField_getArrayMsg
  (JNIEnv* env, jobject this)
{
    const mamaMsg*  tempResult      =   NULL;
    size_t          lenTempResult   =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jobjectArray    result          =   NULL;
    jlong           jMsgArrayPointer=   0;
    jint            jMsgArraySize   =   0;
    
    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    /* Both will be 0/NULL if this is the first call */
    jMsgArrayPointer = (*env)->GetLongField(env,this,jMsgArrayFieldId_g);
    jMsgArraySize = (*env)->GetIntField(env,this,jMsgArraySizeFieldId_g);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getVectorMsg(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        &tempResult, &lenTempResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get vector message from mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    if(lenTempResult>0)
    {
        jobject* jMsgArray = CAST_JLONG_TO_POINTER (jobject*, jMsgArrayPointer);
        result = utils_getReusedJniMsgArrayFromVectorMsg(env,
                (mamaMsg*)tempResult,lenTempResult,messageConstructorId_g,
                messagePointerFieldId_g, 
                &jMsgArray, 
                &jMsgArraySize);
        (*env)->SetIntField (env, this, jMsgArraySizeFieldId_g, jMsgArraySize);
        (*env)->SetLongField (env, this, jMsgArrayFieldId_g, 
                             CAST_POINTER_TO_JLONG (jMsgArray));
    }
    return result;
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    toString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsgField_toString
  (JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgFieldPointer =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    char retVal_c[128];

    msgFieldPointer = (*env)->GetLongField(env,this,messageFieldPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgFieldPointer,  
		"Null parameter, MamaMsgField may have already been destroyed.", NULL);

    if(MAMA_STATUS_OK!=(status=mamaMsgField_getAsString(
                        CAST_JLONG_TO_POINTER(mamaMsgField,msgFieldPointer),
                        retVal_c, 128)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get as String for mamaMsgField.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    return (*env)->NewStringUTF(env, retVal_c);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgField_destroy 
  (JNIEnv* env, jobject this)
{
    jlong           jMsgArrayPointer=   0;
    jobject*        jMsgArray       =   NULL;
    jint            jMsgArraySize   =   0;
    jint            i               =   0;
    jobject         jMsg            =   NULL;
    /* Both will be 0/NULL if this is the first call */
    jMsgArrayPointer = (*env)->GetLongField(env,this,jMsgArrayFieldId_g);
    jMsgArraySize = (*env)->GetIntField(env,this,jMsgArraySizeFieldId_g);   
    
    jMsgArray = CAST_JLONG_TO_POINTER(jobject*,jMsgArrayPointer);
    for (; i != jMsgArraySize; ++i)
    {
       jMsg = CAST_JLONG_TO_POINTER(jobject,jMsgArray[i]);
       (*env)->SetLongField(env, jMsg,
                         messagePointerFieldId_g,
                         0);
       (*env)->DeleteGlobalRef(env,jMsg);
    }
    
    if (jMsgArray)
    {
        free (CAST_JLONG_TO_POINTER(jobject*,jMsgArray));
        jMsgArray = 0; 
        (*env)->SetLongField(env, this,
                     jMsgArrayFieldId_g,
                     0);
    } 
    jMsgArraySize = 0;
    
    (*env)->SetIntField (env, this, jMsgArraySizeFieldId_g, jMsgArraySize);
}

/*
 * Class:     com_wombat_mama_MamaMsgField
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgField_initIDs
  (JNIEnv* env, jclass class)
{
    jclass priceClass       =   NULL;
    jclass dateTimeClass    =   NULL;
    jclass  fieldDescriptorClass    =   NULL;
   
    messageFieldPointerFieldId_g = (*env)->GetFieldID(env,
            class, "msgFieldPointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!messageFieldPointerFieldId_g) return;/*Exception auto thrown*/

    messageFieldConstructorId_g = (*env)->GetMethodID(env,
            class,
            "<init>",
            "()V" );
          
    pricePointerFieldId_g = (*env)->GetFieldID(env,
                             class,"pricePointer_i",
                             UTILS_JAVA_POINTER_TYPE_SIGNATURE);
                         
    subMsgFieldId_g  = (*env)->GetFieldID(env,
                             class,"myMsg",
                             "Lcom/wombat/mama/MamaMsg;");
                         
    dateTimePointerFieldId_g = (*env)->GetFieldID(env,
                               class,"dateTimePointer_i",
                               UTILS_JAVA_POINTER_TYPE_SIGNATURE); 
                               
    jMsgArrayFieldId_g  = (*env)->GetFieldID(env,
                             class,"jMsgArray_i",
                             UTILS_JAVA_POINTER_TYPE_SIGNATURE);
 
    jMsgArraySizeFieldId_g = (*env)->GetFieldID(env,
                               class,"jMsgArraySize_i",
                               "I");   
                               
    /*Get a reference to the MamaFieldDescriptor class*/
    fieldDescriptorClass = (*env)->FindClass(env,
                                 UTILS_FIELD_DESCRIPTOR_CLASS_NAME);
    
    /*Get the field descriptor ID's*/
    fieldDescriptorConstructorId_s = (*env)->GetMethodID(env, fieldDescriptorClass,
                               "<init>",
                               UTILS_FIELD_DESCRIPTOR_CONSTRUCTOR_PROTOTYPE
                               );
                            
    return;
}

/******************************************************************************
* Local function implementation
*******************************************************************************/

jobject createJavaMamaFieldDescriptor(JNIEnv* env, int fid,
                             mamaFieldType type, const char* name)
{
    jclass  fieldDescriptorClass    =   NULL;
    jobject j_fieldDescriptor       =   NULL;
    char stringifiedFieldDescriptor[255];/*Should be long enough!!*/

    /*Get a reference to the MamaFieldDescriptor class*/
    fieldDescriptorClass = (*env)->FindClass(env,
                         UTILS_FIELD_DESCRIPTOR_CLASS_NAME);
    if (!fieldDescriptorClass) return NULL;/*Exception auto thrown*/

    snprintf(stringifiedFieldDescriptor,255,"%3d : %-3d : %s",fid,type,name);
    
    j_fieldDescriptor =  (*env)->NewObject(env, fieldDescriptorClass,
                             fieldDescriptorConstructorId_s,fid,
                             type,(*env)->NewStringUTF(env, name),
                             (*env)->NewStringUTF(env,
                                     stringifiedFieldDescriptor));

    (*env)->DeleteLocalRef(env, fieldDescriptorClass);
    
    return j_fieldDescriptor;
}

