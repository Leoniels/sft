# Generic makefile
# Author: Leonardo Niels Pardi

###	Name of the final executable
name = facedetection 

###	Directories
IDIR = /usr/include/opencv4 
ODIR = obj
SDIR = src
BDIR = bin

###	Compiler settings
CC = gcc
CXX = g++
CXXFLAGS = -I$(IDIR) #-O3
LDFLAGS = -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_objdetect

###	Sources
csrc = $(wildcard $(SDIR)/*.c)
cppsrc = $(wildcard $(SDIR)/*.cpp)
###	Objects
cobj = $(csrc:$(SDIR)/%.c=$(ODIR)/%.o)
cppobj = $(cppsrc:$(SDIR)/%.cpp=$(ODIR)/%.o)
allobj = $(cobj) $(cppobj)

###	Rules
$(BDIR)/$(name): $(allobj)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(cobj): $(csrc)
	$(CC) -c -o $@ $^ $(CXXFLAGS)

$(cppobj): $(cppsrc)
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

###	Remove
.PHONY: clean
clean:
	rm -f $(allobj) $(BDIR)/$(name)
