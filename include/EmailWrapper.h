// EmailWrapper.h

#ifndef EMAIL_WRAPPER_H
#define EMAIL_WRAPPER_H

#include <Python.h>
#include <string>

namespace EmailWrapper
{
    bool sendEmail(std::string recipient, std::string subject, std::string message, std::string username, std::string password, std::string attachmentPath = "");
};

#endif //EMAIL_WRAPPER_H

