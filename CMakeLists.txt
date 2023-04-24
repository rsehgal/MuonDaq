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

# Find tinyxml2
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)
find_package(TinyXml2 REQUIRED)
include_directories(${TINYXML2_INCLUDE_DIR})

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
#message(STATUS ${ROOT_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/inc)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.h)

add_executable(runDaq runDaq.cpp ${sources} ${headers})
target_link_libraries(runDaq ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(stop test/stop.cpp ${sources} ${headers})
target_link_libraries(stop ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(connect test/connect.cpp ${sources} ${headers})
target_link_libraries(connect ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(connectBoard test/connectBoard.cpp ${sources} ${headers})
target_link_libraries(connectBoard ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(setparameters test/setparameters.cpp ${sources} ${headers})
target_link_libraries(setparameters ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(Test_RunNum test/Test_RunNum.cpp ${sources} ${headers})
target_link_libraries(Test_RunNum ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(WaveForm test/WaveForm.cpp ${sources} ${headers})
target_link_libraries(WaveForm ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})

add_executable(Test_ReadConf test/Test_ReadConf.cpp ${sources} ${headers})
target_link_libraries(Test_ReadConf ${ROOT_LIBRARIES} -pthread -lncurses ${TINYXML2_LIBRARIES})
