# Allterrain makefile for compile a C++ project
# Copyright (C) 2019 Leonardo Niels Pardi
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
