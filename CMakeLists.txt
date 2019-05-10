cmake_minimum_required (VERSION 3.9.0)

project(CabbagesSmartPointers VERSION 1.0.0)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

# I basically build a second library version for the debug configuration
# This might not work if you generate a visual studio solution since
# multiconfig and whatnot but that'll do for now
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(debug_suffix "d")
endif()

# We'll install the library in lib64 if we're in x64, in lib otherwise
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(arch_suffix "64")
endif()

set(target_name ${PROJECT_NAME}${debug_suffix})
set(install_folder lib${arch_suffix}/${target_name}-${PROJECT_VERSION})

add_library(${target_name} INTERFACE) 

target_include_directories( ${target_name} INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${install_folder}/include>)

enable_testing()
add_subdirectory(test)

#####################################################
# Installing the library and exporting the targets
#####################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(	${target_name}ConfigVersion.cmake
                                    VERSION ${PROJECT_VERSION}
                                    COMPATIBILITY SameMajorVersion)

install(	TARGETS		${target_name} EXPORT ${target_name}-target
            LIBRARY		DESTINATION ${install_folder}
            ARCHIVE     DESTINATION ${install_folder}
            RUNTIME     DESTINATION ${install_folder}
            INCLUDES    DESTINATION ${install_folder}include)

# Installs the packaging information
install(	EXPORT  ${target_name}-target
            FILE    ${target_name}-target.cmake
            DESTINATION ${install_folder}/cmake)

configure_file(cmake/config.cmake.in ${target_name}Config.cmake @ONLY)

# Installs the config and config version file 
install(	FILES 
            "${CMAKE_CURRENT_BINARY_DIR}/${target_name}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${target_name}ConfigVersion.cmake"  
            DESTINATION ${install_folder}/cmake)

# Installs the header files
install(	DIRECTORY include/
            DESTINATION ${install_folder}/include
            FILES_MATCHING PATTERN *.h)