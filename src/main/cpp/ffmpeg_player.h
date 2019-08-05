//
// Created by saite on 2019/7/22.
//
#ifndef ANDROIDPROJECT_FFMPEG_PLAYER_H
#define ANDROIDPROJECT_FFMPEG_PLAYER_H

#include <jni.h>
#include <string>

#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "Player.h"

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "will_e", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "will_i", format, ##__VA_ARGS__)

#endif //ANDROIDPROJECT_FFMPEG_PLAYER_H