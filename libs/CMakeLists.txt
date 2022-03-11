cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OIS2 VERSION 1.0.0)

#set_target_properties(OIS2 PROPERTIES CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX "_d")
endif()

# Configure Library and Internal runtime versions
set(OIS2_MAJOR_VERSION 1)
set(OIS2_MINOR_VERSION 0)
set(OIS2_PATCH_VERSION 0)

set(OIS2_VERSION ${OIS2_MAJOR_VERSION}.${OIS2_MINOR_VERSION}.${OIS2_PATCH_VERSION})
set(OIS2_COMPATIBILITY_VERSION ${OIS2_MAJOR_VERSION}.${OIS2_MINOR_VERSION}.0)
set(OIS2_SOVERSION ${OIS2_MAJOR_VERSION}.${OIS2_MINOR_VERSION}.0)

set(OIS2_LIBRARY_NAME OIS2)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes/OIS2Prereqs.h.in ${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes/OIS2Prereqs.h @ONLY)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif()

option(OIS_BUILD_SHARED_LIBS "Build shared libraries" ON)
option(OIS_BUILD_DEMOS "Build demo applications" ON)
set(CMAKE_MACOSX_RPATH 0)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes
)

if (UNIX AND NOT APPLE)
	# Needed for gcc at least. maybe not clang?
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)

	# Could be optional - and, then disable watching udev for changes.
	# pretty much all Linux based OS's should have udev - even embedded
	find_library(UDEV_LIB udev REQUIRED)

	include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/linux
		${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/linux/x11
		${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/linux/libevent
	)
endif()

# Common lib Source files - all platforms
set(ois_source
    ${ois_source}
    "${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/OIS2DeviceDescriptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/OIS2InputManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/OIS2Logger.cpp"
)

source_group("OIS2" FILES ${ois_source})

add_library(OIS2 ${ois_source})

#target_link_libraries(Test PRIVATE Threads::Threads)
#target_link_libraries(Test PRIVATE udev)

# TODO: Investigate this cross platform installer
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
