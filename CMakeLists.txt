cmake_minimum_required(VERSION 3.10.0)
project(project)

# for integrating to ROS2, which needs C++14
set(CMAKE_CXX_STANDARD 14)

# set cmake build type
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
endif()

# set executable and library path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

find_package(catkin COMPONENTS rosbag dvs_msgs)
find_package(Boost REQUIRED COMPONENTS system thread date_time)

# append sources here
list(APPEND sources
    src/main.cpp
)

# append library here
list(APPEND thirdparty_libraries
    ${Boost_LIBRARIES}
    ${catkin_LIBRARIES}
)

# append include directories here
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${catkin_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
    ${sources}
)

target_link_libraries(${PROJECT_NAME}
    ${thirdparty_libraries}
)
