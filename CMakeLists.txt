# CMakeLists.txt (Root)

cmake_minimum_required(VERSION 3.21)
project(VirtualEncryptedDisk VERSION 1.0.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(WDK REQUIRED)

add_subdirectory(TESTDriver)

