//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_PLAYER_H
#define ANDROIDPROJECT_PLAYER_H


#include <sys/types.h>
#include <jni.h>
#include "AVPacketQueue.h"
#include "AudioOpensl.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}



int play_video(JNIEnv *env, jobject surface, AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx,
               int videoindex, AVPacketQueue *video_queue);

void * putPacketToQueue(void *param);
void * decode_toFrame(void *param);


void player_play();

void player_play_audio(JNIEnv *env, jclass clz, jstring url_,
                       jobject surface);

void player_play_video();
void player_pause_video();
void player_stop_video();

int prepare(const char *input_str);

void pause();

void stop();

void reset();

int init_stream(AVFormatContext *avformat_context, AVMediaType type, int sel);

void InitOpenSL(JNIEnv *env, jclass clz);

int createFFmpegAudioPlay(int *rate, int *channel, int *simpleFmt);

//void getPCM(void **pcm, size_t *pcmSize);

struct QueueTools {
    AVPacketQueue *quque;
    AVPacket *avPacket;
};


#endif //ANDROIDPROJECT_PLAYER_H
