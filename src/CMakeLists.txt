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

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files (WARNING: make clean will delete the source .ts files! Danger!)" OFF)

set(QTGUITUNE_SRCS main.cpp)

set(QTGUITUNE_UI_SRCS ./guitune.cpp
                      ./audio_base.cpp
                      ./audio_alsa.cpp
                      ./audio_oss.cpp
                      ./mainwindow.cpp
                      ./osziview.cpp
		              ./logview.cpp)

set(QTGUITUNE_UI_HEADERS ./guitune.h
                         ./mainwindow.h)

set(QTGUITUNE_TRANS ./translations/qtguitune_en.ts)

# Manage translation files
set(FILES_TO_TRANSLATE)

file(GLOB FTT_HEADER_FILES *.h)
file(GLOB FTT_CPP_FILES    *.cpp)
file(GLOB FTT_UI_FILES     *.ui)

set(FILES_TO_TRANSLATE ${FTT_HEADER_FILES} ${FTT_CPP_FILES} ${FTT_UI_FILES})

file(GLOB TRANSLATIONS_FILES translations/*.ts)

if (UPDATE_TRANSLATIONS)
  qt5_create_translation(QTGUITUNE_QM ${FILES_TO_TRANSLATE} ${TRANSLATIONS_FILES})
else (UPDATE_TRANSLATIONS)
  qt5_add_translation(QTGUITUNE_QM ${TRANSLATIONS_FILES})
endif (UPDATE_TRANSLATIONS) 

add_custom_target(translations_target DEPENDS ${QTGUITUNE_QM})

qt5_wrap_cpp(QTGUITUNE_UI_HEADERS_MOC ${QTGUITUNE_UI_HEADERS})

#qt5_add_resources(QTGUITUNE_RSRCS ./qtguitune.qrc)
set(QTGUITUNE_RSRCS )

#qt5_add_translation(QTGUITUNE_QM ${QTGUITUNE_TRANS})

add_executable(qtguitune ${QTGUITUNE_SRCS} 
                         ${QTGUITUNE_RSRCS}
			             ${QTGUITUNE_QM}
			             ${QTGUITUNE_UI_SRCS}
			             ${QTGUITUNE_UI_HEADERS_MOC})

target_link_libraries(qtguitune ${Qt5Widgets_LIBRARIES} ${JACK_LIBRARIES} ${ALSA_LIBRARIES})

install(TARGETS qtguitune
        DESTINATION bin)

install(FILES ${QM_FILES} 
        DESTINATION ${CMAKE_INSTALL_PREFIX}/translations)
