// DetectObject.h

#include <opencv2/opencv.hpp>
#include <math.h>

class DetectObject
{
    // Constants
public:
    static const int IMAGE_WIDTH = 320;
    static const int IMAGE_HEIGHT = 240;
    static const int IMAGE_CHANNELS_HLS = 3;
    enum IMAGE_CHANNELS_HLS_ENUM { HUE, LIGHTNESS, SATURATION };
    static const int CELL_SIZE = 20;
    
private:
    static const int ROWS = IMAGE_HEIGHT / CELL_SIZE;
    static const int COLUMNS = IMAGE_WIDTH / CELL_SIZE;
    
    static constexpr float CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_HUE = 1.4f;
    static constexpr float CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_LIGHTNESS = 1.6f;
    static constexpr float CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_SATURATION = 1.6f;
    static constexpr float CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_GRAY = 1.3f;
    
    static const int MIN_OBJECT_WIDTH = 5;
    static const int MIN_OBJECT_HEIGHT = 4;
    
    // Variables
private:
    int trainingHistoryLengthHLS;
    int trainingHistoryLengthGray;
    struct ImageTrainingData
    {
        // Important values
        unsigned char mean;
        float standardDeviation;
        
        // Needed for training
        unsigned int sum;
        unsigned long sumSquares;
    };
    ImageTrainingData trainingDataHLS[ROWS][COLUMNS][IMAGE_CHANNELS_HLS];
    ImageTrainingData trainingDataGray[ROWS][COLUMNS];
    
    bool imageResults[ROWS][COLUMNS];
    int imageResultsObjectWidth;
    int imageResultsObjectHeight;
    
    // Constructors
public:
    DetectObject();
    
    // Destructor
public:
    ~DetectObject();
    
    // Methods
public:
    void init();
    
    void trainHLS(cv::Mat image);
    void trainGray(cv::Mat image);
    
    void resetTrainingHLS();
    void resetTrainingGray();
    
    bool checkObjectHLS(cv::Mat image);
    bool checkObjectGray(cv::Mat image);
    
    bool checkObjectSize();
    
    cv::Mat generateDebugImage(cv::Mat inputImage);
    
    int getObjectWidth() { return this->imageResultsObjectWidth; }
    int getObjectHeight() { return this->imageResultsObjectHeight; }
    
private:
    void updateImageResultsHLS(cv::Mat* imageHLS, cv::Mat* imageBGR);
    void updateImageResultsGray(cv::Mat* imageGray);
    
    cv::Scalar cellFunctionHLS(int row, int column, cv::Mat* imageHLS, cv::Mat* imageBGR);
    float cellFunctionGray(int row, int column, cv::Mat* imageGray);
    
    int sizeRecurseUp(int x, int y);
    int sizeRecurseDown(int x, int y);
};

