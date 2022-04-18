cmake_minimum_required(VERSION 3.2)

PROJECT(dmlinq)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

SET(DMLINQ_VERSION_MAJOR "1")
SET(DMLINQ_VERSION_MINOR "0")
SET(DMLINQ_VERSION_PATCH "1")
SET(DMLINQ_VERSION "${DMLINQ_VERSION_MAJOR}.${DMLINQ_VERSION_MINOR}.${DMLINQ_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${DMLINQ_VERSION}")

OPTION(USE_DMLINQ "use dmlinq" OFF)

ModuleImportAll("thirdparty")

ExeImport("test" "dmgmock;benchmark_main")

#ADD_CUSTOM_COMMAND(
#TARGET dmlinqtest
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different
#${CMAKE_CURRENT_SOURCE_DIR}/config/test.csv
#$<TARGET_FILE_DIR:dmlinqtest>
#)

#INSTALL(DIRECTORY include/ DESTINATION include)
#INSTALL(TARGETS dmlinq
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

#SET(DMLINQ_LIBS "dmlinq")
#MARK_AS_ADVANCED(${DMLINQ_LIBS})
