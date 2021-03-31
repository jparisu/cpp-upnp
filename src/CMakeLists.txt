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
cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE ALL_HEADERS
    ${PROJECT_SOURCE_DIR}/include/**/*.h
    ${PROJECT_SOURCE_DIR}/include/**/*.hpp
    ${PROJECT_SOURCE_DIR}/src/**/*.h
    ${PROJECT_SOURCE_DIR}/src/**/*.hpp
    )

set(${PROJECT_NAME}_SOURCES
    ${ALL_HEADERS}
    xml.cpp
    igd.cpp
    ssdp.cpp
    url.cpp
    parse_device.cpp
    )

# Create library
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Define public headers
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/upnp>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/upnp/detail>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/upnp/third_party>
        )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Boost::system
        Boost::coroutine
    PRIVATE
        ${THREAD_LIB}
)

target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_14
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
)
