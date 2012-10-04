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
* Utility functions for the jni implementaion.
*/
#include <jni.h>
#include <mama/mama.h>
#include <assert.h>

#ifndef JNI_UTILS_H__
#define JNI_GUTILS_H__

#define UTILS_MAX_ERROR_STRING_LENGTH 128

/*Constants for the MamaFieldDescriptor Java class*/
#define UTILS_FIELD_DESCRIPTOR_CLASS_NAME "com/wombat/mama/MamaFieldDescriptor"
#define UTILS_FIELD_DESCRIPTOR_CONSTRUCTOR_PROTOTYPE "(ISLjava/lang/String;Ljava/lang/Object;)V"

/*Constants for the MamaTransportListener Java class*/
#define UTILS_TRANSPORT_LISTENER_CLASS_NAME "com/wombat/mama/MamaTransportListener"
#define UTILS_TL_ON_RECONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_DISCONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_QUALITY_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_ACCEPT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_ACCEPT_RECONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_PUBLISHER_DISCONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_CONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_NAMING_SERVICE_CONNECT_PROTOTYPE "(SLjava/lang/Object;)V"
#define UTILS_TL_ON_NAMING_SERVICE_DISCONNECT_PROTOTYPE "(SLjava/lang/Object;)V"

/*Constants for the MamaTransportTopicListener Java class*/
#define UTILS_TRANSPORT_TOPIC_LISTENER_CLASS_NAME "com/wombat/mama/MamaTransportTopicListener"
#define UTILS_TTL_ON_TOPIC_SUBSCRIBE "(SLjava/lang/Object;)V"
#define UTILS_TTL_ON_TOPIC_UNSUBSCRIBE "(SLjava/lang/Object;)V"

/*Constants for the Colleciton interface in Java*/
#define UTILS_ARRAYLIST_CLASS_NAME "java/util/ArrayList"
#define UTILS_ARRAYLIST_ADD_PROTOTYPE "(Ljava/lang/Object;)Z"
#define UTILS_ARRAYLIST_CONSTRUCTOR_PROTOTYPE "()V"

/*Constants for the MamaDictionary Java class*/
#define UTILS_DICTIONARY_CONSTRUCTOR_PROTOTYPE "()V"
#define UTILS_DICTIONARY_POINTER_FIELD "dictionaryPointer_i"
#define UTILS_DICTIONARY_CLOSURE_POINTER_FIELD "closurePointer_i"

#define UTILS_JAVA_POINTER_TYPE_SIGNATURE "J"

/*
   These two macros are needed to get rid of compiler warning when casting pointers
   to and from jlongs. The intptr_t data type is guaranteed to be at least big
   enough to store a pointer regardless of the platform (32 or 64 bit.)
*/
#define CAST_POINTER_TO_JLONG(pointer) \
        (jlong)(intptr_t)(pointer)

#define CAST_JLONG_TO_POINTER(pointer,jnilong) \
        (pointer)(intptr_t)(jnilong)

/*
    Macros to check if the passed in parameter is NULL and if so throw an exception.
	In JNI you must return for the exception to be thrown.
*/
#define MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(p,errorString,retValue)	\
	if(!p){utils_throwMamaException(env, errorString);	return retValue; }														

#define MAMA_THROW_NULL_PARAMETER_RETURN_VOID(p,errorString)	\
	if(!p){utils_throwMamaException(env, errorString);	return; }	

/**
 * This function should be called by all native threads that enter a function
 * where Java objects will be accessed in anyway. Before leaving scope the
 * mamaJniUtils_detachNativeThread function should be called to clean-up.
 *
 * @param[in] jvm The Java Virtual Machine pointer.
 *
 * @return Environment pointer or NULL on an error.
 */
JNIEnv *mamaJniUtils_attachNativeThread(JavaVM *jvm);

/**
 * This function should be called by all native threads that leave a function
 * where Java objects will be accessed in anyway. Note that the
 * mamaJniUtils_attachNativeThread function should be called when entering
 * the function.
 *
 * @param[in] jvm The Java Virtual Machine pointer.
 */
void mamaJniUtils_detachNativeThread(JavaVM *jvm);

/*
   Will return a pointer to a JNIEnv for the the current thread.
   Will attach the current thread if not already attached.
*/
JNIEnv* utils_getENV(JavaVM* jvm);


/*Throw the appropriate Java Exception for a given mama_status*/
void utils_throwExceptionForMamaStatus (
                            JNIEnv*     env,
                            mama_status status,
                            const char* msg);
/*
   Each throws a specific type of Java Exception using the 
   specified message.
*/
void utils_throwWombatException (JNIEnv* env, const char* msg);
void utils_throwMamaException (JNIEnv* env, const char* msg);
void utils_throwMamaFIDConflictException (JNIEnv* env, const char* msg);
void utils_throwMamaFieldNotFoundException (JNIEnv* env, const char* msg);
void utils_throwMamaFieldTypeException (JNIEnv* env, const char* msg);
void utils_throwMamaNoEntitlementsForUserException (JNIEnv* env, const char* msg);
void utils_throwMamaNotEntitledException (JNIEnv* env, const char* msg);

/*
   General function for throwing arbitrary exceptions
*/
void utils_throwException(JNIEnv* env,const char* exceptionClass, const char* msg);

/*
  Populates the buffer with the message,
  the stringified status and the status value
*/
void utils_buildErrorStringForStatus(char buffer[], int bufferLength,
                         const char* message, mama_status status);

/*
 Creates a new Java MamaMsg object and returns it as a jobject.
 The newly created Java object does not create an underlying 
 C structure. 
 A NULL return indicates failure and that an exception has been
 thrown.
*/
jobject utils_createJavaMamaMsg(JNIEnv* env);

/*
 Used from mamamsgjni.c and mamamsgfieldjni.c. Creates and Populates a jobjectArray
 MamaMsg objects from the contents of the mamaMsg vector supplied.
*/
jobjectArray utils_getJniMsgArrayFromVectorMsg(JNIEnv* env, mamaMsg* msgVector,
        size_t vectorLength, jmethodID messageConstructorId,
        jfieldID messagePointerFieldId);

        
/*
 Used from mamamsgjni.c and mamamsgfieldjni.c. Creates and Populates a 
 jobjectArray of MamaMsgs from the contents of the mamaMsg vector supplied. 
 The MamaMsg objects are reused from the jMsgArray, and more will be allocated
 if needed.
*/
jobjectArray utils_getReusedJniMsgArrayFromVectorMsg(JNIEnv* env, 
        mamaMsg* msgVector, size_t vectorLength, 
        jmethodID messageConstructorId, jfieldID messagePointerFieldId, 
        jobject** jMsgArray, jint* jMsgArraySize);   
 
/*
 Will reallocate jMsgArray and populate it with MamaMsg objects if 
 newSize > currentSize
*/               
void utils_growJMsgArray (JNIEnv*   env,
                          jmethodID messageConstructorId,
                          jfieldID  messagePointerFieldId,
                          jobject** jMsgArray,
                          jint*     currentSize,
                          jint      newSize,
                          mamaMsg*  msgVector);            
/*
 Checks the thread stack for any pending Exceptions. Prints the first
 Exception on the stack and then clears the stack.
 
 @param env Pointer to the JNI Environment
 @param where The function in which the Exception was caught
*/
void utils_printAndClearExceptionFromStack(JNIEnv* env, const char* functName);

/**
 * This function determines if the supplied object is an instance of the supplied type.
 *
 * @param[in] env The JNI environment.
 * @param[in] objectToTest An instance of the object to test.
 * @param[in] classType The type of the class to test.
 *
 * @returns True if the supplied instance is of the supplied type.
 */
jboolean utils_isInstanceOf(JNIEnv *env, jobject objectToTest, const char *classType);

#endif /*JNI_UTILS_H__*/
