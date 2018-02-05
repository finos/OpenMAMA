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
* MamaTimerImpl.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaTimer.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/
typedef struct timerClosure
{
    /*The callback object for the timer. This is stored as a global
      reference and must be deleted when the timer is deleted*/
    jobject mTimerCallback; 

    /* The java timer object. */
    jobject mTimer;

} timerClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern JavaVM* javaVM_g;/*Defined in mamajni.c*/

extern jfieldID    queuePointerFieldId_g;/*Defined in mamaqueuejni.c*/

/*Field ids for MamaTransportImpl.java field callbacks*/
static  jfieldID    timerPointerFieldId_g   =   NULL;
static  jfieldID    timerClosureFieldId_g   =   NULL;

static  jmethodID   onTimerId_g             =   NULL;
static  jmethodID   onDestroy_g             =   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/
static void MAMACALLTYPE mamaTimerCB (mamaTimer  timer, void*   closure);
static void MAMACALLTYPE mamaTimerDestroyCB(mamaTimer timer, void *closure);

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    create
 * Signature:
 * (Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaTimerCallback;D)V
 */
JNIEXPORT void JNICALL
Java_com_wombat_mama_MamaTimer_create
  (JNIEnv* env, jobject this, jobject queue, 
   jobject callback, jdouble interval)
{
    mamaTimer       timer           =   NULL;
    mama_status     status          =   MAMA_STATUS_OK;
    mamaQueue       queue_c         =   NULL;
    jlong           queuePointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    /*Used internally to gain access to various objects in async callbacks*/
    timerClosure* closureImpl = (timerClosure*)calloc(1,
            sizeof(timerClosure));

    if(!closureImpl)
    {
        utils_throwMamaException(env,"createMamaTimer(): Could not allocate.");
        return;
    }

    closureImpl->mTimerCallback    = (*env)->NewGlobalRef (env, callback);
    closureImpl->mTimer            = (*env)->NewGlobalRef (env, this);
        
    /*Check if a queue was specified.*/
    if(queue)
    {
        /*Get the queue pointer value from the MamaQueue java object*/
        queuePointer = (*env)->GetLongField(env, queue, queuePointerFieldId_g);
        queue_c = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    if (MAMA_STATUS_OK != (status = mamaTimer_allocate2(
                    &timer,
                    queue_c,
					mamaTimerDestroyCB)))

    {
        (*env)->DeleteGlobalRef(env,closureImpl->mTimerCallback);
        (*env)->DeleteGlobalRef(env,closureImpl->mTimer);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create timer.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env, this, timerPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(timer));

    (*env)->SetLongField(env, this, timerClosureFieldId_g,
                         CAST_POINTER_TO_JLONG(closureImpl));

    if (MAMA_STATUS_OK != (status = mamaTimer_start(
                    timer,
                    (mamaTimerCb)mamaTimerCB,
                    (double)interval,
                    closureImpl)))

    {
        (*env)->DeleteGlobalRef(env,closureImpl->mTimerCallback);
        (*env)->DeleteGlobalRef(env,closureImpl->mTimer);
        free(closureImpl);
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create timer.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTimer_destroy(JNIEnv* env, jobject this)
{
    /* Errors. */
    mama_status ret = MAMA_STATUS_NULL_ARG;    

    /* Get the C timer object from the java class. */
    jlong timerPointer = (*env)->GetLongField(env, this, timerPointerFieldId_g);
    if(0 != timerPointer)
    {        
        /* Set the native timer and closure to NULL inside the java class. */
        (*env)->SetLongField(env, this, timerPointerFieldId_g, 0);
        (*env)->SetLongField(env, this, timerClosureFieldId_g, 0);

        /* Destroy the C timer. */
        ret = mamaTimer_destroy(CAST_JLONG_TO_POINTER(mamaTimer, timerPointer));        
    }

    /* If anything went wrong throw an exception. */
    if(MAMA_STATUS_OK != ret)
    {
        /* Format an error message. */
        char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
        utils_buildErrorStringForStatus(errorString, UTILS_MAX_ERROR_STRING_LENGTH, "MamaTimer.destroy(): Failed to destroy timer.", ret);

        /* Convert the error code into an exception. */
        utils_throwMamaException(env, errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    resetInterval
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTimer_resetInterval
  (JNIEnv* env, jobject this, jdouble interval)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       timerPointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    timerPointer    = (*env)->GetLongField(env,this,timerPointerFieldId_g);

    if(MAMA_STATUS_OK!=(status=mamaTimer_setInterval(
                    CAST_JLONG_TO_POINTER(mamaTimer,timerPointer),
                    interval)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to reset timer interval.",
                status);
         utils_throwMamaException(env,errorString);
    }

    return;
}

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    getInterval
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaTimer_getInterval
  (JNIEnv* env, jobject this)
{ 
    mama_status status          =   MAMA_STATUS_OK;
    jlong       timerPointer    =   0;
    double      c_interval      =   0.0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
    
    timerPointer    = (*env)->GetLongField(env,this,timerPointerFieldId_g);

    if(MAMA_STATUS_OK!=(status=mamaTimer_getInterval(
                    CAST_JLONG_TO_POINTER(mamaTimer,timerPointer),
                    &c_interval)))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to get timer interval.",
                status);
         utils_throwMamaException(env,errorString);
         return 0.0;
    }

    return (jdouble) c_interval;
}

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    reset
 * Signature: ()V
 */ 
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTimer_reset
  (JNIEnv* env, jobject this)
{
    mama_status status          =   MAMA_STATUS_OK;
    jlong       timerPointer    =   0;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    timerPointer    = (*env)->GetLongField(env,this,timerPointerFieldId_g);

    if(MAMA_STATUS_OK!=(status=mamaTimer_reset(
                    CAST_JLONG_TO_POINTER(mamaTimer,timerPointer)
                    )))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to reset timer.",
                status);
         utils_throwMamaException(env,errorString);
    }

    return;
}   

/*
 * Class:     com_wombat_mama_MamaTimer
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaTimer_initIDs
  (JNIEnv* env, jclass class)
{
    jclass   javaRunnableClass          =   NULL;    
    jclass   mamaTimerCallbackClass     =   NULL;

    timerPointerFieldId_g = (*env)->GetFieldID(env,
                               class, "timerPointer_i",
                               UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!timerPointerFieldId_g) return;/*Exception auto thrown*/

    timerClosureFieldId_g = (*env)->GetFieldID(env,
                                class,"closurePointer_i",
                                UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if(!timerClosureFieldId_g)return;

    mamaTimerCallbackClass = (*env)->FindClass(env, "com/wombat/mama/MamaTimerCallback");
    if (!mamaTimerCallbackClass) return; /*Exception auto thrown*/

    onTimerId_g = (*env)->GetMethodID(env,mamaTimerCallbackClass,
                        "onTimer","(Lcom/wombat/mama/MamaTimer;)V");

    onDestroy_g = (*env)->GetMethodID(env,mamaTimerCallbackClass,
                        "onDestroy","(Lcom/wombat/mama/MamaTimer;)V");

    /*Tidy up local refs*/
    (*env)->DeleteLocalRef(env, mamaTimerCallbackClass);
    
    return;
}

void MAMACALLTYPE mamaTimerDestroyCB(mamaTimer timer, void *closure)
{
    /* Get the environment pointer. */
    JNIEnv *env = utils_getENV(javaVM_g);
    if(NULL != env)
    {
        /* Get the impl from the closure. */
        timerClosure *closureImpl = (timerClosure *)closure;
        if(NULL != closureImpl)
        {
            /* Invoke the callback if it has been supplied. */        
            if((NULL != closureImpl->mTimer) && (NULL != closureImpl->mTimerCallback))
            {             
			    (*env)->CallVoidMethod(env,
					                   closureImpl->mTimerCallback,
					                   onDestroy_g,
					                   closureImpl->mTimer);   
            }

            /* Perform the clean-up. */
            /* Delete the callback refs. */
            if(NULL != closureImpl->mTimerCallback)
            {
                (*env)->DeleteGlobalRef(env, closureImpl->mTimerCallback);            
            }

            if(NULL != closureImpl->mTimer)
            {
                (*env)->DeleteGlobalRef(env, closureImpl->mTimer);            
            }

            /* Free the closure object. */
            free(closureImpl);
        }
    }
}

void MAMACALLTYPE mamaTimerCB (mamaTimer  timer,  void*   closure)
{
    JNIEnv*         env             =   NULL;
    timerClosure*   closureImpl     =   NULL;

    closureImpl = (timerClosure*)closure;

    assert(closureImpl!=NULL);

    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv*/
    
    if (closureImpl->mTimer && closureImpl->mTimerCallback)
    {
         /*invoke the onTimer() callback*/
        (*env)->CallVoidMethod(env,
                               closureImpl->mTimerCallback,
                               onTimerId_g,
                               closureImpl->mTimer);   
    }
      
    return;
}
