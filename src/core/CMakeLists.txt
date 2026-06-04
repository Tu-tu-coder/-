file(GLOB CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/skills/*.cpp
)

# OBJECT library ensures ALL object files are linked,
# so REGISTER_SKILL static initializers are never stripped
add_library(tetris_core OBJECT
    ${CORE_SOURCES}
)

target_include_directories(tetris_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tetris_core PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
