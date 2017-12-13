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
#ifndef MsgUtilsH__
#define MsgUtilsH__

#include "mama/subscmsgtype.h"
#include <mama/integration/msgutils.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define DQ_SUBSCRIBER   7
#define DQ_PUBLISHER    8
#define DQ_NETWORK      9
#define DQ_UNKNOWN      10
#define DQ_DISCONNECT   12
#define DQ_CONNECT      13

#if defined(__cplusplus)
}
#endif

#endif /* MsgUtilsH__*/
