#include "libavformat/avformat.h"
#include <stdio.h>
#include <string.h>

#define IS_HLS_STREAMING(ctx_ptr) (strcmp("applehttp", ctx_ptr->iformat->name)==0) 

AVStream * new_stream(AVFormatContext *in_ctx, AVFormatContext *out_ctx, int index) {
    AVStream *s = avformat_new_stream(out_ctx, in_ctx->streams[ index ]->codec->codec);
    if(s) {
        int ret = avcodec_copy_context(s->codec, in_ctx->streams[ index ]->codec);
        if(ret != 0) {
            fprintf(stderr, "failed to create stream %d\n", index);
            return NULL;
        }

        s->codec->codec_tag = 0;
        if (out_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            s->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
    }
    return s;
}

void convert_selected_avstreams(AVFormatContext *ctx, AVFormatContext *out_ctx, int video_index, int audio_index) {

    AVOutputFormat *out_fmt = out_ctx->oformat;
    AVStream *video_stream = new_stream(ctx, out_ctx, video_index);
    AVStream *audio_stream = new_stream(ctx, out_ctx, audio_index);

    if(!video_stream || !audio_stream) {
        return ;
    }
  
    int ret = 0;
    if (!(out_ctx->flags & AVFMT_NOFILE)) {
        ret = avio_open(&out_ctx->pb, "out.flv", AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", "out.flv");
            return ;
        }
    }    

    ret = avformat_write_header(out_ctx, NULL);
    printf("ret, %d\n", ret);
    if(ret<0){
        fprintf(stderr, "Error occurred when opening output file\n");
        return ;
    }

    /* o yeah */
    av_write_trailer(out_ctx);

    /* close output */
    if (out_ctx && !(out_fmt->flags & AVFMT_NOFILE))
        avio_close(out_ctx->pb);


}

void convert_media(AVFormatContext *ctx, const char* input) {
    int ret = avformat_open_input(&ctx, input, NULL, NULL);
    if(ret != 0) {
        printf("open failed\n");
        return ;
    }

    avformat_find_stream_info(ctx, NULL);

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
            AVFormatContext *out_ctx = NULL;
            avformat_alloc_output_context2(&out_ctx, NULL, NULL, "out.flv");

            if(out_ctx){
                convert_selected_avstreams(ctx, out_ctx, video_index, audio_index);
                avformat_free_context(out_ctx);
            }
        }
    }
    avformat_close_input(&ctx);
}


int main(void) {
    av_log_set_level(AV_LOG_DEBUG);
    printf("configuration: %s \n", avformat_configuration());

    av_register_all();
    avformat_network_init();

    AVFormatContext *ctx = avformat_alloc_context();
    convert_media(ctx, "http://live.dltv.cn:81/live2/live2.m3u8");
    avformat_free_context(ctx);
}
