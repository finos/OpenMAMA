/* $Id: wCommon.h,v 1.1.2.3.8.2 2011/08/10 14:53:24 nicholasmarriott Exp $
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#ifndef _WOMBAT_WCOMMON_H
#define _WOMBAT_WCOMMON_H

/* Inline function declarations are different on each platform, this define
 * provides the correct keyword.
 */
#ifndef WCOMMONINLINE

/* Windows */
#ifdef WIN32

#define WCOMMONINLINE __inline
#define WCOMMONFORCEINLINE __forceinline

/* Solaris */
#elif __SUNPRO_C

#define WCOMMONINLINE inline static
#define WCOMMONFORCEINLINE inline static

/* All other OS, assuming that GCC is supported. */
#else
#define WCOMMONINLINE inline static
#define WCOMMONFORCEINLINE inline static

#endif

#endif

#endif /* _WOMBAT_WCOMMON_H */
