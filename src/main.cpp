// main.cpp

#if defined(__linux__) && defined(__arm__)
#define RASPI
#endif

// Python.h must be included before anything else in our program
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ConfigManager.h"
#include "Camera.h"
#include "DetectObject.h"
#include "EmailWrapper.h"

#ifdef RASPI
#include "magneto.h"
#include "sonar.h"
#include "pir.h"
#endif

void* cameraThreadMain(void* arg);

int main(int argc, char** argv)
{
    // Load config and parse config changes from command line args
    ConfigManager::init();
    char argCmd = ' ';
    for(int i=1; i<argc; i++)
    {
        std::string currentArg(argv[i]);
        
        if(currentArg[0] == '-' && currentArg.length() >= 2)
        {
            argCmd = currentArg[1];
        }
        else
        {
            switch(argCmd)
            {
            case 'e':
                ConfigManager::emailRecipient = currentArg;
                break;
            case 'u':
                ConfigManager::gmailUsername = currentArg;
                break;
            case 'p':
                ConfigManager::gmailPassword = currentArg;
                break;
            default:
                break;
            }
        }
    }
    ConfigManager::save();
    
    // Start camera thread
    pthread_t* cameraThread = new pthread_t;
    pthread_attr_t* cameraThreadAttr = new pthread_attr_t;
    pthread_attr_init(cameraThreadAttr);
    pthread_create(cameraThread, cameraThreadAttr, cameraThreadMain, NULL);
    
#ifdef RASPI
    initMagneto();
    initSonar(); 
    initPIR();
#endif

    while(1)
    {
#ifdef RASPI
        readSonar();
        delay(100);
#endif
    }

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

