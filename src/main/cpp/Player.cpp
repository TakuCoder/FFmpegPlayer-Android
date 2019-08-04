//
// Created by saite on 2019/8/4.
//

#include "Player.h"
#include "AudioOpensl.h"
#include <android/log.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>

}

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_E", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "will_I", __VA_ARGS__))


Player::Player(JNIEnv *env, jclass clz,const char *url) {
    queue = new AVPacketQueue();
    prepare(url);
}

int Player::prepare(const  char *input_str ) {
    av_register_all();
    avformat_context = avformat_alloc_context();
    if (avformat_open_input(&avformat_context, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info(avformat_context, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
//    init_stream(avformat_context,AVMEDIA_TYPE_VIDEO,-1);
    init_stream(avformat_context,AVMEDIA_TYPE_AUDIO,-1);
    return 0;
}

 int Player::init_stream(AVFormatContext *avformat_context, enum AVMediaType type, int sel) {
    int idx = -1, cur = -1;
    for (int i = 0; i < avformat_context->nb_streams; i++) {
        if (avformat_context->streams[i]->codec->codec_type == type) {
            idx = i;
            if (++cur == sel) break;
        }
    }
     LOGE("init_stream ==  %d", idx);
    if (idx == -1) return -1;
    avcodec_context = avformat_context->streams[idx]->codec;
    AVCodec *decoder = avcodec_find_decoder(avcodec_context->codec_id);
     LOGE("init_stream ==  %d", avcodec_context->codec_id );
    AVFrame *vFrame = av_frame_alloc();
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
     LOGE("init_stream == 3");
    if (decoder == NULL) {
        LOGE("Couldn't find Codec.\n");
        return -1;
    }
     LOGE("init_stream == 4");
    if (avcodec_open2(avcodec_context, decoder, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }
     LOGE("init_stream == 5");
    if(type == AVMEDIA_TYPE_AUDIO){
        LOGE("init_stream == 6");
        //读取帧
        while (av_read_frame(avformat_context, avPacket) >= 0) {
            if (avPacket->stream_index == idx) {
                queue->put_aduio_packet(avPacket);
            }
            av_packet_unref(avPacket);
        }
        LOGE("init_stream == 7");
    }else  if(type == AVMEDIA_TYPE_VIDEO) {
        while (av_read_frame(avformat_context, avPacket) >= 0) {
//            LOGI("av_read_frame  idx = %d" , idx);
            if (avPacket->stream_index == idx) {
                queue->put_video_packet(avPacket);
            }
            av_packet_unref(avPacket);
        }
    }
     LOGE("init_stream == 8");
    return 0;
}

void Player::player_play_audio() {
    LOGE("player_play_audio ==1");
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    while((&queue->audio_packets)->size() > 0) {
        LOGE("player_play_audio ==  %d",avPacket->pts);
        queue->get_audio_packet(avPacket);
    }
    LOGE("player_play_audio ==3");
//    queue->stop = 0;
}

void Player::InitOpenSL(JNIEnv *env,jclass  clz){
    AudioOpensl *audioOpensl;
    audioOpensl = new AudioOpensl();
    int rate, channel,simpleFmt;
    // 创建FFmpeg音频解码器
    createFFmpegAudioPlay(&rate, &channel,&simpleFmt);
//    // 创建播放引擎
    audioOpensl->createEngine(env,clz);
//
//    // 创建缓冲队列音频播放器
    audioOpensl->createBufferQueueAudioPlayer(env,clz,rate, channel, SL_PCMSAMPLEFORMAT_FIXED_16);
//
////    getPCM(&buffer,&bufferSize);
//    // 启动音频播放
//    audio->bqPlayerCallback(audio.bqPlayerBufferQueue, NULL);
//    bqPlayerCallback(audioOpensl.bqPlayerBufferQueue, NULL);
}

int Player::createFFmpegAudioPlay(int *rate,int *channel,int *simpleFmt){
    // 返回sample rate和channels
    *rate = avcodec_context->sample_rate;
    *channel = avcodec_context->channels;
    *simpleFmt = avcodec_context->sample_fmt;
    return 0;
}
