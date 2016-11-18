cmake_minimum_required (VERSION 2.6)
project (FIS)
find_package(OpenCV REQUIRED)
add_definitions( "-std=c++11" )

set(MY_DATABASE_FILE ${PROJECT_SOURCE_DIR}/database/Minutiae.db)

file(COPY ${MY_DATABASE_FILE} DESTINATION ${CMAKE_BINARY_DIR})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

add_executable(FIS FIS.cpp)
add_executable(InsertData InsertDataToSQL.cpp)
target_link_libraries( FIS ${OpenCV_LIBS} ${SQLITE3_LIBRARIES} ExtractionAlgorithms ImageProcessing Others sqlite3 wiringPi wiringPiDev)
target_link_libraries( InsertData ${OpenCV_LIBS} ${SQLITE3_LIBRARIES} ExtractionAlgorithms ImageProcessing Others sqlite3 wiringPi wiringPiDev)
