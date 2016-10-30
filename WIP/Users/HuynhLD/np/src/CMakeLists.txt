cmake_minimum_required (VERSION 2.6)
project (Trainning)
add_definitions( "-std=c++11" )


include_directories ("${PROJECT_SOURCE_DIR}/model")
add_subdirectory(model)

add_executable(exec main.cpp)
target_link_libraries(exec TrainningData Net Neuron)