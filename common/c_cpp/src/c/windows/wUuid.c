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

#include "wombat/wUuid.h"
#include <Rpc.h>
#include <combaseapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void wUuid_unparse (wUuid myUuid, char* out)
{
    RPC_CSTR outputUuid = NULL;
    UuidToString (&myUuid[0], &outputUuid);
    strncpy (out, (const char*)outputUuid, WUUID_UNPARSE_OUTPUT_LENGTH);
    RpcStringFree (&outputUuid);
}

void wUuid_generate (wUuid myUuid) {
    UuidCreate (&myUuid[0]);
}

void wUuid_generate_time (wUuid myUuid) {
    UuidCreateSequential (&myUuid[0]);
}

int wUuid_generate_time_safe (wUuid myUuid) {
    CoCreateGuid (&myUuid[0]);
}

void wUuid_generate_random (wUuid myUuid) {
    UuidCreate (&myUuid[0]);
}

void wUuid_clear (wUuid myUuid) {
    memset_s(&myUuid[0], sizeof(UUID), '\0', sizeof(UUID));
}
