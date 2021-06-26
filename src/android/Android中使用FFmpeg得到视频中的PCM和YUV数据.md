使用FFmpeg获取PCM和YUV数据的流程基本上一样的，下面就以获取YUV数据的流程为例，说明这个过程：
* 初始化AVFormatContext 。
* 打开文件，获取流信息，获取视频流/音频流。
* 找到解码器，并且初始化解码器。
* 初始化AVPacket ，AVFrame ，和buffer。
* 对输出格式进行规范，如视频的宽高，音频的采用率，声道数等。
* 读取一帧数据，然后把数据写入到文件。
* 读完数据后，释放内存。

```
#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include "android/log.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
};

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"jni",FORMAT,##__VA_ARGS__);

extern "C"
JNIEXPORT jint JNICALL
Java_audioplayer_MainActivity_getYuvData(JNIEnv *env, jobject instance, jstring srcPath_,
                                                   jstring desPath_) {
    const char *srcPath = env->GetStringUTFChars(srcPath_, 0);
    const char *desPath = env->GetStringUTFChars(desPath_, 0);

    AVFormatContext *pFormatCtx;
    int i,videoIndex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    /* init */
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    /* open file,find streams,and then find a video stream */
    if(avformat_open_input(&pFormatCtx,srcPath,NULL,NULL)!=0){
        LOGD("Couldn't open input stream.");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        LOGD("Couldn't find stream information.");
        return -1;
    }
    videoIndex = -1;
    for(i=0;i<pFormatCtx->nb_streams;i++){
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
            videoIndex = i;
            break;
        }
    }
    if(videoIndex == -1){
        LOGD("Don't find a video stream.");
        return -1;
    }

    /* find a decoder */
    pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        LOGD("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx,pCodec,NULL)<0){
        LOGD("Could not open codec.");
        return -1;
    }

    LOGD("File format: %s.\nVideo duration: %lld.\nVideo width: %d,Video height: %d.",
        pFormatCtx->iformat->name,pFormatCtx->duration,pCodecCtx->width,pCodecCtx->height);

    /* init Buffer */
    AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameYuv = av_frame_alloc();
    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrameYuv,out_buffer,AV_PIX_FMT_YUV420P,pCodecCtx->width,pCodecCtx->height);

    struct SwsContext *swsContext= sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    int ret,got_picture;

    FILE *fp = fopen(desPath,"wb+");
    int frame_cnt = 0;

    /* ever time read a frame and decode it */
    while(av_read_frame(pFormatCtx,packet)>=0){
        if(packet->stream_index == videoIndex){
            ret = avcodec_decode_video2(pCodecCtx,pFrame,&got_picture,packet);
            if(ret < 0){
                LOGD("Decode Error.");
                return -1;
            }
            if(got_picture){
                sws_scale(swsContext, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                          pFrameYuv->data, pFrameYuv->linesize);
                LOGD("Decoded frame index: %d\n",frame_cnt);
                fwrite(pFrameYuv->data[0],1,pCodecCtx->width * pCodecCtx->height,fp);
                fwrite(pFrameYuv->data[1],1,pCodecCtx->width * pCodecCtx->height / 4,fp);
                fwrite(pFrameYuv->data[2],1,pCodecCtx->width * pCodecCtx->height / 4,fp);
                fflush(fp);
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }

    LOGD("Decode end");
    fclose(fp);
    sws_freeContext(swsContext);
    av_frame_free(&pFrameYuv);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(srcPath_, srcPath);
    env->ReleaseStringUTFChars(desPath_, desPath);
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_audioplayer_MainActivity_getPcmData(JNIEnv *env, jobject instance, jstring srcPath_,
                                                   jstring desPath_) {
    const char *srcPath = env->GetStringUTFChars(srcPath_, 0);
    const char *desPath = env->GetStringUTFChars(desPath_, 0);

    AVFormatContext *pFormatCtx;
    int i,audioIndex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    /* init */
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    /* open file,find streams,and then find a video stream */
    if(avformat_open_input(&pFormatCtx,srcPath,NULL,NULL)!=0){
        LOGD("Couldn't open input stream.");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        LOGD("Couldn't find stream information.");
        return -1;
    }
    audioIndex = -1;
    for(i=0;i<pFormatCtx->nb_streams;i++){
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audioIndex = i;
            break;
        }
    }
    if(audioIndex == -1){
        LOGD("Don't find a video stream.");
        return -1;
    }

    /* find a decoder */
    pCodecCtx = pFormatCtx->streams[audioIndex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL){
        LOGD("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx,pCodec,NULL)<0){
        LOGD("Could not open codec.");
        return -1;
    }

    LOGD("File format: %s.\nVideo duration: %lld.\nVideo width: %d,Video height: %d.",
         pFormatCtx->iformat->name,pFormatCtx->duration,pCodecCtx->width,pCodecCtx->height);

    /* init Buffer */
    AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
    AVFrame *pFrame = av_frame_alloc();

    /* change sample rate and format to a standard format */
    SwrContext *swrCtx = swr_alloc();
    enum AVSampleFormat in_sample_fmt = pCodecCtx->sample_fmt;
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int in_sample_rate = pCodecCtx->sample_rate;
    int out_sample_rate = 44100;
    uint64_t in_ch_layout = pCodecCtx->channel_layout;
    uint64_t  out_ch_layout = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(swrCtx,out_ch_layout,out_sample_fmt,out_sample_rate,
        in_ch_layout,in_sample_fmt,in_sample_rate,0,NULL);
    swr_init(swrCtx);

    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    uint8_t *out_buffer = (uint8_t *) av_malloc(2 * 44100);
    FILE *fp = fopen(desPath,"wb+");

    int ret,got_frame,frame_cnt = 0;

    while(av_read_frame(pFormatCtx,packet)>=0){
        if(packet->stream_index == audioIndex){
            ret = avcodec_decode_audio4(pCodecCtx,pFrame,&got_frame,packet);
            if(ret < 0){
                LOGD("Decode end");
            }
            if(got_frame){
                LOGD("Decoded frame index: %d\n",frame_cnt);
                swr_convert(swrCtx,&out_buffer,2 * 44100,(const uint8_t **)pFrame->data,pFrame->nb_samples);
                int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb, pFrame->nb_samples, out_sample_fmt, 1);
                fwrite(out_buffer, 1, out_buffer_size, fp);
                fflush(fp);
                frame_cnt++;
            }

        }
        av_free_packet(packet);
    }
    LOGD("Decode finish");
    fclose(fp);
    swr_free(&swrCtx);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(srcPath_, srcPath);
    env->ReleaseStringUTFChars(desPath_, desPath);
    return 0;
}
```
