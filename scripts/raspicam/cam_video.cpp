#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include <opencv4/opencv2/opencv.hpp>


using namespace std;

int main ( int argc,char **argv ) {
    raspicam::RaspiCam Camera; //Cmaera object

    Camera.setFormat(raspicam::RASPICAM_FORMAT_RGB);
    Camera.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
    Camera.setAWB(raspicam::RASPICAM_AWB_AUTO);
    Camera.setImageEffect(raspicam::RASPICAM_IMAGE_EFFECT_NONE);
    Camera.setWidth(640);
    Camera.setHeight(480);
    Camera.setVideoStabilization(true);


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
    cv::Mat img;


    cv::VideoWriter writer;
    if(!writer.open("test.avi", cv::VideoWriter::fourcc('M','J','P','G'), 24, cv::Size(640, 480), true))
    {
      cout << "Cannot open file" << endl;
      return 0;
    }
    while(true)
    {
      Camera.grab();
      //extract the image in rgb format
      Camera.retrieve ( data );//get camera image

      img.data = data;
      writer.write(img);

      usleep(33000);

    }

    writer.release();

    //free resrources    
    delete data;
    return 0;
}
//