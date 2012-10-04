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

/******************************************************************************
* includes
*******************************************************************************/
#include <assert.h>
#include "mamajniutils.h"
#include "mamajni/com_wombat_mama_MamaLogHandler.h"

/******************************************************************************
* Public function implementation
*******************************************************************************/

/*
 * Class:     com_wombat_mama_MamaLogHandler
 * Method:    log
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_wombat_mama_MamaLogHandler_log
  (JNIEnv* env, jobject this, jint mamaLevel, jstring logString)
{
    const char* c_string      =   NULL;

    if(logString)
    {
        c_string = (*env)->GetStringUTFChars(env,logString,0);
        if(!c_string) return;/*Exception auto thrown*/
    }

    mama_log ((int)mamaLevel, c_string);
    
    if(logString)
    {
        (*env)->ReleaseStringUTFChars(env,logString, c_string);
    }

}
