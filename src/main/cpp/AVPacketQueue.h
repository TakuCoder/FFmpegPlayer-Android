//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_AVPACKETQUEUE_H
#define ANDROIDPROJECT_AVPACKETQUEUE_H

#include <queue>
#include <pthread.h>
#include <android/log.h>
extern "C"
{
#include <libavcodec/avcodec.h>
}

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_E", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_I", __VA_ARGS__))

using namespace std;


class AVPacketQueue {

public:
    queue<AVPacket *> audio_packets;
    queue<AVPacket *> video_packets;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int stop = 1;

public:
    AVPacketQueue();
    ~AVPacketQueue();
   int put_aduio_packet(AVPacket *avPacket);
   int get_audio_packet(AVPacket *avPacket);

   int put_video_packet(AVPacket *avPacket);
   int get_video_packet(AVPacket *avPacket);

   int put_packet( queue<AVPacket *> *queue_,AVPacket *avPacket);
   int get_packet( queue<AVPacket *> *queue_,AVPacket *avPacket);
    void clearPackets();

//    int size(queue<AVPacket *> *queue_);
//    int size_audio();
//    int size_video();

};


#endif //ANDROIDPROJECT_AVPACKETQUEUE_H
