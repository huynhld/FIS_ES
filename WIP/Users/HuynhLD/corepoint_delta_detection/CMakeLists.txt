cmake_minimum_required (VERSION 2.6)
project (CalPointCare)
find_package(OpenCV REQUIRED)
add_definitions( "-std=c++11" )

add_executable(exec main.cpp)
target_link_libraries( exec ${OpenCV_LIBS} )
