file(GLOB SourceFiles "header/*.h" "src/*.cpp")

add_library(
    ExtractionAlgorithms
    Filter.cpp
    Minutiae.cpp
    CrossingNumber.cpp
)

add_library(
    ImageProcessing
    AdaptiveLocalThreshold.cpp
    GuoHall.cpp
    Ideka.cpp
    MaskGabor.cpp
    Functions.cpp
)

add_library(
    Others
    SQL.cpp
    synoapi.cpp
    Config.cpp
    PiNetwork.cpp
    logs.cpp
)