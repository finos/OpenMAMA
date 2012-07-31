/* $Id: wConfig.h,v 1.1.2.1 2012/02/22 03:02:23 mikeschonberg Exp $
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

#ifndef _WOMBAT_WCONFIG_H
#define _WOMBAT_WCONFIG_H

/* Calling conventions */
#define MAMACALLTYPE __stdcall

#if defined( COMMON_DLL )
    /* We are building common dll */
#   define COMMONExpDLL __declspec( dllexport )
#elif defined( MAMA_DLL ) && defined( MAMA )
    /* We are building mama as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL __declspec( dllexport )
#elif defined( MAMA_DLL ) && defined( BRIDGE )
    /* We are building mama bridge as a dll */
#   define MAMAExpDLL __declspec( dllimport )
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpBridgeDLL __declspec( dllexport )
#   define WMWExpDLL __declspec( dllimport )
#elif defined( MAMA_DLL ) && defined( MAMACPP )
    /* We are building mamacpp as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif defined( MAMDA_DLL ) && defined( MAMDA )
    /* We are building mamda as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif defined( MAMDA_DLL ) && defined( MAMDAOPT )
    /* We are building extra mamda as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAOPTExpDLL __declspec( dllexport )
#   define MAMAExpBridgeDLL
#elif !defined ( MAMA_STATIC ) && !defined ( WMW_STATIC ) && !defined (WIRECACHE_STATIC)
    /* We are building mama apps (non static) */
#   define COMMONExpDLL __declspec( dllimport )
#   define WMWExpDLL __declspec( dllexport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAOPTExpDLL __declspec( dllimport )
#   define MAMAExpBridgeDLL
#elif defined( WIN32  ) && defined( WMW_DLL )
    /* We are building wmw as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define WMWExpDLL __declspec( dllexport )
#elif defined( WIN32  ) && defined( WMW_APP )
    /* We are building wmw test apps */
#   define COMMONExpDLL __declspec( dllimport )
#   define WMWExpDLL __declspec( dllimport )
#else
    /* We are building on linux or statically */
#   define COMMONExpDLL
#   define WMWExpDLL
#   define WCACHEExpDLL
#   define MAMAExpDLL
#   define MAMACPPExpDLL
#   define MAMDAExpDLL
#   define MAMDAOPTExpDLL
#   define MAMAExpBridgeDLL
#endif

#define WCOMMONINLINE __inline
#define WCOMMONFORCEINLINE __forceinline


#endif /* _WOMBAT_WCONFIG_H */


