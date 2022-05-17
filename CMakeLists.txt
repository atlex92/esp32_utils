file(GLOB_RECURSE SOURCES  src/*.cpp src/*.c)
set(srcs ${SOURCES})

set(includedirs
inc
)

idf_component_register(INCLUDE_DIRS ${includedirs} SRCS ${srcs} REQUIRES)