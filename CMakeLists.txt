# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.0)

# 设置项目名称
project(NMEAi)

# 将所有源文件存储在变量中
file(GLOB SOURCES_FILES 
    "${PROJECT_SOURCE_DIR}/src/*.cpp" 
    "${PROJECT_SOURCE_DIR}/src/*.h" 
)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES_FILES})
