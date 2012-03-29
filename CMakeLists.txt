project(mplayerthumbs)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
include_directories(${QT_INCLUDES} ${KDE4_INCLUDES})

add_subdirectory( src )
