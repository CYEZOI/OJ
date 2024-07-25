cmake_minimum_required(VERSION 3.9)
project(OJ)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")
include_directories(${PROJECT_SOURCE_DIR})

add_executable(OJ
    APIProceed.cpp
    Database.cpp
    EmailVerificationCodes.cpp
    Exception.cpp
    Files.cpp
    HTTPRequest.cpp
    HTTPResponse.cpp
    JudgingList.cpp
    Logger.cpp
    Passkey.cpp
    Problem.cpp
    Problems.cpp
    Regexes.cpp
    Settings.cpp
    Socket.cpp
    Submission.cpp
    Submissions.cpp
    TempTestData.cpp
    TestCase.cpp
    TestGroup.cpp
    TestGroupData.cpp
    Tokens.cpp
    Users.cpp
    Utilities.cpp
    WebDataProceed.cpp
    main.cpp)

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
