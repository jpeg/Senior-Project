Senior-Project
==============

Main repository for senior project code.

### Command line usage:
`./vasc [OPTIONS]`  
#### Optional arguments:
`-e` - Email address to send alerts to.  
`-u` - Username for Gmail account to send alerts from.  
`-p` - Password for Gmail account to send alerts from.  
`-t` - How often to retrain the camera detection in minutes.
`-i` - Max number of images to save to disk.
`-f` - Directory to save images.

### Required libraries to build:  
* libopencv-dev  
* python-dev
* wiringPi (Raspberry Pi builds only, http://wiringpi.com/download-and-install/)
