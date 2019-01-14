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
    input.open("/path/to/test.yuv",  std::ios::in | std::ios_base::binary);
    std::ofstream file;
    file.open("/path/to/test.mp4",  std::ios::out | std::ios_base::binary);
    AVCodec *codec;
    AVFrame *frame;
    AVCodecContext *context;
    AVPacket *pkt;
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

    pkt = av_packet_alloc();
    if (!pkt) {
        std::cout << "6" << std::endl;
        return -1;
    }


    frame->pts = 0;

    for(int i = 0; i < 100; i++) {
        uint8_t* data = (uint8_t *)av_malloc(478 * 850 * 3 / 2);
        input.read((char *)(data), 478 * 850 * 3 / 2);
        frame->data[0] = (uint8_t *)av_malloc(frame->linesize[0] * 850);
        //file.write((char*)buffer->DataY(), buffer->width() * buffer->height());
        //std::cout << "Y" << frame->linesize[kYPlaneIndex] << std::endl;
        frame->data[1] = (uint8_t *)av_malloc(frame->linesize[1] * 850);
        //file.write((char*)buffer->DataU(), buffer->width() * buffer->height() * 1 / 4);
        //std::cout << "U" << frame->linesize[kUPlaneIndex] << std::endl;
        frame->data[2] = (uint8_t *)av_malloc(frame->linesize[2] * 850);
        for (int j = 0 ; j < 850; j ++) {
            memcpy(frame->data[0] + j * frame->linesize[0], data + 478 * j, 478);
        }
        for (int j = 0 ; j < 850; j ++) {
            memcpy(frame->data[1] + j * frame->linesize[1], data + 478 * 850 + 239 * j, 239);
        }
        for (int j = 0 ; j < 850; j ++) {
            memcpy(frame->data[2] + j * frame->linesize[2], data + 478 * 850 * 5 / 4 + 239 * j, 239);
        }
        // Encode!
        int enc_ret;
        enc_ret = avcodec_send_frame(context, frame);
        if (enc_ret != 0) {
            return -1;
        }
        frame->pts++;
        while (enc_ret >= 0) {
            enc_ret = avcodec_receive_packet(context, pkt);
            if (enc_ret == AVERROR(EAGAIN) || enc_ret == AVERROR_EOF) {
                break;
            } else if (enc_ret < 0) {
                return -1;
            }

            file.write((char*)pkt->data, pkt->size);
            av_packet_unref(pkt);

        }

    }

}