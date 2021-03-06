#include "ConfigManager.h"

char* ConfigManager::homeDir = new char('\0');
std::string ConfigManager::configDir = ".vasc";
std::string ConfigManager::configFile = "vasc.cfg";
std::string ConfigManager::configPath = "";

// Default Values
std::string ConfigManager::emailRecipient = "";
std::string ConfigManager::gmailUsername = "";
std::string ConfigManager::gmailPassword = "";
int ConfigManager::cameraTrainingDelay = 30;
int ConfigManager::savedImages = 100;
std::string ConfigManager::savePath = ""; //default set in init
int ConfigManager::initFrames = 0;

void ConfigManager::init()
{
    delete ConfigManager::homeDir;
    ConfigManager::homeDir = getenv("HOME");
    if(ConfigManager::homeDir == NULL)
    {
        ConfigManager::homeDir = new char('\0');
    }
    
    // Default set here instead as it requires getenv call
    ConfigManager::savePath = getenv("HOME");
    ConfigManager::savePath.append("/vasc_images/");
    
    // Assemble config filepath
    char pathSeperator = '/';
    std::string filepath(ConfigManager::homeDir);
    filepath.push_back(pathSeperator);
    filepath.append(ConfigManager::configDir);
    filepath.push_back(pathSeperator);
    ConfigManager::configPath = filepath;
    
    // Load from config file
    std::fstream cfgFile;
    cfgFile.open(ConfigManager::configPath + ConfigManager::configFile, std::ios::in);
    while(cfgFile.good() && !cfgFile.eof())
    {
        std::string line;
        std::getline(cfgFile, line);
        
        // Remove comment from line
        size_t pos = line.find('#', 0);
        if(pos != std::string::npos)
        {
            line = line.substr(0, pos);
        }
        
        pos = line.find(':', 0);
        if(pos != std::string::npos)
        {
            std::string var = line.substr(0, pos);
            std::string value = line.substr(pos+1, line.length()-1);
            
            if(var == "emailRecipient")
            {
                ConfigManager::emailRecipient = value;
            }
            else if(var == "gmailUsername")
            {
                ConfigManager::gmailUsername = value;
            }
            else if(var == "gmailPassword")
            {
                ConfigManager::gmailPassword = value;
            }
            else if(var == "cameraTrainingDelay")
            {
                ConfigManager::cameraTrainingDelay = std::stoi(value);
            }
            else if(var == "savedImages")
            {
                ConfigManager::savedImages = std::stoi(value);
            }
            else if(var == "savePath")
            {
                ConfigManager::savePath = value;
            }
            else if(var == "initFrames")
            {
                ConfigManager::initFrames = std::stoi(value);
            }
        }
    }
    cfgFile.close();
}

void ConfigManager::save()
{
    // Make sure .vasc directory exists
    DIR* dir = opendir(ConfigManager::configPath.c_str());
    if(!dir)
    {
        mkdir(ConfigManager::configPath.c_str(), S_IRWXU);
    }
    closedir(dir);
    
    // Save to config file
    std::fstream cfgFile;
    cfgFile.open(ConfigManager::configPath + ConfigManager::configFile, std::ios::out | std::ios::trunc);
    if(cfgFile.good())
    {
        cfgFile << "emailRecipient:" << ConfigManager::emailRecipient << '\n';
        cfgFile << "gmailUsername:" << ConfigManager::gmailUsername << '\n';
        cfgFile << "gmailPassword:" << ConfigManager::gmailPassword << '\n';
        cfgFile << "cameraTrainingDelay:" << ConfigManager::cameraTrainingDelay << '\n';
        cfgFile << "savedImages:" << ConfigManager::savedImages << '\n';
        cfgFile << "savePath:" << ConfigManager::savePath << '\n';
        cfgFile << "initFrames:" << ConfigManager::initFrames << '\n';
    }
    cfgFile.close();
}

