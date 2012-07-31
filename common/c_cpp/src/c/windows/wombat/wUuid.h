/* $Id: wUuid.h,v 1.1.2.2 2012/03/20 11:15:17 emmapollock Exp $
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
#ifndef WUUID_H__
#define WUUID_H__

#include "wombat/port.h"

typedef char* wUuid;

COMMONExpDLL
void wUuid_generate_time (wUuid myUuid);

COMMONExpDLL
void wUuid_unparse (wUuid myUuid, char* out);

#endif /* WUUID_H__ */
