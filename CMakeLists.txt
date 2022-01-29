#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(cozmonaut)

add_subdirectory(third_party/fmt)
add_subdirectory(third_party/spdyface)

find_package(PythonInterp 3.7 REQUIRED)
find_package(PythonLibs 3.7 REQUIRED)
find_package(Threads REQUIRED)

set(cozmo_SRC_FILES
        src/client.c
        src/cozmo_image.cpp
        src/log.cpp
        src/main.c
        src/service.c
        src/tracker.c
        )

add_executable(cozmo ${cozmo_SRC_FILES})
set_target_properties(cozmo PROPERTIES C_STANADRD 99 CXX_STANDARD 17)
target_include_directories(cozmo PRIVATE third_party ${PYTHON_INCLUDE_DIR})
target_link_libraries(cozmo PRIVATE fmt::fmt-header-only spdyface ${PYTHON_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
