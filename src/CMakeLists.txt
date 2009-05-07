include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(PHONON_API FALSE)

if(ENABLE_PHONON_SUPPORT)
	set(PHONON_API TRUE)
endif(ENABLE_PHONON_SUPPORT)

configure_file(mplayerthumbs-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/mplayerthumbs-config.h )

set(videopreview_PART_SRCS 
	frameselector.cpp 
	videopreview.cpp  
	mplayervideobackend.cpp 
	videobackendiface.cpp 
	previewingfile.cpp 
	thumbnail.cpp 
	thumbnailsmap.cpp 
	servicesfactory.cpp)

if( PHONON_API )
	set( videopreview_PART_SRCS
		${videopreview_PART_SRCS}
		phononbackend.cpp )
endif( PHONON_API )

kde4_add_kcfg_files(videopreview_PART_SRCS mplayerthumbs.kcfgc)
kde4_add_plugin(videopreview ${videopreview_PART_SRCS})

target_link_libraries(videopreview ${KDE4_KIO_LIBS} )

if( PHONON_API )
	target_link_libraries(videopreview ${PHONON_LIBRARY} )
endif( PHONON_API )

install(TARGETS videopreview DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES  mplayerthumbs.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES  videopreview.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES  sprocket-small.png sprocket-medium.png sprocket-large.png DESTINATION ${DATA_INSTALL_DIR}/videothumbnail)

add_subdirectory(configuration)
