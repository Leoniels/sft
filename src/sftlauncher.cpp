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

#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include "SimpleFacetracker.hpp"

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
bool noseLocation = false, eyesLocation = false;// output location
bool procTime = false;							// Write processing time

int main(int argc, char** argv){
	// Parse command line arguments and init global variables 
	if (readArguments(argc, argv))
		return 1;

	// Init facetracker
	SimpleFacetracker sft;
	sft.initFaceCascade(faceCascade);
	// Init optional facetracker components
	if (!eyesCascade.empty())
		if (sft.initEyesCascade(eyesCascade))
			return 1;
	if (!noseCascade.empty())
		if (sft.initNoseCascade(noseCascade))
			return 2;
	if (!videoInput.empty()){
		const String name = videoInput;
		if (sft.videoSource(name))
			return 3;
	}else 
		if (sft.cameraSource(stoi(cameraInput)))
			return 4;
	if (!videoOutput.empty()){
		const String name = videoOutput;
		if (sft.videoOutput(name))
			return 5;
	}
	if (gray)
		sft.outputGray();
	if (procTime)
		sft.writeProcessingTime();

	// Main loop
	float x1, y1, x2, y2, x12, y12, x22, y22;
	sft.track();
	while(!sft.finished()){
		sft.drawLocations();

		sft.faceLocation(x1, y1, x2, y2);
		cout << " f: (" << format("(%1.3f,", x1) << format(" %1.3f), ", y1) <<
			format("(%1.3f,", x2) << format(" %1.3f)", y2) << ")";
		sft.noseLocation(x1, y1, x2, y2);

		if (noseLocation)
			cout << " | n: (" << format("(%1.3f,", x1) << format(" %1.3f), ", y1) <<
				format("(%1.3f,", x2) << format(" %1.3f)", y2) << ")";
		cout << endl;

		if (eyesLocation){
			sft.eyesLocation(x1, y1, x2, y2, x12, y12, x22, y22);
			cout << "e1: (" << format("(%1.3f,", x1) << format(" %1.3f), ", y1) <<
				format("(%1.3f,", x2) << format(" %1.3f)", y2) << ")";
			cout << " | e2: (" << format("(%1.3f,", x12) << format(" %1.3f), ", y12) <<
				format("(%1.3f,", x22) << format(" %1.3f)", y22) << ")" << endl;
		}
		sft.track();
	}

	// Release resources
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
		return 1;
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

