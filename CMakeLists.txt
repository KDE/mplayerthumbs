project(PLAYGROUND_MPLAYERTHUMBS)
#include(ConfigureChecks.cmake)
#configure_file(config-mplayerthumbs ${CMAKE_CURRENT_BINARY_DIR}/config-mplayerthumbs.h)

add_subdirectory( src )
