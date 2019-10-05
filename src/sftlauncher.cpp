/*
 * Simple facetracker launcher. CLI for the facetracker.
 * Copyright (C) 2019 Leonardo Niels Pardi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <opencv2/core.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Parse command line arguments and init program
int readArguments(int argc, char** argv);

const String keys =
	"{help h|| Print help message}"
	"{camera c|0| Camera device number as video stream input}"
	"{video v|| Video file as video stream input}"
	"{@face_cascade f|/usr/share/opencv4/haarcascades/haarcascade_frontalface"
	"_default.xml|.xml file}"
	"{eyes_cascade e|| track eyes using cascade file specified}"
	"{nose_cascade n|| track eyes using cascade file specified}"
	"{out o|| Write a video file -o <name>.avi}"
	"{gray g|| Output video in grayscale}"
	"{nose_location nl|| Writes to stdout the nose location if detected, "
	"otherwise \'-1, -1\'}"
	"{eyes_location el|| Writes to stdout the eyes location if detected, "
	"otherwise \'-1, -1\'}"
	"{proc_time p|| Write the processing time of detection in a file "
	"(camproctime or <vid>proctime)}";
const String about =
	"Simple facetracker v1.3\n"
	"This program is a facetracker concept implemented using haarcascades";

String faceCascade, eyesCascade, noseCascade;	// haarclassifier
String videoInput, cameraInput, videoOutput;	// video source and output
bool gray = false;				// Output gray video instead of coloured
bool noseLocation = false, eyesLocation = false;	// output location
bool procTime = false;	// Write processing time

int main(int argc, char** argv){

	// Parse command line arguments and init global variables 
	if (readArguments(argc, argv) != 0)
		return 1;

	// Init facetracker
	SimpleFacetracker sft = new SimpleFacetracker(2.0);
	sft.initFaceCascade(faceCascade);
	if (!eyesCascade.empty())
		sft.initEyesCascade(eyesCascade);
	if (!noseCascade.empty())
		sft.initNoseCascade(noseCascade);
	if (!videoInput.empty()){
		const String name = videoInput;
		sft.videoSource(name);
	}else 
		sft.cameraSource(stoi(cameraInput));
	if (!videoOutput.empty()){
		const String name = videoOutput;
		sft.writeVideo(name);
	}
	if (gray)
		sft.outputGray();
	if (procTime)
		sft.writeProcessingTime();

	float x, y;
	while(!sft.finished){
		sft.track();
		sft.faceLocation(x, y);
		cout << "f: (" << x << ", " << y << ")" << endl;
	}

	sft.release();

	return 0;
}

// Parse command line arguments and initialize program
int readArguments(int argc, char** argv){
	CommandLineParser parser(argc, argv, keys);
	parser.about(about);

	// Check arguments
	if (!parser.check()){
		parser.printErrors();
		parser.printMessage();
		return 1;
	}
	if (parser.has("help")){
		parser.printMessage();
		return 0;
	}

	// Cascade files
	faceCascade = parser.get<String>(0);
	eyesCascade = parser.get<String>("eyes_cascade");
	noseCascade = parser.get<String>("nose_cascade");

	// Video stream
	cameraInput = parser.get<String>("camera");
	videoInput = parser.get<String>("video");
	videoOutput = parser.get<String>("out");

	// FLAGS
	gray = parser.has("gray");
	noseLocation = parser.has("nose_location");
	eyesLocation = parser.has("eyes_location");
	procTime = parser.has("proc_time");

	return 0;
}

