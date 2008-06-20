
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(mplayerthumbsconfig)


########### next target ###############

set(videopreview_PART_SRCS videopreview.cpp)

kde4_add_kcfg_files(videopreview_PART_SRCS mplayerthumbs.kcfgc)

kde4_add_plugin(videopreview ${videopreview_PART_SRCS})

target_link_libraries(videopreview ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS})

install(TARGETS videopreview DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  mplayerthumbs.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  videopreview.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  sprocket-small.png sprocket-medium.png sprocket-large.png DESTINATION ${DATA_INSTALL_DIR}/videothumbnail)



#original Makefile.am contents follow:

#INCLUDES    = $(all_includes)
#METASOURCES = AUTO
#
#SUBDIRS = . mplayerthumbsconfig
#
#kde_module_LTLIBRARIES = videopreview.la
#
#videopreview_la_SOURCES = videopreview.cpp mplayerthumbs.kcfgc
#videopreview_la_LDFLAGS = -avoid-version -module $(all_libraries) $(KDE_PLUGIN)
#
#protocoldir = $(kde_servicesdir)
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/videopreview.pot
#protocol_DATA = videopreview.desktop
#videopreview_la_LIBADD = $(LIB_KDECORE)
#
#videothumbnail_DATA = sprocket-small.png sprocket-medium.png sprocket-large.png
#videothumbnaildir = $(kde_datadir)/videothumbnail
#kde_kcfg_DATA = mplayerthumbs.kcfg
#
