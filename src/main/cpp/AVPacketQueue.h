//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_AVPACKETQUEUE_H
#define ANDROIDPROJECT_AVPACKETQUEUE_H

#include <queue>
#include <pthread.h>
#include <android/log.h>
#include "Log.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}


using namespace std;

class AVPacketQueue {

public:
    queue<AVPacket *> audio_packets;
    queue<AVPacket *> video_packets;
    queue<AVPacket *> free_packets;
    queue<AVFrame *> frame_packets;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t cond_Frame;
    int stop = 1;

public:
    AVPacketQueue();
    ~AVPacketQueue();
   int put_aduio_packet(AVPacket *avPacket);
   int get_audio_packet(AVPacket *avPacket);

   int put_video_packet(AVPacket *avPacket);
   int get_video_packet(AVPacket *avPacket);

   int put_free_packet(AVPacket *avPacket);
   int get_free_packet(AVPacket *avPacket);

    int put_Item(AVFrame * item);
    int get_Item(AVFrame * item);

   int put_packet( queue<AVPacket *> *queue_,AVPacket *avPacket);
   int get_packet( queue<AVPacket *> *queue_,AVPacket *avPacket);
   void clearPackets();

//    int size(avPacketQueue<AVPacket *> *queue_);
//    int size_audio();
//    int size_video();

};



#endif //ANDROIDPROJECT_AVPACKETQUEUE_H
