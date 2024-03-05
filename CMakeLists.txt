cmake_minimum_required(VERSION 3.10)
project(screenshot)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(OpenCV_STATIC ON)
set(CMAKE_EXE_LINKER_FLAGS "-static")
# 注释下面为了给别人用
set(CMAKE_PREFIX_PATH  "F:/c/C++/install/Qt6.2.7-Windows-x86_64-MinGW11.2.0-staticFull;F:/c/C++/install/opencv_static")
find_package(OpenCV REQUIRED )
find_package(Iconv REQUIRED)
find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

# 添加资源文件
file(GLOB_RECURSE QRC_SOURCE_FILES assert/*.qrc)

file(GLOB_RECURSE srcs ./src/*.cpp include/*.h main.cpp)
message("要编译的文件有"${srcs})
add_executable(screenshot WIN32)
target_sources(screenshot PUBLIC ${srcs} ${QRC_SOURCE_FILES})
target_include_directories(screenshot PUBLIC ./include)

target_include_directories(screenshot PUBLIC "F:/c/C++/install/Qt6.2.7-Windows-x86_64-MinGW11.2.0-staticFull/include/QtWidgets")
target_link_libraries(screenshot PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
target_link_libraries(screenshot PRIVATE ${OpenCV_LIBS})


