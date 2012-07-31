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
* Common functions used by both the mamasubscription and mamabasicsubscription
* implementations.
*/

#ifndef SUBSCRIPTION_COMMON_H__
#define SUBSCRIPTION_COMMON_H__

/******************************************************************************
* Includes
*******************************************************************************/
#include "mamajniutils.h"

/******************************************************************************
* Data structures
*******************************************************************************/

/*We use this as the closure to the subscription creation. We need to maintain
 references to various objects throughout the lifetime of the subscription*/
typedef struct callbackClosure
{
    /*
       The closure passed by the client. This is stored as a global reference
       and must be deleted when the subscription is deleted.
     */
    jobject mUserData;
    
    /*
       The callback object for the subscription. This is stored as a
       global reference and must be deleted when the subscription is deleted
     */
    jobject mClientCB;
    
    /*
       The java subscription object. Stored as a global reference
       and deleted when the subscription is destroyed.
     */
    jobject  mSubscription;
    
    /*
       The Message which is to be reused for the callbacks.
       This is more efficient than creating a new message for each.
       Global reference which gets deleted when the subscription is destroyed.
     */
    jobject mMessage;
    
} callbackClosure;


/******************************************************************************
* Common functions
*******************************************************************************/

/*Used from the destroy functions of the subscriptions*/
extern void subCommon_destroy (JNIEnv* env, jobject subscription,
                              jfieldID    subscriptionPointerFieldId);

/*Used from the destroy functions of the subscriptions*/
extern void subCommon_destroyEx (JNIEnv* env, jobject subscription,
                              jfieldID    subscriptionPointerFieldId);

/*Common code for subscription callbacks*/
extern void subCommon_createCb ( mamaSubscription    subscription,
                                 void*               closure,
                                 jfieldID            subscriptionPointerFieldId,
                                 jmethodID           subCallbackonCreateId); 

extern void subCommon_onErrorCb ( mamaSubscription    subscription,
                                  mama_status         status,
                                  void*               platformError,
                                  const char*         subject,
                                  void*               closure,
                                  jmethodID           subCallbackonErrorId);                                  
                                  
extern void subCommon_onMsgCb ( mamaSubscription    subscription,
                                mamaMsg             msg,
                                void*               closure,
                                jfieldID            messagePointerFieldId,
                                jmethodID           subCallbackonMsgId); 

                                
extern void subCommon_qualityCb ( mamaSubscription    subscription,
                                  mamaQuality         quality,
                                  const char*         symbol,
                                  short               cause,
                                  const void*         platformInfo,
                                  void*               closure,
                                  jmethodID           subCallbackonQualityId);
                      
extern void subCommon_gapCb (mamaSubscription    subscription,
                             void*               closure,
                             jfieldID            subscriptionPointerFieldId,
                             jmethodID           subCallbackonRecapId);

extern void subCommon_recapCb (mamaSubscription    subscription,
                               void*               closure,
                               jfieldID            subscriptionPointerFieldId,
                               jmethodID           subCallbackonRecapId);

extern void subCommon_destroyCb (mamaSubscription  subscription,
                                 void*               closure,
                                 jfieldID            subscriptionPointerFieldId,
                                 jmethodID           subCallbackonDestroyId);

#endif /*SUBSCRIPTION_COMMON_H__*/
