# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# 编译参数
set(CMAKE_C_FLAGS, "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fexceptions -Wall -Werror -Wunused-variable")

#头文件
include_directories(
        common
        sample
        glm
)

#GLSample库
set(samples_src gl_sample_jni.c)
aux_source_directory(./samples samples_src)
aux_source_directory(./common samples_src)
add_library(gl_sample SHARED ${samples_src})
target_link_libraries(
        gl_sample
        log
        android
        EGL
        GLESv3
)

#GLCamera渲染库
set(camera_render_src gl_camera_render_jni.c)
aux_source_directory(./common camera_render_src)
aux_source_directory(./camera camera_render_src)
add_library(gl_camera_render SHARED ${camera_render_src})
target_link_libraries(
        gl_camera_render
        log
        android
        EGL
        GLESv3
)

##GLCamera采集库
set(camera_capture_src gl_camera_capture_jni.c)
aux_source_directory(./common camera_capture_src)
add_library(gl_camera_capture SHARED ${camera_capture_src})
target_link_libraries(
        gl_camera_capture
        log
        android
        EGL
        GLESv3
)