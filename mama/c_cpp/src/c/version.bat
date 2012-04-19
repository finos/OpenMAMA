
set VERSION_MAJOR=6
set VERSION_MINOR=0
set VERSION_RELEASE=0rc14
set VERSION_RELEASE_WIN=0
set VERSION_BUILD=16



set VERSION=mama_%VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%
set RC_VERSION_NUMERICAL=%VERSION_MAJOR%,%VERSION_MINOR%,%VERSION_RELEASE%,%VERSION_BUILD%
set RC_VERSION_STRING="%VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%\0"

echo /* This file was automatically generated */ > mama\version.h
echo #ifndef MamaVersionH >> mama\version.h
echo #define MamaVersionH >> mama\version.h
echo #define MAMA_VERSION_MAJOR %VERSION_MAJOR% >> mama\version.h
echo #define MAMA_VERSION_MINOR %VERSION_MINOR% >> mama\version.h
echo #define MAMA_VERSION_RELEASE %VERSION_RELEASE% >> mama\version.h
echo #define MAMA_VERSION "mama %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%" >> mama\version.h	
echo #define RC_VERSION_NUMERICAL %RC_VERSION_NUMERICAL% >> mama\version.h
echo #define RC_VERSION_STRING %RC_VERSION_STRING% >> mama\version.h
echo #define RC_C_DLL_NAME "libmamacmd\0" >> mama\version.h
echo #define RC_CPP_DLL_NAME "libmamacppmd\0" >> mama\version.h
echo extern const char* mama_version; >> mama\version.h
echo #endif >> mama\version.h
echo /* This file was automatically generated */ > version.c
echo const char* mama_version = "mama %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%"; >> version.c

