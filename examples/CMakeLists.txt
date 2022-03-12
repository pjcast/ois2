cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OIS2DEMOS)

set(CMAKE_CXX_STANDARD 17)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/ois2/includes
)

find_package(Threads REQUIRED)
find_library(UDEV_LIB udev REQUIRED)
find_library(OIS2_LIB OIS2_d REQUIRED)

source_group("ConsoleApp" FILES "${CMAKE_CURRENT_SOURCE_DIR}/linux/console.cpp")
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
add_executable(ConsoleApp "${CMAKE_CURRENT_SOURCE_DIR}/linux/console.cpp")
set_target_properties(ConsoleApp PROPERTIES SOLUTION_FOLDER "OIS2 Console Demo")
target_link_libraries(ConsoleApp OIS2_d)
target_link_libraries(ConsoleApp Threads::Threads)
target_link_libraries(ConsoleApp udev)
