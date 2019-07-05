# All terran Makefile
# Author: Leonardo Niels Pardi

###	Name of the final executable
name = sft 

###	Directories
IDIR = /usr/include/opencv4 
ODIR = obj
SDIR = src
BDIR = bin

###	Compiler settings
CXXFLAGS = -I$(IDIR)
LDFLAGS = -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_objdetect

###	Sources
cppsrc = $(wildcard $(SDIR)/*.cpp)
###	Objects
cppobj = $(cppsrc:$(SDIR)/%.cpp=%.o)
###	ODIR + Obj
pcppobj = $(patsubst %,$(ODIR)/%,$(cppobj))

###	Rules
$(ODIR)/%.o: $(SDIR)/%.cpp 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BDIR)/$(name): $(pcppobj)
	$(CXX) -o $@ $^ $(LDFLAGS)

###	Remove
.PHONY: clean
clean:
	rm -f $(pcppobj) $(BDIR)/$(name)
