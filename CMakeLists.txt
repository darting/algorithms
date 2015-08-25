cmake_minimum_required (VERSION 2.8.7)
project (algorithms)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(DEL ${EXECUTABLE_OUTPUT_PATH})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${warnings}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings}")

include_directories(./includes)
enable_testing(true)

foreach(D ${DEL})
    if(EXISTS ${D})
        file(REMOVE_RECURSE ${D})
    endif()
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB APP_SOURCES src/*.cc)
foreach(appsourcefile ${APP_SOURCES})
  get_filename_component(test_name ${appsourcefile} NAME_WE)
    add_executable(${test_name} ${appsourcefile})
    add_test(${test_name} "${EXECUTABLE_OUTPUT_PATH}/${test_name}" "-r xml")
endforeach(appsourcefile ${APP_SOURCES})
