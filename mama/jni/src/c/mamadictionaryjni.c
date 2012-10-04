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
* MamaDictionary.java source file.
*
* Note: mamaFieldDescriptor underlying C structures will get deleted
*       once the dictionary has been destroyed.
*/

/******************************************************************************
* includes
*******************************************************************************/

#include "mamajniutils.h"
#include "mamajni/dictionaryimpl.h"
#include "mamajni/com_wombat_mama_MamaDictionary.h"

#include <assert.h>

/******************************************************************************
* defines
*******************************************************************************/
#define UTILS_MAX_FIELD_DESCRIPTORS 100

/******************************************************************************
* Local data structures
*******************************************************************************/
                                                                                                                                               
/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern jmethodID   messageConstructorId_g;
extern jfieldID    messagePointerFieldId_g;


/*Field ids for MamaTransportImpl.java field callbacks*/
jfieldID    dictionaryPointerFieldId_g             =   NULL;/*Pointer to sub instance*/
jfieldID    dictionaryClosurePointerFieldId_g      =   NULL;/*Pointer to sub instance*/

/*Method ids for MamaTransportImpl.java method callbacks*/
jmethodID   dictionaryConstructorId_g                =   NULL;

/*Used for creating MamaFieldDescriptor Java Objects*/
static  jmethodID    fieldDescriptorConstructorId_s  =   NULL;
static  jmethodID    javaArrayListAddId_s           =   NULL;
static  jmethodID    javaArrayListConstructorId_s   =   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/

/*Utility function which returns a new MamaFieldDescriptor object*/
static jobject createJavaMamaFieldDescriptor(JNIEnv* env, int fid,
                                     mamaFieldType type, const char* name);

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    createFieldDescriptor
 * Signature: (ILjava/lang/String;S)Lcom/wombat/mama/MamaFieldDescriptor;
 */
JNIEXPORT jobject JNICALL
Java_com_wombat_mama_MamaDictionary_createFieldDescriptor
  (JNIEnv* env, jobject this, jint fid, jstring name, jshort type)
{
    jlong               dictionaryPointer   =   0;
    jobject             j_fieldDescriptor   =   NULL;
    const char*         c_name              =   NULL;
    mamaFieldDescriptor descriptor;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    if (name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return NULL;/*Exception auto thrown*/
    }

    if(MAMA_STATUS_OK!=(status = 
            mamaDictionary_createFieldDescriptor (
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        (mama_fid_t)fid,
                        c_name,
                        (mamaFieldType)type,
                        &descriptor)))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
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
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    /*
     If createJavaMamaFieldDescriptor returned NULL an exception will
     already be on the stack
    */ 
    return j_fieldDescriptor;
}
                                                                                                  
/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    buildDictionaryFromMessage
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaDictionary_buildDictionaryFromMessage
  (JNIEnv* env, jobject this, jobject msg)
{
    jlong               dictionaryPointer   =   0;
    jlong               messagePointer      =   0;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    dictionaryPointer =
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);

    if(MAMA_STATUS_OK!=(status =
            mamaDictionary_buildDictionaryFromMessage (
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        CAST_JLONG_TO_POINTER(mamaMsg,messagePointer))))
    {
        utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to build dictionary from message.",
                status);
        utils_throwMamaException(env,errorString);
    }

    return;
}
                                                                                                  
/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getDictionaryMessage
 * Signature: ()Lcom/wombat/mama/MamaMsg;
 */
JNIEXPORT jobject JNICALL
Java_com_wombat_mama_MamaDictionary_getDictionaryMessage
  (JNIEnv* env, jobject this)
{
    jlong               dictionaryPointer   =   0;
    mama_status         status              =   MAMA_STATUS_OK;
    mamaMsg             msg                 =   NULL;
    jobject             j_msg               =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    dictionaryPointer =
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    if(MAMA_STATUS_OK!=(status =
            mamaDictionary_getDictionaryMessage (
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &msg)))
    {
        utils_buildErrorStringForStatus (
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed get dictionary message.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }

    j_msg = utils_createJavaMamaMsg (env);

    (*env)->SetLongField(env, j_msg, messagePointerFieldId_g,
                                     CAST_POINTER_TO_JLONG(msg));

    return j_msg;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFeedName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaDictionary_getFeedName
  (JNIEnv* env, jobject this)
{
    jlong               dictionaryPointer   =   0;
    const char*         feedName            =   NULL;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    status = mamaDictionary_getFeedName(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &feedName);
    
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, feedName);
        case MAMA_STATUS_NOT_FOUND:
        case MAMA_STATUS_NULL_ARG:
            return NULL;     
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaDictionary_getFeedName() Failed.",
                status);
            utils_throwMamaException(env,errorString);
            return NULL;   
    }
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFeedHost
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaDictionary_getFeedHost
  (JNIEnv* env, jobject this)
{
    jlong               dictionaryPointer   =   0;
    const char*         feedHost            =   NULL;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    status = mamaDictionary_getFeedHost(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &feedHost);
    
    switch (status)
    {
        case MAMA_STATUS_OK:
            return (*env)->NewStringUTF(env, feedHost);
        case MAMA_STATUS_NOT_FOUND:
        case MAMA_STATUS_NULL_ARG:
            return NULL;     
        default:
            utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaDictionary_getFeedHost() Failed.",
                status);
            utils_throwMamaException(env,errorString);
            return NULL;   
    }
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFieldByFid
 * Signature: (I)Lcom/wombat/mama/MamaMsgField;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDictionary_getFieldByFid
  (JNIEnv* env, jobject this, jint fid)
{
    jlong               dictionaryPointer   =   0;
    jobject             j_fieldDescriptor   =   NULL;
    mamaFieldDescriptor descriptor;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);
    

    if(MAMA_STATUS_OK != ( status = 
            mamaDictionary_getFieldDescriptorByFid(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &descriptor,
                        (unsigned short)fid)))
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
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFieldByIndex
 * Signature: (I)Lcom/wombat/mama/MamaMsgField;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDictionary_getFieldByIndex
  (JNIEnv* env, jobject this, jint index)
{
    jlong               dictionaryPointer   =   0;
    jobject             j_fieldDescriptor   =   NULL;
    mamaFieldDescriptor descriptor;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);
    
    if(MAMA_STATUS_OK != ( status = 
    mamaDictionary_getFieldDescriptorByIndex(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &descriptor,
                        (unsigned short)index)))
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

    return j_fieldDescriptor;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFieldByName
 * Signature: (Ljava/lang/String;)Lcom/wombat/mama/MamaMsgField;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDictionary_getFieldByName
  (JNIEnv* env, jobject this, jstring name)
{
    jlong               dictionaryPointer   =   0;
    jobject             j_fieldDescriptor   =   NULL;
    mamaFieldDescriptor descriptor;
    mama_status         status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    const char*         c_name              =   NULL;
    
    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    c_name = (*env)->GetStringUTFChars(env,name,0);
    if(!c_name)return NULL;/*Exception auto thrown*/

    if(MAMA_STATUS_OK != (status=
        mamaDictionary_getFieldDescriptorByName(
                CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                &descriptor,
                c_name)))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        /* MAMDA expects null if not found */
        if( MAMA_STATUS_NOT_FOUND != status )
        {
            utils_buildErrorStringForStatus(
                            errorString,
                            UTILS_MAX_ERROR_STRING_LENGTH,
                            "Failed to create field descriptor for name.",
                            status);
            utils_throwMamaException(env,errorString);
        }
        return NULL;
    }

    j_fieldDescriptor = createJavaMamaFieldDescriptor ( env,
                        mamaFieldDescriptor_getFid(descriptor),
                        mamaFieldDescriptor_getType(descriptor),
                        mamaFieldDescriptor_getName(descriptor));
    /*Tidy up*/
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        
    return j_fieldDescriptor;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getFieldByNameAll
 * Signature: (Ljava/lang/String;)Ljava/util/Collection;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaDictionary_getFieldByNameAll
  (JNIEnv* env, jobject this, jstring name)
{
    jlong dictionaryPointer     =   0;
    jobject j_fieldDescriptor   =   NULL;
    mama_status status          =   MAMA_STATUS_OK;
    const char* c_name          =   NULL;
    int i                       =   0;
    jobject javaCollection      =   NULL;
    size_t  size          =   UTILS_MAX_FIELD_DESCRIPTORS;
    mamaFieldDescriptor descriptors[UTILS_MAX_FIELD_DESCRIPTORS];
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    c_name = (*env)->GetStringUTFChars(env,name,0);
    if(!c_name)return NULL;/*Exception auto thrown*/
    
    if(MAMA_STATUS_OK != (status = mamaDictionary_getFieldDescriptorByNameAll(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        c_name,
                        descriptors,
                        &size
                        )))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create field descriptor by name all.",
                status);
        utils_throwMamaException(env,errorString);
        return NULL;
    }
    
    /*We no longer need the name char*/ 
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    
    /*
     If we actually got any descriptors, create and return a
     new ArrayList (Java expects some implementation of the Collection
     interface)
     */
    if (size>0)
    {
        jclass javaArrayListClass = 
            (*env)->FindClass(env, UTILS_ARRAYLIST_CLASS_NAME); 

        if(!javaArrayListClass)return NULL;/*Exception auto thrown*/

        javaCollection = (*env)->NewObject(env,javaArrayListClass,
                            javaArrayListConstructorId_s);

        if(!javaCollection)return NULL;/*Exception auto thrown*/
        
        for(i=0;i<size;i++)
        {
            jboolean retVal = JNI_TRUE;
            mamaFieldDescriptor descriptor = descriptors[i];
            jobject j_fieldDescriptor = createJavaMamaFieldDescriptor ( env,
                        mamaFieldDescriptor_getFid(descriptor),
                        mamaFieldDescriptor_getType(descriptor),
                        mamaFieldDescriptor_getName(descriptor));
            
            /*No exception expected here - use ret val instead*/
            retVal = (*env)->CallBooleanMethod(env,javaCollection,
                           javaArrayListAddId_s,j_fieldDescriptor);

            if(!retVal)
            {
                /*Throw an exception to that effect*/
                utils_buildErrorStringForStatus(
                    errorString,
                    UTILS_MAX_ERROR_STRING_LENGTH,
                    "Could not add to Java ArrayList.",
                    status);
                utils_throwMamaException(env,errorString);
                return NULL;
            }
        }
    }
    return javaCollection;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getMaxFid
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaDictionary_getMaxFid
  (JNIEnv* env, jobject this)
{
    mama_status status  =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong dictionaryPointer;
    mama_fid_t c_maxFid    =   0;

    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);
    
    if (MAMA_STATUS_OK != 
            (status = mamaDictionary_getMaxFid(
                   CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                   &c_maxFid)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get max fid.",
                status);
        utils_throwMamaException(env,errorString);
        return 0;
    }
    return (jint)c_maxFid;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    getSize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaDictionary_getSize
  (JNIEnv* env, jobject this)
{
    size_t      c_size  =   0;
    mama_status status  =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong dictionaryPointer;

    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    if( MAMA_STATUS_OK != 
            ( status = mamaDictionary_getSize(
                        CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                        &c_size)))
    {
        
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get dictionary size.",
                status);
        
        utils_throwMamaException(env,errorString);

        return 0;
    }
    
    return (jint)c_size;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    hasDuplicates
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaDictionary_hasDuplicates
  (JNIEnv* env, jobject this)
{
    mama_status status      =   MAMA_STATUS_OK;
    jlong dictionaryPointer =   0;
    int hasDuplicates       =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);
            
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(dictionaryPointer,  
		"Null parameter, MamaDictionary may have already been destroyed.", 0)
    
    if(MAMA_STATUS_OK != (status = mamaDictionary_hasDuplicates(
                    CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                    &hasDuplicates)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed hasDuplicates.",
                status);
        utils_throwMamaException(env,errorString);
        return JNI_FALSE;
    }
    
    return hasDuplicates ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDictionary_doFinalizer
  (JNIEnv* env, jobject this)
{   
    jlong dictionaryPointer         =   0;
    jlong dictClosurePointer        =   0;
     
    dictClosurePointer = 
            (*env)->GetLongField(env,this,dictionaryClosurePointerFieldId_g);

    if(dictClosurePointer)
    {
        dictionaryClosure* dictionary_p =
            CAST_JLONG_TO_POINTER(dictionaryClosure*,dictClosurePointer);
        
        if(dictionary_p->mClientCB)
        {
            (*env)->DeleteGlobalRef(env,dictionary_p->mClientCB);
        }
        free(dictionary_p);
        (*env)->SetLongField(env, this,
                 dictionaryClosurePointerFieldId_g,
                 0);
    }

    dictionaryPointer = 
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);
   
    if(dictionaryPointer)
    {
        mamaDictionary_destroy(
               CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer));

        (*env)->SetLongField(env, this,
                     dictionaryPointerFieldId_g,
                     0);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    writeToFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDictionary_writeToFile
  (JNIEnv* env, jobject this, jstring file)
{
    mama_status status  =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong dictionaryPointer;
    const char* file_c = NULL;

    dictionaryPointer =
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    if(file)
    {
        file_c = (*env)->GetStringUTFChars(env,file,0);
        if(!file_c) return;/*Exception thrown*/
    }

    if (MAMA_STATUS_OK !=
            (status = mamaDictionary_writeToFile(
                   CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                   file_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to write to file",
                status);
        utils_throwMamaException(env,errorString);
    }

    /*Tidy up all local refs*/
    if(file_c)
        (*env)->ReleaseStringUTFChars( env,file, file_c);

    return;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    _populateFromFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDictionary__1populateFromFile
  (JNIEnv* env, jobject this, jstring file)
{
    mama_status status  =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong dictionaryPointer;
    const char* file_c = NULL;

    dictionaryPointer =
            (*env)->GetLongField(env,this,dictionaryPointerFieldId_g);

    if(file)
    {
        file_c = (*env)->GetStringUTFChars(env,file,0);
        if(!file_c) return;/*Exception thrown*/
    }

    if (MAMA_STATUS_OK !=
            (status = mamaDictionary_populateFromFile(
                   CAST_JLONG_TO_POINTER(mamaDictionary,dictionaryPointer),
                   file_c)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to populate from file",
                status);
        utils_throwMamaException(env,errorString);
    }

    /*Tidy up all local refs*/
    if(file_c)
        (*env)->ReleaseStringUTFChars( env,file, file_c);

    return;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDictionary_create
  (JNIEnv* env, jobject this)
{
    mama_status                 status              =   MAMA_STATUS_OK;
    mamaDictionary              c_dictionary        =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if(MAMA_STATUS_OK!=(status =
            mamaDictionary_create (&c_dictionary)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create dictionary.",
                status);
        utils_throwMamaException(env,errorString); 
    }

    (*env)->SetLongField(env, this, dictionaryPointerFieldId_g,
                     CAST_POINTER_TO_JLONG(c_dictionary));

    return;
}

/*
 * Class:     com_wombat_mama_MamaDictionary
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDictionary_initIDs
  (JNIEnv* env, jclass class)
{
    jclass  fieldDescriptorClass    =   NULL;
    jclass  javaArrayListClass      =   NULL;

    /*Get a reference to the Java Collection class*/
    javaArrayListClass = (*env)->FindClass(env, UTILS_ARRAYLIST_CLASS_NAME);
    if(!javaArrayListClass)
    {
        (*env)->DeleteLocalRef(env, fieldDescriptorClass);
        return;
    }
   
    javaArrayListAddId_s = (*env)->GetMethodID(env,javaArrayListClass,
                   "add",UTILS_ARRAYLIST_ADD_PROTOTYPE);

    if(!javaArrayListAddId_s)
    {
        (*env)->DeleteLocalRef(env, javaArrayListClass);
        return;/*Exception auto thrown*/
    }

    javaArrayListConstructorId_s = (*env)->GetMethodID(env,javaArrayListClass,
                    "<init>",UTILS_ARRAYLIST_CONSTRUCTOR_PROTOTYPE);

    if(!javaArrayListConstructorId_s)
    {
        (*env)->DeleteLocalRef(env, javaArrayListClass);
        return;/*Exception auto thrown*/
    }
    
    
    /*Get a reference to the MamaFieldDescriptor class*/
    fieldDescriptorClass = (*env)->FindClass(env, UTILS_FIELD_DESCRIPTOR_CLASS_NAME);
    if (!fieldDescriptorClass) return;/*Exception auto thrown*/
   
    /*Get the dictionary ID's*/ 
    dictionaryPointerFieldId_g = (*env)->GetFieldID(env,
                           class, UTILS_DICTIONARY_POINTER_FIELD,
                           UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    if (!dictionaryPointerFieldId_g)
    {
        (*env)->DeleteLocalRef(env, fieldDescriptorClass);
        (*env)->DeleteLocalRef(env, javaArrayListClass);
        return;/*Exception auto thrown*/
    }

    dictionaryClosurePointerFieldId_g = (*env)->GetFieldID(env,
                            class,UTILS_DICTIONARY_CLOSURE_POINTER_FIELD,
                            UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    if (!dictionaryClosurePointerFieldId_g)
    {
        (*env)->DeleteLocalRef(env, fieldDescriptorClass);
        (*env)->DeleteLocalRef(env, javaArrayListClass);
        return;/*Exception auto thrown*/
    }
     
    dictionaryConstructorId_g = (*env)->GetMethodID(env, class,
                        "<init>", UTILS_DICTIONARY_CONSTRUCTOR_PROTOTYPE );
    
    if(!dictionaryConstructorId_g)
    {
        (*env)->DeleteLocalRef(env, fieldDescriptorClass);
        (*env)->DeleteLocalRef(env, javaArrayListClass);
        return;/*Exception auto thrown*/
    }

    /*Get the field descriptor ID's*/
    fieldDescriptorConstructorId_s = (*env)->GetMethodID(env, fieldDescriptorClass,
                               "<init>", UTILS_FIELD_DESCRIPTOR_CONSTRUCTOR_PROTOTYPE );

    
    (*env)->DeleteLocalRef(env, fieldDescriptorClass);
    (*env)->DeleteLocalRef(env, javaArrayListClass);
    
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


