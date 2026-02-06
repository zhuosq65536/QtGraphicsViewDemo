cmake_minimum_required(VERSION 3.20)

project(qt_graphics_view_demo LANGUAGES CXX)

# 1. 基础配置：强制x64、C++17、Debug模式（新手先跑通Debug）
if (MSVC)
    set(CMAKE_GENERATOR_PLATFORM x64 CACHE STRING "" FORCE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 配置OpenCV（仅改这1行：你的OpenCV解压路径）
set(OpenCV_DIR "D:/ProgramData/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 3. 配置Qt5（仅改这1行：你的Qt5.15.1安装路径）
find_package(QT NAMES Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTOMOC ON) # 仅保留MOC，其他UIR/RCC关闭

# 4. 源文件列表（不变）
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/custom_graphics_view.cpp
    src/custom_graphics_view.h
    src/tools/itool.h
    src/tools/detection_result.h
    src/tools/line_tool.cpp
    src/tools/line_tool.h
    src/tools/point_tool.cpp
    src/tools/point_tool.h
    src/tools/circle_tool.cpp
    src/tools/circle_tool.h
)
add_executable(${PROJECT_NAME} ${SOURCES})

# 5. 链接库（仅链接核心库，自动匹配Debug/Release）
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    ${OpenCV_LIBS}
)

# 6. 自动复制OpenCV的DLL到输出目录（运行时不用手动拷贝）
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${OpenCV_DIR}/../bin/opencv_world4110d.dll" # Debug版DLL
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)