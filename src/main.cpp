// main.cpp

// Python.h must be included before anything else in our program
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>

#include "ConfigManager.h"
#include "Camera.h"
#include "DetectObject.h"
#include "EmailWrapper.h"

int main(int argc, char** argv)
{
    ConfigManager::init();
    
    if(EmailWrapper::sendEmail(ConfigManager::emailRecipient, "Test Email", "This is a test message.", ConfigManager::gmailUsername, ConfigManager::gmailPassword, "README.md"))
    {
        printf("Sent email.\n");
    }
    else
    {
        printf("Failed to send email.\n");
    }
    
    ConfigManager::save();
    
    return 0;
}

