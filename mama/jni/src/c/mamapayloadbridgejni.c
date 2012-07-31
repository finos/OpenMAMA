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
* MamaPayloadBridge.java source file.
*/

/******************************************************************************
* includes
*******************************************************************************/
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaPayloadBridge.h"

/******************************************************************************
* Local data structures
*******************************************************************************/
jfieldID    payloadBridgePointerFieldId_g   =   NULL;
jmethodID   payloadBridgeConstructorId_g    =   NULL;

/******************************************************************************
* Public function implementation
*******************************************************************************/
/*
 * Class:     com_wombat_mama_MamaPayloadBridge
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaPayloadBridge_initIDs
  (JNIEnv* env, jclass class)
{
    payloadBridgePointerFieldId_g = (*env)->GetFieldID(
            env, class,"bridgePointer_i",
            UTILS_JAVA_POINTER_TYPE_SIGNATURE);
    if (!payloadBridgePointerFieldId_g) return;/*Exception auto thrown*/      
    
    payloadBridgeConstructorId_g = (*env)->GetMethodID(env, class,
                              "<init>", "()V" );
    if (!payloadBridgeConstructorId_g) return;/*Exception auto thrown*/        
    return;
}
