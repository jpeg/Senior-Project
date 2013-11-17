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
            case 'c':
                ConfigManager::initFrames = std::stoi(currentArg);
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
    sleep(1);
    
    Camera* cam = new Camera;
    int imageCounter = 1;
    
#ifdef RASPI
    bool motion = false;
    bool approach = false;
    bool magnetified = false;
    if(wiringPiSetup()){}
    system("clear");
    initMagneto();
    initSonar(); 
    initPIR();
    printf("Initialization Complete...........\n\n\n\n");
#endif

    // Hack to get around weird OpenCV HighGUI error
    if(getenv("DISPLAY") != NULL)
    {
        cv::namedWindow("Hack", CV_WINDOW_AUTOSIZE);
    }

    
    while(1)
    {
        bool detected = true;
        
#ifdef RASPI
        magnetified = fieldDisruptionDetected();
        approach = approachDetected();
        motion = motionDetected();
        detected = motion & approach & magnetified;
        printf("\x1B[Kmagnetified: %d     approach: %d     motion: %d     detected: %d\n", magnetified, approach, motion, detected ); 
        delay(50);
#endif
        
        if(detected)
        {
            printf("Sensors detected vehicle!\n");
            
            pthread_mutex_lock(cameraMutex);
            cam->init();
            for(int i=0; i<ConfigManager::initFrames; i++)
            {
                // Need several frames for camera to init
                cam->captureFrame();
            }
            cam->captureFrame();
            cv::Mat image = cam->getLastFrame();
            bool cameraDetected = detectObject->checkObjectGray(image);
            cam->shutdown();
            pthread_mutex_unlock(cameraMutex);
            
            if(cameraDetected)
            {
                printf("Camera detected vehicle!\n");
                
                // Make sure vasc_images directory exists
                DIR* dir = opendir(ConfigManager::savePath.c_str());
                if(!dir)
                {
                    mkdir(ConfigManager::savePath.c_str(), S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH);
                }
                closedir(dir);
                
                // Save image to disk
                std::stringstream filepath;
                filepath << ConfigManager::savePath << imageCounter << ".jpg";
                imageCounter = (imageCounter < ConfigManager::savedImages ? imageCounter+1 : 1);
                std::vector<int> qualityType;
                qualityType.push_back(CV_IMWRITE_JPEG_QUALITY);
                qualityType.push_back(90);
                if(cv::imwrite(filepath.str(), image, qualityType))
                {
                    printf("Saved image %s\n", filepath.str().c_str());
                    printf("Sending email alert...\n");
                }
                else
                {
                    printf("Failed to save image.\n");
                }
                
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
            printf("Resuming detection in 10 seconds...\n\n\n\n");
            sleep(10);
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
        cam->init();
        for(int i=0; i<ConfigManager::initFrames; i++)
        {
            // Need several frames for camera to init
            cam->captureFrame();
        }
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

