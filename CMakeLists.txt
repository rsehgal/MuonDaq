cmake_minimum_required(VERSION 2.8)
project(PositionEstimation)

#Added to detect compiler support for C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
#message(STATUS ${ROOT_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.h)
add_executable(main main.cpp ${sources} ${headers})
target_link_libraries(main ${ROOT_LIBRARIES} -pthread -lncurses)

add_executable(stop stop.cpp ${sources} ${headers})
target_link_libraries(stop ${ROOT_LIBRARIES} -pthread -lncurses)
