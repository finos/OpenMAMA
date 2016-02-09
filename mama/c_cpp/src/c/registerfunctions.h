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

#ifndef RegisterFunctionsH__
#define RegisterFunctionsH__

#include <platform.h>


mama_status
mamaInternal_registerMiddlewareFunctions (LIB_HANDLE  bridgeLib,
                                          mamaBridge* bridge,
                                          const char* name);

mama_status
mamaInternal_registerPayloadFunctions (LIB_HANDLE         bridgeLib,
                                       mamaPayloadBridge* bridge,
                                       const char*        name);

mama_status
mamaInternal_registerEntitlementFunctions (LIB_HANDLE               bridgeLib,
                                           mamaEntitlementBridge*    bridge,
                                           const char*              name);


/**
 * @brief Mechanism for registering required bridge functions.
 *
 * Taking a function string name search the shared library handle, using the
 * loadLibFunc portability method, for the function. If it is found, set the
 * appropriate function pointer in the bridge struct to the result. If not,
 * log as an error the fact that the funciton cannot be found, and return
 * MAMA_STATUS_PLATFORM
 *
 * @param FUNCSTRINGNAME The string function name.
 * @param FUNCIMPLNAME The name of the function pointer in the bridge struct
 * @param FUNCIMPLTYPE The type of the function pointer expected.
 */

#define REGISTER_BRIDGE_FUNCTION(FUNCSTRINGNAME, FUNCIMPLNAME, FUNCIMPLTYPE)    \
do {                                                                            \
    snprintf (functionName, 256, "%s"#FUNCSTRINGNAME, name);                    \
    result = loadLibFunc (bridgeLib, functionName);                             \
                                                                                \
    if (NULL != result) {                                                       \
        (*bridge)->FUNCIMPLNAME = *(FUNCIMPLTYPE*)&result;                      \
        result = NULL;                                                          \
    } else {                                                                    \
        mama_log (MAMA_LOG_LEVEL_ERROR,                                         \
                  "mamaInternal_registerBridgeFunctions: "                      \
                  "Cannot load bridge, does not implement required function: [%s]",\
                  functionName);                                                \
        status = MAMA_STATUS_PLATFORM;                                          \
        return status;                                                          \
    }                                                                           \
} while (0)

/**
 * @brief Mechanism for registering required bridge functions.
 *
 * Taking a function string name search the shared library handle, using the
 * loadLibFunc portability method, for the function. If it is found, set the
 * appropriate function pointer in the bridge struct to the result. If not
 * log the fact that the function has not been found, and continue.
 *
 * @param FUNCSTRINGNAME The string function name.
 * @param FUNCIMPLNAME The name of the function pointer in the bridge struct
 * @param FUNCIMPLTYPE The type of the function pointer expected.
 */
#define REGISTER_OPTIONAL_BRIDGE_FUNCTION(FUNCSTRINGNAME,                       \
                                          FUNCIMPLNAME,                         \
                                          FUNCIMPLTYPE)                         \
do {                                                                            \
    snprintf (functionName, 256, "%s"#FUNCSTRINGNAME, name);                    \
    result = loadLibFunc (bridgeLib, functionName);                             \
                                                                                \
    if (NULL != result) {                                                       \
        (*bridge)->FUNCIMPLNAME = *(FUNCIMPLTYPE*)&result;                      \
        result = NULL;                                                          \
    } else {                                                                    \
        mama_log (MAMA_LOG_LEVEL_FINE,                                          \
                  "mamaInternal_registerBridgeFunctions: "                      \
                  "Optional bridge function [%s] not found. Unavailable.",      \
                  functionName);                                                \
    }                                                                           \
} while (0)

#endif /* RegisterFunctionsH__ */
