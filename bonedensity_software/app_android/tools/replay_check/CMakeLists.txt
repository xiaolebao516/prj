cmake_minimum_required(VERSION 3.16)
project(bonedensity_replay_check LANGUAGES CXX)

# Host-side check that the desktop core builds with CMake + clang and gives the
# same per-frame results as the desktop app. The corebridge module of the
# Android project reuses the same core.pri parsing.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BONE_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../app_win" CACHE PATH "bonedensity_software/app_win directory")
set(BONE_CORE_DIR "${BONE_APP_DIR}/src/core")

# core.pri is the single source list: read its SOURCES block so a file added
# for the desktop app is picked up here without a second list.
file(READ "${BONE_CORE_DIR}/core.pri" BONE_CORE_PRI)
# Only the SOURCES block lists .cpp files, so matching them in the whole file
# also works with CRLF line endings.
string(REGEX MATCHALL "\\$\\$PWD/[A-Za-z0-9_./-]+\\.cpp" BONE_SOURCES_RAW "${BONE_CORE_PRI}")
set(BONE_CORE_SOURCES "")
foreach(src IN LISTS BONE_SOURCES_RAW)
    string(REPLACE "$$PWD" "${BONE_CORE_DIR}" src "${src}")
    list(APPEND BONE_CORE_SOURCES "${src}")
endforeach()
list(LENGTH BONE_CORE_SOURCES BONE_CORE_COUNT)
if(BONE_CORE_COUNT EQUAL 0)
    message(FATAL_ERROR "No SOURCES found in ${BONE_CORE_DIR}/core.pri")
endif()
message(STATUS "core.pri: ${BONE_CORE_COUNT} sources")

find_package(Qt6 REQUIRED COMPONENTS Core Xml)

add_library(bonecore STATIC ${BONE_CORE_SOURCES})
target_include_directories(bonecore PUBLIC "${BONE_CORE_DIR}")
target_link_libraries(bonecore PUBLIC Qt6::Core Qt6::Xml)

add_executable(replay_check replay_check.cpp)
target_link_libraries(replay_check PRIVATE bonecore)
