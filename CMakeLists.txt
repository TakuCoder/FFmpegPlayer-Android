# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             ffmpegplayer-lib
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             src/main/cpp/ffmpeg_player.cpp
        src/main/cpp/ffmpeg_player.h
        src/main/cpp/AVPacketQueue.h
        src/main/cpp/AVPacketQueue.cpp
        src/main/cpp/Player.h
        src/main/cpp/Player.cpp
        src/main/cpp/AudioOpensl.cpp
        src/main/cpp/AudioOpensl.h
        src/main/cpp/DecodeTools.cpp
        src/main/cpp/DecodeTools.h
        )
find_library(
        log-lib
        log)

#find_library(
#        native_window
#        native_window
#)
#
#find_library(
#        native_window_jni
#        native_window_jni
#)




include_directories(  src/main/cpp/include/)

add_library( avcodeec-58
        SHARED
        IMPORTED )
set_target_properties(avcodeec-58
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libavcodec-58.so )
#
add_library( avdevice-58
        SHARED
        IMPORTED )
set_target_properties(avdevice-58
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libavdevice-58.so )
#
add_library( avfilter-7
        SHARED
        IMPORTED )
set_target_properties(avfilter-7
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libavfilter-7.so )
#
add_library( avformat-58
        SHARED
        IMPORTED )
set_target_properties(avformat-58
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libavformat-58.so )
#
add_library( avutil-56
        SHARED
        IMPORTED )
set_target_properties(avutil-56
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libavutil-56.so )
##
add_library( swresample-3
        SHARED
        IMPORTED )
set_target_properties(swresample-3
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libswresample-3.so )
#
add_library( swscale-5
        SHARED
        IMPORTED )
set_target_properties(swscale-5
        PROPERTIES IMPORTED_LOCATION
        ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI}/libswscale-5.so )
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_link_libraries(
        ffmpegplayer-lib
        #ffmpeg------start----------
        avcodeec-58
        avdevice-58
        avfilter-7
        avformat-58
        avutil-56
        swresample-3
        swscale-5
        #ffmpeg------end------------
        android
        OpenSLES
        ${log-lib}
#        ${native_window}
#        ${native_window_jni}
         )
