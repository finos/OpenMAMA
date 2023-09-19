# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# - Find Apache Portable Runtime
# Find the APR includes and libraries
# This module defines
#  APR_INCLUDE_DIR and APRUTIL_INCLUDE_DIR, where to find apr.h, etc.
#  APR_LIBRARIES and APRUTIL_LIBRARIES, the libraries needed to use APR.
#  APR_FOUND and APRUTIL_FOUND, If false, do not try to use APR.
# also defined, but not for general use are
#  APR_LIBRARY and APRUTIL_LIBRARY, where to find the APR library.

# APR first.

FIND_PATH(APR_INCLUDE_DIR apr.h
  PATHS
    ${APR_ROOT}/include/apr-1
    ${APR_ROOT}/include/apr-1.0
    ${APR_ROOT}/include
    ${OPENMAMA_DEPENDENCY_ROOT}/include/apr-1
    ${OPENMAMA_DEPENDENCY_ROOT}/include/apr-1.0
    ${OPENMAMA_DEPENDENCY_ROOT}/include
    /usr/local/opt/apr/include/apr-1
    /usr/local/include/apr-1
    /usr/local/include/apr-1.0
    /usr/include/apr-1
    /usr/include/apr-1.0
    /usr/local/apr/include/apr-1
    "C:/Program Files/APR/include"
    "C:/Program Files (x86)/APR/include"
)

SET(APR_NAMES ${APR_NAMES} libapr-1 apr-1.0 apr-1)
FIND_LIBRARY(APR_LIBRARY
  NAMES ${APR_NAMES}
  PATHS
    ${APR_ROOT}/lib
    ${OPENMAMA_DEPENDENCY_ROOT}/lib
    /usr/local/opt/apr/lib
    /usr/lib
    /usr/local/lib
    /usr/local/apr/lib
    /usr/lib/x86_64-linux-gnu
    "C:/Program Files/APR/lib"
    "C:/Program Files (x86)/APR/lib"
   NO_DEFAULT_PATH
)

IF (APR_LIBRARY AND APR_INCLUDE_DIR)
    SET(APR_LIBRARIES ${APR_LIBRARY})
    SET(APR_FOUND "YES")
ELSE (APR_LIBRARY AND APR_INCLUDE_DIR)
  SET(APR_FOUND "NO")
ENDIF (APR_LIBRARY AND APR_INCLUDE_DIR)


IF (APR_FOUND)
   IF (NOT APR_FIND_QUIETLY)
      MESSAGE(STATUS "Found APR: ${APR_LIBRARIES}")
   ENDIF (NOT APR_FIND_QUIETLY)
ELSE (APR_FOUND)
   IF (APR_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find APR library")
   ENDIF (APR_FIND_REQUIRED)
ENDIF (APR_FOUND)

# Deprecated declarations.
SET (NATIVE_APR_INCLUDE_PATH ${APR_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_APR_LIB_PATH ${APR_LIBRARY} PATH)

MARK_AS_ADVANCED(
  APR_LIBRARY
  APR_INCLUDE_DIR
  )

# Next, APRUTIL.

FIND_PATH(APRUTIL_INCLUDE_DIR apu.h
  PATHS
    ${APRUTIL_ROOT}/include/apr-1
    ${APRUTIL_ROOT}/include/apr-1.0
    ${APRUTIL_ROOT}/include
    ${OPENMAMA_DEPENDENCY_ROOT}/include/apr-1
    ${OPENMAMA_DEPENDENCY_ROOT}/include/apr-1.0
    ${OPENMAMA_DEPENDENCY_ROOT}/include
    ${APR_ROOT}/include/apr-1
    ${APR_ROOT}/include/apr-1.0
    ${APR_ROOT}/include
    /usr/local/opt/apr-util/include/apr-1
    /usr/local/include/apr-1
    /usr/local/include/apr-1.0
    /usr/include/apr-1
    /usr/include/apr-1.0
    /usr/local/apr/include/apr-1
    "C:/Program Files/APR-Util/include"
    "C:/Program Files (x86)/APR-Util/include"
)

SET(APRUTIL_NAMES ${APRUTIL_NAMES} libaprutil-1 aprutil-1)
FIND_LIBRARY(APRUTIL_LIBRARY
  NAMES ${APRUTIL_NAMES}
  PATHS
    ${APRUTIL_ROOT}/lib
    ${OPENMAMA_DEPENDENCY_ROOT}/lib
    ${APR_ROOT}/lib
    /usr/local/opt/apr-util/lib
    /usr/lib
    /usr/local/lib
    /usr/local/apr/lib
    /usr/lib/x86_64-linux-gnu
    "C:/Program Files/APR-Util/lib"
    "C:/Program Files (x86)/APR-Util/lib"
  NO_DEFAULT_PATH
)

IF (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
    SET(APRUTIL_LIBRARIES ${APRUTIL_LIBRARY})
    SET(APRUTIL_FOUND "YES")
ELSE (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)
  SET(APRUTIL_FOUND "NO")
ENDIF (APRUTIL_LIBRARY AND APRUTIL_INCLUDE_DIR)


IF (APRUTIL_FOUND)
   IF (NOT APRUTIL_FIND_QUIETLY)
      MESSAGE(STATUS "Found APRUTIL: ${APRUTIL_LIBRARIES}")
   ENDIF (NOT APRUTIL_FIND_QUIETLY)
ELSE (APRUTIL_FOUND)
   IF (APRUTIL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find APRUTIL library")
   ENDIF (APRUTIL_FIND_REQUIRED)
ENDIF (APRUTIL_FOUND)

# Deprecated declarations.
SET (NATIVE_APRUTIL_INCLUDE_PATH ${APRUTIL_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_APRUTIL_LIB_PATH ${APRUTIL_LIBRARY} PATH)

MARK_AS_ADVANCED(
  APRUTIL_LIBRARY
  APRUTIL_INCLUDE_DIR
  )

FILE(GLOB APR_LIBRARY_FILES ${APR_LIBRARY}*)
FILE(GLOB APRUTIL_LIBRARY_FILES ${APRUTIL_LIBRARY}*)