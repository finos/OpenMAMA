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
#ifndef _WOMBAT_TARGETSXS_H
#define _WOMBAT_TARGETSXS_H

#if (_MSC_VER >= 1900 && _MSC_VER < 2000)
#define VC14
#elif (_MSC_VER >= 1800 && _MSC_VER < 1900)
#define VC12
#elif (_MSC_VER >= 1700 && _MSC_VER < 1800)
#define VC11
#elif (_MSC_VER >= 1600 && _MSC_VER < 1700)
#define VC10
#elif (_MSC_VER >= 1600 && _MSC_VER < 1500)
#define VC9
#elif (_MSC_VER >= 1400 && _MSC_VER < 1500)
#define VC8
#elif (_MSC_VER >= 1400 && _MSC_VER < 1400)
#define VC7
#endif

#ifndef VC7

#ifdef VC8
#define _SXS_ASSEMBLY_VERSION "8.0.50727.762"
#endif

#ifdef VC9
#define _SXS_ASSEMBLY_VERSION "9.0.21022.8"
#endif

#ifdef VC10
#define _SXS_ASSEMBLY_VERSION "10.0.30319.1"
#endif

#ifdef VC11
#define _SXS_ASSEMBLY_VERSION "11.0.50727.1"
#endif

#ifdef VC12
#define _SXS_ASSEMBLY_VERSION "12.0.21005.1"
#endif

#ifdef VC14
#define _SXS_ASSEMBLY_VERSION "14.0.23026.0"
#endif

#ifndef __midl

#define _CRT_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION
#define _MFC_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION
#define _ATL_ASSEMBLY_VERSION _SXS_ASSEMBLY_VERSION

#ifdef __cplusplus
extern "C" {
#endif
__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceAtlDllManifestRTM;
#ifdef __cplusplus
}
#endif
#endif
#endif
#endif
