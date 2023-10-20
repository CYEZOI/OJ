cmake_minimum_required(VERSION 3.9)
project(OJ)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

add_executable(main
    APIProceed.cpp
    Database.cpp
    EmailVerificationCodes.cpp
    Files.cpp
    HTTPRequest.cpp
    HTTPResponse.cpp
    JudgingList.cpp
    Logger.cpp
    Problem.cpp
    Problems.cpp
    Regexes.cpp
    Result.cpp
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
)
target_link_libraries(main PRIVATE ${LIBS})
target_include_directories(main PRIVATE "/usr/include/mysql")
