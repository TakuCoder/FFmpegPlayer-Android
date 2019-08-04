//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_PLAYER_H
#define ANDROIDPROJECT_PLAYER_H


#include <sys/types.h>
#include <jni.h>
#include "AVPacketQueue.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
    class Player {
    public:
        AVFormatContext *avformat_context;
        AVCodecContext *avcodec_context;

        pthread_t avdemux_thead;
        pthread_t audio_decode_thead;
        pthread_t vedio_decode_thead;

        AVPacketQueue *queue;

        void *packetQueue;

    public:
        Player(JNIEnv *env, jclass clz,const char *url);

        ~Player();

        void player_play();

        void player_play_audio();

        void player_play_video();

        int prepare(const  char *input_str);

        void pause();

        void stop();

        void reset();

        int init_stream(AVFormatContext *avformat_context, AVMediaType type, int sel);

        void InitOpenSL(JNIEnv *env, jclass clz);

        int createFFmpegAudioPlay(int *rate, int *channel, int *simpleFmt);
    };

#endif //ANDROIDPROJECT_PLAYER_H
