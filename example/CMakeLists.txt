cmake_minimum_required(VERSION 3.13)

project(examples)

if(NOT cppupnp_FOUND)
    find_package(cppupnp REQUIRED)
endif()

# Native example
# add_executable            (add-port-mapping ./add-port-mapping.cpp)
# target_compile_features   (add-port-mapping PRIVATE cxx_std_17)
# target_include_directories(add-port-mapping PRIVATE cppupnp)
# target_link_libraries     (add-port-mapping cppupnp)

# jparisu example
add_executable(port-forwarding ./main.cpp)
target_compile_features(port-forwarding PRIVATE cxx_std_17)
target_include_directories(port-forwarding PRIVATE cppupnp)
target_link_libraries(port-forwarding cppupnp)
