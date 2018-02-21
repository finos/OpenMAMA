set BUILD_DIR=%1
set VERSION_MAJOR=6
set VERSION_MINOR=2
set VERSION_RELEASE="2_rc2"
set RC_VERSION_NUMERICAL=1
set RC_VERSION_STRING=1
set MAMACDLL=libmamacmdd.dll
set MAMACPPDLL=libmamacppmdd.dll

echo "generating version files.."  

echo /* This file was automatically generated */ > %BUILD_DIR%\mama\version.h
echo #ifndef MamaVersionH >> %BUILD_DIR%\mama\version.h
echo #define MamaVersionH >> %BUILD_DIR%\mama\version.h
echo #define MAMA_VERSION_MAJOR %VERSION_MAJOR% >> %BUILD_DIR%\mama\version.h
echo #define MAMA_VERSION_MINOR %VERSION_MINOR% >> %BUILD_DIR%\mama\version.h
echo #define MAMA_VERSION_RELEASE "%VERSION_RELEASE%" >> %BUILD_DIR%\mama\version.h
echo #define MAMA_VERSION "mama %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%" >> %BUILD_DIR%\mama\version.h	
echo #define RC_VERSION_NUMERICAL %RC_VERSION_NUMERICAL% >> %BUILD_DIR%\mama\version.h
echo #define RC_VERSION_STRING "%RC_VERSION_STRING%" >> %BUILD_DIR%\mama\version.h
echo #define RC_C_DLL_NAME "%MAMACDLL%" >> %BUILD_DIR%\mama\version.h
echo #define RC_CPP_DLL_NAME "%MAMACPPDLL%" >> %BUILD_DIR%\mama\version.h
echo extern const char* mama_version; >> %BUILD_DIR%\mama\version.h
echo #endif >> %BUILD_DIR%\mama\version.h
echo /* This file was automatically generated */ > %BUILD_DIR%\version.c
echo const char* mama_version = "mama %VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_RELEASE%"; >> %BUILD_DIR%\version.c

echo "generating %BUILD_DIR%/entitlementlibraries.c"  

echo /* This file was automatically generated */ > %BUILD_DIR%\entitlementlibraries.c
echo #include ^<mamainternal.h^> >> %BUILD_DIR%\entitlementlibraries.c
echo const char* gEntitlementBridges [MAX_ENTITLEMENT_BRIDGES] = {"noop", NULL}; >> %BUILD_DIR%\entitlementlibraries.c

echo "complete" 
