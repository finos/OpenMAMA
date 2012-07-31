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
* mamaDQPublisher.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaDQPublisher.h"


#include <assert.h>

/******************************************************************************
* Local data structures
*******************************************************************************/


/******************************************************************************
* Global/Static variables
*******************************************************************************/

jmethodID           dqpublisherConstructorId_g    =   NULL;
jfieldID            dqpublisherPointerFieldId_g = NULL;

extern   jfieldID            transportPointerFieldId_g;
extern   jfieldID            messagePointerFieldId_g;
extern  JavaVM*     javaVM_g;

/******************************************************************************
* Local function declarations
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _create
 * Signature: (Lcom/wombat/mama/MamaTransport;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1create
  (JNIEnv *env, jobject this, jobject transport, jstring topic)
{
    mamaDQPublisher   cPublisher          =   NULL;
    mamaTransport   cTransport          =   NULL;
    const char*     cTopic              =   NULL;
    jlong           transportPointer    =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];
  
    /*Get the transport pointer*/
    assert(transport!=NULL);
    transportPointer = (*env)->GetLongField(env, transport,
            transportPointerFieldId_g);
    cTransport = CAST_JLONG_TO_POINTER(mamaTransport, transportPointer);
    assert(transportPointer!=0);

    /*Get the char* from the jstring*/
    if(NULL!=topic)
    {
         cTopic = (*env)->GetStringUTFChars(env,topic,0);
         if(!cTopic)return;
    }
    
    if(MAMA_STATUS_OK!=(mamaDQPublisher_allocate(
                    &cPublisher)))
    {
        if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }
                    
    if(MAMA_STATUS_OK!=(mamaDQPublisher_create(
                    cPublisher,
                    cTransport,
                    cTopic)))
    {
        if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
        utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to create publisher.", status);
        utils_throwMamaException(env,errorString);
        return;
    }

    (*env)->SetLongField(env,this,dqpublisherPointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPublisher));
        
    if(cTopic)(*env)->ReleaseStringUTFChars(env,topic, cTopic);
    
    return;
}
       

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _send
 * Signature: (Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1send
  (JNIEnv * env, jobject this , jobject msg)
 {
    jlong           messagePointer      =   0;
    jlong           publisherPointer    =   0;
    mama_status     status              =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);
    messagePointer = (*env)->GetLongField(env,msg,messagePointerFieldId_g);
    assert(messagePointer!=0);

    if(MAMA_STATUS_OK!=(mamaDQPublisher_send(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,messagePointer))))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from publisher.", status);
        utils_throwMamaException(env,errorString);
    }
    
    return;

 }
  

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _sendReply
 * Signature: (Lcom/wombat/mama/MamaMsg;Lcom/wombat/mama/MamaMsg;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1sendReply
  (JNIEnv *env, jobject this, jobject request, jobject reply)
 {
    jlong           requestMessagePointer       =   0;
    jlong           replyMessagePointer         =   0;
    jlong           publisherPointer            =   0;
    mama_status     status                      =   MAMA_STATUS_OK;
    char errorString[UTILS_MAX_ERROR_STRING_LENGTH];

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);
    requestMessagePointer = (*env)->GetLongField(env,request,messagePointerFieldId_g);
    assert(requestMessagePointer!=0);
    replyMessagePointer = (*env)->GetLongField(env,reply,messagePointerFieldId_g);
    assert(replyMessagePointer!=0);

    if(MAMA_STATUS_OK!=(mamaDQPublisher_sendReply(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,requestMessagePointer),
                    CAST_JLONG_TO_POINTER(mamaMsg,replyMessagePointer))))
    {
         utils_buildErrorStringForStatus(
                errorString, UTILS_MAX_ERROR_STRING_LENGTH,
                "Failed to send from publisher.", status);
        utils_throwMamaException(env,errorString);
    }
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1destroy
  (JNIEnv *env, jobject this)
 {
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisher_destroy(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer));
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _setStatus
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1setStatus
  (JNIEnv *env, jobject this, jint status)
{
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);  
    
    mamaDQPublisher_setStatus(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer),
                    status);
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _setSenderId
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1setSenderId
  (JNIEnv *env, jobject this, jlong senderId)
 {
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisher_setSenderId(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer),
                    (uint64_t)senderId);
                    
    return;
 }

/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    _setSeqNum
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher__1setSeqNum
  (JNIEnv *env, jobject this, jlong seqnum)
 {
    jlong           publisherPointer            =   0;

    publisherPointer = (*env)->GetLongField(env,this,dqpublisherPointerFieldId_g);
    assert(publisherPointer!=0);
 
    mamaDQPublisher_setSeqNum(
                    CAST_JLONG_TO_POINTER(mamaDQPublisher,publisherPointer),
                    (mama_seqnum_t) seqnum);
                    
    return;
 }


  
/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaDQPublisher
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaDQPublisher_initIDs
  (JNIEnv *env, jclass class)
{

    dqpublisherPointerFieldId_g = (*env)->GetFieldID(env,
            class, "dqPublisherPointer_i", UTILS_JAVA_POINTER_TYPE_SIGNATURE);

    if (!dqpublisherPointerFieldId_g) return;/*Exception auto thrown*/

    dqpublisherConstructorId_g = (*env)->GetMethodID(env, class,
                              "<init>", "()V" );
                              
    if (!dqpublisherConstructorId_g) return;/*Exception auto thrown*/     
    
    return;
}
