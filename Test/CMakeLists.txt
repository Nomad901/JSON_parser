cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
project(JSONParserTest)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src" 
                    "${CMAKE_CURRENT_SOURCE_DIR}/../include")

add_library(JSONParser_lib "${CMAKE_CURRENT_SOURCE_DIR}/../src/JSONParser.cpp")
target_include_directories(JSONParser_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src"
                                                 "${CMAKE_CURRENT_SOURCE_DIR}/../include")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1 
)
if(WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

add_executable("${PROJECT_NAME}" JSONParserTest.cpp)

target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src/JSONParser.h")
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include/JSON/")

target_link_libraries("${PROJECT_NAME}" PUBLIC JSONParser_lib GTest::gtest_main)

add_test(JSONParser_gtest JSONParser_lib)

add_test(NAME script_test 
         COMMAND "${CMAKE_CURRENT_LIST_DIR}/script_test.sh"
         WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")