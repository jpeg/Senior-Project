// EmailWrapper.cpp

#include "EmailWrapper.h"

namespace EmailWrapper
{
    std::string getScript();
    
    bool sendEmail(std::string recipient, std::string subject, std::string message, std::string username, std::string password, std::string attachmentPath)
    {
        bool sent = true;
        
        Py_Initialize();
        
        PyObject *pName, *pModule, *pFunc, *pRecipient, *pSubject, *pMessage, *pUsername, *pPassword, *pAttachmentPath, *pArgs;
        
        try
        {
            PyRun_SimpleString("import sys");
            PyRun_SimpleString("sys.path.append('.')");
            PyRun_SimpleString("sys.path.append('./bin')");
            
            pName = PyString_FromString("SendEmail");
            pModule = PyImport_Import(pName);
            
            pFunc = PyObject_GetAttrString(pModule, "sendEmail");
            pRecipient = PyString_FromString(recipient.c_str());
            pSubject = PyString_FromString(subject.c_str());
            pMessage = PyString_FromString(message.c_str());
            pUsername = PyString_FromString(username.c_str());
            pPassword = PyString_FromString(password.c_str());
            pAttachmentPath = (attachmentPath == "" ? Py_None : PyString_FromString(attachmentPath.c_str()));
            
            pArgs = PyTuple_New(6);
            PyTuple_SetItem(pArgs, 0, pRecipient);
            PyTuple_SetItem(pArgs, 1, pSubject);
            PyTuple_SetItem(pArgs, 2, pMessage);
            PyTuple_SetItem(pArgs, 3, pUsername);
            PyTuple_SetItem(pArgs, 4, pPassword);
            PyTuple_SetItem(pArgs, 5, pAttachmentPath);
            
            if(pFunc && PyCallable_Check(pFunc))
            {
                PyObject* returnValue = PyObject_CallObject(pFunc, pArgs);
                
                if(PyObject_Not(returnValue))
                {
                    sent = false;
                }
                
                Py_DECREF(returnValue);
            }
            else
            {
                sent = false;
            }
        }
        catch(...)
        {
            sent = false;
        }
        
        if(pArgs) Py_DECREF(pArgs);
        if(pFunc) Py_DECREF(pFunc);
        if(pModule) Py_DECREF(pModule);
        if(pName) Py_DECREF(pName);
        
        Py_Finalize();
        
        return sent;
    }
}

