#include <iostream>
#include <opencv2/opencv.hpp>

#ifdef __cplusplus
extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswscale/swscale.h>
}
#endif

int decode_video_packet(SwsContext *swsContext, AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame, cv::Mat &mat, cv::Mat &processedMat) {
  int response = avcodec_send_packet(pCodecContext, pPacket);
  if (response < 0) {
    std::cerr << "Error while sending a packet to the decoder" << std::endl;
    return response;
  }
  while (response >= 0) {
    response = avcodec_receive_frame(pCodecContext, pFrame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      break;
    } else if (response < 0) {
      std::cerr << "Error while receiving a frame from the decoder" << std::endl;
      break;
    }
    int cvLinesizes[1];
    cvLinesizes[0] = mat.step1();
    sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, &mat.data, cvLinesizes);
    cv::cvtColor(mat, processedMat, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(processedMat, processedMat, cv::Size(7, 7), 1.5, 1.5);
    cv::Canny(processedMat, processedMat, 0, 30, 3);
    cv::imshow("Stream", processedMat);
    cv::waitKey(1);
    av_frame_unref(pFrame);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  (void) argc; (void) argv;
  cv::namedWindow("Stream");
  std::string uri = "rtsp://sonbn812.ddns.net:8086/ch0_0.h264";
  av_register_all();
  avcodec_register_all();
  avformat_network_init();

  AVFormatContext *ctx = avformat_alloc_context();
  if (!ctx) {
    std::cerr << "Error allocating avformat context" << std::endl;
    return EXIT_FAILURE;
  } else {
    AVInputFormat *rtsp_input_format = NULL;
    rtsp_input_format = av_find_input_format("rtsp");
    if (rtsp_input_format == NULL) {
      std::cerr << "Cannot find rtsp format" << std::endl;
      return EXIT_FAILURE;
    } else {
      std::cout << "Opening stream " << uri << std::endl;
      AVDictionary *rtsp_options = NULL;
      if (av_dict_set(&rtsp_options, "rtsp_transport", "tcp", 0) < 0) {
        std::cerr << "Cannot set rtsp option" << std::endl;
        return EXIT_FAILURE;
      } else {
        if (avformat_open_input(&ctx, uri.c_str(), rtsp_input_format, &rtsp_options) != 0) {
          std::cerr << "Error opening stream" << std::endl;
          return EXIT_FAILURE;
        } else {
          std::cout << "Probing stream ..." << std::endl;
          if (avformat_find_stream_info(ctx,  NULL) < 0) {
            std::cerr << "ERROR could not get the stream info" << std::endl;
            return EXIT_FAILURE;
          } else {
            std::cout << "Finding suitable codecs for each streams ..." << std::endl;
            int video_stream_index = 0, audio_stream_index = 1;
            AVCodecContext *vCodecContext = NULL, *aCodecContext = NULL;
            SwsContext *swsContext = NULL;
            cv::Mat mat, dstMat;
            for (unsigned int i = 0; i < ctx->nb_streams; i++) {
              AVCodec *pCodec = NULL;
              pCodec = avcodec_find_decoder(ctx->streams[i]->codecpar->codec_id);
              if (pCodec == NULL) {
                std::cerr << "ERROR unsupported codec for stream " << ctx->streams[i]->id << std::endl;
              } else {
                std::cout << "Codec for stream " << ctx->streams[i]->id << " is type " << av_get_media_type_string(pCodec->type) << ": " << pCodec->long_name << std::endl;
                AVCodecContext *pCodecContext = NULL;
                pCodecContext = avcodec_alloc_context3(pCodec);
                if (pCodecContext == NULL) {
                  std::cerr << "Cannot allocate codec context" << std::endl;
                  return EXIT_FAILURE;
                } else {
                  if (avcodec_parameters_to_context(pCodecContext, ctx->streams[i]->codecpar) < 0) {
                    std::cerr << "Cannot pass parameters to codec" << std::endl;
                    return EXIT_FAILURE;
                  } else {
                    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
                      std::cerr << "Cannot open codec" << std::endl;
                      return EXIT_FAILURE;
                    } else {
                      if (pCodec->type == AVMEDIA_TYPE_VIDEO) {
                        vCodecContext = pCodecContext;
                        video_stream_index = i;
                        swsContext = sws_getContext(vCodecContext->width, vCodecContext->height, vCodecContext->pix_fmt, vCodecContext->width, vCodecContext->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
                        mat = cv::Mat(vCodecContext->height, vCodecContext->width, CV_8UC3);
                        dstMat = cv::Mat(vCodecContext->height, vCodecContext->width, CV_8UC3);
                      }
                      if (pCodec->type == AVMEDIA_TYPE_AUDIO) {
                        aCodecContext = pCodecContext;
                        audio_stream_index = i;
                      }
                    }
                  }
                }
              }
            }
            AVPacket *pPacket = av_packet_alloc();
            if (!pPacket) {
              std::cerr << "Failed to allocated memory for AVPacket" << std::endl;
              return EXIT_FAILURE;
            }
            AVFrame *pFrame = av_frame_alloc();
            if (!pFrame) {
              std::cerr << "Failed to allocated memory for AVFrame" << std::endl;
              return EXIT_FAILURE;
            }
            while (av_read_frame(ctx, pPacket) >= 0) {
              if (pPacket->stream_index == video_stream_index) {
//                std::cout << "Processing video packet" << std::endl;
                decode_video_packet(swsContext, pPacket, vCodecContext, pFrame, mat, dstMat);
                av_packet_unref(pPacket);
              } else if (pPacket->stream_index == audio_stream_index) {
//                std::cout << "Processing audio packet" << std::endl;
//                decode_packet(pPacket, aCodecContext, pFrame);
//                av_packet_unref(pPacket);
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

