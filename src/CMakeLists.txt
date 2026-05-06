#=============================================================================
#  QtGuiTune
#  Qt Guitar Tuner
#
#  Copyright (C) 2013-2015 Yann Collette
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#=============================================================================

set(QTGUITUNE_SRCS main.cpp)

set(QTGUITUNE_UI_SRCS ./guitune.cpp
                      ./audio_base.cpp
                      ./audio_alsa.cpp
                      ./audio_oss.cpp
                      ./mainwindow.cpp
                      ./osziview.cpp
                      ./logview.cpp)

if (ENABLE_JACK)
  set(QTGUITUNE_UI_SRCS ${QTGUITUNE_UI_SRCS} ./audio_jack.cpp)
endif (ENABLE_JACK)

set(QTGUITUNE_UI_HEADERS ./guitune.h
                         ./audio_base.h
                         ./audio_oss.h
                         ./audio_alsa.h
                         ./audio_jack.h
                         ./mainwindow.h)

# Manage translation files
set(FILES_TO_TRANSLATE)

file(GLOB FTT_HEADER_FILES *.h)
file(GLOB FTT_CPP_FILES    *.cpp)
file(GLOB FTT_UI_FILES     *.ui)

set(FILES_TO_TRANSLATE ${FTT_HEADER_FILES} ${FTT_CPP_FILES} ${FTT_UI_FILES})

file(GLOB TRANSLATIONS_FILES translations/*.ts)

qt6_create_translation(QTGUITUNE_QM ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})

add_custom_target(translations_target ALL DEPENDS ${QTGUITUNE_QM})

qt6_wrap_cpp(QTGUITUNE_UI_HEADERS_MOC ${QTGUITUNE_UI_HEADERS})

#qt6_add_resources(QTGUITUNE_RSRCS ./qtguitune.qrc)
set(QTGUITUNE_RSRCS )

add_executable(qtguitune ${QTGUITUNE_SRCS} 
                         ${QTGUITUNE_RSRCS}
	                 ${QTGUITUNE_QM}
	                 ${QTGUITUNE_UI_SRCS}
	                 ${QTGUITUNE_UI_HEADERS_MOC})

target_link_libraries(qtguitune Qt6::Widgets ${JACK_LIBRARIES} ${ALSA_LIBRARIES})

install(TARGETS qtguitune
        DESTINATION bin)

install(FILES ${QM_FILES} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/translations)
