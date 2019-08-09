//
// Created by saite on 2019/8/4.
//

#include "Player.h"
#include <android/log.h>
#include <sys/time.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>

}



pthread_t avdemux_thead;
pthread_t audio_decode_thead;
pthread_t vedio_decode_thead;

AVPacketQueue *avPacketQueue;

struct FFmpeg_param {
    AVFormatContext *avformat_context;
    AVCodecContext *avcodec_audio_context;
    AVCodecContext *avcodec_video_context;
    int videoStreamindex;
    int audioStreamindex;
};

FFmpeg_param * playerParam = (FFmpeg_param*)calloc(1, sizeof(FFmpeg_param));
SwrContext *swr;
void *packetQueue;
AudioOpensl *audioOpensl;

int state = 0;

int prepare(const  char *input_str ) {
    avPacketQueue = new AVPacketQueue();
    av_register_all();
    playerParam->avformat_context = avformat_alloc_context();
    LOGE("open input stream.  == %s",input_str );
    if (avformat_open_input(& playerParam->avformat_context, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info( playerParam->avformat_context, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    init_stream( playerParam->avformat_context,AVMEDIA_TYPE_VIDEO,-1);
    init_stream( playerParam->avformat_context,AVMEDIA_TYPE_AUDIO,-1);
    pthread_create(&avdemux_thead,NULL,putPacketToQueue,playerParam);
    pthread_create(&vedio_decode_thead,NULL,decode_toFrame,playerParam);
//    putPacketToQueue(avformat_context,avPacketQueue,audioStreamindex,videoStreamindex);
    return 0;
}

 int init_stream(AVFormatContext *avformat_context, enum AVMediaType type, int sel) {
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
        playerParam->audioStreamindex = idx;
        playerParam->avcodec_audio_context = avcodec_context = avformat_context->streams[idx]->codec;
    }else if(type == AVMEDIA_TYPE_VIDEO){
        playerParam->videoStreamindex = idx;
        playerParam->avcodec_video_context =   avcodec_context = avformat_context->streams[idx]->codec;
    }
     avcodec_context = avformat_context->streams[idx]->codec;
    AVCodec *decoder = avcodec_find_decoder(avcodec_context->codec_id);
     LOGE("init_stream ==  %d", avcodec_context->codec_id );
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
//                avPacketQueue->put_aduio_packet(avPacket);
//            }
////            av_packet_unref(avPacket);
//            avPacket = av_packet_alloc();
//        }
//        LOGE("init_stream == 7");
//    }else  if(type == AVMEDIA_TYPE_VIDEO) {
//        while (av_read_frame(avformat_context, avPacket) >= 0) {
////            LOGI("av_read_frame  idx = %d" , idx);
//            if (avPacket->stream_index == idx) {
//                avPacketQueue->put_video_packet(avPacket);
//            }
////            av_packet_unref(avPacket);
//            avPacket = av_packet_alloc();
//        }
//    }
//     LOGE("init_stream == 8");
    return 0;
}

void * putPacketToQueue(void *param){
//    FFmpeg_param * playerParam = (FFmpeg_param *) param;
    AVPacket *avPacket = av_packet_alloc();

    while (av_read_frame(playerParam->avformat_context, avPacket) >= 0) {
        LOGE("putPacketToQueue= %d   %x", 111,avPacket);
        if (avPacket->stream_index == playerParam->audioStreamindex) {
            avPacketQueue->put_aduio_packet(avPacket);
        }

        if (avPacket->stream_index == playerParam->videoStreamindex) {
            avPacketQueue->put_video_packet(avPacket);
        }
        avPacket = av_packet_alloc();
    }
    state = 5;
    return NULL;
}

void player_play_audio(JNIEnv *env, jclass clz, jstring url_,
                               jobject surface) {
    if (state == 0) {
         bqPlayerCallback(audioOpensl->bqPlayerBufferQueue, NULL);
    }else{
        audioOpensl->SetPlayState(SL_PLAYSTATE_PLAYING);
    }
    state = 1;
    play_video(env,surface,playerParam->avformat_context,playerParam->avcodec_video_context,playerParam->videoStreamindex,avPacketQueue);
}

void player_pause_video(){
    state = 2;
    // 暂停音乐
    audioOpensl->SetPlayState(SL_PLAYSTATE_PAUSED);
}

void player_stop_video(){
    state = 3;
    audioOpensl->SetPlayState(SL_PLAYSTATE_PAUSED);

    avcodec_close(playerParam->avcodec_audio_context);
    avcodec_close(playerParam->avcodec_video_context);
    avformat_close_input(&playerParam->avformat_context);
}
void InitOpenSL(JNIEnv *env,jclass  clz){
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

void initParam(AudioOpensl *audioOpensl){
    DecodeParam d ;
    d.avformat_context  = playerParam->avformat_context;
    d.avcodec_context  = playerParam->avcodec_audio_context;
    d.queue = avPacketQueue;
    d.audioStream = playerParam->audioStreamindex;
    d.swr = swr;

    initDecodePCM(d,audioOpensl);
}

int createFFmpegAudioPlay(int *rate,int *channel,int *simpleFmt){
    // 返回sample rate和channels
    *rate = playerParam->avcodec_audio_context->sample_rate;
    *channel = playerParam->avcodec_audio_context->channels;
    *simpleFmt = playerParam->avcodec_audio_context->sample_fmt;

    swr = swr_alloc();
    swr =  swr_alloc_set_opts(swr, playerParam->avcodec_audio_context->channel_layout, AV_SAMPLE_FMT_S16, playerParam->avcodec_audio_context->sample_rate,
                              playerParam->avcodec_audio_context->channel_layout, playerParam->avcodec_audio_context->sample_fmt, playerParam->avcodec_audio_context->sample_rate, 0, NULL);

    swr_init(swr);
    initParam(audioOpensl);
    return 0;
}


static void* av_demux_thread_proc(void *param){



}

ANativeWindow *nativeWindow;
ANativeWindow_Buffer windowBuffer;


long getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

queue<AVFrame *> frame_packets;
void * decode_toFrame(void * inParam){
    FFmpeg_param * param =  (FFmpeg_param *)inParam;


    int width = playerParam->avcodec_video_context->width;
    int height = playerParam->avcodec_video_context->height;

//    struct SwsContext *img_convert_ctx;
//    img_convert_ctx = sws_getContext(width, height, playerParam->avcodec_video_context->pix_fmt,
//                                     width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    //分配一个帧指针，指向解码后的原始帧
    AVFrame *vFrame = av_frame_alloc();
//    AVFrame *pFrameRGBA = av_frame_alloc();
    AVPacket *vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //读取帧
    while(state != 3) {
        if((&avPacketQueue->video_packets)->size() <= 0){
            continue;
        }
        long start = getCurrentTime();
//        LOGE("decode_toFrame start sysTime = %d ", start);
        avPacketQueue->get_video_packet(vPacket);
        LOGE("decode_toFrame sws_scale size = %d  vPacket  %x ", frame_packets.size(),vPacket);
        if (vPacket->stream_index == playerParam->videoStreamindex) {
            //视频解码
            int ret = avcodec_send_packet(playerParam->avcodec_video_context, vPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGE("video avcodec_send_packet error %d", ret);
                return NULL;
            }
            LOGE("decode_toFrame sws_scale size = %d  111  %x ", frame_packets.size(),vFrame);
            avPacketQueue->get_Item(vFrame);

            LOGE("decode_toFrame sws_scale size = %d  222 %x ", frame_packets.size(),&vFrame->data);
            ret = avcodec_receive_frame(playerParam->avcodec_video_context, vFrame);
//            LOGE("decode_toFrame Second sysTime = %d   = %d ", getCurrentTime(), getCurrentTime() - start);
            if (ret < 0 && ret != AVERROR_EOF) {
                LOGE("video avcodec_receive_frame error %d", ret);
                av_packet_unref(vPacket);
                continue;
            }
//            LOGE("decode_toFrame size = %d    %x ", frame_packets.size(),pFrameRGBA);
            //转化格式
//            avPacketQueue->get_Item(pFrameRGBA);
//            sws_scale(img_convert_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize, 0,
//                      playerParam->avcodec_video_context->height,
//                      pFrameRGBA->data, pFrameRGBA->linesize);
//            LOGE("decode_toFrame sws_scale sysTime = %d   = %d ", getCurrentTime(), getCurrentTime() - start);
//            avPacketQueue->put_Item(pFrameRGBA);
//            avPacketQueue->frame_packets.push(pFrameRGBA);
//            frame_packets.push(pFrameRGBA);
            frame_packets.push(vFrame);
//            vFrame = av_frame_alloc();
            LOGE("decode_toFrame sws_scale size = %d  3333   %x     ", frame_packets.size(),&vFrame->data);
        }
    }
    return NULL;
}


static void sleep_ms(unsigned int secs)

{

    struct timeval tval;

    tval.tv_sec=secs/1000;

    tval.tv_usec=(secs*1000)%1000000;

    select(0,NULL,NULL,NULL,&tval);

}

int delay= 20;
int  play_video(JNIEnv *env, jobject surface , AVFormatContext	*pFormatCtx,AVCodecContext	*pCodecCtx,int videoindex,AVPacketQueue *video_queue){
    int width = pCodecCtx->width;
    int height = pCodecCtx->height;
    //获取界面传下来的surface
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow) {
        LOGE("Couldn't get native window from surface.\n");
        return -1;
    }
    if (0 >
        ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888)) {
        LOGE("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return -1;
    }

    struct SwsContext *img_convert_ctx;

    img_convert_ctx = sws_getContext(width, height, playerParam->avcodec_video_context->pix_fmt,
                                     width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);

    AVFrame *pFrameRGBA = av_frame_alloc();
    AVFrame  *vFrame;
    while (state == 1) {
//        if (avPacketQueue->frame_packets.size() <= 0) {
        if (frame_packets.size() <= 0) {
            continue;
        }
        long start = getCurrentTime();
        LOGE("play start sysTime = %d ", start);

//        avPacketQueue->get_Item(pFrameRGBA);
//        pFrameRGBA =  frame_packets.front();
//        frame_packets.pop();

        vFrame =  frame_packets.front();
        frame_packets.pop();

//        pFrameRGBA=   avPacketQueue->frame_packets.front();
//        avPacketQueue->frame_packets.pop();

        sws_scale(img_convert_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize, 0,
                      playerParam->avcodec_video_context->height,
                      pFrameRGBA->data, pFrameRGBA->linesize);

        if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
            LOGE("cannot lock window");
            return -1;
        } else {
//            LOGE("get_Item success = %d   %x",  getCurrentTime() - start,pFrameRGBA);
            av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize,
                                 (const uint8_t *) windowBuffer.bits, AV_PIX_FMT_RGBA,
                                 width, height, 1);
            ANativeWindow_unlockAndPost(nativeWindow);
//            av_frame_unref(pFrameRGBA);
//            avPacketQueue->put_Item(pFrameRGBA);

            av_frame_unref(vFrame);
            avPacketQueue->put_Item(vFrame);
        }
        if(frame_packets.size() >1){
            sleep_ms(delay);
        }
//        av_frame_free(&vFrame);
//        av_packet_unref(vPacket);
         delay +=(41- (getCurrentTime() - start));
        LOGE("play end sysTime = %d   = %d ", getCurrentTime(), getCurrentTime() - start);
//        av_free(vPacket);
    }
    //释放内存
    sws_freeContext(img_convert_ctx);
//    av_free(vPacket);
    av_free(pFrameRGBA);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
    return 0;
}

