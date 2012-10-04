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
* MamaSource.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaSource.h"

/******************************************************************************
* Global/Static variables
*******************************************************************************/
jfieldID   sourcePointerFieldId_g      = NULL;
jmethodID  transportConstructorId_g    = NULL;
extern jfieldID   transportPointerFieldId_g;
jclass     transportClass_g            = NULL;

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setId
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setId
   (JNIEnv * env , jobject this , jstring sourceId)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* sourceId_c  = NULL;

    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
    
    if(sourceId)
    {
        sourceId_c = (*env)->GetStringUTFChars(env,sourceId,0);
        if(!sourceId_c) return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK != (mamaSource_setId (
                              CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                              sourceId_c
                              )))
    {
        if(sourceId_c)(*env)->ReleaseStringUTFChars(env,
                                                    sourceId, sourceId_c);
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_setId() Failed.",
            status);
        utils_throwMamaException(env,errorString);
        return;
    }
    /*Tidy up all local refs*/
    if(sourceId_c)
        (*env)->ReleaseStringUTFChars( env,sourceId, sourceId_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setMappedId
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setMappedId
(JNIEnv * env, jobject this, jstring mappedId)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* mappedId_c ;
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
      
    if(mappedId)
    {
        mappedId_c = (*env)->GetStringUTFChars(env,mappedId,0);
        if(!mappedId_c) return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK!=(mamaSource_setMappedId (
                            CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                            mappedId_c
                            )))
    {
      if(mappedId_c)(*env)->ReleaseStringUTFChars(env,
                                                  mappedId, mappedId_c);
          utils_buildErrorStringForStatus(
                  errorString,
                  UTILS_MAX_ERROR_STRING_LENGTH,
                  "mamaSource_setMappedId() Failed.",
                  status);
          utils_throwMamaException(env,errorString);
          return;
    }
    /*Tidy up all local refs*/
    if(mappedId_c)(*env)->ReleaseStringUTFChars(
        env,mappedId, mappedId_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setDisplayId
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setDisplayId
(JNIEnv * env, jobject this , jstring displayId)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* displayId_c ;
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
    
    if(displayId)
    {
        displayId_c = (*env)->GetStringUTFChars(env,displayId,0);
        if(!displayId_c)return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK!=(mamaSource_setDisplayId (
                  CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                  displayId_c
                  )))
       {
           if(displayId_c)(*env)->ReleaseStringUTFChars(env,
                                                        displayId, displayId_c);
           utils_buildErrorStringForStatus(
               errorString,
               UTILS_MAX_ERROR_STRING_LENGTH,
               "mamaSource_setDisplayId() Failed.",
               status);
          utils_throwMamaException(env,errorString);
          return;
       }
       /*Tidy up all local refs*/
    if(displayId_c)(*env)->ReleaseStringUTFChars(
        env,displayId, displayId_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setQuality
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setQuality
(JNIEnv * env, jobject this, jshort quality)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
    if(MAMA_STATUS_OK!=(mamaSource_setQuality (
                            CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                            (mama_i16_t)quality
                  )))
    {
          utils_buildErrorStringForStatus(
              errorString,
              UTILS_MAX_ERROR_STRING_LENGTH,
              "mamaSource_setQuality() Failed.",
              status);
          utils_throwMamaException(env,errorString);
          return;
       }
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setSymbolNamespace
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setSymbolNamespace
(JNIEnv * env, jobject this, jstring symbolNamespace)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* symbolNamespace_c = NULL;
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
    
    if(symbolNamespace)
    {
        symbolNamespace_c = (*env)->GetStringUTFChars(env,symbolNamespace,0);
        if(!symbolNamespace_c)return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK!=(mamaSource_setSymbolNamespace (
                            CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                            symbolNamespace_c
                            )))
    {
        if(symbolNamespace_c)(*env)->ReleaseStringUTFChars(env,
                                                           symbolNamespace, symbolNamespace_c);
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_setDisplayId() Failed.",
                  status);
        utils_throwMamaException(env,errorString);
        return;
    }
    /*Tidy up all local refs*/
        if(symbolNamespace_c)(*env)->ReleaseStringUTFChars(
            env,symbolNamespace, symbolNamespace_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setTransportName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setTransportName
(JNIEnv * env, jobject this, jstring transportName)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* transportName_c ;
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.");
    if(transportName)
    {
        transportName_c = (*env)->GetStringUTFChars(env,transportName,0);
        if(!transportName_c)return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK!=(mamaSource_setTransportName (
                            CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                            transportName_c
                            )))
    {
        if(transportName_c)(*env)->ReleaseStringUTFChars(env,
                  transportName, transportName_c);
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_setTransportName() Failed.",
            status);
        utils_throwMamaException(env,errorString);
        return;
    }
    /*Tidy up all local refs*/
    if(transportName_c)(*env)->ReleaseStringUTFChars(
        env,transportName, transportName_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    _setTransportName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource__1setTransportName
(JNIEnv * env, jobject this, jstring transportName)
{
    jlong       pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    const char* transportName_c ;
   
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,
        "Null parameter, MamaSource may have already been destroyed.");
    if(transportName)
    {
        transportName_c = (*env)->GetStringUTFChars(env,transportName,0);
        if(!transportName_c)return;/*Exception thrown*/
    }
    if(MAMA_STATUS_OK!=(mamaSource_setTransportName (
                            CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                            transportName_c
                            )))
    {
        if(transportName_c)(*env)->ReleaseStringUTFChars(env,
                  transportName, transportName_c);
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_setTransportName() Failed.",
            status);
        utils_throwMamaException(env,errorString);
        return;
    }
    /*Tidy up all local refs*/
    if(transportName_c)(*env)->ReleaseStringUTFChars(
        env,transportName, transportName_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    setTransport
 * Signature: (Lcom/wombat/mama/MamaTransport;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_setTransport
(JNIEnv * env, jobject this, jobject transport)
{
     jlong         pMamaSource = 0;
     jlong         pTransport  = 0;
     mama_status   status      = MAMA_STATUS_OK;
     char          errorString [UTILS_MAX_ERROR_STRING_LENGTH];
     
     pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
     MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  
		 "Null parameter, MamaSource may have already been destroyed.");
     
     pTransport = (*env)->GetLongField (env,transport,
                                        transportPointerFieldId_g);
     assert(0!=pTransport);
     if(MAMA_STATUS_OK!=(mamaSource_setTransport (
                  CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                  CAST_JLONG_TO_POINTER (mamaTransport,pTransport)
                  )))
       {
          utils_buildErrorStringForStatus(
                  errorString,
                  UTILS_MAX_ERROR_STRING_LENGTH,
                  "mamaSource_setTransport() Failed.",
                  status);
          utils_throwMamaException(env,errorString);
          return;
       } 
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    _setTransport
 * Signature: (Lcom/wombat/mama/MamaTransport;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource__1setTransport
(JNIEnv * env, jobject this, jobject transport)
{
     jlong         pMamaSource = 0;
     jlong         pTransport  = 0;
     mama_status   status      = MAMA_STATUS_OK;
     char          errorString [UTILS_MAX_ERROR_STRING_LENGTH];

     pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
     MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,
         "Null parameter, MamaSource may have already been destroyed.");

     pTransport = (*env)->GetLongField (env,transport,
                                        transportPointerFieldId_g);
     assert(0!=pTransport);
     if(MAMA_STATUS_OK!=(mamaSource_setTransport (
                  CAST_JLONG_TO_POINTER (mamaSource, pMamaSource),
                  CAST_JLONG_TO_POINTER (mamaTransport,pTransport)
                  )))
       {
          utils_buildErrorStringForStatus(
                  errorString,
                  UTILS_MAX_ERROR_STRING_LENGTH,
                  "mamaSource_setTransport() Failed.",
                  status);
          utils_throwMamaException(env,errorString);
          return;
       }
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getId
 * Signature: ()Ljava/lang/String;
 */
 JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSource_getId
(JNIEnv * env, jobject this)
{
    const char*     name_c          =   NULL;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaSource_getId (
                            CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_getId() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getMappedId
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSource_getMappedId
(JNIEnv * env, jobject this)
{
    const char*     name_c          =   NULL;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaSource_getMappedId (
                            CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaSource_getMappedId() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getDisplayId
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSource_getDisplayId
(JNIEnv *env, jobject this)
{
    const char*     name_c          =   NULL;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status=mamaSource_getDisplayId (
                        CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                        &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_getDisplayId() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getQuality
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaSource_getQuality
(JNIEnv * env, jobject this)
{
    mamaQuality  retVal_c        =   0;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
      
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", 0);
    
    if(MAMA_STATUS_OK!=(status=mamaSource_getQuality (
                            CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                            &retVal_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "mamaSource_getQuality() failed",
            status);
        utils_throwWombatException(env,errorString);
    }
    
    return (jshort) retVal_c;
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getSymbolNamespace
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaSource_getSymbolNamespace
(JNIEnv * env, jobject this)
{
    const char*     name_c          =   NULL;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", NULL);
    if(MAMA_STATUS_OK!=(status= mamaSource_getSymbolNamespace(
                            CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                            &name_c)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaSource_getSymbolNamespace() failed",
            status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }
    
    return (*env)->NewStringUTF(env, name_c);
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    getTransport
 * Signature: ()Lcom/wombat/mama/MamaTransport;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaSource_getTransport
(JNIEnv * env , jobject this)
{
    mamaTransport transport  = NULL;

    jobject      result;
    jlong        pMamaSource = 0;
    mama_status status      = MAMA_STATUS_OK;
    char         errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    jclass       transportClass_g = (*env)->FindClass(env,
                                                      "com/wombat/mama/MamaTransport");
    if(!transportClass_g) return;
    
    transportConstructorId_g = (*env)->GetMethodID(env, transportClass_g,
                                                   "<init>", "()V" );
    
    pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pMamaSource,  
		"Null parameter, MamaSource may have already been destroyed.", NULL);
    
        
    if(MAMA_STATUS_OK!=(status= mamaSource_getTransport(
                            CAST_JLONG_TO_POINTER(mamaSource,pMamaSource),
                            &transport)))
    {
        utils_buildErrorStringForStatus(
                  errorString,
                  UTILS_MAX_ERROR_STRING_LENGTH,
                  "mamaSource_getTransport() failed",
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
 * Class:     com_wombat_mama_MamaSource
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_initIDs
(JNIEnv * env, jclass cls)
{
    sourcePointerFieldId_g  = (*env)->GetFieldID(
        env, cls,"mamaSourcePointer_i",
        UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!sourcePointerFieldId_g) return;/*Exception auto thrown*/

    transportClass_g = (*env)->FindClass(env,
                                         "com/wombat/mama/MamaTransport");
    if(!transportClass_g) return;

    transportConstructorId_g = (*env)->GetMethodID(env, transportClass_g,
                                                   "<init>", "()V" );

    transportPointerFieldId_g =  
        (*env)->GetFieldID(env,
                           transportClass_g,"transportPointer_i",
                           UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    
    
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    createMamaSource
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_createMamaSource
(JNIEnv * env , jobject this)
{
    mamaSource    source  = NULL;
    mama_status     status     = MAMA_STATUS_OK;
    char errorString           [UTILS_MAX_ERROR_STRING_LENGTH];
    
    if (MAMA_STATUS_OK!=(status=mamaSource_create(&source)))
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not create mamaSource.",
            status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return ;
    }
    
    /*If we get this far we have a valid mamaSource pointer to set*/
    (*env)->SetLongField(env, this,
                         sourcePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(source));
}

/*
 * Class:     com_wombat_mama_MamaSource
 * Method:    destroyMamaSource
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaSource_destroyMamaSource
(JNIEnv * env , jobject this)
{
	jlong       pMamaSource = 0;
    mama_status     status     = MAMA_STATUS_OK;
    char errorString           [UTILS_MAX_ERROR_STRING_LENGTH];

	pMamaSource = (*env)->GetLongField (env,this,sourcePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pMamaSource,  "Null parameter, MamaSource may have already been destroyed.");
    status = mamaSource_destroy(CAST_JLONG_TO_POINTER(mamaSource, pMamaSource));                  
    if (MAMA_STATUS_OK!=status)
    {
        utils_buildErrorStringForStatus(
            errorString,
            UTILS_MAX_ERROR_STRING_LENGTH,
            "Could not destroy mamaSource.",
            status);
        utils_throwExceptionForMamaStatus(env,status,errorString);    
		return;
    }   

	/* Clear the environment pointer. */
	(*env)->SetLongField (env, this, sourcePointerFieldId_g, 0);
}

