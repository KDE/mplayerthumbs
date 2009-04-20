set(mplayerthumbsconfig_SRCS main.cpp mplayerthumbscfg.cpp)
include_directories( .. )
kde4_add_kcfg_files(mplayerthumbsconfig_SRCS ../mplayerthumbs.kcfgc)
kde4_add_ui_files(mplayerthumbsconfig_SRCS configDialog.ui mplayerConfig.ui)
kde4_add_executable(mplayerthumbsconfig ${mplayerthumbsconfig_SRCS})

target_link_libraries(mplayerthumbsconfig ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBRARY} ${videopreview} )

install(TARGETS mplayerthumbsconfig ${INSTALL_TARGETS_DEFAULT_ARGS})

