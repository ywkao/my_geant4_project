ROOTLIBS = $(shell root-config --cflags --libs) -lMinuit -lMLP -lXMLIO -lTMVA -lGenVector

CC = g++
CFLAGS = -g -std=c++1y

DIR = ./bin
TARGET = $(DIR)/examine

all: $(TARGET)

$(info $(shell mkdir -p $(DIR)))

$(DIR)/examine: examine.cpp Hits.C Hits.h
	$(CC) $(CFLAGS) -I${ROOTSYS}/include -o $(TARGET) examine.cpp $(ROOTLIBS)
