# Michael Amrhein. Copyright (C) 2020.


#
# Project Properties
#
cmake_minimum_required (VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

project (fpdec)
set (LIBRARY_NAME "libfpdec")
set (APPLICATION_NAME "fpdec")
set (APPLICATION_CODENAME "${PROJECT_NAME}")
set (APPLICATION_COPYRIGHT_YEARS "2020")
set (APPLICATION_VERSION_MAJOR 0)
set (APPLICATION_VERSION_MINOR 1)
set (APPLICATION_VERSION_PATCH 0)
set (APPLICATION_VERSION_TYPE SNAPSHOT)
set (APPLICATION_VERSION_STRING "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}-${APPLICATION_VERSION_TYPE}")


#
# Build Options
#
set(PROJECT_BUILD_STATIC 0)
set(PROJECT_BUILD_SHARED 1)


#
# Debugging Options
#
set (CMAKE_VERBOSE_MAKEFILE 0) # Use 1 for debugging, 0 for release


#
# Project Output Paths
#
set (MAINFOLDER ${PROJECT_SOURCE_DIR})
set (EXECUTABLE_OUTPUT_PATH "${MAINFOLDER}/bin")
set (LIBRARY_OUTPUT_PATH "${MAINFOLDER}/lib")
set (CATCH_DIR "${MAINFOLDER}/thirdparty/catch")


#
# Project Search Paths
#
list (APPEND CMAKE_PREFIX_PATH "${MAINFOLDER}")
include_directories("${MAINFOLDER}/include")


#
# Add Build Targets
#
add_subdirectory(src)

enable_testing()
add_subdirectory(test)


#
# Add Install Targets
#
if (EXISTS "${MAINFOLDER}/include/${LIBRARY_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${LIBRARY_NAME}")
    install(DIRECTORY "${MAINFOLDER}/include/${LIBRARY_NAME}" DESTINATION "include")
endif (EXISTS "${MAINFOLDER}/include/${LIBRARY_NAME}" AND IS_DIRECTORY "${MAINFOLDER}/include/${LIBRARY_NAME}")
