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

    cam.Stop();
    cam.Run();
    InitData initData;
    initData.framerate = 30;
    initData.width = 640; // 320
    initData.height = 480; // 240
    initData.filename = "test.avi";
    cam.Init(&initData);
    cam.Run();

    usleep(10000000);
    cam.Stop();

    return 0;
}