cmake_minimum_required (VERSION 3.0.0)
project (algorithms)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(DEL ${EXECUTABLE_OUTPUT_PATH})

foreach(D ${DEL})
    if(EXISTS ${D})
        file(REMOVE_RECURSE ${D})
    endif()
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB APP_SOURCES src/*.cc)
foreach(appsourcefile ${APP_SOURCES})
  get_filename_component(test_name ${appsourcefile} NAME_WE)
	add_executable("${test_name}" ${appsourcefile})
endforeach(appsourcefile ${APP_SOURCES})