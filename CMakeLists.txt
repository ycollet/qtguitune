#=============================================================================
#  QtGuiTune
#  Qt Guitar Tuner
#  $Id:$
#
#  Copyright (C) 2013 by Yann Collette and others
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

project(QtGuiTune C CXX)

cmake_minimum_required(VERSION 2.8)

option(ENABLE_JACK "Enable jack (for GUI debugging, turn this option to off)" ON)
option(ENABLE_DEBUG_MESSAGES "Enable debug messages" OFF)

if (ENABLE_DEBUG_MESSAGES)
  add_definitions("-DQT_NO_DEBUG_OUTPUT")
endif (ENABLE_DEBUG_MESSAGES)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(ALSA REQUIRED)

find_package(Qt4 REQUIRED)
#find_package(Jack REQUIRED)

include(${QT_USE_FILE})
include_directories("${CMAKE_SOURCE_DIR}/src"
                    "${CMAKE_BINARY_DIR}"
		    "${ALSA_INCLUDE_DIRS}")

if (ENABLE_JACK)
  add_definitions("-DENABLE_JACK")
endif (ENABLE_JACK)

add_subdirectory(src)
