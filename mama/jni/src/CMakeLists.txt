cmake_minimum_required (VERSION 2.8.3)
project (mamajni)

find_package(Java)
include(UseJava)

file(GLOB sources 
	../../../common/jni/src/com/wombat/common/*.java
	com/wombat/mama/*.java
)

set(CLASSES "")
foreach(file ${sources})
	string(FIND ${file} "com/" startidx)
	string(FIND "${file}" ".java" stopidx REVERSE)
	math(EXPR len "${stopidx}-${startidx}")
	string(SUBSTRING "${file}" ${startidx} ${len} subfile)
	string(REPLACE "/" "." classname ${subfile})
	list(APPEND CLASSES ${classname})
endforeach()

set(CMAKE_JAVA_COMPILE_FLAGS -source 1.5 -target 1.5)
add_jar(mamajnijava ${sources} OUTPUT_NAME mamajni)

get_target_property(MAMAJAR mamajnijava JAR_FILE)

add_custom_target(mamajnijavah DEPENDS mamajnijava
	COMMAND "${Java_JAVAH_EXECUTABLE}" -jni -classpath "${MAMAJAR}" -d mamajni ${CLASSES}
	COMMENT "Updating JNI headers for mamajni" VERBATIM
)

set_target_properties(mamajnijava PROPERTIES JAVAH_INCLUDES ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory (c)
