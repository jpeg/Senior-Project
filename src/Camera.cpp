// Camera.cpp

#include "Camera.h"

Camera::Camera()
{
#if defined(__linux__) && defined(__arm__)
    // Start raspicam V4L driver if there's no video device
    system("if ! [ -e /dev/video0 ]\nthen\n uv4l --driver raspicam --auto-video_nr --encoding jpeg --nopreview\nfi");
#endif
}

Camera::~Camera()
{
}

bool Camera::init()
{
    this->vidCap = new cv::VideoCapture(-1);
    if (!this->vidCap->isOpened())
    {
        return false;
    }
    
    return true;
}

void Camera::shutdown()
{
    delete this->vidCap;
}

bool Camera::captureFrame()
{
    this->lastFrameTime = this->currentFrameTime;
    
#if __GNUC__ == 4 && __GNUC_MINOR__ < 7
    this->currentFrameTime = std::chrono::monotonic_clock::now();
#else
    this->currentFrameTime = std::chrono::steady_clock::now();
#endif
    
    return this->vidCap->read(this->currentFrame);
}

float Camera::getFPS()
{
    std::chrono::duration<float> time = this->currentFrameTime - this->lastFrameTime;
    
    return 1 / time.count();;
}

