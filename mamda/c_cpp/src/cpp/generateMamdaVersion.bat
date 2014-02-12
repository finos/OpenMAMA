set BUILD_DIR=%1
set VERSION_MAJOR=2
set VERSION_MINOR=3
set VERSION_RELEASE=0

echo "generating version files.."  

echo /* This file was automatically generated */ > %BUILD_DIR%\version.h
echo #ifndef MamdaVersionHH >> %BUILD_DIR%\version.h
echo #define MamdaVersionHH >> %BUILD_DIR%\version.h
echo #define MAMDA_VERSION_MAJOR %VERSION_MAJOR% >> %BUILD_DIR%\version.h
echo #define MAMDA_VERSION_MINOR %VERSION_MINOR% >> %BUILD_DIR%\version.h
echo #define MAMDA_VERSION_RELEASE %VERSION_RELEASE% >> %BUILD_DIR%\version.h
echo #define MAMDA_VERSION "mamda %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%" >> %BUILD_DIR%\version.h	
echo extern "C" const char* mamda_version; >> %BUILD_DIR%\version.h
echo #endif >> %BUILD_DIR%\version.h
echo /* This file was automatically generated */ > %BUILD_DIR%\version.c
echo const char* mamda_version = "mamda %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%"; >> %BUILD_DIR%\version.c

echo "complete"
