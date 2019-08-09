//
// Created by saite on 2019/7/22.
//

#include "ffmpeg_player.h"



//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_willxing_example_NDKActivity_stringFromJNI(JNIEnv *env, jobject instance) {
//
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}
//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_willxing_example_FFmpegActivity_helloNDK(JNIEnv *env, jobject instance, jstring msg_) {
//    char* chello = (char *) env->GetStringUTFChars(msg_, JNI_FALSE);
//    __android_log_print(ANDROID_LOG_ERROR,"tag","c : %s",chello);//og日志打印
//            __android_log_print(ANDROID_LOG_ERROR,"tag","编码器配置： %s",avcodec_configuration());//log日志打印
//    char * newstr = strcat(chello,avcodec_configuration());//将java传过来的字符串和编码器配置信息拼接起来并返回去
//    return env->NewStringUTF(newstr);
//}
//
//extern "C"
//JNIEXPORT void JNICALL
//Java_com_willxing_example_FFmpegActivity_run(JNIEnv *env, jclass type) {
//
//    char info[40000] = {0};
//    av_register_all();
//    AVCodec *c_temp = av_codec_next(NULL);
//    while (c_temp != NULL) {
//        if (c_temp->decode != NULL) {
//            sprintf(info, "%s[Dec]", info);
//        } else {
//            sprintf(info, "%s[Enc]", info);
//        }
//        switch (c_temp->type) {
//            case AVMEDIA_TYPE_VIDEO:
//                sprintf(info, "%s[Video]", info);
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                sprintf(info, "%s[AudioOpensl]", info);
//                break;
//            default:
//                sprintf(info, "%s[Other]", info);
//                break;
//        }
//        sprintf(info, "%s[%10s]\n", info, c_temp->name);
//        c_temp = c_temp->next;
//    }
//    __android_log_print(ANDROID_LOG_INFO, "myTag", "info:\n%s", info);
//}
//
//
//void custom_log(void *ptr, int level, const char* fmt, va_list vl){
//    FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
//    if(fp){
//        vfprintf(fp,fmt,vl);
//        fflush(fp);
//        fclose(fp);
//    }
//}
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_willxing_example_FFmpegActivity_decode
//        (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr)
//{
//    AVFormatContext	*pFormatCtx;
//    int				i, videoStreamindex;
//    AVCodecContext	*pCodecCtx;
//    AVCodec			*pCodec;
//    AVFrame	*pFrame,*pFrameYUV;
//    uint8_t *out_buffer;
//    AVPacket *packet;
//    int y_size;
//    int ret, got_picture;
//    struct SwsContext *img_convert_ctx;
//    FILE *fp_yuv;
//    int frame_cnt;
//    clock_t time_start, time_finish;
//    double  time_duration = 0.0;
//    char input_str[500]={0};
//    char output_str[500]={0};
//    char info[1000]={0};
//    sprintf(input_str,"%s",env->GetStringUTFChars(input_jstr, NULL));
//    sprintf(output_str,"%s",env->GetStringUTFChars(output_jstr, NULL));
//    LOGE("input_str = %s",input_str);//测试
//    //FFmpeg av_log() callback
//    av_log_set_callback(custom_log);
//    av_register_all();
//    avformat_network_init();
//    pFormatCtx = avformat_alloc_context();
//    if(avformat_open_input(&pFormatCtx,input_str,NULL,NULL)!=0){
//        LOGE("Couldn't open input stream.\n");
//        return -1;
//    }
//    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
//        LOGE("Couldn't find stream information.\n");
//        return -1;
//    }
//    videoStreamindex=-1;
//    for(i=0; i<pFormatCtx->nb_streams; i++)
//        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
//            videoStreamindex=i;
//            break;
//        }
//    if(videoStreamindex==-1){
//        LOGE("Couldn't find a video stream.\n");
//        return -1;
//    }
//    pCodecCtx=pFormatCtx->streams[videoStreamindex]->codec;
//    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
//    if(pCodec==NULL){
//        LOGE("Couldn't find Codec.\n");
//        return -1;
//    }
//    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
//        LOGE("Couldn't open codec.\n");
//        return -1;
//    }
//    pFrame=av_frame_alloc();
//    pFrameYUV=av_frame_alloc();
//    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1));
//    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
//                         AV_PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height,1);
//    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//    sprintf(info,   "[Input     ]%s\n", input_str);
//    sprintf(info, "%s[Output    ]%s\n",info,output_str);
//    sprintf(info, "%s[Format    ]%s\n",info, pFormatCtx->iformat->name);
//    sprintf(info, "%s[Codec     ]%s\n",info, pCodecCtx->codec->name);
//    sprintf(info, "%s[Resolution]%dx%d\n",info, pCodecCtx->width,pCodecCtx->height);
//    fp_yuv=fopen(output_str,"wb+");
//    if(fp_yuv==NULL){
//        printf("Cannot open output file.\n");
//        return -1;
//    }
//    frame_cnt=0;
//    time_start = clock();
//    while(av_read_frame(pFormatCtx, packet)>=0){
//        if(packet->stream_index==videoStreamindex){
//            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//            if(ret < 0){
//                LOGE("Decode Error.\n");
//                return -1;
//            }
//            if(got_picture){
//                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//                          pFrameYUV->data, pFrameYUV->linesize);
//                y_size=pCodecCtx->width*pCodecCtx->height;
//                fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
//                fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
//                fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
//                //Output info
//                char pictype_str[10]={0};
//                switch(pFrame->pict_type){
//                    case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
//                    case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
//                    case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
//                    default:sprintf(pictype_str,"Other");break;
//                }
//                LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
//                frame_cnt++;
//            }
//        }
//        av_free_packet(packet);
//    }
//    //flush decoder
//    //FIX: Flush Frames remained in Codec
//    while (1) {
//        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//        if (ret < 0)
//            break;
//        if (!got_picture)
//            break;
//        sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//                  pFrameYUV->data, pFrameYUV->linesize);
//        int y_size=pCodecCtx->width*pCodecCtx->height;
//        fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
//        fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
//        fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
//        //Output info
//        char pictype_str[10]={0};
//        switch(pFrame->pict_type){
//            case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
//            case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
//            case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
//            default:sprintf(pictype_str,"Other");break;
//        }
//        LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
//        frame_cnt++;
//    }
//    time_finish = clock();
//    time_duration=(double)(time_finish - time_start);
//    sprintf(info, "%s[Time      ]%fms\n",info,time_duration);
//    sprintf(info, "%s[Count     ]%d\n",info,frame_cnt);
//    sws_freeContext(img_convert_ctx);
//    fclose(fp_yuv);
//    av_frame_free(&pFrameYUV);
//    av_frame_free(&pFrame);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//    return 0;
//}
//
//ANativeWindow *nativeWindow;
//ANativeWindow_Buffer windowBuffer;
//
//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_willxing_example_FFmpegActivity_play
//        (JNIEnv *env, jobject obj, jstring input_jstr, jobject surface) {
//    AVFormatContext *pFormatCtx;
//    int i, videoStreamindex;
//    AVCodecContext *pCodecCtx;
//    AVCodec *pCodec;
//    struct SwsContext *img_convert_ctx;
//    char input_str[500] = {0};
//    sprintf(input_str, "%s", env->GetStringUTFChars(input_jstr, NULL));
//    av_register_all();
//    avformat_network_init();
//    pFormatCtx = avformat_alloc_context();
//    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
//        LOGE("Couldn't open input stream.\n");
//        return -1;
//    }
//    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//        LOGE("Couldn't find stream information.\n");
//        return -1;
//    }
//    videoStreamindex = -1;
//    for (i = 0; i < pFormatCtx->nb_streams; i++)
//        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoStreamindex = i;
//            break;
//        }
//    if (videoStreamindex == -1) {
//        LOGE("Couldn't find a video stream.\n");
//        return -1;
//    }
//    pCodecCtx = pFormatCtx->streams[videoStreamindex]->codec;
//    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//    LOGE("pCodecCtx ==  %d" ,pCodecCtx->codec_id );
//    if (pCodec == NULL) {
//        LOGE("Couldn't find Codec.\n");
//        return -1;
//    }
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//        LOGE("Couldn't open codec.\n");
//        return -1;
//    }
//    //获取界面传下来的surface
//    nativeWindow = ANativeWindow_fromSurface(env, surface);
//    if (0 == nativeWindow) {
//        LOGE("Couldn't get native window from surface.\n");
//        return -1;
//    }
//    int width = pCodecCtx->width;
//    int height = pCodecCtx->height;
//    //分配一个帧指针，指向解码后的原始帧
//    AVFrame *vFrame = av_frame_alloc();
//    AVPacket *vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
//    AVFrame *pFrameRGBA = av_frame_alloc();
//    img_convert_ctx = sws_getContext(width, height, pCodecCtx->pix_fmt,
//                                     width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
//    if (0 >
//        ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888)) {
//        LOGE("Couldn't set buffers geometry.\n");
//        ANativeWindow_release(nativeWindow);
//        return -1;
//    }
//    //读取帧
//    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
//        if (vPacket->stream_index == videoStreamindex) {
//            //视频解码
//            int ret = avcodec_send_packet(pCodecCtx, vPacket);
//            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
//                LOGE("video avcodec_send_packet error %d", ret);
//                return -1;
//            }
//            ret = avcodec_receive_frame(pCodecCtx, vFrame);
//            if (ret < 0 && ret != AVERROR_EOF) {
//                LOGE("video avcodec_receive_frame error %d", ret);
//                av_packet_unref(vPacket);
//                continue;
//            }
//            //转化格式
//            sws_scale(img_convert_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize, 0,
//                      pCodecCtx->height,
//                      pFrameRGBA->data, pFrameRGBA->linesize);
//            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
//                LOGE("cannot lock window");
//                return -1;
//            } else {
//                av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize,
//                                     (const uint8_t *) windowBuffer.bits, AV_PIX_FMT_RGBA,
//                                     width, height, 1);
//                ANativeWindow_unlockAndPost(nativeWindow);
//            }
//        }
//        av_packet_unref(vPacket);
//    }
//    //释放内存
//    sws_freeContext(img_convert_ctx);
//    av_free(vPacket);
//    av_free(pFrameRGBA);
//    avcodec_close(pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//    return 0;
//}

extern "C"
JNIEXPORT void JNICALL
Java_com_willxing_ffmpegplayer_MainActivity_play(JNIEnv *env, jclass clz, jstring url_,
                                                 jobject surface) {

    player_play_audio(env,clz,url_,surface);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_willxing_ffmpegplayer_MainActivity_pause(JNIEnv *env, jobject instance) {
    LOGE("video Java_com_willxing_ffmpegplayer_MainActivity_pause error %d", 1);
    player_pause_video();

}


extern "C"
JNIEXPORT void JNICALL
Java_com_willxing_ffmpegplayer_MainActivity_stop(JNIEnv *env, jobject instance) {
    LOGE("video Java_com_willxing_ffmpegplayer_MainActivity_stop error %d", 1);
    player_stop_video();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_willxing_ffmpegplayer_MainActivity_reset(JNIEnv *env, jclass clz,jstring url_) {
    LOGE("video Java_com_willxing_ffmpegplayer_MainActivity_reset error %d", 1);
    const char * url = env->GetStringUTFChars(url_, 0);
//    Java_com_willxing_example_FFmpegActivity_play(env,instance,url_,surface);
//    env->ReleaseStringUTFChars(url_, url);
    LOGE("open input stream.  == %s",url_ );
    LOGE("open input stream.  ====== %s",url );
    prepare(url);
    InitOpenSL(env,clz);
}