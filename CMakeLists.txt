#MAKE

set(CMAKE_CXX_COMPILER g++)
set(BUILD_SHARED_LIBS ON)

cmake_minimum_required(VERSION 3.11)
project(mymoulette LANGUAGES CXX)
option(BUILD_DEBUG "Build with debug compile options." OFF)
option(BUILD_TEST "Build with the package as well as the tests." OFF)

find_program(REQUIRED go)
find_library(REQUIRED libcurl)

include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
    GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

execute_process(COMMAND go env GOHOSTARCH OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND go env GOHOSTOS OUTPUT_VARIABLE OS OUTPUT_STRIP_TRAILING_WHITESPACE)
add_compile_definitions(OS="${OS}" ARCH="${ARCH}")

if (BUILD_DEBUG)
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c++17 -g3 -fno-inline -O0)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c++17 -O3)
endif ()

include_directories(src)
add_subdirectory(src)

add_executable(mymoulette src/mymoulette.cc)

target_link_libraries(mymoulette
        capabilities
        cgroups
        isolate
        options
)

target_include_directories(mymoulette
    INTERFACE ${CMAKE_ROOT}/src/
)

    #add_custom_command(TARGET mymoulette POST_BUILD
    #COMMAND setcap cap_dac_override=ep ./mymoulette
    #COMMENT "Setting capabilities"
    #)

if (BUILD_TEST)
    add_subdirectory(tests)
endif (BUILD_TEST)
