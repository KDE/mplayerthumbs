project(PLAYGROUND_MPLAYERTHUMBS)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include(MacroOptionalAddSubdirectory)
#include(ConfigureChecks.cmake)
#configure_file(config-mplayerthumbs ${CMAKE_CURRENT_BINARY_DIR}/config-mplayerthumbs.h)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory( src )
