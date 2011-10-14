/* $Id: wConfig.h,v 1.6.2.1.12.4 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_WCONFIG_H
#define _WOMBAT_WCONFIG_H

/* This section defines export semantics for functions exported or
 * imported from Windows libraries. All exported functions must
 * be decorated with the appropriate modifier.
 */

#if defined( WIN32  ) && defined( COMMON_DLL )
    /* We are building common dll */
#   define COMMONExpDLL __declspec( dllexport )
#elif defined( WIN32  ) && defined( MAMA_DLL ) && defined( MAMA )
    /* We are building mama as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL __declspec( dllexport )
#elif defined( WIN32  ) && defined( MAMA_DLL ) && defined( BRIDGE )
    /* We are building mama bridge as a dll */
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAExpBridgeDLL __declspec( dllimport )
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL
#   define MAMDAExpDLL
#   define MAMDAOPTExpDLL
#elif defined( WIN32  ) && defined( MAMA_DLL ) && defined( MAMACPP )
    /* We are building mamacpp as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif defined( WIN32  ) && defined( MAMDA_DLL ) && defined( MAMDA )
    /* We are building mamda as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif defined( WIN32  ) && defined( MAMDA_DLL ) && defined( MAMDAOPT )
    /* We are building extra mamda as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAOPTExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif defined( WIN32  ) && !defined ( MAMA_STATIC ) && !defined ( WMW_STATIC ) && !defined (WIRECACHE_STATIC)
    /* We are building mama apps (non static) */
#   define COMMONExpDLL __declspec( dllimport )
#   define WMWExpDLL __declspec( dllexport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAOPTExpDLL __declspec( dllimport )
#   define MAMAExpBridgeDLL
#else
    /* We are building on linux or statically */
#   define COMMONExpDLL
#   define WMWExpDLL
#   define MAMAExpDLL
#   define MAMACPPExpDLL
#   define MAMDAExpDLL
#   define MAMDAOPTExpDLL
#   define MAMAExpBridgeDLL
#endif /* WIN32 */

/* 
 * Windows callback functions require the standard call type.
 */
#ifdef WIN32
#define MAMACALLTYPE __stdcall
#else
#define MAMACALLTYPE
#endif

#endif /* _WOMBAT_WCONFIG_H */


