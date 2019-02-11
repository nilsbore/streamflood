cmake_minimum_required(VERSION 2.8.3)
project(StreamFlood)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

#add_subdirectory(external)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  external/shelf-pack-cpp/include
  ${CURSES_INCLUDE_DIR}
)

## Declare a C++ library
add_library(streamflood
  src/streamflood.cpp
)

target_include_directories(streamflood PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(streamflood -lncursesw)

#add_executable(test_monitor src/test.cpp external/libfort/lib/fort.c)

#target_link_libraries(test_monitor termcolor yaml-cpp ${catkin_LIBRARIES} ${CURSES_LIBRARIES})
#target_link_libraries(test_monitor termcolor yaml-cpp streamflood ${catkin_LIBRARIES})

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS streamflood EXPORT StreamFloodConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT StreamFloodConfig DESTINATION share/StreamFlood/cmake)

# This makes the project importable from the build directory
export(TARGETS streamflood FILE StreamFloodConfig.cmake)
