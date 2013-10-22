// Camera.h

#include <opencv2/opencv.hpp>
#include <chrono>

class Camera
{
private:
    cv::VideoCapture* vidCap;
    int width;
    int height;
    cv::Mat currentFrame;
    
#if __GNUC__ == 4 && __GNUC_MINOR__ < 7
    std::chrono::time_point<std::chrono::monotonic_clock> currentFrameTime;
    std::chrono::time_point<std::chrono::monotonic_clock> lastFrameTime;
#else
    std::chrono::time_point<std::chrono::steady_clock> currentFrameTime;
    std::chrono::time_point<std::chrono::steady_clock> lastFrameTime;
#endif
    
    // Constructors
public:
    Camera();
    
    // Destructor
public:
    ~Camera();
    
    // Methods
public:
    bool init(int width, int height);
    void shutdown();
    bool captureFrame();
    float getFPS();
    
    // Accessor Methods
public:
    cv::Mat getLastFrame() { return currentFrame; }
};

