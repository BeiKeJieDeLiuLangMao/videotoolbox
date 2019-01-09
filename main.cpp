//
// Created by 贝克街的流浪猫 on 2019-01-09.
//

#include <fstream>
extern "C" {
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
};
#include "iostream"

int main(int ac, char** av) {
    std::cout << "Start" << std::endl;
    std::ifstream input;
    input.open("/Users/CL10060-N/Downloads/haha.yuv",  std::ios::in | std::ios_base::binary);
    std::ofstream file;
    file.open("/Users/CL10060-N/Downloads/main.mp4",  std::ios::out | std::ios_base::binary);
    AVCodec *codec;
    AVFrame *frame;
    AVCodecContext *context;
    AVPacket pkt;
    int ret;
    /* find the mpeg1 video encoder */
    codec = avcodec_find_encoder_by_name("h264_videotoolbox");
    if (!codec) {
        std::cout << "1" << std::endl;
        return -1;
    }

    context = avcodec_alloc_context3(codec);
    if (!context) {
        std::cout << "2" << std::endl;
        return -1;
    }

    context->bit_rate = 2 * 1000 * 1000;
    context->rc_max_rate = 2 * 1000 * 1000;
    context->rc_min_rate = static_cast<int64_t>(2 * 1000 * 1000 * 0.8);
    /* resolution must be a multiple of two */
    context->width = 478;
    context->height = 850;

    /* frames per second */
    AVRational rational = {1, 25};
    context->time_base = rational;
    //context->max_b_frames = 0;
    context->pix_fmt = AV_PIX_FMT_YUV420P;
    /*context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->codec_id = AV_CODEC_ID_H264;
    context->gop_size = 500;
    context->color_range = AVCOL_RANGE_JPEG;*/
    //context->rc_buffer_size = 1000 * 75;

    /* open it */
    if (avcodec_open2(context, codec, nullptr) < 0) {
        std::cout << "3" << std::endl;
        return -1;
    }

    frame = av_frame_alloc();
    if (!frame) {
        std::cout << "4" << std::endl;
        return -1;
    }



    frame->width = 478;
    frame->height = 850;
    frame->format = AV_PIX_FMT_YUV420P;

    frame->key_frame = 1;
    frame->pict_type = AV_PICTURE_TYPE_I;

    /*frame->format = context->pix_fmt;
    frame->width = context->width;
    frame->height = context->height;*/
    //frame->color_range = AVCOL_RANGE_JPEG;
    //frame->sample_aspect_ratio = av_make_q(config.width, config.height);
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(frame->data, frame->linesize, context->width, context->height,
                         context->pix_fmt, 32);
    if (ret < 0) {
        std::cout << "5" << std::endl;
        return -1;
    }
    frame->pts = 0;

    while(true) {
        uint8_t* y = (uint8_t *)av_malloc(478 * 850);
        input.read((char *)(y), 478 * 850);
        frame->data[0] = y;
        frame->linesize[0] = 478;
        //file.write((char*)buffer->DataY(), buffer->width() * buffer->height());
        //std::cout << "Y" << frame->linesize[kYPlaneIndex] << std::endl;
        uint8_t* u = (uint8_t *)av_malloc(478 * 850 / 4);
        input.read((char *)(u), 478 * 850 / 4);
        frame->data[1] = u;
        frame->linesize[1] = 239;
        //file.write((char*)buffer->DataU(), buffer->width() * buffer->height() * 1 / 4);
        //std::cout << "U" << frame->linesize[kUPlaneIndex] << std::endl;
        uint8_t* v = (uint8_t *)av_malloc(478 * 850 / 4);
        input.read((char *)(v), 478 * 850 / 4);
        frame->data[2] = v;
        frame->linesize[2] = 239;
        std::cout << "Read Finished" << std::endl;
        // Encode!
        int enc_ret;
        enc_ret = avcodec_send_frame(context, frame);
        if (enc_ret != 0) {
            return -1;
        }
        std::cout << "Send Finished" << std::endl;
        frame->pts++;
        while (enc_ret >= 0) {
            enc_ret = avcodec_receive_packet(context, &pkt);
            std::cout << "Receive Finished" << std::endl;
            if (enc_ret == AVERROR(EAGAIN) || enc_ret == AVERROR_EOF) {
                break;
            } else if (enc_ret < 0) {
                return -1;
            }

            file.write((char*)pkt.data, pkt.size);
            av_packet_unref(&pkt);

        }

    }

}