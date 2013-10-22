CC=g++
CFLAGS=-c -Wall -std=c++0x
INCLUDES=-I include/
LDFLAGS=-lopencv_core -lopencv_highgui -lopencv_imgproc
UNAME_P:=$(shell uname -p)
ifneq ($(filter unknown,$(UNAME_P)),)
	RASPIFLAGS=-L/usr/lib/uv4l/uv4lext/armv6l -luv4lext -Wl,-rpath,'/usr/lib/uv4l/uv4lext/armv6l'
else
	RASPIFLAGS=
endif
SOURCE_DIR=src
SOURCES=main.cpp Camera.cpp DetectObject.cpp
OBJECT_DIR=build
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE_DIR=bin
EXECUTABLE=VASC

all: $(OBJECTS) $(EXECUTABLE)

execute: all
	@./$(EXECUTABLE_DIR)/$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(EXECUTABLE_DIR)
	$(CC) $(addprefix $(OBJECT_DIR)/,$(OBJECTS)) $(INCLUDES) $(LDFLAGS) $(RASPIFLAGS) -o $(EXECUTABLE_DIR)/$@

main.o: $(SOURCE_DIR)/main.cpp $(OBJECT_DIR)
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $(OBJECT_DIR)/$@

Camera.o: $(SOURCE_DIR)/Camera.cpp $(OBJECT_DIR)
	$(CC) $(CFLAGS) $< $(INCLUDES) -o $(OBJECT_DIR)/$@

DetectObject.o: $(SOURCE_DIR)/DetectObject.cpp $(OBJECT_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $(OBJECT_DIR)/$@

$(EXECUTABLE_DIR):
	@mkdir $(EXECUTABLE_DIR)

$(OBJECT_DIR):
	@mkdir $(OBJECT_DIR)

.PHONY: clean

clean:
	@rm -rf $(EXECUTABLE_DIR)/$(EXECUTABLE) $(addprefix $(OBJECT_DIR)/,$(OBJECTS))
