#[[
    File modified from:
    https://github.com/pananton/cpp-lib-template/blob/main/CMakeLists.txt

    cpp-lib-template licensed under Apache-2.0
]]

cmake_minimum_required(VERSION 3.14)
project(ignorelib
    VERSION 0.0.1
    DESCRIPTION ".gitignore-style file parser and pattern matcher"
    LANGUAGES CXX)

#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

include(cmake/utils.cmake)
include(GNUInstallDirs)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

# IGNORELIB_SHARED_LIBS option (undefined by default) can be used to force shared/static build
option(IGNORELIB_BUILD_TESTS "Build ignorelib tests" OFF)
option(IGNORELIB_BUILD_EXAMPLES "Build ignorelib examples" OFF)
option(IGNORELIB_BUILD_DOCS "Build ignorelib documentation" OFF)
option(IGNORELIB_INSTALL "Generate target for installing ignorelib" ${is_top_level})
set_if_undefined(IGNORELIB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/ignorelib" CACHE STRING
    "Install path for ignorelib package-related CMake files")

if(DEFINED IGNORELIB_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${IGNORELIB_SHARED_LIBS})
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_if_undefined(CMAKE_CXX_VISIBILITY_PRESET hidden)
set_if_undefined(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

add_library(ignorelib) # initialized below
add_library(ignorelib::ignorelib ALIAS ignorelib)

#----------------------------------------------------------------------------------------------------------------------
# ignorelib dependencies
#----------------------------------------------------------------------------------------------------------------------

# Search for your dependencies here

#----------------------------------------------------------------------------------------------------------------------
# ignorelib sources
#----------------------------------------------------------------------------------------------------------------------

include(GenerateExportHeader)
set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

generate_export_header(ignorelib EXPORT_FILE_NAME include/ignorelib/${export_file_name})

set(sources
#   include/ignorelib/export.h
    include/ignorelib/ignorelib.h
    src/ignorelib.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# ignorelib target
#----------------------------------------------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

target_sources(ignorelib PRIVATE ${sources})
target_compile_definitions(ignorelib PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:IGNORELIB_STATIC_DEFINE>")

target_include_directories(ignorelib
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(ignorelib PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

if(IGNORELIB_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    configure_package_config_file(cmake/ignorelib-config.cmake.in ignorelib-config.cmake
        INSTALL_DESTINATION "${IGNORELIB_INSTALL_CMAKEDIR}")

    write_basic_package_version_file(ignorelib-config-version.cmake
        COMPATIBILITY SameMajorVersion)

    install(TARGETS ignorelib EXPORT ignorelib_export
        RUNTIME COMPONENT ignorelib
        LIBRARY COMPONENT ignorelib NAMELINK_COMPONENT ignorelib-dev
        ARCHIVE COMPONENT ignorelib-dev
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(DIRECTORY include/
        TYPE INCLUDE
        COMPONENT ignorelib-dev)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/ignorelib/${export_file_name}"
        COMPONENT ignorelib-dev
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/ignorelib")

    set(targets_file "ignorelib-shared-targets.cmake")

    if(NOT BUILD_SHARED_LIBS)
        set(targets_file "ignorelib-static-targets.cmake")
    endif()

    install(EXPORT ignorelib_export
        COMPONENT ignorelib-dev
        FILE "${targets_file}"
        DESTINATION "${IGNORELIB_INSTALL_CMAKEDIR}"
        NAMESPACE ignorelib::)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ignorelib-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ignorelib-config-version.cmake"
        COMPONENT ignorelib-dev
        DESTINATION "${IGNORELIB_INSTALL_CMAKEDIR}")

    if(MSVC)
        set(pdb_file "")
        set(pdb_file_destination "")

        if(BUILD_SHARED_LIBS)
            set(pdb_file "$<TARGET_PDB_FILE:ignorelib>")
            set(pdb_file_destination "${CMAKE_INSTALL_BINDIR}")
        else()
            # TARGET_PDB_FILE does not work for pdb file generated for static library build, determining it manually
            set(pdb_file "$<TARGET_FILE_DIR:ignorelib>/$<TARGET_FILE_PREFIX:ignorelib>$<TARGET_FILE_BASE_NAME:ignorelib>.pdb")
            set(pdb_file_destination "${CMAKE_INSTALL_LIBDIR}")
        endif()

        install(FILES "${pdb_file}"
            COMPONENT ignorelib-dev
            CONFIGURATIONS Debug RelWithDebInfo
            DESTINATION "${pdb_file_destination}"
            OPTIONAL)
    endif()
endif()

#----------------------------------------------------------------------------------------------------------------------
# other targets
#----------------------------------------------------------------------------------------------------------------------

if(IGNORELIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(IGNORELIB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(IGNORELIB_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs include)
endif()
