file(GLOB SourceFiles "header/*.h" "src/*.cpp")

add_library(
    ExtractionAlgorithms
    Filter.cpp
    Minutiae.cpp
    CrossingNumber.cpp
)

add_library(
    BinarisationAlgorithms
    AdaptiveLocalThreshold.cpp
)

add_library(
    ThinningAlgorithms
    GuoHall.cpp
)

add_library(
    OptimisationAlgorithms
    Ideka.cpp
)

add_library(
    SQL
    SQL.cpp
)

add_library(
    Functions
    Functions.cpp
)

add_library(
    Synoapi
    synoapi.cpp
)

add_library(
    MaskGabor
    MaskGabor.cpp
)
