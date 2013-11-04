// main.cpp

// Python.h must be included before anything else in our program
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ConfigManager.h"
#include "Camera.h"
#include "DetectObject.h"
#include "EmailWrapper.h"

void* cameraThreadMain(void* arg);

int main(int argc, char** argv)
{
    ConfigManager::init();
    
    // Start camera thread
    pthread_t* cameraThread = new pthread_t;
    pthread_attr_t* cameraThreadAttr = new pthread_attr_t;
    pthread_attr_init(cameraThreadAttr);
    pthread_create(cameraThread, cameraThreadAttr, cameraThreadMain, NULL);
    
    ConfigManager::save();
    
    return 0;
}

void* cameraThreadMain(void* arg)
{
    Camera* raspiCam = new Camera();
    raspiCam->init(DetectObject::IMAGE_WIDTH, DetectObject::IMAGE_HEIGHT);
    
    DetectObject* detectObject = new DetectObject();
    detectObject->init();
    
    // Train object detection
    detectObject->resetTrainingGray();
    const int trainingFrames = 50;
    for(int i=0; i<trainingFrames; i++)
    {
        raspiCam->captureFrame();
        detectObject->trainGray(raspiCam->getLastFrame());
    }
    
    return NULL;
}

