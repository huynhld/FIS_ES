file(GLOB SourceFiles "header/*.h" "model/*.cpp")

add_library(
    Net
    Net.cpp
)

add_library(
    TrainningData
    TrainningData.cpp
)

add_library(
    Neuron
    Neuron.cpp
)
