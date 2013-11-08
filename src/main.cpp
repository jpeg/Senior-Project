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
pthread_mutex_t* cameraMutex;

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
            case 't':
                ConfigManager::cameraTrainingDelay = std::stoi(currentArg);
            case 'i':
                ConfigManager::savedImages = std::stoi(currentArg);
            case 'f':
                ConfigManager::savePath = currentArg;
            default:
                break;
            }
        }
    }
    ConfigManager::save();
    
    // Init camera stuff
    DetectObject* detectObject = new DetectObject();
    detectObject->init();
    
    // Create camera mutex
    pthread_mutexattr_t* cameraMutexAttr = new pthread_mutexattr_t;
    pthread_mutexattr_init(cameraMutexAttr);
    cameraMutex = new pthread_mutex_t;
    pthread_mutex_init(cameraMutex, cameraMutexAttr);
    
    // Start camera thread
    pthread_t* cameraThread = new pthread_t;
    pthread_create(cameraThread, NULL, cameraThreadMain, detectObject);
    
    Camera* cam = new Camera;
    int imageCounter = 1;
    
#ifdef RASPI
    initMagneto();
    initSonar(); 
    initPIR();
#endif

    while(1)
    {
        bool detected = false;
#ifdef RASPI
        readSonar();
        delay(100);
#endif
        
        if(detected)
        {
            printf("Sensors detected vehicle!\n");
            
            pthread_mutex_lock(cameraMutex);
            cam->init(DetectObject::IMAGE_WIDTH, DetectObject::IMAGE_HEIGHT);
            cam->captureFrame();
            cv::Mat image = cam->getLastFrame();
            bool cameraDetected = detectObject->checkObjectGray(image);
            cam->shutdown();
            pthread_mutex_unlock(cameraMutex);
            
            if(cameraDetected)
            {
                printf("Camera detected vehicle!\n");
                
                // Save image to disk
                std::stringstream filepath;
                filepath << ConfigManager::savePath << imageCounter << ".jpg";
                imageCounter = (imageCounter < ConfigManager::savedImages ? imageCounter+1 : 1);
                cv::imwrite(filepath.str(), image);
                
                // Send email alert
                if(EmailWrapper::sendEmail(ConfigManager::emailRecipient, "VASC Email Alert!", "A vehicle was detected! See attached image.", 
                             ConfigManager::gmailUsername, ConfigManager::gmailPassword, filepath.str()))
                {
                    printf("Sent email alert!\n");
                }
                else
                {
                    printf("ERROR: Failed to send email alert.\n");
                }
            }
            else
            {
                printf("Camera did not detect a vehicle.\n");
            }
        }
    }
    
    delete detectObject;
    
    return 0;
}

void* cameraThreadMain(void* arg)
{
    if(arg == NULL)
    {
        return NULL;
    }
    
    Camera* cam = new Camera();
    DetectObject* detectObject = (DetectObject*)arg;
    
    while(1)
    {
        // Train object detection
        pthread_mutex_lock(cameraMutex);
        detectObject->resetTrainingGray();
        cam->init(DetectObject::IMAGE_WIDTH, DetectObject::IMAGE_HEIGHT);
        const int trainingFrames = 50;
        for(int i=0; i<trainingFrames; i++)
        {
            cam->captureFrame();
            detectObject->trainGray(cam->getLastFrame());
        }
        cam->shutdown();
        pthread_mutex_unlock(cameraMutex);
        
        sleep(60 * ConfigManager::cameraTrainingDelay);
    }
    
    delete cam;
    
    return NULL;
}

