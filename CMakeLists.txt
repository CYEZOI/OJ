cmake_minimum_required(VERSION 3.9)
project(OJ)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SRCS CONFIGURE_DEPENDS Source/*.cpp)
add_executable(OJ ${SRCS})

set(LIBS
    pthread
    curl
    mysqlcppconn
    ssl
    crypto
    minizip
)
target_link_libraries(OJ PRIVATE ${LIBS})
target_include_directories(OJ PRIVATE "/usr/include/mysql")
