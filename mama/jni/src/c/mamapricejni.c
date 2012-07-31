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
* MamaPrice.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>

#include "mamajniutils.h"

/******************************************************************************
* Global/Static variables
*******************************************************************************/
jfieldID         pricePointerFieldId_g     =   NULL;

/******************************************************************************
* Local function declarations
*******************************************************************************/

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    clear
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_clear
  (JNIEnv* env, jobject this)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];   
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_clear(
                        CAST_JLONG_TO_POINTER(mamaPrice,pPrice))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.clear().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    setValue
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_setValue
  (JNIEnv* env, jobject this, jdouble value)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];   
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_setValue(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), (double) value)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.setValue().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    getValue
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaPrice_getValue
  (JNIEnv* env, jobject this)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    double      value       = 0.0;  
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.", 0);
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_getValue(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), &value)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.getValue().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return (jdouble) value;
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    _getRoundedValue
 * Signature: (B)D
 */
JNIEXPORT jdouble JNICALL Java_com_wombat_mama_MamaPrice__1getRoundedValue
  (JNIEnv *env , jobject this , jbyte pecision)
{

    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    double      value       = 0.0;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,
		"Null parameter, MamaPrice may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaPrice_getRoundedValue(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), pecision, &value)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.getValue().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return (jdouble) value;
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    toString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_wombat_mama_MamaPrice_toString
  (JNIEnv* env, jobject this)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        ret_c       [MAMA_PRICE_MAX_STR_LEN+1];
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.", NULL);
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_getAsString(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), ret_c, MAMA_PRICE_MAX_STR_LEN)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.toString()",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return NULL;
    }
    return (*env)->NewStringUTF(env, ret_c);
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    copy
 * Signature: ()Lcom/wombat/mama/MamaPrice;
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_copy
  (JNIEnv* env, jobject this, jobject other)
{
    jlong       pPrice       = 0;
    jlong       pPriceToCopy = 0;
    mama_status status       = MAMA_STATUS_OK;
    char        errorString  [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");
    
    pPriceToCopy = (*env)->GetLongField(env,other,pricePointerFieldId_g);   
    
    if(!pPriceToCopy || 
       MAMA_STATUS_OK!=(status=mamaPrice_copy(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), 
                            CAST_JLONG_TO_POINTER(mamaPrice,pPriceToCopy))))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                 "Error calling MamaPrice.copy().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}
   
/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    _create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice__1create
  (JNIEnv* env , jobject this)
{  
    mamaPrice   cPrice      = NULL;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_create(&cPrice)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Could not create mamaPrice.",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
        return;
    }
    /*If we get this far we have a valid price pointer to set*/
    (*env)->SetLongField(env, this,
                         pricePointerFieldId_g,
                         CAST_POINTER_TO_JLONG(cPrice));
    return;
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    _setPrecision
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice__1setPrecision
  (JNIEnv* env, jobject this, jbyte precision)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];   
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_setPrecision(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), 
                            (mamaPricePrecision) precision)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.setPrecision().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    setIsValidPrice
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_setIsValidPrice
  (JNIEnv* env, jobject this, jboolean valid)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");

    if(MAMA_STATUS_OK!=(status=mamaPrice_setIsValidPrice(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice),
                            (mama_bool_t) valid)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.setIsValidPrice().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    _getPrecision
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_com_wombat_mama_MamaPrice__1getPrecision
  (JNIEnv* env, jobject this)
{
    jlong               pPrice      = 0;
    mama_status         status      = MAMA_STATUS_OK;
    mamaPricePrecision  precision   = 0;   
    char                errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.", 0);
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_getPrecision(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), &precision)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.getPrecision().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return (jbyte) precision;
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    getIsValidPrice
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_wombat_mama_MamaPrice_getIsValidPrice
  (JNIEnv* env, jobject this)
{
    jlong               pPrice      = 0;
    mama_status         status      = MAMA_STATUS_OK;
    mama_bool_t         valid       = 0;
    char                errorString [UTILS_MAX_ERROR_STRING_LENGTH];

    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.", 0);

    if(MAMA_STATUS_OK!=(status=mamaPrice_getIsValidPrice(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), &valid)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.getIsValidPrice().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return (jboolean) valid;
}


/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_destroy
  (JNIEnv* env, jobject this)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;   
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];
 
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
        
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice, 
    "MamaPrice.destroy(): Null parameter, MamaPrice may have already been destroyed."); 

    if(MAMA_STATUS_OK!=(status=mamaPrice_destroy(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice))))
    {
        utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice.destroy().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    
    (*env)->SetLongField(env, this,
         pricePointerFieldId_g,
         0);
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    _getHints
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_com_wombat_mama_MamaPrice__1getHints
  (JNIEnv* env, jobject this)
{
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    mama_u8_t   hints       = 0;  
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH]; 
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VALUE(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.", 0);
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_getHints(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), 
                            &hints)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice._getHints().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
    return (jbyte) hints; 
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    setHints
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice__1setHints
  (JNIEnv* env, jobject this, jbyte hints)
 {
    jlong       pPrice      = 0;
    mama_status status      = MAMA_STATUS_OK;
    char        errorString [UTILS_MAX_ERROR_STRING_LENGTH];   
    
    pPrice = (*env)->GetLongField(env,this,pricePointerFieldId_g);
    MAMA_THROW_NULL_PARAMETER_RETURN_VOID(pPrice,  
		"Null parameter, MamaPrice may have already been destroyed.");
    
    if(MAMA_STATUS_OK!=(status=mamaPrice_setHints(
                            CAST_JLONG_TO_POINTER(mamaPrice,pPrice), 
                            (mamaPriceHints) hints)))
    {
         utils_buildErrorStringForStatus(
                errorString,
                UTILS_MAX_ERROR_STRING_LENGTH,
                "Error calling MamaPrice._setHints().",
                status);
        utils_throwExceptionForMamaStatus(env,status,errorString);
    }
}

/*
 * Class:     com_wombat_mama_MamaPrice
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPrice_initIDs
  (JNIEnv* env, jclass class)
{
    pricePointerFieldId_g = (*env)->GetFieldID(
            env, class,"pricePointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    return;
}

