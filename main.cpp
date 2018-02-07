#include <iostream>
//#include <opencv2/opencv.hpp>


extern "C"{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
}

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
  avformat_network_init();

  AVFormatContext *ctx = avformat_alloc_context();
  if(!ctx) {
    std::cerr << "Error allocating avformat context" << std::endl;
    return EXIT_FAILURE;
  }


  return 0;
}

