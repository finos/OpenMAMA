FIND_PATH(PROTON_INCLUDE_DIR proton/version.h
	PATHS
		${QPID_ROOT}/include
		${PROTON_ROOT}/include
		/usr/local/include
		/usr/include
		"C:/Program Files/Proton/include"
		"C:/Program Files (x86)/Proton/include"
)

FIND_LIBRARY(PROTON_LIBRARY qpid-proton
	PATHS
		${QPID_ROOT}/lib64
		${PROTON_ROOT}/lib64
		/usr/lib64
		/usr/local/lib64
		${QPID_ROOT}/lib
		${PROTON_ROOT}/lib
		/usr/lib
		/usr/local/lib
		/usr/lib/x86_64-linux-gnu
		"C:/Program Files/Proton/lib"
		"C:/Program Files (x86)/Proton/lib"
	NO_DEFAULT_PATH
)

IF (PROTON_LIBRARY AND PROTON_INCLUDE_DIR)
    SET(PROTON_LIBRARIES ${PROTON_LIBRARY})
    SET(PROTON_FOUND "YES")
ELSE ()
    SET(PROTON_FOUND "NO")
ENDIF ()


IF (PROTON_FOUND)
   IF (NOT PROTON_FIND_QUIETLY)
      MESSAGE(STATUS "Found Proton: ${PROTON_LIBRARIES}")
   ENDIF ()
ELSE ()
   IF (Proton_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Proton library")
   ENDIF ()
ENDIF ()

FILE(GLOB PROTON_LIBRARY_FILES ${PROTON_LIBRARY}*)
