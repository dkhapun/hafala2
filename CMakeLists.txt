cmake_minimum_required(VERSION 3.1)
project(hafala2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    main_test.cpp
    )

add_executable(hafala2 ${SOURCE_FILES} main_test.h main_test.cpp api_test.h)