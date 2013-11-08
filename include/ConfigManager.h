// ConfigManager.h

#pragma once

#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <fstream>

static class ConfigManager
{
private:
    static char* homeDir;
    static std::string configDir;
    static std::string configFile;
    static std::string configPath;
    
public:
    ConfigManager() {}
    ~ConfigManager() {}
    
    static void init();
    static void save();
    
public:
    static std::string emailRecipient;
    static std::string gmailUsername;
    static std::string gmailPassword;
    static int cameraTrainingDelay;
} configManager_staticInstance;

