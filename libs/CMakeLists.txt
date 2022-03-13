cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OIS2 VERSION 1.0.0)

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
set(CMAKE_MACOSX_RPATH 0)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes
	${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes/internal
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
	"${CMAKE_CURRENT_SOURCE_DIR}/ois2/src/OIS2Tokenizer.cpp"
)

source_group("OIS2" FILES ${ois_source})

add_library(OIS2 ${ois_source})

if(UNIX)
	#if (NOT APPLE)
	#	add_dependencies(OIS2 X11)
	#	target_link_libraries(OIS2 X11)
	#endif()

	set_target_properties(OIS2 PROPERTIES
		VERSION ${OIS2_VERSION}
		SOVERSION ${OIS2_SOVERSION}
	)
endif()

if(UNIX)
	include(GNUInstallDirs)
	set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
else()
	set(LIB_INSTALL_DIR "lib")
endif()

set(OIS2_LIB_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})

file(GLOB PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/ois2/includes/*.h")

install(FILES ${PUBLIC_HEADERS}
	DESTINATION include/ois2
)

install(TARGETS OIS2
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
	FRAMEWORK DESTINATION Library/Frameworks
)

#target_link_libraries(Test PRIVATE Threads::Threads)
#target_link_libraries(Test PRIVATE udev)

# Test runner
source_group("Ois2Tests" FILES "${CMAKE_CURRENT_SOURCE_DIR}/ois2/tests/LoggerTests.cpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ois2/tests)
add_executable(Ois2Tests "${CMAKE_CURRENT_SOURCE_DIR}/ois2/tests/LoggerTests.cpp")
#set_target_properties(Ois2Tests PROPERTIES SOLUTION_FOLDER "OIS2 Console Demo")
target_link_libraries(Ois2Tests OIS2)
target_link_libraries(Ois2Tests Threads::Threads)
target_link_libraries(Ois2Tests udev)
