# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# 编译参数
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

#源文件
aux_source_directory(. src)
aux_source_directory(./common src)
aux_source_directory(./samples src)

#生成库
set(lib-name native-lib)
add_library(${lib-name} SHARED ${src})
target_link_libraries(
        ${lib-name}
        log
        android
        EGL
        GLESv3
)