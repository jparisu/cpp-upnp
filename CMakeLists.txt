cmake_minimum_required(VERSION 3.15)

project(cpp-upnp)

add_executable            (test-url ./test-url.cpp ../src/url.cpp)
target_include_directories(test-url PRIVATE ../include)


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
cmake_minimum_required(VERSION 3.15)

set(FORCE_CXX "14" CACHE STRING "C++ standard fulfillment selection")

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()

project("cpp-upnp")

###############################################################################
# Installation paths
###############################################################################
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C++ headers")
set(LIB_INSTALL_DIR lib${LIB_SUFFIX}/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")

###############################################################################
# Compile examples
###############################################################################

add_subdirectory(example)
