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
* Implementation for each of the native methods defined in the mamajniutils.h
* header file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"

extern  jmethodID   messageConstructorId_g;

/******************************************************************************
* Public function implementation
*******************************************************************************/

JNIEnv *mamaJniUtils_attachNativeThread(JavaVM *jvm)
{
    /* Returns. */
    JNIEnv *ret = NULL;

    /* Attach the thread to the JVM. */
    jint act = (*jvm)->AttachCurrentThread(jvm, (void**)&ret, NULL);

    /* Log a message if this function failed. */
    if(0 != act)
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL, "Could not attach current thread.\n");
    }

    return ret;
}

void mamaJniUtils_detachNativeThread(JavaVM *jvm)
{
    /* Detach the thread from the JVM. */
    jint dct = (*jvm)->DetachCurrentThread(jvm);

    /* Log a message if this function failed. */
    if(0 != dct)
    {
        mama_log(MAMA_LOG_LEVEL_NORMAL, "Could not dettach current thread.\n");
    }
}



JNIEnv* utils_getENV(JavaVM* jvm)
{
    JNIEnv* env = NULL;

    (*jvm)->GetEnv(jvm,(void**)&env,JNI_VERSION_1_2);

    /*env will still be NULL if the current thread was not attached to the JVM*/
    if(!env)
    {
        if (0!= (*jvm)->AttachCurrentThread(jvm,(void**)&env,NULL))
        {
            /*We can't throw an exception as we need a JNIEnv**/
            mama_log (MAMA_LOG_LEVEL_NORMAL, "Could not attach current thread.\n");
            return NULL;
        }
    }
    
    return env;
}

void utils_throwExceptionForMamaStatus (
        JNIEnv*         env,
        mama_status     status,
        const char*     msg)
{
    switch (status)
    {
        case MAMA_STATUS_NOT_FOUND:
            utils_throwMamaFieldNotFoundException (env,msg); 
            break;
        case MAMA_STATUS_NOT_ENTITLED:
            utils_throwMamaNotEntitledException (env,msg);
            break;
        case MAMA_STATUS_WRONG_FIELD_TYPE:
            utils_throwMamaFieldTypeException (env, msg);
            break;
        default:
            utils_throwWombatException (env,msg);
    }
    return;
}


void utils_throwException(JNIEnv* env,const char* exceptionClass, const char* msg)
{
    jclass cls  = (*env)->FindClass(env,exceptionClass);

    if(cls) (*env)->ThrowNew(env, cls, msg);

    (*env)->DeleteLocalRef(env,cls);
    
    return;
}

void utils_throwWombatException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/common/WombatException",msg);
}

void utils_throwMamaException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaException",msg);
}

void utils_throwMamaFIDConflictException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaFIDConflictException",msg);
}

void utils_throwMamaFieldNotFoundException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaFieldNotFoundException",msg);
}

void utils_throwMamaFieldTypeException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaFieldTypeException",msg);
}

void utils_throwMamaNoEntitlementsForUserException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaNoEntitlementsForUserException",msg);
}

void utils_throwMamaNotEntitledException(JNIEnv* env, const char* msg)
{
    utils_throwException(env,
            "com/wombat/mama/MamaNotEntitledException",msg);
}

void utils_buildErrorStringForStatus(char buffer[], int bufferLength,
                                 const char* message, mama_status status)
{
    snprintf(buffer, bufferLength, "%s [%s] [%d]",message,
           mamaStatus_stringForStatus(status), (int)status );
}

jobject utils_createJavaMamaMsg(JNIEnv* env)
{
    jobject javaMamaMsg         =   NULL;
    jclass  mamaMsgImplClass    =   NULL;

    mamaMsgImplClass = (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    if (!mamaMsgImplClass) return;
    
    /*Create the message object*/
    javaMamaMsg = (*env)->NewObject(env, mamaMsgImplClass,
            messageConstructorId_g,JNI_FALSE);
    
    /*Will have returned NULL if an exception was thrown*/
    (*env)->DeleteLocalRef(env, mamaMsgImplClass);
       
    return javaMamaMsg;
}

jobjectArray utils_getJniMsgArrayFromVectorMsg(JNIEnv* env, mamaMsg* msgVector,
                              size_t vectorLength, jmethodID messageConstructorId,
                              jfieldID messagePointerFieldId)
{
    jobjectArray    result          =   NULL;
    jobject         messageImpl     =   NULL;
    int             i               =   0;
    jclass          msgArrayClass   = 
                    (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    if(!msgArrayClass) return NULL;/*Exception auto thrown*/
    
    /*Create the object array*/
    result = (*env)->NewObjectArray(env,vectorLength,msgArrayClass,NULL);
    if(!result)
    {
        (*env)->DeleteLocalRef(env, msgArrayClass);
        return NULL;
    }
    
    for(i=0;i<vectorLength;i++)
    {
        /*Create new Java MamaMsg object*/
        messageImpl = (*env)->NewObject(env, msgArrayClass,
                              messageConstructorId, JNI_FALSE);
        /*Set the pointer instance var on the new msg object*/
        (*env)->SetLongField(env, messageImpl, messagePointerFieldId,
                             CAST_POINTER_TO_JLONG(msgVector[i]));
        /*Add the new msg to the Java MamaMsg array*/
        (*env)->SetObjectArrayElement(env,result,i,messageImpl);
        (*env)->DeleteLocalRef(env,messageImpl);
    }

    (*env)->DeleteLocalRef(env, msgArrayClass);
    return result;
}

jobjectArray utils_getReusedJniMsgArrayFromVectorMsg(
                        JNIEnv* env, mamaMsg* msgVector, size_t vectorLength,
                        jmethodID messageConstructorId, 
                        jfieldID messagePointerFieldId, jobject** jMsgArray,
                        jint* jMsgArraySize)
{
    jobjectArray    result           =  NULL;
    jint            i                =  0;
    jobject         jMsg             =  NULL; 
    jclass          msgClass         = 
                    (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    
    if(!msgClass) return NULL;/*Exception auto thrown*/
    
    /*Create the object array*/
    result = (*env)->NewObjectArray(env,vectorLength,msgClass,NULL);
    if(!result)
    {
        (*env)->DeleteLocalRef(env, msgClass);
        return NULL;
    }
    
    utils_growJMsgArray (env, 
                        messageConstructorId,
                        messagePointerFieldId, 
                        jMsgArray, 
                        jMsgArraySize, 
                        vectorLength,
                        msgVector);
    
    for(;i<vectorLength;++i)
    { 
        jobject jMsg = (*jMsgArray)[i];
        
        (*env)->SetLongField(env, jMsg, messagePointerFieldId,
                             CAST_POINTER_TO_JLONG(msgVector[i]));
      
        /*Add the msg to the new Java MamaMsg array*/
        (*env)->SetObjectArrayElement(env,result,i,jMsg);
    }

    (*env)->DeleteLocalRef(env, msgClass);
    return result;
}

void utils_growJMsgArray (JNIEnv*   env,
                          jmethodID messageConstructorId,
                          jfieldID  messagePointerFieldId,
                          jobject** jMsgArray,
                          jint*     currentSize,                         
                          jint      vectorSize,
                          mamaMsg*  msgVector)
{
    jobject* newJMsgArray       = NULL;
    jint    i                   = 0;
    jobject javaMamaMsg         = NULL;
    jclass  mamaMsgClass        = NULL;
        
    if (*currentSize >= vectorSize)
        return;
    mamaMsgClass  = (*env)->FindClass(env,"com/wombat/mama/MamaMsg");
    if(!mamaMsgClass) return ;
       
    newJMsgArray = malloc (vectorSize * sizeof (jobject*));
    
    /* Copy current elements */     
    for (; i < *currentSize; ++i)
    {
        newJMsgArray[i] = (*jMsgArray)[i];
    }  
    
    /* Create new elements. */
    for (; i < vectorSize; ++i)
    {
        /*Create new Java MamaMsg object*/
        javaMamaMsg = (*env)->NewObject(env, mamaMsgClass,
                              messageConstructorId, JNI_FALSE);     
        javaMamaMsg = (*env)->NewGlobalRef(env,javaMamaMsg); 
        
        /* Note: We are taking advantage of the fact that Mama C reuses
        arrays under the hood so we only need to set the C pointer once */                                          
      /*  (*env)->SetLongField(env, javaMamaMsg, messagePointerFieldId,
                             CAST_POINTER_TO_JLONG(msgVector[i]));*/
        newJMsgArray[i] = javaMamaMsg;
    }
    
    if (*jMsgArray) 
    {
        free (*jMsgArray);  
    }
    
    *jMsgArray = newJMsgArray;
    *currentSize = vectorSize;
}             

void utils_printAndClearExceptionFromStack(JNIEnv* env, const char* functName)
{
    jthrowable exc  =   NULL;

    exc = (*env)->ExceptionOccurred(env);
    if(exc)
    {        
#if 0
        mama_log(MAMA_LOG_LEVEL_FINE,"%s(): Got exception"
                                     "on the stack: Clearing.",functName);
#endif
        /*(*env)->ExceptionDescribe(env);*/
        (*env)->ExceptionClear(env);
    }
}

jboolean utils_isInstanceOf(JNIEnv *env, jobject objectToTest, const char *classType)
{
    /* Returns. */
    jboolean ret = JNI_FALSE;

    /* Get the callback class. */
    jclass instanceOfType = (*env)->FindClass(env, classType);
    if(NULL != instanceOfType)
    {
        /* Check to see if callback is an instance of this class. */
        ret = (*env)->IsInstanceOf(env, objectToTest, instanceOfType);

        /* Free the reference to the callback class. */
        (*env)->DeleteLocalRef(env, instanceOfType);
    }

    return ret;
}
