cmake_minimum_required(VERSION 3.16)
project(TetrisSkillBattle VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)          # Auto-run moc on QObject headers
set(CMAKE_AUTORCC ON)          # Auto-compile .qrc resource files

# Try Qt6 first, fall back to Qt5
find_package(Qt6 QUIET COMPONENTS Widgets Core Gui)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Core Gui)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

add_subdirectory(src/core)
add_subdirectory(src/ui)

# Main executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/data/resources.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    tetris_core
    tetris_ui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)

# Copy skills.json to build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/data/skills.json
     DESTINATION ${CMAKE_BINARY_DIR})

# Windows: set subsystem to avoid console window popup
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
