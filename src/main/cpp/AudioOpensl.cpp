//
// Created by saite on 2019/8/4.
//

#include <android/log.h>
#include "AudioOpensl.h"

AudioOpensl::AudioOpensl() {

}

AudioOpensl::~AudioOpensl() {
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
}
DecodeParam localparams;
AudioOpensl *audio ;
void  bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    LOGI("playerCallback");
    assert(bq == audio->bqPlayerBufferQueue);

    getPCM(&localparams.buffer, &localparams.bufferSize);;
    //assert(NULL == context);
//    audio.getPCM(&audio.buffer, &audio.bufferSize);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (NULL != localparams.buffer && 0 != localparams.bufferSize) {
        SLresult result;
        // enqueue another buffer
        result = (*audio->bqPlayerBufferQueue)->Enqueue(audio->bqPlayerBufferQueue, localparams.buffer,
                                                       localparams.bufferSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
        LOGI("Enqueue:%d", result);
        (void)result;
    }
}

void  initDecodePCM(DecodeParam params,AudioOpensl *aa){
    localparams = params;
    audio = aa;
}

void getPCM(void **pcm, size_t *pcmSize) {
    LOGE("player_play_audio ==1");
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    int i_time =0;
    int frameNumber = 120;
    int  data_size_all = 0;

    void *buffer = localparams.buff;
    size_t bufferSize  = localparams.bufferSize;
    AVFormatContext* avformat_context  = localparams.avformat_context;
    AVCodecContext*  avcodec_context  = localparams.avcodec_context;
    AVPacketQueue* queue = localparams.queue;
    int audioStream = localparams. audioStream;
    SwrContext * swr = localparams. swr;
    uint8_t * outputBuffer = localparams.outputBuffer;
    size_t  outputBufferSize  = localparams.outputBufferSize;
    uint8_t * buff = localparams.  buff;

    outputBufferSize = 8192 ;
    outputBuffer = (uint8_t *) malloc(sizeof(uint8_t) * outputBufferSize * frameNumber);
    uint8_t *tmp = outputBuffer;
    while((&queue->audio_packets)->size() > 0 && i_time < frameNumber) {
        queue->get_audio_packet(avPacket);
        LOGE("player_play_audio ==  %d  === %d", avPacket->pts, &avPacket);
        AVFrame *aFrame = av_frame_alloc();
        int frameFinished = 0;
        // Is this a packet from the audio stream?
        if ((avPacket->stream_index) == audioStream) {
            avcodec_decode_audio4(avcodec_context, aFrame, &frameFinished, avPacket);

            if (frameFinished) {
                // data_size为音频数据所占的字节数
                int data_size_in = av_samples_get_buffer_size(
                        aFrame->linesize, avcodec_context->channels,
                        aFrame->nb_samples, avcodec_context->sample_fmt, 1);

                int data_size_out = av_samples_get_buffer_size(
                        aFrame->linesize, avcodec_context->channels,
                        aFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);
//                // 音频格式转换
                uint8_t *buff = (uint8_t *) av_malloc(data_size_out);
                swr_convert(swr, &buff, aFrame->nb_samples,
                            (const uint8_t **) aFrame->extended_data,
                            aFrame->nb_samples);

                data_size_all += data_size_out;
                memcpy(tmp,buff,data_size_out);
                tmp = tmp + data_size_out;
                i_time++;
                LOGE("memcpy data_size_all ==  %d  data_size_out === %d", data_size_all,data_size_out);
            }
        }
    }
    *pcm = outputBuffer;
    *pcmSize = data_size_all;
}


void AudioOpensl::createEngine(JNIEnv* env, jclass clazz)
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example
}

void AudioOpensl::createBufferQueueAudioPlayer(JNIEnv* env, jclass clazz, int rate, int channel,int bitsPerSample)
{
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
//    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_16,
//        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
//        SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};
    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.numChannels = channel;
    format_pcm.samplesPerSec = rate * 1000;
    format_pcm.bitsPerSample = bitsPerSample;
    format_pcm.containerSize = 16;
    if(channel == 2)
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    else
        format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
// realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                             &bqPlayerEffectSend);
    assert(SL_RESULT_SUCCESS == result);

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
#endif

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);

// set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);

}

void AudioOpensl::AudioWrite(const void*buffer, int size)
{
    (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, size);
}


