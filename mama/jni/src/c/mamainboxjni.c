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
* MamaInbox.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaInbox.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/
/*Used internally only as the closure to the mamaInbox_create() call*/
typedef struct inboxCbClosure
{
    jobject mInbox;
    jobject mJavaCallback;
    jobject mReuseableMsgObject;    
} inboxCbClosure;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
jfieldID    inboxPointerFieldId_g                =  NULL;
jfieldID    internalClosurePointerFieldId_g      =  NULL;
jmethodID   callbackOnMsgId_g                    =  NULL;
jmethodID   callbackOnDestroyId_g                =  NULL;

/* Needed during create() */
extern jfieldID    transportPointerFieldId_g;
extern jfieldID    queuePointerFieldId_g;
extern jmethodID   messageConstructorId_g;
extern jfieldID    messagePointerFieldId_g;

extern  JavaVM*     javaVM_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/
/*The callback function passed to creation of inbox*/
static void MAMACALLTYPE inboxCB(mamaMsg msg, void *closure);
static void MAMACALLTYPE destroyCB(mamaInbox inbox, void *closure);

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaInbox
 * Method:    create
 * Signature: (Lcom/wombat/mama/MamaTransport;Lcom/wombat/mama/MamaQueue;Lcom/wombat/mama/MamaInboxCallback;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaInbox_create
  (JNIEnv* env, jobject this, jobject transport,
   jobject queue, jobject callback)
{
    jlong           transportPointer    =   0;
    jlong           queuePointer        =   0;
    jobject         messageImpl         =   NULL;
    mamaQueue       cQueue              =   NULL;
    inboxCbClosure* closureImpl         =   NULL;
    mamaInbox       cInbox              =   NULL;
    mama_status     status              =   MAMA_STATUS_OK;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];
        
    closureImpl = (inboxCbClosure*) calloc(1,sizeof(inboxCbClosure));
    if(!closureImpl)
    {
        utils_throwMamaException(env,"MamaInbox.create():"
                " Could not allocate.");
        return;
    }
    
    assert(NULL!=transport);
    assert(NULL!=callback);
    
    closureImpl->mInbox = (*env)->NewGlobalRef(env,this);
    closureImpl->mJavaCallback = (*env)->NewGlobalRef(env,callback);    

    transportPointer =
        (*env)->GetLongField(env,transport,transportPointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(transportPointer,  
		"Null parameter, MamaTransport may have already been destroyed.")

    if(queue)/*The queue is optional*/
    {
        queuePointer = (*env)->GetLongField(env,queue,queuePointerFieldId_g);
        MAMA_THROW_NULL_PARAMETER_RETURN_VOID(queuePointer,  
			"Null parameter, MamaQueue may have already been destroyed.")

        cQueue = CAST_JLONG_TO_POINTER(mamaQueue, queuePointer);
    }

    /*Create the reuseable Java message object*/
    messageImpl = utils_createJavaMamaMsg(env);
    if(NULL==messageImpl) return;/*Exception will have been thrown*/

    closureImpl->mReuseableMsgObject = (*env)->NewGlobalRef(env,messageImpl);

    /*Finally! Create the mamaInbox structure*/    
    if(MAMA_STATUS_OK!=(status=mamaInbox_create2(
                        &cInbox,
                        CAST_JLONG_TO_POINTER(mamaTransport,transportPointer),
                        cQueue,
                        (mamaInboxMsgCallback)inboxCB,
                        NULL/*onError callback not used*/,
                        (mamaInboxDestroyCallback)destroyCB,
                        closureImpl)))
    {
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "mamaInbox_create() Failed.", status);
        utils_throwMamaException(env,errorString);
        return;
    }

    /*Set the inbox callback in the MamaInbox object*/
    (*env)->SetLongField(env,this,inboxPointerFieldId_g,
                             CAST_POINTER_TO_JLONG(cInbox));
    /*Store the closure pointer so we can delete it when destroy()
     is called.*/
    (*env)->SetLongField(env,this,internalClosurePointerFieldId_g,
                            CAST_POINTER_TO_JLONG(closureImpl));
    return;
}

/*
 * Class:     com_wombat_mama_MamaInbox
 * Method:    _destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaInbox__1destroy(JNIEnv* env, jobject this)
{
    /* Errors. */
    mama_status ret = MAMA_STATUS_NULL_ARG;    

    /* Get the C inbox object from the java class. */
    jlong inboxPointer = (*env)->GetLongField(env, this, inboxPointerFieldId_g);
    if(0 != inboxPointer)
    {        
        /* Set the native inbox and closure to NULL inside the java class. */
        (*env)->SetLongField(env, this, inboxPointerFieldId_g, 0);     
	    (*env)->SetLongField(env, this, internalClosurePointerFieldId_g, 0);			    

        /* Destroy the C timer. */
        ret = mamaInbox_destroy(CAST_JLONG_TO_POINTER(mamaInbox, inboxPointer));        
    }

    /* If anything went wrong throw an exception. */
    if(MAMA_STATUS_OK != ret)
    {
        /* Format an error message. */
        char errorString[UTILS_MAX_ERROR_STRING_LENGTH] = "";
        utils_buildErrorStringForStatus(errorString, UTILS_MAX_ERROR_STRING_LENGTH, "MamaInbox.destroy(): Failed to destroy inbox.", ret);

        /* Convert the error code into an exception. */
        utils_throwMamaException(env, errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaInbox
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaInbox_initIDs
   (JNIEnv* env, jclass class)
{

    jclass  inboxCallbackClass   =   NULL;
    
    inboxPointerFieldId_g = (*env)->GetFieldID(env,
            class, "inboxPointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!inboxPointerFieldId_g) return;/*Exception auto thrown*/
    
    internalClosurePointerFieldId_g = (*env)->GetFieldID(env,
            class, "internalCClosurePointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!internalClosurePointerFieldId_g) return;/*Exception auto thrown*/
    
    inboxCallbackClass = (*env)->FindClass(env,
            "com/wombat/mama/MamaInboxCallback");
    if (!inboxCallbackClass) return;


    callbackOnMsgId_g = (*env)->GetMethodID(env, inboxCallbackClass,
            "onMsg", "(Lcom/wombat/mama/MamaInbox;"
            "Lcom/wombat/mama/MamaMsg;)V" );
    if (!callbackOnMsgId_g)
    {
        (*env)->DeleteLocalRef(env, inboxCallbackClass);
        return;/*Exception auto thrown*/
    }

    callbackOnDestroyId_g = (*env)->GetMethodID(env,inboxCallbackClass,
                        "onDestroy","(Lcom/wombat/mama/MamaInbox;)V");
    
    return;
}

void MAMACALLTYPE destroyCB(mamaInbox inbox, void *closure)
{
    /* Get the environment pointer. */
    JNIEnv *env = utils_getENV(javaVM_g);
    if(NULL != env)
    {
        /* Get the impl from the closure. */
        inboxCbClosure *closureImpl = (inboxCbClosure *)closure;
        if(NULL != closureImpl)
        {
            /* Invoke the callback if it has been supplied. */        
            if((NULL != closureImpl->mInbox) && (NULL != closureImpl->mJavaCallback))
            {       
			    (*env)->CallVoidMethod(env, closureImpl->mJavaCallback, callbackOnDestroyId_g, closureImpl->mInbox);
            }

            /* Perform the clean-up. */
            /* Destroy the global refs - allows objects to be garbage collected */
            if(NULL != closureImpl->mReuseableMsgObject)
            {
                /* Clear the message field pointer in the re-usable message, it will then be destroyed by the garbage collector
                 * when the global ref is deleted below, note that this object is only accessible to the JNI layer.
                 */
                (*env)->SetLongField(env, closureImpl->mReuseableMsgObject, messagePointerFieldId_g, 0);

                /* Delete the global ref. */
                (*env)->DeleteGlobalRef(env, closureImpl->mReuseableMsgObject);
            }

            if(NULL != closureImpl->mJavaCallback)
            {
                (*env)->DeleteGlobalRef(env, closureImpl->mJavaCallback);
            }

            if(NULL != closureImpl->mInbox)
            {
                (*env)->DeleteGlobalRef(env, closureImpl->mInbox);
            }

            /* Free the closure object. */
            free(closureImpl);
        }
    }
}

/*
 * Class:     com_wombat_mama_MamaInbox
 * Method:    initIDs
 * Signature: ()V
 */
void MAMACALLTYPE inboxCB(mamaMsg msg, void *closure)
{
    JNIEnv*             env             =   NULL;
    inboxCbClosure*     closureImpl     =   (inboxCbClosure*)closure;
    assert(NULL!=closureImpl);

    env = utils_getENV(javaVM_g);
    if (!env) return;/*Can't throw exception without JNIEnv!!*/

    /*Set the msg pointer on the reuseable msg object*/
    (*env)->SetLongField(env, closureImpl->mReuseableMsgObject,
                         messagePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(msg));

    (*env)->CallVoidMethod(env, closureImpl->mJavaCallback,
                           callbackOnMsgId_g,
                           closureImpl->mInbox,
                           closureImpl->mReuseableMsgObject);
    return;
}
