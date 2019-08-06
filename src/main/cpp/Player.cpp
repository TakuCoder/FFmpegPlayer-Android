//
// Created by saite on 2019/8/4.
//

#include "Player.h"
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
//    prepare(url);
}

int Player::prepare(const  char *input_str ) {
    av_register_all();
    avformat_context = avformat_alloc_context();
    LOGE("open input stream.  == %s",input_str );
    if (avformat_open_input(&avformat_context, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info(avformat_context, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    init_stream(avformat_context,AVMEDIA_TYPE_VIDEO,-1);
    init_stream(avformat_context,AVMEDIA_TYPE_AUDIO,-1);
    putPacketToQueue(avformat_context,queue,audioStreamindex,videoStreamindex);
    return 0;
}

 int Player::init_stream(AVFormatContext *avformat_context, enum AVMediaType type, int sel) {
     AVCodecContext *avcodec_context;
    int idx = -1, cur = -1;
    for (int i = 0; i < avformat_context->nb_streams; i++) {
        if (avformat_context->streams[i]->codec->codec_type == type) {
            idx = i;
            if (++cur == sel) break;
        }
    }
     LOGE("init_stream ==  %d", idx);
    if (idx == -1) return -1;
    if(type == AVMEDIA_TYPE_AUDIO){
        audioStreamindex = idx;
        avcodec_audio_context = avcodec_context = avformat_context->streams[idx]->codec;
    }else if(type == AVMEDIA_TYPE_VIDEO){
        videoStreamindex = idx;
        avcodec_video_context =   avcodec_context = avformat_context->streams[idx]->codec;
    }
     avcodec_context = avformat_context->streams[idx]->codec;
    AVCodec *decoder = avcodec_find_decoder(avcodec_context->codec_id);
     LOGE("init_stream ==  %d", avcodec_context->codec_id );
    AVFrame *vFrame = av_frame_alloc();
    AVPacket *avPacket = av_packet_alloc();
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
//     LOGE("init_stream == 5");
//    if(type == AVMEDIA_TYPE_AUDIO){
//        LOGE("init_stream == 6");
//        //读取帧
//        while (av_read_frame(avformat_context, avPacket) >= 0) {
//            if (avPacket->stream_index == idx) {
//                queue->put_aduio_packet(avPacket);
//            }
////            av_packet_unref(avPacket);
//            avPacket = av_packet_alloc();
//        }
//        LOGE("init_stream == 7");
//    }else  if(type == AVMEDIA_TYPE_VIDEO) {
//        while (av_read_frame(avformat_context, avPacket) >= 0) {
////            LOGI("av_read_frame  idx = %d" , idx);
//            if (avPacket->stream_index == idx) {
//                queue->put_video_packet(avPacket);
//            }
////            av_packet_unref(avPacket);
//            avPacket = av_packet_alloc();
//        }
//    }
//     LOGE("init_stream == 8");
    return 0;
}

void putPacketToQueue(AVFormatContext *avformat_context,AVPacketQueue *queue,int audioStreamindex,int videoStreamindex){
    AVPacket *avPacket = av_packet_alloc();
    while (av_read_frame(avformat_context, avPacket) >= 0) {
        if (avPacket->stream_index == audioStreamindex) {
            queue->put_aduio_packet(avPacket);
        }

        if (avPacket->stream_index == videoStreamindex) {
            queue->put_video_packet(avPacket);
        }
//            av_packet_unref(avPacket);
        avPacket = av_packet_alloc();
    }
}

void Player::player_play_audio(JNIEnv *env, jclass clz, jstring url_,
                               jobject surface) {
    bqPlayerCallback(audioOpensl->bqPlayerBufferQueue, NULL);
    play_video(env,surface,avformat_context,avcodec_video_context,videoStreamindex,queue);
}

void Player::InitOpenSL(JNIEnv *env,jclass  clz){
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

}

void initParam(Player * player,AudioOpensl *audioOpensl){
    DecodeParam d ;
    d.avformat_context  = player->avformat_context;
    d.avcodec_context  = player->avcodec_audio_context;
    d.queue = player->queue;
    d.audioStream = player->audioStreamindex;
    d.swr = player->swr;

    initDecodePCM(d,audioOpensl);
}

int Player::createFFmpegAudioPlay(int *rate,int *channel,int *simpleFmt){
    // 返回sample rate和channels
    *rate = avcodec_audio_context->sample_rate;
    *channel = avcodec_audio_context->channels;
    *simpleFmt = avcodec_audio_context->sample_fmt;

    swr = swr_alloc();
    swr =  swr_alloc_set_opts(swr, avcodec_audio_context->channel_layout, AV_SAMPLE_FMT_S16, avcodec_audio_context->sample_rate,
                              avcodec_audio_context->channel_layout, avcodec_audio_context->sample_fmt, avcodec_audio_context->sample_rate, 0, NULL);

    swr_init(swr);
    initParam(this,audioOpensl);
    return 0;
}


static void* av_demux_thread_proc(void *param){



}

ANativeWindow *nativeWindow;
ANativeWindow_Buffer windowBuffer;

int  play_video(JNIEnv *env, jobject surface , AVFormatContext	*pFormatCtx,AVCodecContext	*pCodecCtx,int videoindex,AVPacketQueue *video_queue){

    struct SwsContext *img_convert_ctx;
    //获取界面传下来的surface
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow) {
        LOGE("Couldn't get native window from surface.\n");
        return -1;
    }
    int width = pCodecCtx->width;
    int height = pCodecCtx->height;
    //分配一个帧指针，指向解码后的原始帧
    AVFrame *vFrame = av_frame_alloc();
    AVPacket *vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    AVFrame *pFrameRGBA = av_frame_alloc();
    img_convert_ctx = sws_getContext(width, height, pCodecCtx->pix_fmt,
                                     width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    if (0 >
        ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888)) {
        LOGE("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return -1;
    }
    //读取帧
//    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
        while((&video_queue->video_packets)->size() > 0) {
            video_queue->get_video_packet(vPacket);
        if (vPacket->stream_index == videoindex) {
            //视频解码
            int ret = avcodec_send_packet(pCodecCtx, vPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGE("video avcodec_send_packet error %d", ret);
                return -1;
            }
            ret = avcodec_receive_frame(pCodecCtx, vFrame);
            if (ret < 0 && ret != AVERROR_EOF) {
                LOGE("video avcodec_receive_frame error %d", ret);
                av_packet_unref(vPacket);
                continue;
            }
            //转化格式
            sws_scale(img_convert_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize, 0,
                      pCodecCtx->height,
                      pFrameRGBA->data, pFrameRGBA->linesize);
            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                LOGE("cannot lock window");
                return -1;
            } else {
                av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize,
                                     (const uint8_t *) windowBuffer.bits, AV_PIX_FMT_RGBA,
                                     width, height, 1);
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(vPacket);
    }
    //释放内存
//    sws_freeContext(img_convert_ctx);
//    av_free(vPacket);
//    av_free(pFrameRGBA);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
    return 0;
}

