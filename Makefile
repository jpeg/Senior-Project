CC=g++
CFLAGS=-c -Wall -std=c++0x
INCLUDES=-Iinclude/ -I/usr/include/python2.7
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_imgproc -lpython2.7 #-lwiringPi
UNAME_P:=$(shell uname -p)
ifneq ($(filter unknown,$(UNAME_P)),)
	RASPIFLAGS=#-L/usr/lib/uv4l/uv4lext/armv6l -luv4lext -Wl,-rpath,'/usr/lib/uv4l/uv4lext/armv6l'
else
	RASPIFLAGS=
endif
SOURCE_DIR=src
SOURCES=main.cpp ConfigManager.cpp Camera.cpp DetectObject.cpp EmailWrapper.cpp #magneto.c
PYTHON_SOURCES=SendEmail.py
OBJECT_DIR=build
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
EXECUTABLE_DIR=bin
EXECUTABLE=vasc

all: $(OBJECTS) $(EXECUTABLE)

execute: all
	@./$(EXECUTABLE_DIR)/$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(EXECUTABLE_DIR)
	$(CC) $(addprefix $(OBJECT_DIR)/,$(OBJECTS)) $(INCLUDES) $(LDFLAGS) $(RASPIFLAGS) -o $(EXECUTABLE_DIR)/$@
	@cp $(addprefix $(SOURCE_DIR)/,$(PYTHON_SOURCES)) $(EXECUTABLE_DIR)

main.o: $(SOURCE_DIR)/main.cpp $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

ConfigManager.o: $(SOURCE_DIR)/ConfigManager.cpp $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

Camera.o: $(SOURCE_DIR)/Camera.cpp $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

DetectObject.o: $(SOURCE_DIR)/DetectObject.cpp $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

EmailWrapper.o: $(SOURCE_DIR)/EmailWrapper.cpp $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

magneto.o: $(SOURCE_DIR)/magneto.c $(OBJECT_DIR)
	$(CC) $< $(CFLAGS) $(INCLUDES) -o $(OBJECT_DIR)/$@

$(EXECUTABLE_DIR):
	@mkdir $(EXECUTABLE_DIR)

$(OBJECT_DIR):
	@mkdir $(OBJECT_DIR)

.PHONY: clean

clean:
	@rm -rf $(EXECUTABLE_DIR) $(OBJECT_DIR)
