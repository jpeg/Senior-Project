// DetectObject.cpp

#include "DetectObject.h"

DetectObject::DetectObject()
{
}

DetectObject::~DetectObject()
{
}

void DetectObject::init()
{
    this->resetTrainingHLS();
    this->resetTrainingGray();
}

void DetectObject::trainHLS(cv::Mat image)
{
    cv::Mat imageHLS;
    cv::cvtColor(image, imageHLS, CV_BGR2HLS);
    
    this->trainingHistoryLengthHLS++;
    
    for(int row=0; row<ROWS; row++)
    {
        for(int column=0; column<COLUMNS; column++)
        {
            cv::Scalar cellData = this->cellFunctionHLS(row, column, &imageHLS, &image);
            
            if(trainingHistoryLengthHLS == 0)
            {
                for(int channel=0; channel<IMAGE_CHANNELS_HLS; channel++)
                {
                    this->trainingDataHLS[row][column][channel].sum = cellData[channel];
                    this->trainingDataHLS[row][column][channel].sumSquares = (int)cellData[channel]*cellData[channel];
                }
            }
            else
            {
                for(int channel=0; channel<IMAGE_CHANNELS_HLS; channel++)
                {
                    this->trainingDataHLS[row][column][channel].sum += cellData[channel];
                    this->trainingDataHLS[row][column][channel].sumSquares += (int)cellData[channel]*cellData[channel];
                }
            }
            
            for(int channel=0; channel<IMAGE_CHANNELS_HLS; channel++)
            {
                this->trainingDataHLS[row][column][channel].mean = this->trainingDataHLS[row][column][channel].sum / trainingHistoryLengthHLS;
                this->trainingDataHLS[row][column][channel].standardDeviation = 
                        sqrt((trainingHistoryLengthHLS * trainingDataHLS[row][column][channel].sumSquares 
                              - (long)trainingDataHLS[row][column][channel].sum * trainingDataHLS[row][column][channel].sum) 
                              / (float)((long)trainingHistoryLengthHLS * trainingHistoryLengthHLS));
            }
        }
    }
}

void DetectObject::trainGray(cv::Mat image)
{
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, CV_BGR2GRAY);
    cv::equalizeHist(imageGray,imageGray);
    
    this->trainingHistoryLengthGray++;
    
    for(int row=0; row<ROWS; row++)
    {
        for(int column=0; column<COLUMNS; column++)
        {
            float cellData = this->cellFunctionGray(row, column, &imageGray);
            
            if(trainingHistoryLengthGray == 0)
            {
                this->trainingDataGray[row][column].sum = cellData;
                this->trainingDataGray[row][column].sumSquares = (int)cellData*cellData;
            }
            else
            {
                this->trainingDataGray[row][column].sum += cellData;
                this->trainingDataGray[row][column].sumSquares += (int)cellData*cellData;
            }
            
            this->trainingDataGray[row][column].mean = this->trainingDataGray[row][column].sum / trainingHistoryLengthGray;
            this->trainingDataGray[row][column].standardDeviation = 
                    sqrt((trainingHistoryLengthGray * trainingDataGray[row][column].sumSquares 
                          - (long)trainingDataGray[row][column].sum * trainingDataGray[row][column].sum) 
                          / (float)((long)trainingHistoryLengthGray * trainingHistoryLengthGray));
        }
    }
}

void DetectObject::resetTrainingHLS()
{
    this->trainingHistoryLengthHLS = 0;
}

void DetectObject::resetTrainingGray()
{
    this->trainingHistoryLengthGray = 0;
}

bool DetectObject::checkObjectHLS(cv::Mat image)
{
    cv::Mat imageHLS;
    cv::cvtColor(image, imageHLS, CV_BGR2HLS);
    
    this->updateImageResultsHLS(&imageHLS, &image);
    
    return this->checkObjectSize();
}

bool DetectObject::checkObjectGray(cv::Mat image)
{
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, CV_BGR2GRAY);
    cv::equalizeHist(imageGray, imageGray);
    
    this->updateImageResultsGray(&imageGray);
    
    return this->checkObjectSize();
}

bool DetectObject::checkObjectSize()
{
    int objectWidth = 0;
    int objectHeight = 0;
    int currentWidth;
    int currentHeight;
    int currentMaxHeight;
    
    for(int row=0; row<ROWS; row++)
    {
        currentWidth = 0;
        currentMaxHeight = 0;
        
        for(int column=0; column<COLUMNS; column++)
        {
            if(this->imageResults[row][column])
            {
                currentWidth++;
                currentHeight = 1 + this->sizeRecurseUp(row, column) + this->sizeRecurseDown(row, column);
                
                currentMaxHeight = (currentHeight > currentMaxHeight ? currentHeight : currentMaxHeight);
                
                if(currentWidth * currentMaxHeight > objectWidth * objectHeight)
                {
                    objectWidth = currentWidth;
                    objectHeight = currentMaxHeight;
                }
            }
            else
            {
                currentWidth = 0;
                currentMaxHeight = 0;
            }
        }
    }
    
    this->imageResultsObjectWidth = objectWidth;
    this->imageResultsObjectHeight = objectHeight;
    
    return (objectWidth >= MIN_OBJECT_WIDTH && objectHeight >= MIN_OBJECT_HEIGHT);
}

cv::Mat DetectObject::generateDebugImage(cv::Mat inputImage)
{
    cv::Mat debugImage(inputImage.size(), inputImage.type());
    cv::Mat change(cv::Size(CELL_SIZE, CELL_SIZE), inputImage.type());
    cv::Mat noChange(cv::Size(CELL_SIZE, CELL_SIZE), inputImage.type());
    change = cv::Scalar(0.0f, 0.0f, 80.0f);   // red
    noChange = cv::Scalar(0.0f, 80.0f, 0.0f); // green
    
    for(int row=0; row<ROWS; row++)
    {
        for(int column=0; column<COLUMNS; column++)
        {
            cv::Rect cellRect(column*CELL_SIZE, row*CELL_SIZE, CELL_SIZE, CELL_SIZE);
            cv::Mat cellInputImage(inputImage, cellRect);
            cv::Mat cellDebugImage(debugImage, cellRect);
            
            if(this->imageResults[row][column])
            {
                cv::add(cellInputImage, change, cellDebugImage);
            }
            else
            {
                cv::add(cellInputImage, noChange, cellDebugImage);
            }
        }
    }
    
    return debugImage;
}

void DetectObject::updateImageResultsHLS(cv::Mat* imageHLS, cv::Mat* imageBGR)
{
    for(int row=0; row<ROWS; row++)
    {
        for(int column=0; column<COLUMNS; column++)
        {
            cv::Scalar cellData = this->cellFunctionHLS(row, column, imageHLS, imageBGR);
            
            int diffFromMean = abs((int)cellData[HUE] - this->trainingDataHLS[row][column][HUE].mean);
            if(diffFromMean > 128)
            {
                diffFromMean = 256 - diffFromMean;
            }
            bool hue = (diffFromMean > CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_HUE
                        * this->trainingDataHLS[row][column][HUE].standardDeviation);
            
            diffFromMean = abs((int)cellData[LIGHTNESS] - this->trainingDataHLS[row][column][LIGHTNESS].mean);
            bool lightness = (diffFromMean > CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_LIGHTNESS
                        * this->trainingDataHLS[row][column][LIGHTNESS].standardDeviation);
            
            diffFromMean = abs((int)cellData[SATURATION] - this->trainingDataHLS[row][column][SATURATION].mean);
            bool saturation = (diffFromMean > CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_SATURATION
                        * this->trainingDataHLS[row][column][SATURATION].standardDeviation);
            
            this->imageResults[row][column] = (hue || (saturation && lightness));
        }
    }
}

void DetectObject::updateImageResultsGray(cv::Mat* imageGray)
{
    for(int row=0; row<ROWS; row++)
    {
        for(int column=0; column<COLUMNS; column++)
        {
            float cellData = this->cellFunctionGray(row, column, imageGray);
            
            int diffFromMean = abs((int)cellData - this->trainingDataGray[row][column].mean);
            bool gray = (diffFromMean > CONFIDENCE_LEVEL_STANDARD_DEVIATIONS_GRAY
                        * this->trainingDataGray[row][column].standardDeviation);
            
            this->imageResults[row][column] = gray;
        }
    }
}

cv::Scalar DetectObject::cellFunctionHLS(int row, int column, cv::Mat* imageHLS, cv::Mat* imageBGR)
{
    // Get just the grid cell from image
    cv::Rect cellRect(column*CELL_SIZE, row*CELL_SIZE, CELL_SIZE, CELL_SIZE);
    cv::Mat cellImageHLS(*imageHLS, cellRect);
    cv::Mat cellImageBGR(*imageBGR, cellRect);
    
    // Average each channel for the cell
    cv::Scalar avg = cv::mean(cellImageHLS);
    cv::Scalar avgBGR = cv::mean(cellImageBGR);
    
    // Find hue average from RBG image
    cv::Mat pixel(1,1, imageBGR->type(), avgBGR);
    cv::cvtColor(pixel, pixel, CV_BGR2HLS);
    avg[HUE] = pixel.data[0];
    
    return avg;
}

float DetectObject::cellFunctionGray(int row, int column, cv::Mat* imageGray)
{
    // Get just the grid cell from image
    cv::Rect cellRect(column*CELL_SIZE, row*CELL_SIZE, CELL_SIZE, CELL_SIZE);
    cv::Mat cellImageGray(*imageGray, cellRect);
    
    // Average each channel for the cell
    float avg = cv::mean(cellImageGray)[0];
    
    return avg;
}

int DetectObject::sizeRecurseUp(int row, int column)
{
    if(row > 0)
    {
        if(this->imageResults[row-1][column])
        {
            return 1 + this->sizeRecurseUp(row-1, column);
        }
    }
    
    return 0;
}

int DetectObject::sizeRecurseDown(int row, int column)
{
    if(row < ROWS-1)
    {
        if(this->imageResults[row+1][column])
        {
            return 1 + this->sizeRecurseDown(row+1, column);
        }
    }
    
    return 0;
}

