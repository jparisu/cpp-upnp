# Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

###############################################################################
# Configuration
###############################################################################
cmake_minimum_required(VERSION 3.13)

set(FORCE_CXX "14" CACHE STRING "C++ standard fulfillment selection")

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()

###############################################################################
# Version
###############################################################################
set(PRODUCT_MAJOR_VERSION 1)
set(PRODUCT_MINOR_VERSION 0)
set(PRODUCT_PATCH_VERSION 0)
set(PRODUCT_VERSION_STR ${PRODUCT_MAJOR_VERSION}.${PRODUCT_MINOR_VERSION}.${PRODUCT_PATCH_VERSION})

###############################################################################
# Project
###############################################################################
project(cppupnp)
set(PROJECT_VERSION_MAJOR "${PRODUCT_MAJOR_VERSION}")
set(PROJECT_VERSION_MINOR "${PRODUCT_MINOR_VERSION}")
set(PROJECT_VERSION_PATCH "${PRODUCT_PATCH_VERSION}")
set(PROJECT_VERSION "${PRODUCT_VERSION_STR}")

set(PROJECT_NAME_LARGE "CPP UPnP")
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
set(${PROJECT_NAME}_DESCRIPTION_SUMMARY "C++ library for bridging UPnP protocol main features")

message(STATUS "Configuring ${PROJECT_NAME_LARGE}")
message(STATUS "Version: ${PROJECT_VERSION}")

###############################################################################
# Installation paths
###############################################################################
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C++ headers")
set(LIB_INSTALL_DIR lib${LIB_SUFFIX}/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")

###############################################################################
# Boost requirements
###############################################################################
if (NOT Boost_USE_STATIC_LIBS)
    find_package(Threads REQUIRED)
    find_package(Boost 1.71 REQUIRED COMPONENTS coroutine system)
    set(THREAD_LIB "Threads::Threads")
else()
    # When linking with static Boost, we need to link with libboost_thread.
    find_package(Threads REQUIRED)
    find_package(Boost 1.71 REQUIRED COMPONENTS thread coroutine system)
    # Boost::thread adds Threads::Threads automatically
    set(THREAD_LIB "Boost::thread")
endif()

###############################################################################
# Install headers
###############################################################################
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
    ${ARGN}
    DESTINATION ${INCLUDE_INSTALL_DIR}/${PROJECT_NAME}
    COMPONENT headers
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    )

get_property(${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES GLOBAL PROPERTY ${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES_PROPERTY)
set(${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES ${${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES} ${PROJECT_SOURCE_DIR}/include)
set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES_PROPERTY ${${PROJECT_NAME}_PUBLIC_HEADERS_DIRECTORIES})

###############################################################################
# Compile src
###############################################################################
add_subdirectory(src)
set(cppupnp_FOUND True)

###############################################################################
# Compile examples
###############################################################################
add_subdirectory(example)
