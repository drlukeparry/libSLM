cmake_minimum_required(VERSION 3.0)

project(libSLM VERSION 0.2.6)

# Set c++ to use cx11 as a requirement
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cMake")
# Instruct CMake to run moc automatically when needed.

# Copy the include directory if specified
set(INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}/libSLM")

# Generate DLL Pragma for creating shared libraries in Windows
include (GenerateExportHeader)

add_definitions("-DPROJECT_VERSION=\"${PROJECT_VERSION}\"" )

option(BUILD_PYTHON "Builds a python extension" OFF)

if(WIN32)
    # Remove Security definitions for the library
    # Remove run time checks for windows
    IF(MSVC)

        set(COMMON_LANGUAGE_RUNTIME "")
        set(EIGEN3_INCLUDE_DIR External/eigen)

    endif(MSVC)

    # Copy the shared libraries and libs to the main directory
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

endif(WIN32)

if(UNIX)

    if(APPLE)

        # Apple Platforms

        set(EIGEN3_INCLUDE_DIR External/eigen)

    else(APPLE)

        # Other unix platform (i.e. Linux)

        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

        # Find Required packages
        find_package(Eigen3 3.3 REQUIRED)

    endif(APPLE)

endif(UNIX)


# Use the replacement of Boost::filesystem from a git submodule provided by WJakob
# in order to reduce compile time dependencies
# https://github.com/wjakob/filesystem

set(CPP_FILESYSTEM
    External/filesystem
)


if(BUILD_PYTHON)
    message(STATUS "Building libSLM Python Module")
    add_subdirectory(External/pybind11)
endif(BUILD_PYTHON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CPP_FILESYSTEM}
)

set(BASE_SRCS
#  Base/Tree.h
)

SOURCE_GROUP("Base" FILES ${BASE_SRCS})

set(APP_H_SRCS
    App/Header.h
    App/Layer.h
    App/Model.h
    App/Reader.h
    App/Writer.h
    App/Utils.h
)

set(APP_CPP_SRCS
    App/Layer.cpp
    App/Model.cpp
    App/Reader.cpp
    App/Writer.cpp
    App/Utils.cpp
)

set(APP_SRCS
    ${APP_H_SRCS}
    ${APP_CPP_SRCS}
)

SOURCE_GROUP("App" FILES
    ${APP_SRCS}
)

set(LIBSLM_SRCS
    ${BASE_SRCS}
    ${APP_SRCS}
)

if(BUILD_PYTHON)
    # Add the library
    add_library(SLM_static STATIC ${LIBSLM_SRCS})

    GENERATE_EXPORT_HEADER(SLM_static
                 BASE_NAME SLM
                 EXPORT_MACRO_NAME SLM_EXPORT
                 EXPORT_FILE_NAME SLM_Export.h
                 STATIC_DEFINE SLM_BUILT_AS_STATIC)


else(BUILD_PYTHON)
    message(STATUS "Building libSLM Python Module - Dynamic Library")
    add_library(SLM SHARED ${LIBSLM_SRCS})

    GENERATE_EXPORT_HEADER(SLM
                 BASE_NAME SLM
                 EXPORT_MACRO_NAME SLM_EXPORT
                 EXPORT_FILE_NAME SLM_Export.h
                 STATIC_DEFINE SLM_BUILT_AS_STATIC)

endif(BUILD_PYTHON)

set(App_SRCS
    main.cpp
)

SOURCE_GROUP("App" FILES ${App_SRCS})

add_subdirectory(Translators)

if(BUILD_PYTHON)
    set(LIBSLM_PY_SRCS

        python/libSLM/module.cpp
    )

    SOURCE_GROUP("Python" FILES ${LIBSLM_PY_SRCS})

    pybind11_add_module(slm ${LIBSLM_PY_SRCS})

    #add_library(example MODULE main.cpp)
    target_link_libraries(slm PRIVATE pybind11::module SLM_static)
    set_target_properties(slm PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")


     add_executable(main ${App_SRCS})
     target_link_libraries(main SLM_static MTT_static EOS_static SLMSol_static Realizer_static)

    #install(TARGETS slm DESTINATION lib/libSLM)

else()

    add_executable(main ${App_SRCS})
    target_link_libraries(main SLM EOS MTT Realizer SLMSol)

endif(BUILD_PYTHON)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SLM_Export.h
    ${APP_H_SRCS}
    DESTINATION include/libSLM
)

#install(TARGETS SLM DESTINATION lib/libSLM)
