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

#ifndef _WOMBAT_WCONFIG_H
#define _WOMBAT_WCONFIG_H

/* Calling conventions */
#define MAMACALLTYPE __stdcall

#if defined( COMMON_DLL )
    /* We are building common dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllexport )
#   define COMMONDeprecatedExpDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#elif defined( COMMONENT_DLL )
    /* We are building common enterprise dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllexport )
#elif defined( WIRECACHE_DLL )
    /* We are building wirecache dll */
#   define WCACHEExpDLL __declspec( dllexport )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#elif defined( MAMA_DLL ) && defined( MAMA )
    /* We are building mama as a dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG))
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllexport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL __declspec( dllexport )
#   define MAMAExpDeprecatedBridgeDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#elif defined( MAMA_DLL ) && defined( BRIDGE )
    /* We are building mama bridge as a dll */
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define COMMONDeprecated(MSG) __declspec( deprecated )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL __declspec( dllexport )
#   define MAMAExpDeprecatedBridgeDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define WMWDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define WMWExpDLL __declspec( dllimport )
#   define WMWExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define WCACHEExpDLL __declspec( dllexport )
#elif defined( MAMA_DLL ) && defined( MAMAENT )
    /* We are building mamaenterprisec or mamaenterprisecpp as a dll */
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL __declspec( dllimport )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMACPPExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMAENTCExpDLL __declspec( dllexport )
#elif defined( MAMA_DLL ) && defined( MAMACPP )
    /* We are building mamacpp as a dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMACPPExpDLL __declspec( dllexport )
#   define MAMACPPExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL
#   define MAMAExpDeprecatedBridgeDLL(MSG) __delspec( deprecated(##MSG) )
#elif defined( MAMDA_DLL ) && defined( MAMDA )
    /* We are building mamda as a dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONDeprecatedExpDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMACPPExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMDAExpDLL __declspec( dllexport )
#   define MAMDAExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL(MSG)
#   define MAMAExpDeprecatedBridgeDLL(MSG) __delspec( deprecated(##MSG) )
#elif defined( MAMDA_DLL ) && defined( MAMDAOPT )
    /* We are building extra mamda as a dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMACPPExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMDAOPTExpDLL __declspec( dllexport )
#   define MAMDAOPTExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL
#   define MAMAExpDeprecatedBridgeDLL(MSG) __delspec( deprecated(##MSG) )
#elif !defined ( MAMA_STATIC ) && !defined ( WMW_STATIC ) && !defined (WIRECACHE_STATIC)
    /* We are building mama apps (non static) */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define WMWDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define WMWExpDLL __declspec( dllexport )
#   define WMWExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#   define MAMADeprecated(MSG) __declspec( deprecated(##MSG) )
#   define MAMAExpDLL __declspec( dllimport )
#   define MAMAENTCExpDLL __declspec( dllimport )
#   define MAMAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMACPPExpDLL __declspec( dllimport )
#   define MAMACPPExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMDAExpDLL __declspec( dllimport )
#   define MAMDAExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMDAOPTExpDLL __declspec( dllimport )
#   define MAMDAOPTExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define MAMAExpBridgeDLL(MSG)
#   define MAMAExpDeprecatedBridgeDLL(MSG) __delspec( deprecated(##MSG) )
#elif defined( WIN32  ) && defined( WMW_DLL )
    /* We are building wmw as a dll */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define WMWDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define WMWExpDLL __declspec( dllexport )
#   define WMWExpDeprecatedDLL(MSG) __declspec( dllexport, deprecated(##MSG) )
#elif defined( WIN32  ) && defined( WMW_APP )
    /* We are building wmw test apps */
#   define COMMONDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define COMMONExpDLL __declspec( dllimport )
#   define COMMONENTExpDLL __declspec( dllimport )
#   define COMMONExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#   define WMWDeprecated(MSG) __declspec( deprecated(##MSG) )
#   define WMWExpDLL __declspec( dllimport )
#   define WMWExpDeprecatedDLL(MSG) __declspec( dllimport, deprecated(##MSG) )
#else
    /* We are building on linux or statically */
#   define COMMONDeprecated(MSG)
#   define COMMONExpDLL
#   define COMMONENTExpDLL
#   define COMMONExpDeprecatedDLL(MSG)
#   define WMWDeprecated(MSG)
#   define WMWExpDLL
#   define WMWExpDeprecatedDLL(MSG)
#   define WCACHEExpDLL
#   define WCACHEExpDeprecatedDLL(MSG)
#   define MAMADeprecated(MSG)
#   define MAMAExpDLL
#   define MAMAENTCExpDLL
#   define MAMAExpDeprecatedDLL(MSG)
#   define MAMACPPExpDLL
#   define MAMACPPExpDeprecatedDLL(MSG)
#   define MAMDAExpDLL
#   define MAMDAExpDeprecatedDLL(MSG)
#   define MAMDAOPTExpDLL
#   define MAMDAOPTExpDeprecatedDLL(MSG)
#   define MAMAExpBridgeDLL
#   define MAMAExpDeprecatedBridgeDLL(MSG)
#endif

#define WCOMMONINLINE __inline
#define WCOMMONFORCEINLINE __forceinline

#define MAMATypeDeprecated(NAME, MSG)   \
        __pragma(deprecated(##NAME))


/* Special tags to allow for disabling deprecation messages for code between
 * the MAMAIgnoreDeprecatedOpen and MAMAIgnoreDeprecatedClose tags.
 * We can only disable deprecation warnings for VS or greater
 */
#if (_MSC_VER >= 1400)
#define MAMAIgnoreDeprecatedOpen        \
        __pragma(warning(push))         \
        __pragma(warning(disable: 4995 4996))

/* Renable previous deprecations. */
#define MAMAIgnoreDeprecatedClose       \
        __pragma(warning(pop))
#else
/* Older versions of VS will just have to warn */
#define MAMAIgnoreDeprecatedOpen
#define MAMAIgnoreDeprecatedClose
#endif

#endif /* _WOMBAT_WCONFIG_H */


