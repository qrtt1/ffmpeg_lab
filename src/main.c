#include "libavformat/avformat.h"
#include <stdio.h>
#include <string.h>

#define IS_HLS_STREAMING(ctx_ptr) (strcmp("applehttp", ctx_ptr->iformat->name)==0) 

void convert_selected_avstreams(AVFormatContext *ctx, int video_index, int audio_index) {
}

void convert_media(AVFormatContext *ctx, const char* input) {
    int ret = avformat_open_input(&ctx, input, NULL, NULL);
    if(ret != 0) {
        printf("open failed\n");
        return ;
    }

    printf("nb-streams: %d\n", ctx->nb_streams);
    if (IS_HLS_STREAMING( ctx )) {
        printf("open ok :: %s %d\n", ctx->iformat->name, strcmp("applehttp", ctx->iformat->name));

        int video_index = -1;
        int audio_index = -1;
        for(int i=0; i<ctx->nb_streams; i++) {
            if(AVMEDIA_TYPE_VIDEO == ctx->streams[i]->codec->codec_type && video_index == -1) {
                video_index = i;
            }
            if(AVMEDIA_TYPE_AUDIO == ctx->streams[i]->codec->codec_type && video_index == -1) {
                audio_index = i;
            }

            printf("stream: %p, index: %d\n", ctx->streams[i], i);
        }

        printf("video_index: %d, audio_index: %d\n", video_index, audio_index);
        if(video_index >=0 && audio_index >=0) {
            convert_selected_avstreams(ctx, video_index, audio_index);
        }
    }
    avformat_close_input(&ctx);
}


int main(void) {
    printf("configuration: %s \n", avformat_configuration());

    av_register_all();
    avformat_network_init();

    AVFormatContext *ctx = avformat_alloc_context();
    convert_media(ctx, "http://live.dltv.cn:81/live2/live2.m3u8");
    avformat_free_context(ctx);
}
