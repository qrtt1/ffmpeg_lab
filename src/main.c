#include "libavformat/avformat.h"
#include <stdio.h>

int main(void) {
    printf("configuration: %s \n", avformat_configuration());

    av_register_all();
    avformat_network_init();

    AVFormatContext *ctx = avformat_alloc_context();
    avformat_free_context(ctx);
}
