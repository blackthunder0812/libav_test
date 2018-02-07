#include <iostream>
//#include <opencv2/opencv.hpp>

#ifdef __cplusplus
extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}
#endif

int main(int argc, char* argv[])
{
  (void) argc; (void) argv;
  std::string uri = "rtsp://sonbn812.ddns.net:8086/ch0_0.h264";
//  cv::Mat frame;
//  cv::VideoCapture cap;
//  if(!cap.open(uri, cv::CAP_FFMPEG)) {
//    std::cerr << "Cannot open capture device" << std::endl;
//    return EXIT_FAILURE;
//  } else {
//    for (;;) {
//      if(cap.grab()) {
//        cap.read(frame);
//      } else {
//        std::cerr << "Grab frame failed" << std::endl;
//      }
//    }
//  }

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
      if (avformat_open_input(&ctx, uri.c_str(), rtsp_input_format, NULL) != 0) {
        std::cerr << "Error opening stream" << std::endl;
        return EXIT_FAILURE;
      } else {
        std::cout << "Probing stream ..." << std::endl;
        if (avformat_find_stream_info(ctx,  NULL) < 0) {
          std::cerr << "ERROR could not get the stream info" << std::endl;
          return EXIT_FAILURE;
        } else {
          std::cout << "Finding codecs ..." << std::endl;
          for (unsigned int i = 0; i < ctx->nb_streams; i++) {
            AVCodec *pCodec = NULL;
            pCodec = avcodec_find_decoder(ctx->streams[i]->codecpar->codec_id);
            if (pCodec == NULL) {
              std::cerr << "ERROR unsupported codec for stream " << ctx->streams[i]->id << std::endl;
            } else {
              std::cout << "Codec for stream " << ctx->streams[i]->id << " is " << pCodec->id << std::endl;
            }
          }
        }
      }
    }
  }
  return 0;
}

