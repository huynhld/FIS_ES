cmake_minimum_required (VERSION 2.6)
project (FIS)
find_package(OpenCV REQUIRED)
add_definitions( "-std=c++11" )

set(MY_DATABASE_FILE ${PROJECT_SOURCE_DIR}/database/Minutiae.db)

file(COPY ${MY_DATABASE_FILE} DESTINATION ${CMAKE_BINARY_DIR})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

include_directories ("${PROJECT_SOURCE_DIR}/model")
add_subdirectory(model)


add_executable(np np.cpp)
add_executable(main main.cpp)
add_executable(InsertData InsertDataToSQL.cpp)

target_link_libraries( main ${OpenCV_LIBS} ${SQLITE3_LIBRARIES} ExtractionAlgorithms ThinningAlgorithms BinarisationAlgorithms Functions OptimisationAlgorithms SQL sqlite3 Synoapi )
target_link_libraries( InsertData ${OpenCV_LIBS} ${SQLITE3_LIBRARIES} ExtractionAlgorithms ThinningAlgorithms BinarisationAlgorithms Functions OptimisationAlgorithms SQL sqlite3 Synoapi )
target_link_libraries( np TrainningData Net Neuron)





