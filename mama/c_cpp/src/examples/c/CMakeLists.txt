cmake_minimum_required (VERSION 2.8)
project (mamac_examples)

get_target_property(MAMA_INCLUDES mama INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${MAMA_INCLUDES}
)

macro(cexample name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} mama)
endmacro()

cexample(mamaftmemberc)
cexample(mamainboxc)
cexample(mamaio)
cexample(mamalistencachedc)
cexample(mamalistenc)
cexample(mamamultisubscriberc)
cexample(mamaproxyc)
cexample(mamapublisherc)
cexample(mamasubscriberc)
cexample(mamasymbollistsubscriberc)

