cmake_minimum_required(VERSION 3.21)

PROJECT(dmlinq)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

SET(DMLINQ_VERSION_MAJOR "1")
SET(DMLINQ_VERSION_MINOR "0")
SET(DMLINQ_VERSION_PATCH "1")
SET(DMLINQ_VERSION "${DMLINQ_VERSION_MAJOR}.${DMLINQ_VERSION_MINOR}.${DMLINQ_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMLINQ_VERSION}")

OPTION(USE_DMLINQ "use dmlinq" OFF)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/dmlinq_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/dmlinq_config.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

ModuleImportAll("thirdparty")

LibImport("libdmlinq" "src/libdmlinq")

if(PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "libdmlinq;dmtest")
    ExeImport("examples" "libdmlinq")
endif()

AddInstall("libdmlinq" "${CMAKE_CURRENT_SOURCE_DIR}/include")
