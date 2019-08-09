//
// Created by saite on 2019/8/4.
//

#include "AVPacketQueue.h"

AVPacketQueue::AVPacketQueue() {
    //初始化线程锁和消息
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond_Frame, NULL);
}

AVPacketQueue::~AVPacketQueue() {
    clearPackets();
    //析构函数 释放线程锁和消息
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

int AVPacketQueue::put_aduio_packet(AVPacket *avPacket) {
    return  put_packet(&audio_packets,avPacket);
}

int AVPacketQueue::get_audio_packet(AVPacket *avPacket) {
    return get_packet(&audio_packets,avPacket);
}

int AVPacketQueue::put_video_packet(AVPacket *avPacket) {
    return  put_packet(&video_packets,avPacket);
}

int AVPacketQueue::get_video_packet(AVPacket *avPacket) {
    return get_packet(&video_packets, avPacket);
}

int AVPacketQueue::put_free_packet(AVPacket *avPacket) {
    av_packet_unref(avPacket);
    free_packets.push(avPacket);
    return 0;
}

int AVPacketQueue::get_free_packet(AVPacket *avPacket) {
    if(free_packets.size() > 0) {
        avPacket = free_packets.front();
        free_packets.pop();
    }else{
        avPacket =av_packet_alloc();
    }
    return 0;
}

int AVPacketQueue::put_packet(queue<AVPacket *> *queue_,AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
         queue_->push(avPacket);
    if(queue_->size() < 2400){

    }else{
        pthread_cond_wait(&cond,&mutex);
        LOGI(" pthread_cond_wait = %lu  put_packet = %d",pthread_self() , queue_->size());
    }
    LOGI(" pthread_self = %lu  put_packet = %d  ===%d",pthread_self() , queue_->size(),avPacket->pts);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int AVPacketQueue::get_packet(queue<AVPacket *>  *queue_,AVPacket *avPacket) {
//    LOGI("get_packet ===============1======== %d" , queue_->size());
    pthread_mutex_lock(&mutex);
    if(queue_->size() > 0) {
//        LOGI("get_packet ===========2============ %d" , queue_->size());
        if(queue_->size() <100){
            pthread_cond_signal(&cond);
        }
        AVPacket *pkt = queue_->front();
        LOGI(" pthread_self = %lu get_packet  = %d  ===%d  pkt===%d   ",pthread_self() , queue_->size(),avPacket->pts,pkt->pts);
        if (av_packet_ref(avPacket, pkt) == 0) {
            queue_->pop();
        }
//        LOGI("get_packet = %d  ===%d  pkt===%d   " , queue_->size(),avPacket->pts,pkt->pts);
//        LOGI("get_packet ============3=========== %d" , queue_->size());
//        av_packet_free(&pkt);
//        av_free(pkt);
        pkt = NULL;

    }else{
        LOGI(" pthread_self = %lu pthread_cond_signal ",pthread_self() );
//        LOGI("get_packet ============4=========== %d" , queue_->size());
//        pthread_cond_wait(&cond,&mutex);
        pthread_cond_signal(&cond);
    }

    pthread_mutex_unlock(&mutex);
//    LOGI("get_packet ============5=========== %d" , queue_->size());
    return 0;
}


int AVPacketQueue::put_Item(AVFrame * item){

    pthread_mutex_lock(&mutex);
//    if(frame_packets.size() <10){
     frame_packets.push(item);
    LOGI(" pthread_self = %lu put_Item  size=%d ",pthread_self(),frame_packets.size() );
//    }else{
//        pthread_cond_wait(&cond,&mutex);
//    }
    pthread_cond_signal(&cond_Frame);
    pthread_mutex_unlock(&mutex);
    return 0;
}
static int item_count = 0;
int AVPacketQueue::get_Item(AVFrame * item){
    pthread_mutex_lock(&mutex);
    if(item_count < 100 ) {
        AVFrame *frame = av_frame_alloc();
        frame_packets.push(frame);
        item_count++;
        LOGI(" pthread_self = %lu get_Item 11111 size=%d ",pthread_self(),frame_packets.size() );
    }

    if(frame_packets.size() > 0 ){
        LOGI(" pthread_self = %lu get_Item 22222 size=%d ",pthread_self(),frame_packets.size() );
    }else{
        pthread_cond_wait(&cond_Frame,&mutex);
    }
    AVFrame *pkt = frame_packets.front();
    LOGI(" pthread_self = %lu get_Item 33333  size=%d    %x",pthread_self(),frame_packets.size(),&pkt );
    if (av_frame_ref(item, pkt) == 0) {
        frame_packets.pop();
    }else{
        frame_packets.pop();
        LOGI(" pthread_self = %lu get_Item 44444  size=%d    %x",pthread_self(),frame_packets.size(),&pkt );
    }
    LOGI(" pthread_self = %lu get_Item 55555  size=%d ",pthread_self(),frame_packets.size() );
    pthread_mutex_unlock(&mutex);
    return 0;
}

void AVPacketQueue::clearPackets() {
    pthread_cond_signal(&cond);
    pthread_mutex_lock(&mutex);

    while (!audio_packets.empty()) {
        AVPacket *avPacket = audio_packets.front();
        audio_packets.pop();
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    while (!video_packets.empty()) {
        AVPacket *avPacket = video_packets.front();
        video_packets.pop();
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    pthread_mutex_unlock(&mutex);

//  int  AVPacketQueue::size(avPacketQueue<AVPacket *>  *queue_){
//        return queue_->size();
//  }

//    int AVPacketQueue::size_audio(){
//        return audio_packets->size();
//  }
//    int AVPacketQueue::size_video(){
//       return  size( &video_packets);
//  }
}



