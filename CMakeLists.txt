# CMakeLists.txt (Root)

cmake_minimum_required(VERSION 3.21)
project(VirtualEncryptedDisk VERSION 1.0.0)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(WDK REQUIRED)

add_subdirectory(VirtualEncryptedDiskDriver)
add_subdirectory(VirtualEncryptedDiskUserMode)

if(MSVC)

    # Set the character set for the project (Use "UNICODE" for Unicode or "MultiByte" for Multibyte)
    set_property(TARGET VirtualEncryptedDiskDriver PROPERTY CHARACTER_SET "MultiByte")   # For Win32/Multibyte
    set_property(TARGET VirtualEncryptedDiskDriver PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/build/VirtualEncryptedDiskDriver/$(Platform)/$(ConfigurationName)>)
	set_property(TARGET VirtualEncryptedDiskDriver PROPERTY	ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/build/VirtualEncryptedDiskDriver/$(Platform)/$(ConfigurationName)>)
    set_property(TARGET VirtualEncryptedDiskDriver PROPERTY LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/build/VirtualEncryptedDiskDriver/$(Platform)/$(ConfigurationName)>)
    set_property(TARGET VirtualEncryptedDiskDriver PROPERTY ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/build/VirtualEncryptedDiskDriver/$(Platform)/$(ConfigurationName)>)
    # Set other general properties, if needed...
endif()
