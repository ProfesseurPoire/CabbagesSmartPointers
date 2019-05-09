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


# We'll install the library in lib64 if we're in x64, in lib otherwise
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(arch_suffix "")
else()
    set(arch_suffix "64")
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
       $<INSTALL_INTERFACE:lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/include>)
     

#####################################################
# Installing the library and exporting the targets
#####################################################


include(CMakePackageConfigHelpers)

write_basic_package_version_file(${target_name}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(TARGETS ${target_name} EXPORT ${target_name}-target
    LIBRARY     DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/
    ARCHIVE     DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/
    RUNTIME     DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/
    INCLUDES    DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/include)

install(
    EXPORT  ${target_name}-target
    FILE    ${target_name}-target.cmake
    DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/cmake
)

configure_file(cmake/config.cmake.in ${target_name}Config.cmake @ONLY)

# Installs the config and config version file 
install(
    FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${target_name}ConfigVersion.cmake"  
    DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/cmake
)

# Installing header files
install(
    DIRECTORY include/
    DESTINATION lib${arch_suffix}/${target_name}-${PROJECT_VERSION}/include
    FILES_MATCHING PATTERN *.h)
    
#########################################
# Tests
#########################################



enable_testing()
add_subdirectory(test)
