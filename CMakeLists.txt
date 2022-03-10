cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Test VERSION 0.1.0)

include(CTest)
enable_testing()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_library(UDEV_LIB udev REQUIRED)

add_executable(Test 
	main.cpp
	OIS2InputManager.cpp
	OIS2DeviceDescriptor.cpp
	OIS2Logger.cpp)

target_link_libraries(Test PRIVATE Threads::Threads)
target_link_libraries(Test PRIVATE udev)
set_target_properties(Test PROPERTIES CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
