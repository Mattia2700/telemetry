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

int main ( int argc,char **argv ) {

    Camera cam;
    CamInitData initData;
    CamRunData runData;

    cam.Stop();
    cam.Run(&runData);
    initData.framerate = 30;
    initData.width = 640; // 320
    initData.height = 480; // 240
    cam.Init(&initData);
    runData.filename = "test.avi";
    cam.Run(&runData);

    usleep(10000000);
    cam.Stop();

    return 0;
}