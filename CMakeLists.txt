cmake_minimum_required(VERSION 2.8.4)
project(pathtracer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++-5)

if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wno-narrowing -Ofast -march=native -m64 -O3 -static -funroll-loops")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb3")
endif()

FIND_PACKAGE( OpenMP)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    add_definitions(-D_OPEN_MP_)
endif()

#find_package(OpenCV REQUIRED)
#message(STATUS "OpenCV version: ${OpenCV_VERSION}")
#if(NOT OpenCV_FOUND)
    #message(FATAL_ERROR "OpenCV not found")
#endif()
include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(SOURCE_FILES
    src/main.cpp
    src/Objects.cpp
    src/Kdtree.cpp
    src/Camera.cpp
    src/Scene.cpp
    src/Renderer.cpp
    src/Timer.cpp
    lib/lodepng/lodepng.cpp
    lib/tiny_obj_loader/tiny_obj_loader.cc)

add_executable(pathtracer ${SOURCE_FILES})
#target_link_libraries(pathtracer ${OpenCV_LIBS})
