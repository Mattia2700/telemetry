#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include <opencv4/opencv2/opencv.hpp>

#include "camera.h"

using namespace std;
using namespace chrono;

int framerate = 30;
double get_timestamp();

int main ( int argc,char **argv ) {

    Camera cam;

    cam.Stop();
    cam.Run();
    InitData initData;
    initData.framerate = 30;
    initData.width = 640;
    initData.height = 480;
    initData.filename = "test.avi";
    cam.Init(&initData);
    cam.Run();

    usleep(10000000);
    cam.Stop();

    return 0;

    raspicam::RaspiCam Camera; //Cmaera object

    Camera.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    Camera.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
    Camera.setAWB(raspicam::RASPICAM_AWB_AUTO);
    Camera.setImageEffect(raspicam::RASPICAM_IMAGE_EFFECT_NONE);
    Camera.setWidth(640);
    Camera.setHeight(480);
    Camera.setVideoStabilization(true);
    Camera.setFrameRate(framerate);


    //Open camera 
    cout<< "Opening Camera..."<< endl;
    if ( !Camera.open()) {
      cerr<< "Error opening camera" << endl;
      return -1;
    }
    //wait a while until camera stabilizes
    cout << "Sleeping" << endl;
    usleep(0.5*1000000);
    cout << "Done" << endl;
    //capture
    //allocate memory
    size_t framesize = Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB );
    unsigned char *data = new unsigned char[framesize];
    cv::Mat img(480, 640, CV_8UC3);


    cv::VideoWriter writer;
    if(!writer.open("test.avi", cv::VideoWriter::fourcc('M','J','P','G'), framerate, cv::Size(640, 480), true))
    {
      cout << "Cannot open file" << endl;
      return 0;
    }


    while(true)
    {
      double t0 = get_timestamp();

      Camera.grab();
      // extract the image in rgb format
      Camera.retrieve ( data ); // get camera image

      img.data = data;
      if(!img.empty())
      {
        cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
        writer.write(img);
      }

      double t1 = get_timestamp();

      usleep((1.0 / framerate) - (t1 - t0));
    }

    writer.release();

    //free resrources    
    delete data;
    return 0;
}

double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}