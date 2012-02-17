#ifndef _WOMBAT_TARGETSXS_H
#define _WOMBAT_TARGETSXS_H

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
