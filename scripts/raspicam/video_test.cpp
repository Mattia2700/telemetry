#include <opencv4/opencv2/opencv.hpp>

#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  cv::Mat img;
  cv::Mat src;


  cv::VideoCapture cap;
  cap.open(0);
  cap >> img;

  cv::VideoWriter writer;
  
  if(!writer.open("test.avi", cv::VideoWriter::fourcc('M','J','P','G'), 24, img.size(), true))
  {
    cout << "Cannot open file" << endl;
    return 0;
  }
  for (int i = 0 ; i < 24*10 ; i ++)
  {
      cap >> img;
      writer.write(img);
      usleep(40000);
  }
  cap.release();
  writer.release();
  return 0;
}