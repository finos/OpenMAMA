# gnu definitions
add_definitions(-D_GNU_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wmissing-prototypes -Wstrict-prototypes -pedantic-errors -fPIC")
# dont warn on gnu extensions
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-gnu")
