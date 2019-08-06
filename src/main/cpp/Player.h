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
int  play_video(JNIEnv *env, jobject surface , AVFormatContext	*pFormatCtx,AVCodecContext	*pCodecCtx,int videoindex,AVPacketQueue *video_queue);

void putPacketToQueue(AVFormatContext *avformat_context,AVPacketQueue *queue,int audioStreamindex,int videoStreamindex);

class Player {
    public:
        AVFormatContext *avformat_context;
        AVCodecContext *avcodec_audio_context;
         AVCodecContext *avcodec_video_context;
        pthread_t avdemux_thead;
        pthread_t audio_decode_thead;
        pthread_t vedio_decode_thead;

        AVPacketQueue *queue;
        int videoStreamindex;
        int audioStreamindex;

        SwrContext *swr;

        void *packetQueue;

        AudioOpensl *audioOpensl;

public:
        Player(JNIEnv *env, jclass clz,const char *url);

        ~Player();

        void player_play();

        void player_play_audio(JNIEnv *env, jclass clz, jstring url_,
                               jobject surface);

        void player_play_video();

        int prepare(const  char *input_str);

        void pause();

        void stop();

        void reset();

        int init_stream(AVFormatContext *avformat_context, AVMediaType type, int sel);

        void InitOpenSL(JNIEnv *env, jclass clz);

        int createFFmpegAudioPlay(int *rate, int *channel, int *simpleFmt);

        void getPCM(void **pcm, size_t *pcmSize);
    };

struct QueueTools{
    AVPacketQueue *quque;
    AVPacket *avPacket;
};

#endif //ANDROIDPROJECT_PLAYER_H
