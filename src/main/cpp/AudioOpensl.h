//
// Created by saite on 2019/8/4.
//

#ifndef ANDROIDPROJECT_AUDIO_H
#define ANDROIDPROJECT_AUDIO_H

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <assert.h>


#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "will_e", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "will_i", format, ##__VA_ARGS__)

void  bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

class AudioOpensl {

public:
    // engine interfaces
     SLObjectItf engineObject = NULL;
     SLEngineItf engineEngine;

// output mix interfaces
     SLObjectItf outputMixObject = NULL;
     SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
     SLObjectItf bqPlayerObject = NULL;
     SLPlayItf bqPlayerPlay;
     SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
     SLEffectSendItf bqPlayerEffectSend;
     SLMuteSoloItf bqPlayerMuteSolo;
     SLVolumeItf bqPlayerVolume;


// aux effect on the output mix, used by the buffer queue player
     const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// file descriptor player interfaces
     SLObjectItf fdPlayerObject = NULL;
     SLPlayItf fdPlayerPlay;
     SLSeekItf fdPlayerSeek;
     SLMuteSoloItf fdPlayerMuteSolo;
     SLVolumeItf fdPlayerVolume;

     void *buffer;
     size_t bufferSize;

public:
    AudioOpensl();
    ~AudioOpensl();
    void initopensl();

    void createEngine(JNIEnv *env, jclass clazz);

//    void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    void
    createBufferQueueAudioPlayer(JNIEnv *env, jclass clazz, int rate, int channel, int bitsPerSample);

    void AudioWrite(const void *buffer, int size);

    void getPCM(void **pVoid, size_t *pInt);
};


#endif //ANDROIDPROJECT_AUDIO_H