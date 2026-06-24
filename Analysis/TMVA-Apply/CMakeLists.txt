# Copyright 2023-2026 Bruno M. S. Oliveira

# ----- Target -----
set(TARGET_NAME TMVA-Apply)

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${TARGET_NAME} ${SOURCE_LIST})

target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
    ${TARGET_NAME}
    # ROOT
    PUBLIC ROOT::Core
    PUBLIC ROOT::Physics
    PUBLIC ROOT::ROOTDataFrame
    # SPSS Searches
    PUBLIC Common
    PUBLIC Models
    PUBLIC ReconstructedData
    PUBLIC MachineLearning
)
