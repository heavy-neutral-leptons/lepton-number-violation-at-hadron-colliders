# Copyright 2023-2026 Bruno M. S. Oliveira

# ----- Target -----
set(TARGET_NAME Common)

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(
    GLOB HEADER_LIST
    CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/${TARGET_NAME}/*.hpp"
)

add_library(${TARGET_NAME} STATIC ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
    ${TARGET_NAME}
    # ROOT
    PUBLIC ROOT::Core
    PUBLIC ROOT::Physics
    PUBLIC ROOT::ROOTDataFrame
)
