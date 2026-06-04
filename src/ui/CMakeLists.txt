file(GLOB UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/effects/*.cpp
)

add_library(tetris_ui OBJECT
    ${UI_SOURCES}
)

target_include_directories(tetris_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
)

target_link_libraries(tetris_ui PUBLIC
    tetris_core
    Qt${QT_VERSION_MAJOR}::Widgets
)
