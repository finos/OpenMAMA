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
* MamaDispatcher.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaDispatcher.h"

#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/
static  jfieldID    dispatcherPointerFieldId_g   =   NULL;

/******************************************************************************
* Global/Static variables
*******************************************************************************/
extern jfieldID    queuePointerFieldId_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaDispatcher
 * Method:    create
 * Signature: (Lcom/wombat/mama/MamaQueue;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDispatcher_create
  (JNIEnv* env, jobject this, jobject queue)
{
    mama_status     status          =   MAMA_STATUS_OK;
    mamaDispatcher  dispatcher      =   NULL;
    jlong           queuePointer    =   0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    queuePointer = (*env)->GetLongField(env,queue,queuePointerFieldId_g);

    if(0==queuePointer)
    {
        utils_buildErrorStringForStatus(errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Invalid queue for dispatcher.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    if(MAMA_STATUS_OK!=(status=mamaDispatcher_create(&dispatcher,
                               CAST_JLONG_TO_POINTER(mamaQueue,queuePointer))))
    {
        utils_buildErrorStringForStatus(errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create dispatcher.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env, this, dispatcherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(dispatcher));
    
    return;
}

/*
 * Class:     com_wombat_mama_MamaDispatcher
 * Method:    isDispatching
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaDispatcher_isDispatching
  (JNIEnv* env, jobject this)
{
    mama_status     status              =       MAMA_STATUS_OK;
    int             isDispatching       =       0;
    jlong           dispatcherPointer   =       0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    utils_buildErrorStringForStatus(errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "isDispatching(): Not yet supported.",
                status);
    utils_throwMamaException(env,errorString);
    
    return JNI_TRUE;
}

/*
 * Class:     com_wombat_mama_MamaDispatcher
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDispatcher_destroy
  (JNIEnv* env, jobject this)
{
    mama_status     status              =       MAMA_STATUS_OK;
    jlong           dispatcherPointer   =       0;
    char            errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    dispatcherPointer = (*env)->GetLongField(env,this,
                                             dispatcherPointerFieldId_g);

    if(0==dispatcherPointer)
    {
        utils_buildErrorStringForStatus(errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Must call create() on dispatcher object.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }

    if(MAMA_STATUS_OK!=(status=mamaDispatcher_destroy(
                    CAST_JLONG_TO_POINTER(mamaDispatcher,dispatcherPointer))))
    {
        utils_buildErrorStringForStatus(errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not destroy dispatcher.",
                status);
        utils_throwMamaException(env,errorString);
        return;
    }
    
   (*env)->SetLongField(env, this,
                     dispatcherPointerFieldId_g,
                     0);
    return;
}

/*
 * Class:     com_wombat_mama_MamaDispatcher
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDispatcher_initIDs
  (JNIEnv* env, jclass class)
{
    dispatcherPointerFieldId_g = (*env)->GetFieldID(
            env, class,"dispatcherPointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    return;
}
