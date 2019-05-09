cmake_minimum_required (VERSION 3.9.0)

project(CabbagesSmartPointers VERSION 1.0.0)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

# I basically build a second library version for the debug configuration
# This might not work if you generate a visual studio solution since
# multiconfig and whatnot but that'll do for now
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(debug_suffix "d")
else()
    set(debug_suffix "")
endif()

set(target_name ${PROJECT_NAME}${debug_suffix})

add_library(${target_name} INTERFACE) 

# Public    : Used by the current target and the ones consuming the target
# Private   : Only used by the current target
# Interface : Only used by the targets consuming the current target
# BUILD_INTERFACE and INSTALL_INTERFACE are here to specify which path to
# use depending on if the library is installed or not
target_include_directories( ${target_name} INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

enable_testing()
add_subdirectory(test)
