//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_AUDIO_H
#define ANDROIDPROJECT_AUDIO_H

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <assert.h>
#include "AVPacketQueue.h"
#include <pthread.h>
#include "Log.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}




class AudioOpensl {

public:
    // engine interfaces
     SLObjectItf engineObject = NULL;
     SLEngineItf engineEngine;

// output mix interfaces
     SLObjectItf outputMixObject = NULL;
     SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer avPacketQueue player interfaces
     SLObjectItf bqPlayerObject = NULL;
     SLPlayItf bqPlayerPlay;
     SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
     SLEffectSendItf bqPlayerEffectSend;
     SLMuteSoloItf bqPlayerMuteSolo;
     SLVolumeItf bqPlayerVolume;


// aux effect on the output mix, used by the buffer avPacketQueue player
     const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// file descriptor player interfaces
     SLObjectItf fdPlayerObject = NULL;
     SLPlayItf fdPlayerPlay;
     SLSeekItf fdPlayerSeek;
     SLMuteSoloItf fdPlayerMuteSolo;
     SLVolumeItf fdPlayerVolume;


public:
    AudioOpensl();
    ~AudioOpensl();
    void initopensl();

    void createEngine(JNIEnv *env, jclass clazz);

//    void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    void
    createBufferQueueAudioPlayer(JNIEnv *env, jclass clazz, int rate, int channel, int bitsPerSample);

    void AudioWrite(const void *buffer, int size);


    void SetPlayState(unsigned int);
};

struct DecodeParam{
    AVFormatContext *avformat_context;
    AVCodecContext *avcodec_context;
    AVPacketQueue *queue;
    int audioStream;
    SwrContext *swr;
};
void  initDecodePCM(DecodeParam params,AudioOpensl *audioOpensl);
void  getPCM(void **pVoid, size_t *pInt);
void  bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

#endif //ANDROIDPROJECT_AUDIO_H
