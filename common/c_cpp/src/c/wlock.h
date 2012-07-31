/* $Id: wlock.h,v 1.5.16.1 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_WLOCK_H
#define _WOMBAT_WLOCK_H

#include <wombat/wConfig.h>

#if defined (__cplusplus)
extern "C" {
#endif
typedef void * wLock;

COMMONExpDLL wLock    wlock_create( void );
COMMONExpDLL void     wlock_destroy( wLock lock );
COMMONExpDLL void     wlock_lock( wLock lock );
COMMONExpDLL void     wlock_unlock( wLock lock );
#if defined (__cplusplus)
}
#endif

#endif /* _WOMBAT_WLOCK_H */
