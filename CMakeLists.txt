cmake_minimum_required(VERSION 3.0)

project(libSLM)

# Set c++ to use cx11 as a requirement
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cMake")
# Instruct CMake to run moc automatically when needed.


#SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
#SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Copy the include directory if specified
set(INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}/libSLM")

# Generate DLL Pragma for creating shared libraries in Windows
include (GenerateExportHeader)

option(BUILD_PYTHON "Builds a python extension" ON)

if(WIN32)
    # Remove Security definitions for the library
    # Remove run time checks for windows
    IF(MSVC)
      STRING(REPLACE "/EHsc" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
      STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr /INCREMENTAL:NO /bigobj /arch:AVX2")
      add_definitions(-D_SECURE_SCL=0 )
    endif(MSVC)

    # Copy the shared libraries and libs to the main directory
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

else(WIN32)    

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # Find Required packages
    find_package(Eigen3 3.3 REQUIRED)


endif(WIN32)


# Use the replacement of Boost::filesystem from a git submodule provided by WJakob
# in order to reduce compile time dependencies
# https://github.com/wjakob/filesystem

set(CPP_FILESYSTEM
    External/filesystem
)



if(BUILD_PYTHON)
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

set(APP_SRCS
    App/Header.h
#    App/Iterator.cpp
#    App/Iterator.h
    App/Layer.cpp
    App/Layer.h
    App/Model.cpp
    App/Model.h
    App/Reader.cpp
    App/Reader.h
#    App/Slm.cpp
#    App/Slm.h
    App/Writer.cpp
    App/Writer.h
)


SOURCE_GROUP("App" FILES ${APP_SRCS})

set(LIBSLM_SRCS
    ${BASE_SRCS}
    ${APP_SRCS}
)

# Add the library
add_library(SLM SHARED ${LIBSLM_SRCS})

GENERATE_EXPORT_HEADER(SLM
             BASE_NAME SLM
             EXPORT_MACRO_NAME SLM_EXPORT
             EXPORT_FILE_NAME SLM_Export.h
             STATIC_DEFINE SLM_BUILT_AS_STATIC
)


SOURCE_GROUP("App" FILES ${App_SRCS})
target_link_libraries(SLM)

add_subdirectory(Translators)


if(BUILD_PYTHON)
    #add_subdirectory(External/pybind11)

    set(LIBSLM_PY_SRCS
        Python/module.cpp
    )
    
    SOURCE_GROUP("Python" FILES ${LIBSLM_PY_SRCS})

    pybind11_add_module(slm ${LIBSLM_PY_SRCS})

    #add_library(example MODULE main.cpp)
    target_link_libraries(slm PRIVATE pybind11::module SLM MTT)
    set_target_properties(slm PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                            SUFFIX "${PYTHON_MODULE_EXTENSION}")


endif(BUILD_PYTHON)

set(App_SRCS
    main.cpp
)

add_executable(main ${App_SRCS})
target_link_libraries(main SLM MTT)

install(DIRECTORY App Base Translators DESTINATION include/libSLM
        FILES_MATCHING PATTERN "*.h")

install(FILES
 ${PROJECT_BINARY_DIR}/SLM_Export.h DESTINATION include/libSLM
)

install(TARGETS SLM DESTINATION lib/libSLM)
