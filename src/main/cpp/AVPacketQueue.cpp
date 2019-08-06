//
// Created by saite on 2019/8/4.
//

#include "AVPacketQueue.h"

AVPacketQueue::AVPacketQueue() {
    //初始化线程锁和消息
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
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
    return get_packet(&video_packets,avPacket);
}

int AVPacketQueue::put_packet(queue<AVPacket *> *queue_,AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
//    if(queue_->size() <10000){
        queue_->push(avPacket);
//    }else{
//        pthread_cond_wait(&cond,&mutex);
//    }
    LOGI(" pthread_self = %lu  put_packet = %d  ===%d",pthread_self() , queue_->size(),avPacket->pts);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int AVPacketQueue::get_packet(queue<AVPacket *>  *queue_,AVPacket *avPacket) {
//    LOGI("get_packet ===============1======== %d" , queue_->size());
    pthread_mutex_lock(&mutex);
    if(queue_->size() > 0 && stop != 0) {
//        LOGI("get_packet ===========2============ %d" , queue_->size());
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
//        LOGI("get_packet ============4=========== %d" , queue_->size());
        pthread_cond_wait(&cond,&mutex);
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
//    LOGI("get_packet ============5=========== %d" , queue_->size());
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

//  int  AVPacketQueue::size(queue<AVPacket *>  *queue_){
//        return queue_->size();
//  }

//    int AVPacketQueue::size_audio(){
//        return audio_packets->size();
//  }
//    int AVPacketQueue::size_video(){
//       return  size( &video_packets);
//  }
}



