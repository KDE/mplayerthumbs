# add_subdirectory(mplayerthumbsconfig)


########### next target ###############

set(videopreview_PART_SRCS videopreview.cpp  mplayervideobackend.cpp videobackendiface.cpp previewingfile.cpp thumbnail.cpp thumbnailsmap.cpp servicesfactory.cpp)

kde4_add_kcfg_files(videopreview_PART_SRCS mplayerthumbs.kcfgc)

kde4_add_plugin(videopreview ${videopreview_PART_SRCS})

target_link_libraries(videopreview ${KDE4_KIO_LIBS})

install(TARGETS videopreview DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  mplayerthumbs.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  videopreview.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  sprocket-small.png sprocket-medium.png sprocket-large.png DESTINATION ${DATA_INSTALL_DIR}/videothumbnail)

set(mplayerthumbsconfig_SRCS main.cpp mplayerthumbscfg.cpp)
kde4_add_kcfg_files(mplayerthumbsconfig_SRCS mplayerthumbs.kcfgc)
kde4_add_ui_files(mplayerthumbsconfig_SRCS configDialog.ui)
kde4_add_executable(mplayerthumbsconfig ${mplayerthumbsconfig_SRCS})

target_link_libraries(mplayerthumbsconfig ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBRARY} ${videopreview} )

install(TARGETS mplayerthumbsconfig ${INSTALL_TARGETS_DEFAULT_ARGS})

