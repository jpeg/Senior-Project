// main.cpp

// Python.h must be included before anything else in our program
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>

#include "Camera.h"
#include "DetectObject.h"
#include "EmailWrapper.h"

int main(int argc, char** argv)
{
    if(EmailWrapper::sendEmail(TO_ADDRESS, "Test Email", "This is a test message.", GMAIL_FROM_ADDRESS, GMAIL_PASSWORD, "README.md"))
    {
        printf("Sent email.\n");
    }
    else
    {
        printf("Failed to send email.\n");
    }
    
    return 0;
}

