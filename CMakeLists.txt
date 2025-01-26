cmake_minimum_required(VERSION 3.10)   # 指定最低 CMake 版本

project(MidQuantization)          # 定义项目名称和版本

# 设置 C++ 标准为 C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/midifile/include)
include_directories(${PROJECT_SOURCE_DIR}/midifile/src)

# 添加源文件
add_executable(MidQuantization main.cpp)