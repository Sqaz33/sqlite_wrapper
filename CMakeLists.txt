# Option DB_TEST=ON -> add gtest for lib

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(sqlite3_wrapper)

find_package(SQLite3 REQUIRED)

# option(DB_TEST ON)

if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

file(GLOB WRAPPER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${WRAPPER_SRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLite::SQLite3)
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (DB_TEST)
    enable_testing()
    add_subdirectory("test")
endif()