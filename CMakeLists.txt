cmake_minimum_required(VERSION 2.8.4)

project(worktime)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)

file(GLOB_RECURSE WT_INCLUDES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE WT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE_DIR}/*.cpp)

set(CMAKE_CXX_FLAGS "-fPIC -std=c++11 -stdlib=libc++ -Wall ${CMAKE_CXX_FLAGS} " CACHE STRING "C++ FLAGS" FORCE)

include_directories(${INCLUDE_DIR})
include_directories(/usr/local/include/)

# Toolkit library
add_executable(${PROJECT_NAME} ${WT_INCLUDES} ${WT_SRCS})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

link_directories(/usr/local/lib/)
target_link_libraries(${PROJECT_NAME} toolkit)
target_link_libraries(${PROJECT_NAME} boost_system boost_date_time)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin/)