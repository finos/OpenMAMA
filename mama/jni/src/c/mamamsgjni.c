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
* MamaMsg.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaMsg.h"

/******************************************************************************
* Local data structures
*******************************************************************************/

/******************************************************************************
* Global/Static variables
*******************************************************************************/
/*Field ids for MamaMsg.java field callbacks*/
jfieldID         messagePointerFieldId_g   =   NULL;
static jfieldID  msgFieldObjectFieldId_g   =   NULL;
static jfieldID  pricePointerFieldId_g     =   NULL;
static jfieldID  dateTimePointerFieldId_g  =   NULL;
static jfieldID  msgPricePointerFieldId_g     =   NULL;
static jfieldID  msgDateTimePointerFieldId_g  =   NULL;
static jfieldID  jMsgArrayFieldId_g        =   NULL;
static jfieldID  jMsgArraySizeFieldId_g    =   NULL;

static jfieldID iteratorPointerFieldId_g = NULL;
static jfieldID iteratorFieldPointerFieldId_g = NULL;

static jfieldID jMamaByteValue_g = NULL;
static jfieldID jBooleanValue_g = NULL;
static jfieldID jMamaLongValue_g = NULL;
static jfieldID jMamaShortValue_g = NULL;
static jfieldID jMamaIntValue_g = NULL;
static jfieldID jMamaCharValue_g = NULL;
static jfieldID jMamaDoubleValue_g = NULL;
static jfieldID jMamaFloatValue_g = NULL;
static jfieldID jMamaStringValue_g = NULL; 

/*Method ids for MamaMsg.java method callbacks*/
jmethodID        messageConstructorId_g    =   NULL;

/* This pointer holds the byte buffer used in the createFromByteBuffer function. */
static jbyte *g_byteBuffer = NULL;

/*The onField callback method on the MamaMsgFieldIterator interface*/
static jmethodID iteratorCbOnFieldId_g     =   NULL;

/*Fields and methods defined in other files.*/
extern jfieldID  dictionaryPointerFieldId_g;/*mamadictionaryjni.c*/
extern jfieldID  messageFieldPointerFieldId_g;/*mamamsgfield.c*/
extern jfieldID  payloadBridgePointerFieldId_g; /*mamapayloadbridgejni.c*/
extern jmethodID messageFieldConstructorId_g;/*mamamsgfield.c*/

/******************************************************************************
* Local function declarations
*******************************************************************************/

/*
 Checks the return code from the mamaMsg calls and determines
 whether an exception should be thrown. Uses the message supplied
 in the exception on failure.
*/
static mama_status mamaTry(JNIEnv*     env,
                           mama_status status,
                           const char* msgOnFail);

static mama_status mamaTryIgnoreNotFound(JNIEnv*     env,
                           mama_status status,
                           const char* msgOnFail);
                           
static void growMsgArray (const size_t size);

mama_status convertString(JNIEnv *env, jstring in, const char **out);
mama_status populateCMessageArray(mamaMsg *cArray, JNIEnv *env, jobjectArray javaArray, jint length);

/******************************************************************************
* MACROS
*******************************************************************************/

/*Used for adding all scalar vector types*/
#define ExpandAddOrUpdateScalarVector(JTYPE, FTYPE, CFUNC, CTYPE, OPERATION) \
 do                                                                        \
 {                                                                         \
    jlong       msgPointer        =   0;                                   \
    const char* c_name            =   NULL;                                \
    jsize       native_array_len  =   0;                                   \
    JTYPE*      native_array      =   NULL;                                \
                                                                           \
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);   \
    assert(msgPointer!=0);                                                 \
                                                                           \
    if(name)                                                               \
    {                                                                      \
        c_name = (*env)->GetStringUTFChars(env,name,0);                    \
        if(!c_name)return;/*Exception thrown*/                             \
    }                                                                      \
                                                                           \
    native_array_len = (*env)->GetArrayLength (env, array);                \
                                                                           \
    native_array = (*env)->Get ## FTYPE ## ArrayElements (env, array, NULL);      \
    if (!native_array)                                                     \
    {                                                                      \
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);         \
        return; /*Exception autothrown*/                                   \
    }                                                                      \
                                                                           \
    mamaTry (env,                                                          \
      mamaMsg_ ## OPERATION ## Vector ## CFUNC (CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),     \
                                  c_name, fid, (CTYPE*)native_array,       \
                                  (mama_size_t)native_array_len),          \
             "mamaMsg_" #OPERATION "Vector" #CFUNC "()");                  \
                                                                           \
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);             \
                                                                           \
    /*Needed according to the JNI documentation*/                          \
    (*env)->Release ## FTYPE ## ArrayElements (env, array, native_array, JNI_ABORT); \
                                                                           \
    return;                                                                \
 }                                                                         \
 while(0)                                                                  \

#define ExpandAddOrUpdateScalarVectorU(JTYPE, FTYPE, CFUNC, CTYPE, OPERATION) \
do                                                                          \
{                                                                           \
    jlong       msgPointer        =   0;                                    \
    const char* c_name            =   NULL;                                 \
    jsize       native_array_len  =   0;                                    \
    JTYPE*      native_array      =   NULL;                                 \
    CTYPE*      c_array           =   NULL;                                 \
    int         i                 =   0;                                    \
                                                                            \
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    \
    assert(msgPointer!=0);                                                  \
                                                                            \
    if(name)                                                                \
    {                                                                       \
        c_name = (*env)->GetStringUTFChars(env,name,0);                     \
        if(!c_name)return;/*Exception thrown*/                              \
    }                                                                       \
                                                                            \
    native_array_len = (*env)->GetArrayLength (env, array);                 \
                                                                            \
    native_array = (*env)->Get ## FTYPE ## ArrayElements (env, array, NULL);        \
    if (!native_array)                                                      \
    {                                                                       \
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);          \
        return; /*Exception autothrown*/                                    \
    }                                                                       \
                                                                            \
    /*Need to get the C Array*/                                             \
    c_array = calloc (native_array_len, sizeof (CTYPE));                    \
                                                                            \
    for (i=0;i<native_array_len;i++) c_array[i] = (CTYPE)native_array[i];   \
                                                                            \
    mamaTry (env,                                                           \
      mamaMsg_ ## OPERATION ## Vector ## CFUNC (CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),       \
                                   c_name,                                  \
                                   fid,                                     \
                                   c_array,                                 \
                                   (mama_size_t)native_array_len),          \
             "mamaMsg_" #OPERATION "Vector" #CFUNC "()");                   \
                                                                            \
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);              \
                                                                            \
    /*Needed according to the JNI documentation*/                           \
    (*env)->Release ## FTYPE ## ArrayElements (env, array, native_array, JNI_ABORT);\
                                                                            \
    free (c_array);                                                         \
                                                                            \
    return;                                                                 \
}                                                                           \
while(0)                                                                    \

/*Used for getting all scalar vector types*/
#define ExpandGetScalarVector(JTYPE, FTYPE, CFUNC, CTYPE)                   \
do                                                                          \
{                                                                           \
    JTYPE ## Array     result          =   NULL;                            \
    const char*        c_name          =   NULL;                            \
    jlong              msgPointer      =   0;                               \
    mama_status        status          =   MAMA_STATUS_OK;                  \
    mama_size_t        cResultLen      =   0;                               \
    const CTYPE*       cResult         =   NULL;                            \
    char               errorString     [UTILS_MAX_ERROR_STRING_LENGTH];     \
                                                                            \
    /* Get the pointer to the underlying message*/                          \
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    \
                                                                            \
    /* Get the C string for the name if specified*/                         \
    if(name)                                                                \
    {                                                                       \
        c_name = (*env)->GetStringUTFChars(env,name,0);                     \
        if(!c_name) return NULL;                                            \
    }                                                                       \
                                                                            \
    if (MAMA_STATUS_OK!=(status=mamaMsg_getVector ## CFUNC(                 \
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),              \
                    c_name, fid, &cResult, &cResultLen)))                   \
    {                                                                       \
        utils_buildErrorStringForStatus(                                    \
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,                 \
                "Could not get" #CFUNC "vector from mamaMsg.", status);     \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
        utils_throwExceptionForMamaStatus(env,status,errorString);          \
        return NULL;                                                             \
    }                                                                       \
                                                                            \
    /*If we actually have a valid result convert to Java*/                  \
    if (cResultLen > 0)                                                     \
    {                                                                       \
        result = (*env)->New ## FTYPE ## Array (env, cResultLen);           \
        if (!result)                                                        \
        {                                                                   \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
            return NULL; /*Exception autothrown*/                           \
        }                                                                   \
                                                                            \
        (*env)->Set ## FTYPE ## ArrayRegion (env, result, 0, cResultLen,    \
                                             (JTYPE*)cResult);              \
    }                                                                       \
                                                                            \
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);              \
    return result;                                                          \
}                                                                           \
while(0)                                                                    \


#define ExpandGetScalarVectorU(JTYPE, FTYPE, CFUNC, CTYPE)                  \
do                                                                          \
{                                                                           \
    JTYPE ## Array     result          =   NULL;                            \
    const char*        c_name          =   NULL;                            \
    jlong              msgPointer      =   0;                               \
    mama_status        status          =   MAMA_STATUS_OK;                  \
    mama_size_t        cResultLen      =   0;                               \
    const CTYPE*       cResult         =   NULL;                            \
    char               errorString     [UTILS_MAX_ERROR_STRING_LENGTH];     \
                                                                            \
    /* Get the pointer to the underlying message*/                          \
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    \
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,								\
		"Null parameter, MamaMsg may have already been destroyed.", NULL);        \
                                                                            \
    /* Get the C string for the name if specified*/                         \
    if(name)                                                                \
    {                                                                       \
        c_name = (*env)->GetStringUTFChars(env,name,0);                     \
        if(!c_name) return NULL;                                            \
    }                                                                       \
                                                                            \
    if (MAMA_STATUS_OK!=(status=mamaMsg_getVector ## CFUNC (                \
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),              \
                    c_name, fid, &cResult, &cResultLen)))                   \
    {                                                                       \
        utils_buildErrorStringForStatus(                                    \
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,                 \
                "Could not get " #CFUNC " vector from mamaMsg.", status);   \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
        utils_throwExceptionForMamaStatus(env,status,errorString);          \
        return NULL;                                                        \
    }                                                                       \
                                                                            \
    /*If we actually have a valid result convert to Java*/                  \
    if (cResultLen > 0)                                                     \
    {                                                                       \
        int     i       = 0;                                                \
        JTYPE*  j_array = NULL;                                             \
                                                                            \
        result = (*env)->New ## FTYPE ## Array (env, cResultLen);           \
        if (!result)                                                        \
        {                                                                   \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
            return NULL; /*Exception autothrown*/                           \
        }                                                                   \
                                                                            \
        j_array = calloc (cResultLen, sizeof (JTYPE));                      \
        if (!j_array)                                                       \
        {                                                                   \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
            utils_buildErrorStringForStatus(                                \
                    errorString, UTILS_MAX_ERROR_STRING_LENGTH,             \
                    "getArray " #CFUNC " (): Could not allocate.", status); \
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);      \
            utils_throwExceptionForMamaStatus(env,status,errorString);      \
            return NULL;                                                    \
        }                                                                   \
                                                                            \
        for (i=0;i<cResultLen;i++) j_array[i] = (JTYPE)cResult[i];         \
                                                                            \
        (*env)->Set ## FTYPE ## ArrayRegion (env, result, 0, cResultLen, j_array);  \
                                                                            \
        free ((void*)j_array);                                                     \
    }                                                                       \
                                                                            \
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);              \
    return result;                                                          \
}                                                                           \
while(0)       
                          
#define TryFieldWithType(CFUNC, CTYPE)                  \
do                                                                          \
{                                                                           \
    const char*        c_name          =   NULL;                            \
    jlong              msgPointer      =   0;                               \
    char               errorString     [UTILS_MAX_ERROR_STRING_LENGTH];     \
                                                                            \
    /* Get the pointer to the underlying message*/                          \
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    \
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  					\
		"Null parameter, MamaMsg may have already been destroyed.", JNI_FALSE); \
    /* Get the C string for the name if specified*/                         \
    if(name)                                                                \
    {                                                                       \
        c_name = (*env)->GetStringUTFChars(env,name,0);                     \
        if(!c_name) return NULL;                                            \
    }                                                                       \
                                                                            \
    status=mamaTryIgnoreNotFound(env, mamaMsg_get ## CFUNC (                \
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),              \
                    c_name, fid, &cResult), "Could not get " #CFUNC " from mamaMsg.");                   \
                                                                            \
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);              \
}                                                                     \
while(0)


   
/******************************************************************************
* Public function implementation
*******************************************************************************/

char *CopyBuffer(jbyte *buffer, jsize len)
{
	/* Allocate the memory. */
	char *ret = calloc(len, sizeof(char));
	if(NULL != ret)
	{
		/* Copy the memory over. */
		memcpy(ret, buffer, len);
	}
	
	return ret;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    createFromByteBuffer
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1createFromByteBuffer(JNIEnv *env, jobject this, jbyteArray byteArray)
{
	/* If this function has already been called thrown an error. */
	if(NULL != g_byteBuffer)
	{
		/* Format an error string. */
		char  errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
		utils_buildErrorStringForStatus(
				errorString,
				UTILS_MAX_ERROR_STRING_LENGTH,
				"This message already has a byte buffer, destroy it then re-create.",
				MAMA_STATUS_INVALID_ARG);        
		utils_throwExceptionForMamaStatus(env,MAMA_STATUS_INVALID_ARG ,errorString);
		return;
	}
	{
		/* Get the pointer to the underlying message.  */
		jlong msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
		MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer, "Null parameter, MamaMsg may have already been destroyed.");
		{
			/* Get the number of bytes in the array. */
			jsize arrayLength = (*env)->GetArrayLength(env, byteArray);
			if(arrayLength > 0)
			{
				/* Obtain the native byte array. */
				jbyte *nativeByteArray = (*env)->GetByteArrayElements(env, byteArray, NULL);
				if(nativeByteArray != NULL)
				{
					/* Make a copy of the buffer and save it in the global variable to prevent the
					 * memory from being freed.
					 */
					mama_status status = MAMA_STATUS_NOMEM;
					g_byteBuffer = CopyBuffer(nativeByteArray, arrayLength);
					if(NULL != g_byteBuffer)
					{				
						/* The address of the message pointer must be passed to this function. */
						mamaMsg messagePointer = CAST_JLONG_TO_POINTER(mamaMsg,msgPointer);

						/* Call the native function. */
						status = mamaMsg_createFromByteBuffer (
							&messagePointer,
							(const void*)g_byteBuffer,
							(mama_size_t)arrayLength);

						/* If something went wrong then delete the global array. */
						if(MAMA_STATUS_OK != status)
						{
							free(g_byteBuffer);
							g_byteBuffer = NULL;
						}
					}		
					
					/* Release the array of bytes. */
					(*env)->ReleaseByteArrayElements(env, byteArray, nativeByteArray, JNI_ABORT);
					
					/* If something went wrong with the native function then throw an exception. */
					mamaTry(env, status, "mamaMsg_createFromByteBuffer()");
				}
			}
		}
	}	
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    setNewBuffer
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1setNewBuffer(JNIEnv *env, jobject this, jbyteArray byteArray)
{
	/* Get the pointer to the underlying message.  */
	jlong msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
	MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer, "Null parameter, MamaMsg may have already been destroyed.");
	{
		/* Get the number of bytes in the array. */
		jsize arrayLength = (*env)->GetArrayLength(env, byteArray);
		if(arrayLength > 0)
		{
			/* Obtain the native byte array. */
			jbyte *nativeByteArray = (*env)->GetByteArrayElements(env, byteArray, NULL);
			if(nativeByteArray != NULL)
			{
				/* Make a copy of the buffer and save it in the global variable to prevent the
				 * memory from being freed.
				 */					
				mama_status status = MAMA_STATUS_NOMEM;
				char *newBuffer = CopyBuffer(nativeByteArray, arrayLength);
				if(NULL != newBuffer)
				{				
					/* The address of the message pointer must be passed to this function. */
					mamaMsg messagePointer = CAST_JLONG_TO_POINTER(mamaMsg,msgPointer);

					/* Call the native function. */
					status = mamaMsg_setNewBuffer (
						&messagePointer,
						(const void*)newBuffer,
						(mama_size_t)arrayLength);

					/* If something went wrong then delete the global array. */
					if(MAMA_STATUS_OK == status)
					{
						/* Free the existing global buffer. */
						if(NULL != g_byteBuffer)
						{
							free(g_byteBuffer);
						}
						g_byteBuffer = newBuffer;
					}
					
					/* If something went wrong then delete the global array. */
					else
					{
						free(newBuffer);
						newBuffer = NULL;
					}
				}		
				
				/* Release the array of bytes. */
				(*env)->ReleaseByteArrayElements(env, byteArray, nativeByteArray, JNI_ABORT);
				
				/* If something went wrong with the native function then throw an exception. */
				mamaTry(env, status, "mamaMsg_setNewBuffer()");
			}
		}
	}
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getByteBuffer
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_wombat_mama_MamaMsg_getByteBuffer(JNIEnv* env, jobject this)
{
	/* Returns. */
    jbyteArray ret = NULL;
	
	/* Get the pointer to the underlying message*/
    jlong msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer, "Null parameter, MamaMsg may have already been destroyed.");
	{
		/* Get the byte buffer from the underlying message. */
		mama_size_t cResultLen 	= 0;
		const void* cResult 	= NULL;
		mama_status gbb = mamaMsg_getByteBuffer(
						CAST_JLONG_TO_POINTER(mamaMsg, msgPointer),
						&cResult,
						&cResultLen);
		if(MAMA_STATUS_OK == gbb)
		{
			/* Only continue if the array is valid. */
			if (cResultLen > 0)
			{
				/* Create a java byte array. */
				ret = (*env)->NewByteArray (env, cResultLen);
				if(NULL != ret)
				{		
					/* Copy the bytes across. */
					(*env)->SetByteArrayRegion (env, ret, 0, cResultLen, cResult);
				}			
			}
		}
		
		else
		{
			/* Format an error string. */
			char  errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
			utils_buildErrorStringForStatus(
					errorString,
					UTILS_MAX_ERROR_STRING_LENGTH,
					"Could not get the byte buffer from the mamaMsg.",
					gbb);        
			utils_throwExceptionForMamaStatus(env,gbb ,errorString);
			return;
		}
	}
    
    return ret;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getNumFields
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsg_getNumFields
  (JNIEnv* env, jobject this)
{
    jlong      pointerValue    =   0;
    size_t     numFields       =   0;

    assert(messagePointerFieldId_g!=NULL);

    pointerValue = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    assert(pointerValue!=0);

    mamaTry(env,
            mamaMsg_getNumFields(
                CAST_JLONG_TO_POINTER(mamaMsg,pointerValue),
                &numFields),
            "mamaMsg_getNumFields() ");

    return (jint)numFields;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getBoolean
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getBoolean
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_bool_t boolValue   =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return '0';/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getBool(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&boolValue),
            "mamaMsg_getBool()");

    if(name) (*env)->ReleaseStringUTFChars(env,name, c_name);

    return boolValue == 0 ? JNI_FALSE : JNI_TRUE;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getChar
 * Signature: (Ljava/lang/String;I)C
 */
JNIEXPORT jchar JNICALL Java_com_wombat_mama_MamaMsg_getChar
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    char        charValue   =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

   if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return '0';/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getChar(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&charValue),
            "mamaMsg_getChar() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return charValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI8
 * Signature: (Ljava/lang/String;I)B
 */
JNIEXPORT jbyte JNICALL Java_com_wombat_mama_MamaMsg_getI8
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_i8_t   i8Value     =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return '0';/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getI8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&i8Value),
            "mamaMsg_getI8() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return (jbyte)i8Value;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getU8
 * Signature: (Ljava/lang/String;I)S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaMsg_getU8
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_u8_t   u8Value     =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return '0';/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getU8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&u8Value),
            "mamaMsg_getU8() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return (jshort)u8Value;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI16
 * Signature: (Ljava/lang/String;I)S
 */
JNIEXPORT jshort JNICALL Java_com_wombat_mama_MamaMsg_getI16
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_i16_t  i16Value  =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getI16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&i16Value),
            "mamaMsg_getI16() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return i16Value;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getU16
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsg_getU16
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_u16_t  u16Value    =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getU16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&u16Value),
            "mamaMsg_getU16() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return u16Value;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI32
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsg_getI32
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    int32_t     intValue    =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getI32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&intValue),
            "mamaMsg_getI32() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return intValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getU32
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsg_getU32
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_u32_t  u32Value    =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getU32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&u32Value),
            "mamaMsg_getU32() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return u32Value;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI64
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsg_getI64
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    int64_t     longValue    =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0L;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getI64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&longValue),
            "mamaMsg_getI64() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return longValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getU64
 * Signature: (Ljava/lang/String;I)J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsg_getU64
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer  =   0;
    mama_u64_t  u64Value    =   0;
    const char* c_name      =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getU64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name,(mama_fid_t)fid,&u64Value),
            "mamaMsg_getU64() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    /*Possible loss of precision here!*/
    return (jlong)((mama_i64_t)u64Value);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getSeqNum
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_wombat_mama_MamaMsg_getSeqNum
  (JNIEnv* env, jobject this)
{
    jlong            msgPointer  =   0;
    mama_seqnum_t    seqNum      =   0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);


    mamaTry(env,
            mamaMsg_getSeqNum(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                              &seqNum),
            "mamaMsg_getSeqNum() ");

    return (jlong)seqNum;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getF32
 * Signature: (Ljava/lang/String;I)F
 */
JNIEXPORT jfloat JNICALL Java_com_wombat_mama_MamaMsg_getF32
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer      =   0;
    mama_f32_t  floatValue      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0.0f;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getF32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name,(mama_fid_t)fid,&floatValue),
            "mamaMsg_getF32() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return (jfloat)floatValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getF64
 * Signature: (Ljava/lang/String;I)D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaMsg_getF64
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jlong       msgPointer      =   0;
    mama_f64_t  doubleValue     =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name) return 0.0;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_getF64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name,(mama_fid_t)fid,&doubleValue),
            "mamaMsg_getF64() ");

    if(name)
    {
        (*env)->ReleaseStringUTFChars(env,name, c_name);
    }

    return (jdouble)doubleValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getString
 * Signature: (Ljava/lang/String;I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsg_getString
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jstring     retValue        =   NULL;
    const char* c_retValue      =   NULL;
    const char* c_name          =   NULL;
    jlong       pointerValue    = 0;
    mama_status status;

    assert(messagePointerFieldId_g!=NULL);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return NULL;/*Exception thrown*/
    }

    pointerValue = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    assert(pointerValue!=0);

    status=mamaTry(env,
                   mamaMsg_getString (CAST_JLONG_TO_POINTER(mamaMsg,pointerValue),
                                c_name, (mama_fid_t)fid, &c_retValue),
                   "mamaMsg_getString() ");

    if(MAMA_STATUS_OK!=status)
    {
        /*For any other status an exception will have been thrown
         from mamaTry*/
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return NULL;
    }

    retValue = (*env)->NewStringUTF(env, c_retValue);

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return retValue;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getMsg
 * Signature: (Ljava/lang/String;I)Lcom/wombat/mama/MamaMsg;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaMsg_getMsg
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    mamaMsg         c_result        =   NULL;
    jobject         result          =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jclass          msgClass   =
        (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    if (!msgClass) return NULL;

    assert(messagePointerFieldId_g!=NULL);
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)
        {
            (*env)->DeleteLocalRef(env, msgClass);
            return NULL;/*Exception thrown*/
        }
    }

    if (MAMA_STATUS_OK!=(status=mamaMsg_getMsg(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name, (mama_fid_t)fid,
                    &c_result)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get message from mamaMsg.",
                status);
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        (*env)->DeleteLocalRef(env, msgClass);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    if (c_result)
    {
        /*If we get this far we have a sub message*/
        result = (*env)->NewObject(env, msgClass,
                     messageConstructorId_g, JNI_FALSE);
        (*env)->SetLongField(env, result, messagePointerFieldId_g,
                             CAST_POINTER_TO_JLONG(c_result));
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    (*env)->DeleteLocalRef(env, msgClass);
    return result;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getOpaque
 * Signature: (Ljava/lang/String;I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_wombat_mama_MamaMsg_getOpaque
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jbyteArray      result          =   NULL;
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    mama_size_t     cResultLen      =   0;
    const void*     cResult         =   NULL;
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];

    /* Get the pointer to the underlying message*/
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);

    /* Get the C string for the name if specified*/
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)
            return;
    }
    
    if (MAMA_STATUS_OK!=(status=mamaMsg_getOpaque(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name,
                    fid,
                    &cResult,
                    &cResultLen)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get opaque from mamaMsg.",
                status);
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    /*If we actually have a valid result convert to Java*/
    if (cResultLen > 0)
    {
        result = (*env)->NewByteArray (env, cResultLen);
        
        if (!result)
        {
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
            return NULL; /*Exception autothrown*/
        }

        (*env)->SetByteArrayRegion (env, result, 0, cResultLen, cResult);
        
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return result;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    _getPrice
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1getPrice
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    jlong           pricePointer    =   0;
    jobject         result          =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");
    
    pricePointer   = (*env)->GetLongField(env,this,msgPricePointerFieldId_g);
    assert(0!=pricePointer);
    
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)
            return;
    }

    if (MAMA_STATUS_OK!=(status=mamaMsg_getPrice(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name,
                    (mama_fid_t)fid,
                    CAST_JLONG_TO_POINTER(mamaPrice,pricePointer))))
    {

        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get price from mamaMsg.",
                status);
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
    
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    _getDateTime
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1getDateTime
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    jlong           dateTimePointer =   0;
    jobject         result          =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    char            errorString     [UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    dateTimePointer   = (*env)->GetLongField(env,
                                             this,
                                             msgDateTimePointerFieldId_g);
    assert(0!=dateTimePointer);
    
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)
        {
            return;
        }
    }

    if (MAMA_STATUS_OK!=(status=mamaMsg_getDateTime(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name,
                    (mama_fid_t)fid,
                    CAST_JLONG_TO_POINTER(mamaDateTime,dateTimePointer))))
    {

        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get dateTime from mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayMsg
 * Signature: (Ljava/lang/String;I)[Lcom/wombat/mama/MamaMsg;
 */
JNIEXPORT jobjectArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayMsg
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jobjectArray    result          =   NULL;
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    mamaMsg*        tempResult      =   NULL;
    size_t          resultLength    =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jlong           jMsgArrayPointer=   0;
    jint            jMsgArraySize   =   0;
    
    assert(messagePointerFieldId_g!=NULL);
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);
    
    /* Both will be 0/NULL if this is the first call */
    jMsgArrayPointer = (*env)->GetLongField(env,this,jMsgArrayFieldId_g);
    jMsgArraySize = (*env)->GetIntField(env,this,jMsgArraySizeFieldId_g);
    
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return NULL;/*Exception thrown*/
    }

    /*Get the vector of messages from the C mesage*/
    if(MAMA_STATUS_OK!=(status=mamaMsg_getVectorMsg(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name, (mama_fid_t)fid,
                    (const mamaMsg**)&tempResult,&resultLength)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get vector message "
                "from mamaMsg.",
                status);
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
    
    if(resultLength>0)
    {
        jobject* jMsgArray = CAST_JLONG_TO_POINTER (jobject*, jMsgArrayPointer);
        result = utils_getReusedJniMsgArrayFromVectorMsg(env,
                (mamaMsg*)tempResult,resultLength,messageConstructorId_g,
                messagePointerFieldId_g, 
                &jMsgArray, 
                &jMsgArraySize);
        (*env)->SetIntField (env, this, jMsgArraySizeFieldId_g, jMsgArraySize);
        (*env)->SetLongField (env, this, jMsgArrayFieldId_g, 
                             CAST_POINTER_TO_JLONG (jMsgArray));
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return  result;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayI8
 * Signature: (Ljava/lang/String;I)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayI8
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jbyte, Byte, I8, mama_i8_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayU8
 * Signature: (Ljava/lang/String;I)[S
 */
JNIEXPORT jshortArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayU8
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVectorU (jshort, Short, U8, mama_u8_t); 
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayI16
 * Signature: (Ljava/lang/String;I)[S
 */
JNIEXPORT jshortArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayI16
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jshort, Short, I16, mama_i16_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayU16
 * Signature: (Ljava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayU16
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVectorU (jint, Int, U16, mama_u16_t); 
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayI32
 * Signature: (Ljava/lang/String;I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayI32
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jint, Int, I32, mama_i32_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayU32
 * Signature: (Ljava/lang/String;I)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayU32
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVectorU (jlong, Long, U32, mama_u32_t); 
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayI64
 * Signature: (Ljava/lang/String;I)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayI64
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jlong, Long, I64, mama_i64_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayU64
 * Signature: (Ljava/lang/String;I)[J
 */
JNIEXPORT jlongArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayU64
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVectorU (jlong, Long, U64, mama_u64_t); 
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayF32
 * Signature: (Ljava/lang/String;I)[F
 */
JNIEXPORT jfloatArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayF32
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jfloat, Float, F32, mama_f32_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayF64
 * Signature: (Ljava/lang/String;I)[D
 */
JNIEXPORT jdoubleArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayF64
(JNIEnv* env, jobject this, jstring name, jint fid)
{
    ExpandGetScalarVector (jdouble, Double, F64, mama_f64_t);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getArrayString
 * Signature: (Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_wombat_mama_MamaMsg_getArrayString
  (JNIEnv* env, jobject this, jstring name, jint fid)
{
    jobjectArray    result          =   NULL;
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
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
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return NULL;/*Exception thrown*/
    }

    /*Get the array of char* from the C message*/
    if(MAMA_STATUS_OK!=(status=mamaMsg_getVectorString(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name, (mama_fid_t)fid,
                    &tempResult,&resultLength)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get string vector "
                "from mamaMsg.",
                status);
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        (*env)->DeleteLocalRef(env, stringArrayClass);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
    
    if(resultLength>0)
    {  
        /*Create the jstring array*/
        result = (*env)->NewObjectArray(env,resultLength,stringArrayClass,NULL);
        if(!result)
        {
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
            (*env)->DeleteLocalRef(env, stringArrayClass);
            return NULL;
        }   
        for(i=0;i<resultLength;++i)
        {     
            nextString = (*env)->NewStringUTF(env, tempResult[i]);
            /*Add the new msg to the Java MamaMsg array*/
            (*env)->SetObjectArrayElement(env,result,i,nextString);
        }   
    }
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    (*env)->DeleteLocalRef(env, stringArrayClass);
    return  result;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getIsDefinitelyDuplicate 
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getIsDefinitelyDuplicate
  (JNIEnv* env, jobject this)
{
    jlong           msgPointer      =   0;
    int             cResult         =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getIsDefinitelyDuplicate(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), &cResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getIsDefinitelyDuplicate() failed.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return JNI_FALSE;
    }

    return cResult == 0 ? JNI_FALSE : JNI_TRUE;
} 

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getIsPossiblyDuplicate
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getIsPossiblyDuplicate
  (JNIEnv* env, jobject this)
{
    jlong           msgPointer      =   0;
    int             cResult         =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getIsPossiblyDuplicate(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), &cResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getIsPossiblyDuplicate() failed.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return JNI_FALSE;
    }

    return cResult == 0 ? JNI_FALSE : JNI_TRUE;
}

/*
 * Class:       com_wombat_mama_MamaMsg
 * Method:      getIsPossiblyDelayed
 * Signature:   ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getIsPossiblyDelayed
  (JNIEnv* env, jobject this)
{
    jlong           msgPointer      =   0;
    int             cResult         =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getIsPossiblyDelayed(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), &cResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getIsPossiblyDelayed() failed.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return JNI_FALSE;
    }

    return cResult == 0 ? JNI_FALSE : JNI_TRUE;
}

/*
 * Class:       com_wombat_mama_MamaMsg
 * Method:      getIsDefinitelyDelayed
 * Signature:   ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getIsDefinitelyDelayed
  (JNIEnv* env, jobject this)
{
    jlong           msgPointer      =   0;
    int             cResult         =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getIsDefinitelyDelayed(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), &cResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getIsDefinitelyDelayed() failed.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return JNI_FALSE;
    }
    
    return cResult == 0 ? JNI_FALSE : JNI_TRUE;
}

/*
 * Class:       com_wombat_mama_MamaMsg
 * Method:      getIsOutOfSequence
 * Signature:   ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_getIsOutOfSequence
  (JNIEnv* env, jobject this)
{
    jlong           msgPointer      =   0;
    int             cResult         =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getIsOutOfSequence(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), &cResult)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getIsOutOfSequence() failed.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return JNI_FALSE;
    }

    return cResult == 0 ? JNI_FALSE : JNI_TRUE;
}
        
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addBoolean
 * Signature: (Ljava/lang/String;IZ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addBoolean
  (JNIEnv* env, jobject this, jstring name, jint fid , jboolean value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addBool(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_addBool() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addChar
 * Signature: (Ljava/lang/String;IC)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addChar
  (JNIEnv* env, jobject this, jstring name, jint fid , jchar value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addChar(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_addChar() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addI8Char
 * Signature: (Ljava/lang/String;IB)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addI8
  (JNIEnv* env, jobject this, jstring name, jint fid , jbyte value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addI8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_addI8() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addU8
 * Signature: (Ljava/lang/String;IS)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addU8
  (JNIEnv* env, jobject this, jstring name, jint fid , jshort value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addU8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(uint8_t)value),
            "mamaMsg_addU8() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addI16
 * Signature: (Ljava/lang/String;IS)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addI16
  (JNIEnv* env, jobject this, jstring name, jint fid, jshort value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addI16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int16_t)value),
            "mamaMsg_addI16() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addU16
 * Signature: (Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addU16
  (JNIEnv* env, jobject this, jstring name, jint fid, jint value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addU16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(uint16_t)value),
            "mamaMsg_addU16() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI32
 * Signature: (Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addI32
  (JNIEnv* env, jobject this, jstring name, jint fid, jint value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addI32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int32_t)value),
            "mamaMsg_addI32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addU32
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addU32
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addU32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(uint32_t)value),
            "mamaMsg_addU32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addI64
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addI64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addI64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(int64_t)value),
            "mamaMsg_addI64() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addU64
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addU64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addU64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                               c_name, (mama_fid_t)fid,(uint64_t)value),
            "mamaMsg_addU64() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addF32
 * Signature: (Ljava/lang/String;IF)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addF32
  (JNIEnv* env, jobject this, jstring name, jint fid, jfloat value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addF32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(mama_f32_t)value),
            "mamaMsg_addF32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addF64
 * Signature: (Ljava/lang/String;ID)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addF64
  (JNIEnv* env, jobject this, jstring name, jint fid, jdouble value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_addF64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(mama_f64_t)value),
            "mamaMsg_addF64() ");
    
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}


/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addString
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addString
  (JNIEnv* env, jobject this, jstring name, jint fid, jstring value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    const char* c_value         =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    c_value = (*env)->GetStringUTFChars(env,value,0);
    if(!c_value)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_addString(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,c_value),
            "mamaMsg_addString() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    if(c_value)(*env)->ReleaseStringUTFChars(env,value, c_value);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addPrice
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaPrice;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addPrice
  (JNIEnv* env, jobject this, jstring name, jint fid, jobject price)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    jlong       pricePointer    =   0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    pricePointer = (*env)->GetLongField(env, price,
                        pricePointerFieldId_g);
    if(!pricePointer)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_addPrice(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                             c_name,
                             (mama_fid_t)fid,
                             CAST_JLONG_TO_POINTER(mamaPrice,pricePointer)),
           "mamaMsg_addPrice()");
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addOpaque
 * Signature: (Ljava/lang/String;I[B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addOpaque
  (JNIEnv* env, jobject this, jstring name, jint fid, jbyteArray opaque)
{
    jlong       msgPointer        =   0;
    const char* c_name            =   NULL;
    jsize       native_array_len  =   0;
    jbyte*      native_array      =   NULL;

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");
    
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    native_array_len = (*env)->GetArrayLength (env, opaque);

    native_array = (*env)->GetByteArrayElements (env, opaque, NULL);
    if (!native_array) 
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return; /*Exception autothrown*/
    }

    mamaTry (env, 
             mamaMsg_addOpaque (CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                c_name,
                                fid,
                                (const void*)native_array,
                                (mama_size_t)native_array_len),
             "mamaMsg_addOpaque()");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    /*Needed according to the JNI documentation*/
    (*env)->ReleaseByteArrayElements (env, opaque, native_array, JNI_ABORT);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayI8
 * Signature: (Ljava/lang/String;I[B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayI8
(JNIEnv* env, jobject this, jstring name, jint fid, jbyteArray array)
{
    ExpandAddOrUpdateScalarVector (jbyte, Byte, I8, mama_i8_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayU8
 * Signature: (Ljava/lang/String;I[S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayU8
(JNIEnv* env, jobject this, jstring name, jint fid, jshortArray array)
{
    ExpandAddOrUpdateScalarVectorU (jshort, Short, U8, mama_u8_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayI16
 * Signature: (Ljava/lang/String;I[S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayI16
(JNIEnv* env, jobject this, jstring name, jint fid, jshortArray array)
{
    ExpandAddOrUpdateScalarVector (jshort, Short, I16, mama_i16_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayU16
 * Signature: (Ljava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayU16
(JNIEnv* env, jobject this, jstring name, jint fid, jintArray array)
{
    ExpandAddOrUpdateScalarVectorU (jint, Int, U16, mama_u16_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayI32
 * Signature: (Ljava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayI32
(JNIEnv* env, jobject this, jstring name, jint fid, jintArray array)
{
    ExpandAddOrUpdateScalarVector (jint, Int, I32, mama_i32_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayU32
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayU32
(JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
    ExpandAddOrUpdateScalarVectorU (jlong, Long, U32, mama_u32_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayI64
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayI64
(JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
    ExpandAddOrUpdateScalarVector (jlong, Long, I64, mama_i64_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayU64
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayU64
(JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
    ExpandAddOrUpdateScalarVectorU (jlong, Long, U64, mama_u64_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayF32
 * Signature: (Ljava/lang/String;I[F)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayF32
(JNIEnv* env, jobject this, jstring name, jint fid, jfloatArray array)
{
    ExpandAddOrUpdateScalarVector (jfloat, Float, F32, mama_f32_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayF64
 * Signature: (Ljava/lang/String;I[D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayF64
(JNIEnv* env, jobject this, jstring name, jint fid, jdoubleArray array)
{
    ExpandAddOrUpdateScalarVector (jdouble, Double, F64, mama_f64_t, add);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addDateTime
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaDateTime;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addDateTime
  (JNIEnv* env, jobject this, jstring name, jint fid, jobject dateTime)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    jlong       dateTimePointer =   0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    dateTimePointer = (*env)->GetLongField(env, dateTime,
                        dateTimePointerFieldId_g);
    if(!dateTimePointer)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_addDateTime(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                c_name,
                                (mama_fid_t)fid,
                                CAST_JLONG_TO_POINTER(mamaDateTime, dateTimePointer)),
            "mamaMsg_addDateTime()");
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addMsg
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaMsg)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addMsg
  (JNIEnv* env, jobject this, jstring name, jint fid, jobject msgToAdd)
{
    jlong       msgPointer      = 0;
    const char* c_name          = NULL;
    jlong       msgToAddPointer =  0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    msgToAddPointer = (*env)->GetLongField(env, msgToAdd, messagePointerFieldId_g);
    
    if(!msgToAddPointer)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env, name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_addMsg(CAST_JLONG_TO_POINTER(mamaMsg, msgPointer),
                             c_name,
                             (mama_fid_t)fid,
                             CAST_JLONG_TO_POINTER(mamaMsg, msgToAddPointer)),
           "mamaMsg_addMsg()");
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayMsg
 * Signature: (Ljava/lang/String;I[Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayMsg
  (JNIEnv* env, jobject this, jstring name, jint fid, jobjectArray msgArray)
{
    jobject     nextMsg;
    jlong       msgPointer      = 0;
    jlong       nextMsgPointer  = 0;
    jsize       size            = 0;
    int         i               = 0;
    const char* c_name          = NULL;
    mamaMsg*    cMsgArray       = NULL;

    assert(messagePointerFieldId_g!=NULL);
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }
    /* obtain the size the array */
    size = (*env)->GetArrayLength(env, msgArray);
    /* allocate a C array of the right size */
    if (0 == size)
    {
        return;
    }
    

    cMsgArray = malloc (size * sizeof(mamaMsg));    
    for (i = 0; i < size; ++i)
    {
        /* obtain the next message from the array */
        nextMsg = (*env)->GetObjectArrayElement(env, msgArray, i);
    
        /* get the underlying C structure */
        nextMsgPointer = (*env)->GetLongField(env,nextMsg,messagePointerFieldId_g);
        assert(nextMsgPointer!=0);
        
        /* add it to the array */
        cMsgArray[i] = CAST_JLONG_TO_POINTER(mamaMsg,nextMsgPointer);
    }
   /* add the array to the message */
   mamaTry (env,
            mamaMsg_addVectorMsg(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                 c_name,
                                 (mama_fid_t)fid,
                                 cMsgArray,
                                 (mama_size_t)size),
            "mamaMsg_addVectorMsg()");
   /* clean up */
    free (cMsgArray);
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayMsgWithLength
 * Signature: (Ljava/lang/String;I[Lcom/wombat/mama/MamaMsg;I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayMsgWithLength
  (JNIEnv* env, jobject this, jstring name, jint fid, jobjectArray msgArray, jint length)
{  
    /* The message pointer must be valid. */
    assert(messagePointerFieldId_g != NULL);
    {
        /* Error return. */
        mama_status ret = MAMA_STATUS_NULL_ARG;

        /* Get the message object itself. */    
        jlong msgPointer = (*env)->GetLongField(env, this, messagePointerFieldId_g);
        if((msgPointer != 0) && (msgArray != NULL))
        {    
            /* If the name argument has been provided then convert it to the C equivalent. */
            const char *cName = NULL;
            ret = convertString(env, name, &cName);
            if(ret == MAMA_STATUS_OK)
            {    
                /* obtain the size the array */
                jsize size = (*env)->GetArrayLength(env, msgArray);

                /* Verify that the length parameter provided is within the bounds of the array. */
                ret = MAMA_STATUS_INVALID_ARG;
                if((length > 0) && (length <= size))
                {
                    /* The java array cannot be passed down to the C layer, the underlying message
                     * pointers must be extracted and put into a C array. Allocate this now.
                     */
                    mamaMsg *cArray = calloc(length, sizeof(mamaMsg));  
                    ret = MAMA_STATUS_NOMEM;
                    if(cArray != NULL)
                    {            
                        /* Iterate through the java array up to the supplied length and populate the new C
                         * array allocated above.
                         */
                        ret = populateCMessageArray(cArray, env, msgArray, length);
                        if(ret == MAMA_STATUS_OK)
                        {
                           /* Now the C array has been constructed it can be added to the original message. */
                           ret = mamaMsg_addVectorMsg(
                                CAST_JLONG_TO_POINTER(mamaMsg, msgPointer),
                                cName,
                                (mama_fid_t)fid,
                                cArray,
                                (mama_size_t)length);
                        }

                        /* Free the C array. */
                        free(cArray);
                    }
                }

                /* Free the C name. */
                if(cName != NULL)
                {
                    (*env)->ReleaseStringUTFChars(env, name, cName);    
                }
            }
        }

        /* If something went wrong then throw an exception. */
        if(ret != MAMA_STATUS_OK)
        {
            /* Convert the status value to a string. */
            const char *message = mamaStatus_stringForStatus(ret);

            /* Throw the exception. */
            utils_throwMamaException(env, message);
            return;
        }    
    }
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    addArrayString
 * Signature: (Ljava/lang/String;I[Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_addArrayString
  (JNIEnv* env, jobject this, jstring name, jint fid, jobjectArray stringArray, jint length)
{
    jstring         nextString;
    jlong           msgPointer   = 0;
    const char*     nextCstring  = 0;
    int             i            = 0;
    const char*     c_name       = NULL;
    const char**    cStringArray = NULL;
    assert(messagePointerFieldId_g!=NULL);
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }
   
    
    cStringArray = malloc (length * sizeof(char*));    
    for (i = 0; i < length; ++i)
    {
        /* obtain the next jstring from the array */
        nextString = (jstring)(*env)->GetObjectArrayElement(env, stringArray, i);
    
        /* get the C string */
        nextCstring = (*env)->GetStringUTFChars(env,nextString,0);
        if(!nextCstring)
        {
            if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
            free (cStringArray);
            return;/*Exception auto thrown*/
        }
  
        /* add it to the array */
        cStringArray[i] = nextCstring;
    }
   /* add the array to the message */
   mamaTry (env,
            mamaMsg_addVectorString(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                    c_name,
                                    (mama_fid_t)fid,
                                    cStringArray,
                                    (mama_size_t)length),   
            "mamaMsg_addVectorMsg()");
   /* clean up */
    for (i = 0; i < length; ++i)
    {
       nextString = (jstring)(*env)->GetObjectArrayElement(env, stringArray, i);
       (*env)->ReleaseStringUTFChars(env,nextString, cStringArray[i]);
    }
    free (cStringArray);
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}


/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateI18
 * Signature: (Ljava/lang/String;IB)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateI8
  (JNIEnv* env, jobject this, jstring name, jint fid, jbyte value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateI8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_updateI8() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI16
 * Signature: (Ljava/lang/String;IS)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateI16
  (JNIEnv* env, jobject this, jstring name, jint fid, jshort value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateI16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int16_t)value),
            "mamaMsg_updateI16() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateI32
 * Signature: (Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateI32
  (JNIEnv* env, jobject this, jstring name, jint fid, jint value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateI32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int32_t)value),
            "mamaMsg_updateI32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getI64
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateI64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,mamaMsg_updateI64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int64_t)value),
            "mamaMsg_updateI64() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateU8
 * Signature: (Ljava/lang/String;IS)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateU8
  (JNIEnv* env, jobject this, jstring name, jint fid, jshort value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateU8(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(uint8_t)value),
            "mamaMsg_updateU8() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateU16
 * Signature: (Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateU16
  (JNIEnv* env, jobject this, jstring name, jint fid, jint value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateU16(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(mama_u16_t)value),
            "mamaMsg_updateU16() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateU32
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateU32
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateU32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(uint32_t)value),
            "mamaMsg_updateU32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateF64
 * Signature: (Ljava/lang/String;IJ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateU64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlong value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,mamaMsg_updateU64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(uint64_t)value),
            "mamaMsg_updateU64() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateF32
 * Signature: (Ljava/lang/String;IF)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateF32
  (JNIEnv* env, jobject this, jstring name, jint fid, jfloat value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateF32(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                     c_name, (mama_fid_t)fid,(float)value),
            "mamaMsg_updateF32() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateF64
 * Signature: (Ljava/lang/String;IF)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateF64
  (JNIEnv* env, jobject this, jstring name, jint fid, jdouble value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateF64(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                     c_name, (mama_fid_t)fid,(double)value),
            "mamaMsg_updateF64() ");
            
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateString
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateString
  (JNIEnv* env, jobject this, jstring name, jint fid, jstring value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    const char* c_value         =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    c_value = (*env)->GetStringUTFChars(env,value,0);
    if(!c_value)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_updateString(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                     c_name, (mama_fid_t)fid,c_value),
            "mamaMsg_updateString() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    if(c_value)(*env)->ReleaseStringUTFChars(env,value, c_value);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateChar
 * Signature: (Ljava/lang/String;IC)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateChar
  (JNIEnv* env, jobject this, jstring name, jint fid, jchar value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
        "Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateChar(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_updateChar() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateBoolean
 * Signature: (Ljava/lang/String;IZ)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateBoolean
  (JNIEnv* env, jobject this, jstring name, jint fid, jboolean value)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
        "Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    mamaTry(env,
            mamaMsg_updateBool(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                  c_name, (mama_fid_t)fid,(int8_t)value),
            "mamaMsg_updateBool() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updatePrice
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaPrice;)C
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updatePrice
  (JNIEnv* env, jobject this, jstring name, jint fid, jobject price)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    jlong       pricePointer    =   0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

    pricePointer = (*env)->GetLongField(env, price,
                        pricePointerFieldId_g);
    if(!pricePointer)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_updatePrice(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                     c_name,
                                     (mama_fid_t)fid,
                                     (CAST_JLONG_TO_POINTER(mamaPrice,pricePointer))),
            "mamaMsg_updatePrice() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateDateTime
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaDateTime)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateDateTime
  (JNIEnv* env, jobject this, jstring name, jint fid, jobject dateTime)
{
    jlong       msgPointer      =   0;
    const char* c_name          =   NULL;
    jlong       dateTimePointer =   0;

    assert(messagePointerFieldId_g!=NULL);

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return;/*Exception thrown*/
    }

   dateTimePointer = (*env)->GetLongField(env, dateTime,
                        dateTimePointerFieldId_g);
    if(!dateTimePointer)
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return;/*Exception auto thrown*/
    }

    mamaTry(env,
            mamaMsg_updateDateTime(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                                   c_name,
                                  (mama_fid_t)fid,
                                  (CAST_JLONG_TO_POINTER(mamaDateTime,dateTimePointer))),
            "mamaMsg_updateDateTime() ");

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayI8
 * Signature: (Ljava/lang/String;I[B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayI8
(JNIEnv* env, jobject this, jstring name, jint fid, jbyteArray array)
{
   ExpandAddOrUpdateScalarVector (jbyte, Byte, I8, mama_i8_t, update);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayU8
 * Signature: (Ljava/lang/String;I[S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayU8
(JNIEnv* env, jobject this, jstring name, jint fid, jshortArray array)
{
   ExpandAddOrUpdateScalarVectorU (jshort, Short, U8, mama_u8_t, update);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayI16
 * Signature: (Ljava/lang/String;I[S)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayI16
  (JNIEnv* env, jobject this, jstring name, jint fid, jshortArray array)
{
   ExpandAddOrUpdateScalarVector (jshort, Short, I16, mama_i16_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayU16
 * Signature: (Ljava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayU16
  (JNIEnv* env, jobject this, jstring name, jint fid, jintArray array)
{
   ExpandAddOrUpdateScalarVectorU (jint, Int, U16, mama_u16_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayI32
 * Signature: (Ljava/lang/String;I[I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayI32
  (JNIEnv* env, jobject this, jstring name, jint fid, jintArray array)
{
   ExpandAddOrUpdateScalarVector (jint, Int, I32, mama_i32_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayU32
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayU32
  (JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
   ExpandAddOrUpdateScalarVectorU (jlong, Long, U32, mama_u32_t, update);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayI64
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayI64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
   ExpandAddOrUpdateScalarVector (jlong, Long, I64, mama_i64_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayU64
 * Signature: (Ljava/lang/String;I[J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayU64
  (JNIEnv* env, jobject this, jstring name, jint fid, jlongArray array)
{
    ExpandAddOrUpdateScalarVectorU (jlong, Long, U64, mama_u64_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayF32
 * Signature: (Ljava/lang/String;I[F)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayF32
  (JNIEnv* env, jobject this, jstring name, jint fid, jfloatArray array)
{
    ExpandAddOrUpdateScalarVector (jfloat, Float, F32, mama_f32_t, update);
}
                                                                                                         
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    updateArrayF64
 * Signature: (Ljava/lang/String;I[D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_updateArrayF64
  (JNIEnv* env, jobject this, jstring name, jint fid, jdoubleArray array)
{
    ExpandAddOrUpdateScalarVector (jdouble, Double, F64, mama_f64_t, update);
}

/* Closure for the C mamaMsg iterator */
typedef struct mamaMsgIteratorClosure
{
    JNIEnv*     mEnv;
    jobject     mJavaIterator;
    jobject     mJavaClosure;
    jobject     mJavaDictionary;
    jobject     mJavaMamaMsg;
} mamaMsgIteratorClosure;


/*
 * Class:     com_wombat_mama_MamaMsg_MmIterator
 * Method:    iteratorNext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgIterator_iteratorNext
  (JNIEnv * env , jobject this)
{
    jlong                   iteratorPointer     =   0;
    jobject                 javaMsgField    =   NULL;
    mamaMsgIterator            iteratorPointer_c   =   NULL;
    mamaMsgField            field;

    
    iteratorPointer = (*env)->GetLongField(env,this,iteratorPointerFieldId_g);
    iteratorPointer_c = CAST_JLONG_TO_POINTER(mamaMsgIterator,iteratorPointer);
    
    javaMsgField = (*env)->GetObjectField
                    (env,
                    this,
                    iteratorFieldPointerFieldId_g);
        
    field = mamaMsgIterator_next (iteratorPointer_c);
    
        /*Set the pointer val for this MamaMsgField*/
    (*env)->SetLongField(env, javaMsgField, messageFieldPointerFieldId_g,
                           CAST_POINTER_TO_JLONG(field)); 
                           
}

/*
 * Class:     com_wombat_mama_MamaMsg_MmIterator
 * Method:    iteratorHasNext
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsgIterator_iteratorHasNext
  (JNIEnv * env, jobject this)
{
    jlong                   iteratorPointer     =   0;
    mamaMsgIterator            iteratorPointer_c   =   NULL;
    int ret;
    
    iteratorPointer = (*env)->GetLongField(env,this,iteratorPointerFieldId_g);
    iteratorPointer_c = CAST_JLONG_TO_POINTER(mamaMsgIterator,iteratorPointer);
    
    ret = mamaMsgIterator_hasNext (iteratorPointer_c);

    return (ret==0?JNI_FALSE:JNI_TRUE);
} 
  
/*
 * Class:     com_wombat_mama_MamaMsg_MmIterator
 * Method:    createIterator
 * Signature: (Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaDictionary;)V
*/
  
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgIterator_iteratorCreate
  (JNIEnv * env, jobject this, jobject msg, jobject dictionary)
{
    jlong                   msgPointer      =   0;
    jlong                   dictPointer     =   0;
    mamaMsgIterator            localiterator =     NULL;
    jobject                 javaMsgField    =   NULL;
    mamaDictionary          dictPointer_c   =   NULL;
    mamaMsgField            field;
    
    /*Get the msg pointer from the Java object*/
    msgPointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    
    if(NULL!=dictionary)
    {
        dictPointer = (*env)->GetLongField(env,dictionary,
                                               dictionaryPointerFieldId_g);
        dictPointer_c = CAST_JLONG_TO_POINTER(mamaDictionary,dictPointer);
    }
       
    javaMsgField = (*env)->GetObjectField
                        (env,
                        this,
                        iteratorFieldPointerFieldId_g);
                        
    assert(NULL!=javaMsgField);
       

    mamaMsgIterator_create (&localiterator, dictPointer_c);

    mamaMsgIterator_associate(localiterator, CAST_JLONG_TO_POINTER(mamaMsg,msgPointer));

    field = mamaMsgIterator_begin (localiterator);

    /*Set the pointer val for this iterator*/
    (*env)->SetLongField(env, this, iteratorPointerFieldId_g,
                           CAST_POINTER_TO_JLONG (localiterator)); 
                           
    /*Set the pointer val for this MamaMsgField*/
    (*env)->SetLongField(env, javaMsgField, messageFieldPointerFieldId_g,
                           CAST_POINTER_TO_JLONG(field)); 
    
}

/*
 * Class:     com_wombat_mama_MamaMsg_MmIterator
 * Method:    iteratorDestroy
 * Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgIterator_iteratorDestroy
    (JNIEnv * env, jobject this)
{
    mama_status status    =   MAMA_STATUS_OK;
    jlong       iteratorPointer = (*env)->GetLongField(env,this,iteratorPointerFieldId_g);
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(iteratorPointer,
        "Null parameter, MamaMsgIterator may have already been destroyed.");

    if (MAMA_STATUS_OK!=(status=
        mamaMsgIterator_destroy (CAST_JLONG_TO_POINTER(mamaMsgIterator, iteratorPointer))))
    {
         utils_buildErrorStringForStatus(
                 errorString,
                 UTILS_MAX_ERROR_STRING_LENGTH,
                 "Error from mamaMsgIterator_destroy()",
                 status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }   

    (*env)->SetLongField(env, this, iteratorPointerFieldId_g, 0);
}

/*
 * Class:     com_wombat_mama_MamaMsg_MmIterator
 * Method:    associateIterator
 * Signature: (Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaDictionary;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsgIterator_iteratorAssociate
  (JNIEnv * env, jobject this, jobject msg, jobject dictionary)
{
    jlong                   msgPointer      =   0;
    jlong                   dictPointer     =   0;
    jlong                   iteratorPointer     =   0;
        jobject                 javaMsgField    =   NULL;
    mamaDictionary          dictPointer_c   =   NULL;
    mamaMsgIterator            iteratorPointer_c   =   NULL;
    mamaMsgField            field;
    
    /*Get the msg pointer from the Java object*/
    msgPointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(NULL!=dictionary)
    {
        dictPointer = (*env)->GetLongField(env,dictionary,
                                               dictionaryPointerFieldId_g);
        dictPointer_c = CAST_JLONG_TO_POINTER(mamaDictionary,dictPointer);
    }
    
    iteratorPointer = (*env)->GetLongField(env,this,iteratorPointerFieldId_g);
    iteratorPointer_c = CAST_JLONG_TO_POINTER(mamaMsgIterator,iteratorPointer);
    
    
    mamaMsgIterator_associate(iteratorPointer_c, 
                                CAST_JLONG_TO_POINTER(mamaMsg,msgPointer));
    
    mamaMsgIterator_setDict(iteratorPointer_c, dictPointer_c);
    
    field = mamaMsgIterator_begin (iteratorPointer_c);
         
    javaMsgField = (*env)->GetObjectField
                        (env,
                        this,
                        iteratorFieldPointerFieldId_g);
                        
    /*Set the pointer val for this MamaMsgField*/
    (*env)->SetLongField(env, javaMsgField, messageFieldPointerFieldId_g,
                           CAST_POINTER_TO_JLONG(field)); 
                           
}

/*Callback function for iterateFields() */
static void MAMACALLTYPE msgIteratorCb (
            const mamaMsg       msg,
            const mamaMsgField  field,
            void*               closure)
{
    jobject                 javaMsgField    =   NULL;
    mamaMsgIteratorClosure* cClosure        =   (mamaMsgIteratorClosure*)
                                                closure;
    assert(cClosure!=NULL);

    /*Get the reuseable MamaMsgField from the MamaMsg*/
    javaMsgField = (*cClosure->mEnv)->GetObjectField(
                        cClosure->mEnv,
                        cClosure->mJavaMamaMsg,
                        msgFieldObjectFieldId_g);
    assert(NULL!=javaMsgField);

    /*Set the pointer val for this MamaMsgField*/
    (*cClosure->mEnv)->SetLongField(
                           cClosure->mEnv,
                           javaMsgField,
                           messageFieldPointerFieldId_g,
                           CAST_POINTER_TO_JLONG(field));

    /*Invoke the Java onField callback*/
    (*cClosure->mEnv)->CallVoidMethod(
                           cClosure->mEnv,
                           cClosure->mJavaIterator,
                           iteratorCbOnFieldId_g,
                           cClosure->mJavaMamaMsg,
                           javaMsgField,
                           cClosure->mJavaDictionary,
                           cClosure->mJavaClosure);
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    _iterateFields
 * Signature: (Lcom/wombat/mama/MamaMsgFieldIterator;Lcom/wombat/mama/MamaDictionary;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1iterateFields
  (JNIEnv* env, jobject this, jobject iterator,
   jobject dictionary, jobject closure)
{
    mama_status             status          =   MAMA_STATUS_OK;
    jlong                   msgPointer      =   0;
    jlong                   dictPointer     =   0;
    mamaDictionary          dictPointer_c   =   NULL;
    mamaMsgIteratorClosure  cClosure;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    assert(messagePointerFieldId_g!=NULL);

    cClosure.mJavaIterator     = iterator;
    cClosure.mJavaClosure      = closure;
    cClosure.mJavaDictionary   = dictionary;
    cClosure.mJavaMamaMsg      = this;
    cClosure.mEnv              = env;

    /*Get the msg pointer from the Java object*/
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(NULL!=dictionary)
    {
        dictPointer = (*env)->GetLongField(env,dictionary,
                                               dictionaryPointerFieldId_g);
        dictPointer_c = CAST_JLONG_TO_POINTER(mamaDictionary,dictPointer);
    }

    /*We simply delegate to the mama iterate fields implementation*/
    if(MAMA_STATUS_OK!=(status=mamaMsg_iterateFields(
                              CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                              (mamaMsgIteratorCb)msgIteratorCb,
                              dictPointer_c,
                              &cClosure)))
    {
         utils_buildErrorStringForStatus(
                 errorString,
                 UTILS_MAX_ERROR_STRING_LENGTH,
                 "Error from mamaMsg_iterateFields()",
                 status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getFieldAsString
 * Signature: (ILcom/wombat/mama/MamaDictionary;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsg_getFieldAsString
  (JNIEnv* env, jobject this, jint fid, jobject dictionary)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgPointer      =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    char retVal_c[128];
    
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);


    if(MAMA_STATUS_OK!=(status=mamaMsg_getFieldAsString(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    NULL,(mama_fid_t)fid,
                    retVal_c, 128)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not get as String for mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, retVal_c);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    isFromInbox
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_isFromInbox
  (JNIEnv* env, jobject this)
{
    mama_status status      =   MAMA_STATUS_OK;
    jlong       msgPointer  =   0;
    int         cResult     =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", 0);


    cResult = mamaMsg_isFromInbox(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer));

    return cResult==0?JNI_FALSE:JNI_TRUE;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    toString
 * Signature: ()java/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaMsg_toString
  (JNIEnv* env, jobject this)
{
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           msgPointer      =   0;
    const char*     retVal_c        =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", NULL);


    retVal_c  = mamaMsg_toString(CAST_JLONG_TO_POINTER(mamaMsg,msgPointer));

    if(NULL==retVal_c)
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not call toString on mamaMsg.",
                status);
        utils_throwWombatException(env,errorString);
        return NULL;
    }

    return (*env)->NewStringUTF(env, retVal_c);
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    getPayloadType
 * Signature: ()C
 */
JNIEXPORT jchar JNICALL Java_com_wombat_mama_MamaMsg_getPayloadType
  (JNIEnv* env, jobject this)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    char        cResult         =   MAMA_PAYLOAD_UNKNOWN;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,
        "Null parameter, MamaMsg may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaMsg_getPayloadType(
                        CAST_JLONG_TO_POINTER(mamaMsg, msgPointer), &cResult)));
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "MamaMsg.getPayloadType() failed.",
                status);
        utils_throwWombatException(env,errorString);
    }

    return cResult;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    clear
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_clear
  (JNIEnv* env, jobject this)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    
    if(MAMA_STATUS_OK!=(status=mamaMsg_clear(
                            CAST_JLONG_TO_POINTER(mamaMsg,msgPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not clear mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    copy
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_copy
  (JNIEnv* env, jobject this, jobject rhs)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    jlong       rhsMsgPointer   =   0;
    mamaMsg     c_result        =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");


    rhsMsgPointer = (*env)->GetLongField(env,rhs,messagePointerFieldId_g);
    assert(0!=rhsMsgPointer);

    c_result = CAST_JLONG_TO_POINTER(mamaMsg,msgPointer);

    if(MAMA_STATUS_OK!=(status=mamaMsg_copy(
                            CAST_JLONG_TO_POINTER(mamaMsg,rhsMsgPointer),
                            &c_result)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not copy mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    (*env)->SetLongField(env, this, messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(c_result));
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    apply
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_apply
  (JNIEnv* env, jobject this, jobject rhs)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    jlong       rhsMsgPointer   =   0;
    mamaMsg     c_result        =   NULL;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");


    rhsMsgPointer = (*env)->GetLongField(env,rhs,messagePointerFieldId_g);
    assert(0!=rhsMsgPointer);

    if(MAMA_STATUS_OK!=(status=mamaMsg_applyMsg(
                            CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                            CAST_JLONG_TO_POINTER(mamaMsg,rhsMsgPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not copy mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    detach
 * Signature: ()Lcom/wombat/mama/MamaMsg;
 */
JNIEXPORT jobject JNICALL Java_com_wombat_mama_MamaMsg_detach
  (JNIEnv *env , jobject this)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    mamaMsg     c_result        =   NULL;
    jobject     result          =   NULL;
    jlong       newMsgPointer      =   0;
     char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    jclass          msgClass   =
        (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    if (!msgClass) return NULL;
    
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    assert(0!=msgPointer);
    
    
    if(MAMA_STATUS_OK!=(status=mamaMsg_detach(
                            CAST_JLONG_TO_POINTER(mamaMsg,msgPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not clear mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
    

        /*If we get this far we have a sub message*/
        result = (*env)->NewObject(env, msgClass,
                     messageConstructorId_g, JNI_FALSE);
        (*env)->SetLongField(env, result, messagePointerFieldId_g,
                             msgPointer);

    (*env)->DeleteLocalRef(env, msgClass);
    return result;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    _destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1destroy
  (JNIEnv* env, jobject this)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       msgPointer      =   0;
    long        jMsgArrayPointer=   0;
    jobject*    jMsgArray       =   NULL;
    jint        jMsgArraySize   =   0;
    jint        i               =   0;
    jobject     jMsg            =   NULL;
    
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    

    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");


    if(MAMA_STATUS_OK!=(status=mamaMsg_destroy(
                            CAST_JLONG_TO_POINTER(mamaMsg,msgPointer))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not destroy mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    (*env)->SetLongField(env, this,
         messagePointerFieldId_g,
         0);
    
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
	
	/* Free the byte buffer if it is held. */
	if(NULL != g_byteBuffer)
	{
		free(g_byteBuffer);
		g_byteBuffer = NULL;
	}
    
    return;
}

/*
 * Class:      com_wombat_mama_MamaMsg
 * Method:     create
 * Signature:  ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_create
  (JNIEnv* env, jobject this)
{
    mamaMsg     cMessage    =   NULL;
    mama_status status      =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if(MAMA_STATUS_OK!=(status=mamaMsg_create(&cMessage)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create mamaMsg.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    /*If we get this far we have a valid message pointer to set*/
    (*env)->SetLongField(env, this,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG (cMessage));
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    createForPayload
 * Signature: (C)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_createForPayload
  (JNIEnv* env, jobject this, jchar payloadId)
{
    mamaMsg     cMessage    =   NULL;
    mama_status status      =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if(MAMA_STATUS_OK!=(status=mamaMsg_createForPayload(&cMessage, payloadId)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create MamaMsg for payload.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    /*If we get this far we have a valid message pointer to set*/
    (*env)->SetLongField(env, this,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG (cMessage));
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    createForPayloadBridge
 * Signature: (Lcom/wombat/mama/MamaPayloadBridge;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_createForPayloadBridge
  (JNIEnv* env, jobject this, jobject payloadBridge)
{
    mamaMsg     cMessage                =   NULL;
    mama_status status                  =   MAMA_STATUS_OK;
    jlong       payloadBridgePointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    payloadBridgePointer = (*env)->GetLongField (env, 
                                                 payloadBridge, 
                                                 payloadBridgePointer);
    assert (0!=payloadBridgePointer);

    if(MAMA_STATUS_OK!=(status=mamaMsg_createForPayloadBridge(
                    &cMessage, payloadBridgePointer)))
    {
        utils_buildErrorStringForStatus(    
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create MamaMsg for payload bridge.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }

    /*If we get this far we have a valid message pointer to set*/
    (*env)->SetLongField(env, this, 
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG (cMessage));
    return;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    _getField
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaDictionary;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg__1getField
(JNIEnv * env, jobject this, jstring name, 
 jint fieldId, jobject dictionary)
{
    const  char*    c_name          = NULL;
    jlong           msgPointer      = 0;
    mamaMsgField    c_result        = NULL;
    jobject         result          = NULL;
    mama_status     status          = MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    assert(messagePointerFieldId_g!=NULL);
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.");

    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        assert (NULL != c_name);/*Exception thrown*/
    }
    status=mamaMsg_getField(
        CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
        c_name, (mama_fid_t)fieldId,
        &c_result);
    /*resue the existing field object */
    if (c_result)
    {
        result = (*env)->GetObjectField(env,this,
                                        msgFieldObjectFieldId_g);
        assert(NULL!=result);/*throws an exception*/
        (*env)->SetLongField(env, result, messageFieldPointerFieldId_g,
                             CAST_POINTER_TO_JLONG(c_result));
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryBoolean
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaBoolean;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryBoolean
  (JNIEnv *env , jobject this, jstring name, jint fid, jobject result)
{   
    mama_status     status          = MAMA_STATUS_OK;
    mama_bool_t     cResult         = 0;
    TryFieldWithType (Bool, mama_bool_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetBooleanField(env, result, jBooleanValue_g,
                                cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
        
}
  

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryChar
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaChar;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryChar
    (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_u8_t       cResult         = 0;
    TryFieldWithType (Bool, mama_u8_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetCharField(env, result, jMamaCharValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;

}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI8
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaByte;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryI8
   (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_i8_t       cResult         =   NULL;
    TryFieldWithType (I8, mama_i8_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetByteField(env, result, jMamaByteValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryU8
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaShort;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryU8
   (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_u8_t       cResult         = 0;
    TryFieldWithType (U8, mama_u8_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetShortField(env, result, jMamaShortValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI16
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaShort;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryI16
   (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_i16_t       cResult         =   NULL;
    TryFieldWithType (I16, mama_i16_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetShortField(env, result, jMamaShortValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryU16
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaInteger;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryU16
   (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_u16_t       cResult        = 0;
    TryFieldWithType (U16, mama_u16_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetIntField(env, result, jMamaIntValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI32
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaInteger;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryI32
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_i32_t       cResult         =   NULL;
    TryFieldWithType (I32, mama_i32_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetIntField(env, result, jMamaIntValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryU32
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryU32
   (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_u32_t       cResult         =   NULL;
    TryFieldWithType (U32, mama_u32_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetLongField(env, result, jMamaLongValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryI64
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_i64_t       cResult         =   NULL;
    TryFieldWithType (I64, mama_i64_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetLongField(env, result, jMamaLongValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryU64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryU64
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_u64_t       cResult         =   NULL;
    TryFieldWithType (U64, mama_u64_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetLongField(env, result, jMamaLongValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
  
  /*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryF32
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_f32_t       cResult        =   0;
    TryFieldWithType (F32, mama_f32_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetFloatField(env, result, jMamaFloatValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}
/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryU64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryF64
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    mama_f64_t       cResult         =   0;
    TryFieldWithType (F64, mama_f64_t);
    
    if  (status == MAMA_STATUS_OK)
    {
        (*env)->SetDoubleField(env, result, jMamaDoubleValue_g,
                             cResult);
    
        return JNI_TRUE;
    }
    
    return JNI_FALSE;
}

  /*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryDateTime
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           dateTimePointer     =   0;
    jlong              msgPointer      =   0;       
    const char*        c_name          =   NULL;                            
  
    /* Get the pointer to the underlying message*/                          
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", JNI_FALSE);    
    dateTimePointer   = (*env)->GetLongField(env,result,dateTimePointerFieldId_g);
    assert(0!=dateTimePointer);
       
       /* Get the C string for the name if specified*/                         
    if(name)                                                                
    {                                                                       
        c_name = (*env)->GetStringUTFChars(env,name,0);                     
        if(!c_name) return NULL;                                            
    }    
    
    if(MAMA_STATUS_OK==(mamaTryIgnoreNotFound(env, mamaMsg_getDateTime(
                        CAST_JLONG_TO_POINTER(mamaMsg,msgPointer), c_name, fid,
                        CAST_JLONG_TO_POINTER(mamaDateTime,dateTimePointer)), "Could not get DateYime from mamaMsg.")))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return JNI_TRUE;
    }
    
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    
    return JNI_FALSE;
}


 /*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryPrice
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           pricePointer     =   0;
    jlong              msgPointer      =   0;         
     const char*        c_name          =   NULL;
      /* Get the C string for the name if specified*/                         
    if(name)                                                                
    {                                                                       
        c_name = (*env)->GetStringUTFChars(env,name,0);                     
        if(!c_name) return NULL;                                            
    }  
    
    /* Get the pointer to the underlying message*/                          
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", JNI_FALSE);    
    pricePointer   = (*env)->GetLongField(env,result,pricePointerFieldId_g);
       
    if(MAMA_STATUS_OK==(mamaTryIgnoreNotFound(env, mamaMsg_getPrice(
                        CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),c_name, fid,
                        CAST_JLONG_TO_POINTER(mamaPrice,pricePointer)), "Could not get Price from mamaMsg.")))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return JNI_TRUE;
    }
    
    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    return JNI_FALSE;
}

/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryString
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaBoolean;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryString
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    mama_status     status          = MAMA_STATUS_OK;
    jlong           stringPointer     =   0;
    jlong              msgPointer      =   0;         
     const char*        c_name          =   NULL;
     const char*     retVal_c        =   NULL;
     jstring      resultstr = NULL;  
      /* Get the C string for the name if specified*/                         
    if(name)                                                                
    {                                                                       
        c_name = (*env)->GetStringUTFChars(env,name,0);                     
        if(!c_name) return NULL;                                            
    }  

    /* Get the pointer to the underlying message*/                          
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);    
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", JNI_FALSE);    stringPointer   = (*env)->GetLongField(env,result,jMamaStringValue_g);
    
    if(MAMA_STATUS_OK==(mamaTryIgnoreNotFound(env, mamaMsg_getString(
                        CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),c_name, fid,
                        &retVal_c), "Could not get string from mamaMsg.")))                       
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        
        resultstr =  (*env)->NewStringUTF(env, retVal_c);
        
        (*env)->SetObjectField(env, result, jMamaStringValue_g,
                     resultstr);
                             
        return JNI_TRUE;
    }
    
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
    return JNI_FALSE;
}

 /*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    tryI64
 * Signature: (Ljava/lang/String;ILcom/wombat/mama/MamaLong;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaMsg_tryArrayMsg
  (JNIEnv * env, jobject this, jstring name, jint fid, jobject result)
{
    const char*     c_name          =   NULL;
    jlong           msgPointer      =   0;
    mama_status     status          =   MAMA_STATUS_OK;
    jlong           jMsgArrayPointer=   0;
    jint            jMsgArraySize   =   0;
    
    msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(msgPointer,  
		"Null parameter, MamaMsg may have already been destroyed.", JNI_FALSE);

    jMsgArrayPointer = (*env)->GetLongField(env,result,jMsgArrayFieldId_g);
    jMsgArraySize = (*env)->GetIntField(env,result,jMsgArraySizeFieldId_g);
    
    if(name)
    {
        c_name = (*env)->GetStringUTFChars(env,name,0);
        if(!c_name)return NULL;/*Exception thrown*/
    }

    /*Get the vector of messages from the C mesage*/
     if(MAMA_STATUS_OK==(mamaTryIgnoreNotFound(env, mamaMsg_getVectorMsg(
                    CAST_JLONG_TO_POINTER(mamaMsg,msgPointer),
                    c_name, (mama_fid_t)fid,
                    &jMsgArrayPointer,
                    &jMsgArraySize), "Could not get vectormsg from mamaMsg.")))
    {
        if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);
        return JNI_TRUE;
    }

    if(c_name)(*env)->ReleaseStringUTFChars(env,name, c_name);

    return  JNI_FALSE;
}


/*
 * Class:     com_wombat_mama_MamaMsg
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaMsg_initIDs
  (JNIEnv* env, jclass class)
{
    jclass  fieldIteratorCallbackClass  =   NULL;
    jclass  priceClass                  =   NULL;
    jclass  dateTimeClass               =   NULL;
    jclass  booleanClass               =   NULL;
    jclass  mamaLongClass               =   NULL;
    jclass  mamaShortClass               =   NULL;
	jclass  mamaByteClass               =   NULL;
    jclass  mamaCharClass               =   NULL;
    jclass  mamaDoubleClass               =   NULL;
    jclass  mamaFloatClass               =   NULL;
    jclass  mamaIntClass               =   NULL;
    jclass  mamaStrClass               =   NULL;
    
    jclass mamaMsgIteratorClass=NULL;
    
  
    mamaMsgIteratorClass = (*env)->FindClass(env,"com/wombat/mama/MamaMsgIterator");
    if(!mamaMsgIteratorClass) return;/* Exception auto thrown*/
    
    iteratorPointerFieldId_g = (*env)->GetFieldID(env,
                       mamaMsgIteratorClass, "iteratorPointer_i",
                       UTILS_JAVA_POINTER_TYPE_SIGNATURE);
                        
    iteratorFieldPointerFieldId_g = (*env)->GetFieldID(env,
                       mamaMsgIteratorClass, "myMsgField_i",
                       "Lcom/wombat/mama/MamaMsgField;");
                       
    if(!iteratorPointerFieldId_g)
    {
        (*env)->DeleteLocalRef(env,mamaMsgIteratorClass);
        return;
    }
                    
                        
                        
    fieldIteratorCallbackClass = (*env)->FindClass(env,"com/wombat/mama/MamaMsgFieldIterator");
    if(!fieldIteratorCallbackClass) return;/* Exception auto thrown*/

    iteratorCbOnFieldId_g = (*env)->GetMethodID(env,fieldIteratorCallbackClass,
            "onField","(Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaMsgField;"
                        "Lcom/wombat/mama/MamaDictionary;Ljava/lang/Object;)V");

    if(!iteratorCbOnFieldId_g)
    {
        (*env)->DeleteLocalRef(env,fieldIteratorCallbackClass);
        return;
    }

    messagePointerFieldId_g = (*env)->GetFieldID(env,
                       class, "msgPointer_i",
                       UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    if (!messagePointerFieldId_g) return;/*Exception auto thrown*/

    
    /*The MamaMsgField object in the message for iterateFields.*/
    msgFieldObjectFieldId_g = (*env)->GetFieldID(env,
                              class,"msgField_i",
                              "Lcom/wombat/mama/MamaMsgField;");
    if(!msgFieldObjectFieldId_g) return;

    messageConstructorId_g = (*env)->GetMethodID(env, class,
                             "<init>", "(Z)V" );

    msgPricePointerFieldId_g = (*env)->GetFieldID(env,
                             class,"pricePointer_i",
                             UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    
    msgDateTimePointerFieldId_g = (*env)->GetFieldID(env,
                             class,"dateTimePointer_i",
                             UTILS_JAVA_POINTER_TYPE_SIGNATURE);                                                  
                                                      
    priceClass = (*env)->FindClass(env,"com/wombat/mama/MamaPrice");
    if(!priceClass) return;/* Exception auto thrown */

    pricePointerFieldId_g = (*env)->GetFieldID(env,
                             priceClass,"pricePointer_i",
                              UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    dateTimeClass = (*env)->FindClass(env,"com/wombat/mama/MamaDateTime");
    if(!dateTimeClass) return;/* Exception auto thrown */

    dateTimePointerFieldId_g = (*env)->GetFieldID(env,
                             dateTimeClass,"dateTimePointer_i",
                              UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    jMsgArrayFieldId_g  = (*env)->GetFieldID(env,
                        class,"jMsgArray_i",
                        UTILS_JAVA_POINTER_TYPE_SIGNATURE);
 
    jMsgArraySizeFieldId_g = (*env)->GetFieldID(env,
                               class,"jMsgArraySize_i",
                               "I");                             
                              
    booleanClass = (*env)->FindClass(env,"com/wombat/mama/MamaBoolean");
    if(!booleanClass) return;/* Exception auto thrown */
    jBooleanValue_g = (*env)->GetFieldID(env,
                               booleanClass,"myValue",
                               "Z"); 
    
    mamaLongClass = (*env)->FindClass(env,"com/wombat/mama/MamaLong");
    if(!mamaLongClass) return;/* Exception auto thrown */   
    jMamaLongValue_g = (*env)->GetFieldID(env,
                               mamaLongClass,"myValue",
                               "J"); 
                               
    mamaShortClass = (*env)->FindClass(env,"com/wombat/mama/MamaShort");
    if(!mamaShortClass) return;/* Exception auto thrown */   
    jMamaShortValue_g = (*env)->GetFieldID(env,
                               mamaShortClass,"myValue",
                               "S"); 
    
	mamaByteClass = (*env)->FindClass(env,"com/wombat/mama/MamaByte");
    if(!mamaByteClass) return;/* Exception auto thrown */   
    jMamaByteValue_g = (*env)->GetFieldID(env,
                               mamaByteClass,"myValue",
                               "B");
							   
        mamaIntClass = (*env)->FindClass(env,"com/wombat/mama/MamaInteger");
    if(!mamaIntClass) return;/* Exception auto thrown */   
    jMamaIntValue_g = (*env)->GetFieldID(env,
                               mamaIntClass,"myValue",
                               "I"); 
                               
        mamaCharClass = (*env)->FindClass(env,"com/wombat/mama/MamaChar");
    if(!mamaCharClass) return;/* Exception auto thrown */   
    jMamaCharValue_g = (*env)->GetFieldID(env,
                               mamaCharClass,"myValue",
                               "C"); 
    
        mamaDoubleClass = (*env)->FindClass(env,"com/wombat/mama/MamaDouble");
    if(!mamaDoubleClass) return;/* Exception auto thrown */   
    jMamaDoubleValue_g = (*env)->GetFieldID(env,
                               mamaDoubleClass,"myValue",
                               "D"); 
    
        mamaFloatClass = (*env)->FindClass(env,"com/wombat/mama/MamaFloat");
    if(!mamaFloatClass) return;/* Exception auto thrown */   
    jMamaFloatValue_g = (*env)->GetFieldID(env,
                               mamaFloatClass,"myValue",
                               "F");
    
        mamaStrClass = (*env)->FindClass(env,"com/wombat/mama/MamaString");
    if(!mamaStrClass) return;/* Exception auto thrown */   
    jMamaStringValue_g = (*env)->GetFieldID(env,
                               mamaStrClass,"myValue",
                               "Ljava/lang/String;");
                               
    /*Tidy up any outstanding resources*/
    (*env)->DeleteLocalRef(env,fieldIteratorCallbackClass);
    return;
}

mama_status mamaTry(JNIEnv*     env,
                    mama_status status,
                    const char* msgOnFail)
{
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if(status!=MAMA_STATUS_OK)
    {
         utils_buildErrorStringForStatus(
                 errorString,
                 UTILS_MAX_ERROR_STRING_LENGTH,
                 msgOnFail,
                 status);
        utils_throwExceptionForMamaStatus(env,
                                          status,
                                          errorString);
        return status;
    }

    return MAMA_STATUS_OK;
}

mama_status mamaTryIgnoreNotFound(JNIEnv*     env,
                    mama_status status,
                    const char* msgOnFail)
{
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    if (( status == MAMA_STATUS_NOT_FOUND )  ||
        ( status == MAMA_STATUS_NOT_IMPLEMENTED ))
    {
        return status;
    }

    if(status!=MAMA_STATUS_OK)
    {
         utils_buildErrorStringForStatus(
                 errorString,
                 UTILS_MAX_ERROR_STRING_LENGTH,
                 msgOnFail,
                 status);
        utils_throwExceptionForMamaStatus(env,
                                          status,
                                          errorString);
        return status;
    }

    return MAMA_STATUS_OK;
}

/*  Description :   This function will convert a java string to a C equivalent, free the
 *                  returned string using ReleaseStringUTFChars.
 *  Arguments   :   env [I] The environment pointer.
 *                  in  [I] The java string to convert, if NULL is passed then this function
 *                          will do nothing but will still return MAMA_STATUS_OK.
 *                  out [O] To return the C string.
 *  Returns     :   MAMA_STATUS_PLATFORM
 *                  MAMA_STATUS_OK
 */
mama_status convertString(JNIEnv *env, jstring in, const char **out)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;
    const char *localOut = NULL;

    /* Only continue if the supplied string is valid. */
    if(in != NULL)
    {
        /* Perform the conversion. */
        ret = MAMA_STATUS_PLATFORM;
        localOut = (*env)->GetStringUTFChars(env, in, 0);
        if(localOut != NULL)
        {
            ret = MAMA_STATUS_OK;
        }
    }

    *out = localOut;
    return ret;
}

/*  Description :   This function will populate a C array of mamaMsg objects by extracting the
 *                  native pointers from an array of java MamaMsg objects.
 *  Arguments   :   cArray      [I] The C array to populate.
 *                  env         [I] The environment pointer.
 *                  javaArray   [I] The java array to take the messages from.
 *                  length      [I] Number of messages to take from the java array, if 0 is passed
 *                                  this function will do nothing but will still return MAMA_STATUS_OK.
 *  Returns     :   MAMA_STATUS_PLATFORM
 *                  MAMA_STATUS_OK
 */
mama_status populateCMessageArray(mamaMsg *cArray, JNIEnv *env, jobjectArray javaArray, jint length)
{
    /* Returns. */
    mama_status ret = MAMA_STATUS_OK;
    
    /* Iterate through the java array up to the supplied length and populate the new C
     * array allocated above.
     */
    jint messageIndex = 0;
    while((ret == MAMA_STATUS_OK) && (messageIndex < length))
    {
        /* Obtain the next message from the java array. */        
        jobject nextMsg = (*env)->GetObjectArrayElement(env, javaArray, messageIndex);
        ret = MAMA_STATUS_PLATFORM;
        if(nextMsg != NULL)
        {
            /* Get the underlying C structure. */
            jlong nextMsgPointer = (*env)->GetLongField(env, nextMsg, messagePointerFieldId_g);
            if(nextMsgPointer != 0)
            {
                /* Cast this value to a pointer and save it in the array at the correct
                 * index.
                 */
                cArray[messageIndex] = CAST_JLONG_TO_POINTER(mamaMsg, nextMsgPointer);
                ret = MAMA_STATUS_OK;
            }
        }

        /* Increment the count for the next iteration. */
        messageIndex ++;
    }

    return ret;
}

JNIEXPORT jint JNICALL Java_com_wombat_mama_MamaMsg_nativeGetAsBuffer(JNIEnv *env, jobject this, jstring name, jint fid, jbyteArray byteArray, jint arraySize)
{
    /* Returns. */
    jint ret = 0;

    /* Obtain the pointer to the managed object. */
    jlong msgPointer = (*env)->GetLongField(env,this,messagePointerFieldId_g);
    if(0 == msgPointer)
    {
        utils_throwMamaException(env, "Null parameter, MamaMsg may have already been destroyed.");
    }

    else
    {
        /* Status return. */
        mama_status status = MAMA_STATUS_OK;

        /* Convert the field name from unicode if it has been provided. */
        const char *convertedName = NULL;
        if(NULL != name)
        {            
            convertedName = (*env)->GetStringUTFChars(env, name, 0);            
        }
        {
            /* Get the string from the message. */
            const char *messageString = NULL;
            status = mamaMsg_getString(CAST_JLONG_TO_POINTER(mamaMsg, msgPointer), convertedName, (mama_fid_t)fid, &messageString);
            if(MAMA_STATUS_OK == status)
            {
                /* The return value is the length of the string. */
                ret = strlen(messageString);

                /* Only continue if the return array is big enough. */
                if(ret <= arraySize)
                {
                    /* Write the characters into the managed environment */
	                (*env)->SetByteArrayRegion(env, byteArray, 0, ret, messageString);
                }

                /* Otherwise we must return the required length as a negative value. */
                else
                {
                    ret *= -1;
                }
            }
        }

        /* Release the string name if one was allocated. */
        if(NULL != convertedName)
        {
            (*env)->ReleaseStringUTFChars(env, name, convertedName);
        }

        /* If something went wrong with the native function then throw an exception. */
	    mamaTry(env, status, "MamaMsg_nativeGetAsBuffer()");
    }

    return ret;
}
