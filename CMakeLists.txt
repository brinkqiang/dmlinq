cmake_minimum_required(VERSION 3.2)

PROJECT(dmboolinq)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

SET(DMBOOLINQ_VERSION_MAJOR "1")
SET(DMBOOLINQ_VERSION_MINOR "0")
SET(DMBOOLINQ_VERSION_PATCH "1")
SET(DMBOOLINQ_VERSION "${DMBOOLINQ_VERSION_MAJOR}.${DMBOOLINQ_VERSION_MINOR}.${DMBOOLINQ_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMBOOLINQ_VERSION}")

OPTION(USE_DMBOOLINQ "use dmboolinq" OFF)

ModuleImportAll("thirdparty")

ExeImport("test" "dmgmock;benchmark_main")

#ADD_CUSTOM_COMMAND(
#TARGET dmboolinqtest
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different
#${CMAKE_CURRENT_SOURCE_DIR}/config/test.csv
#$<TARGET_FILE_DIR:dmboolinqtest>
#)

#INSTALL(DIRECTORY include/ DESTINATION include)
#INSTALL(TARGETS dmboolinq
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

#SET(DMBOOLINQ_LIBS "dmboolinq")
#MARK_AS_ADVANCED(${DMBOOLINQ_LIBS})
