/*
 * Simple facetracker. Capture face features in realtime using haarcascades.
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

#include "SimpleFacetracker.hpp"

using namespace cv;
using namespace std;

SimpleFacetracker::SimpleFacetracker()
	:
		searchEyes(false), searchNose(false),
		writeVideo(false), gray(false),
		done(false);
{
	clahe = createCLAHE(2.0);
}
SimpleFacetracker::SimpleFacetracker(float claheClipLimit)
	:
		searchEyes(false), searchNose(false),
		writeVideo(false), gray(false),
		done(false)
{
	clahe = createCLAHE(claheClipLimit);
}
SimpleFacetracker::~SimpleFacetracker(){}

int SimpleFacetracker::initFaceCascade(String cascadeFile){
	if (!faceCascade.load(cascadeFile)){
		cerr << "ERROR::CASCADE::FAILURE_LOADING_FACE_CASCADE_FILE" << endl;
		return 1;
	}
	return 0;
}

int SimpleFacetracker::initEyesCascade(String cascadeFile){
	if (!eyesCascade.load(cascadeFile)){
		cerr << "ERROR::CASCADE::FAILURE_LOADING_EYES_CASCADE_FILE" << endl;
		return 1;
	}
	searchEyes = true;
	return 0;
}

int SimpleFacetracker::initNoseCascade(String cascadeFile){
	if (!noseCascade.load(cascadeFile)){
		cerr << "ERROR::CASCADE::FAILURE_LOADING_NOSE_CASCADE_FILE" << endl;
		return 1;
	}
	searchNose = true;
	return 0;
}

int SimpleFacetracker::videoSource(const String name){
	capture.open(name);
	if (!capture.isOpened()){
		cerr << "ERROR::VIDEO::FAILURE_OPENING_VIDEO_STREAM_INPUT" << endl;
		return 1;
	}
	return 0;
}

int SimpleFacetracker::cameraSource(int camera){
	capture.open(camera);
	if (!capture.isOpened()){
		cerr << "ERROR::VIDEO::FAILURE_OPENING_VIDEO_STREAM_INPUT" << endl;
		return 1;
	}
	return 0;
}

int SimpleFacetracker::videoOutput(const String name){
	// Check video input status
	if (!capture.isOpenend()){
		cerr << "ERROR:VIDEO::VIDEO_INPUT_NOT_OPENED" << endl;
		return 1;
	}

	name += ".avi";
	int codec = VideoWriter::fourcc('X', 'V', 'I', 'D'); // mp4 encoding

	// Get input video stream info
	int srcFrameWidth, srcFrameHeight, srcfps;
	srcFrameWidth = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
	srcFrameHeight = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
	srcfps = static_cast<int>(capture.get(CAP_PROP_FPS));
	outputVideo.open(name, codec, srcfps,
							Size(srcFrameWidth, srcFrameHeight), true);

	// Check video output status
	if (!outputVideo.isOpened()){
			cerr <<
			"ERROR:VIDEO::FAILURE_OPENING_VIDEO_STREAM_OUTPUT"
			<< endl;
			return 2;
	}

	return 0;
}

void SimpleFacetracker::outputGray(){
	gray = true;
}

void SimpleFacetracker::writeProcessingTime(){
	proctimefile.open("proctime", ios::trunc);
}

bool SimpleFacetracker::finished(){
	return done;
}

int SimpleFacetracker::track(){
	int64 processingTime = 0;	// Proccesing time used in detection
	// The video source has frames and the user doesn't hit ESC
	if (capture.read(frame) && !(waitKey(1) == 27/*ESC*/)){
		// Check frame integrity
		if (frame.empty()){
			cerr << "ERROR::FRAME::EMPTY_FRAME_READED" << endl;
			return 1;
		}

		nFrames++;

		// Measure time used in detecting faces and noses
		int64 tp0 = getTickCount();
		// Convert the frame to grayscale
		Mat grayFrame, frameROI;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		clahe->apply(grayFrame, grayFrame);

		// Set the output frame as gray too
		if (gray)
			cvtColor(grayFrame, frame, COLOR_GRAY2BGR);

		// Detect faces on the frame and draw their location 
		vector<Rect> faces, eyesv, noses;
		faceCascade.detectMultiScale(grayFrame, faces, 1.2,	2, 0, Size(100, 150),
									 Size(300, 450));
		face = faces.at(0);
		if (searchNose){
			// Use a 3/4 frame size as Region Of Interest to search the nose
			Rect ROINose = Rect(Point(face.x+face.width*0.2,
									  face.y+face.height*0.35),
	 						    Size(face.width*0.6, face.height*0.5));
			frameROI = grayFrame(ROINose);

			noseCascade.detectMultiScale(frameROI, noses, 1.1, 2, 0,
										 Size(20, 15), Size(90, 80));
			nose = noses.at(0);
			// Use the ROINose as reference for location and dimension
			Point p1(ROINose.x + noses[j].x, ROINose.y + noses[j].y);
			Point p2(ROINose.x + noses[j].x + noses[j].width,
					 ROINose.y + noses[j].y + noses[j].height);
			nose = Rect(p1, p2);
		}// if

		// Same as nose for eyes
		if (searchEyes){
			Rect ROIEyes = Rect(Point(face.x, faces.y),
								Size(face.width, face.height/1.7));
			frameROI = grayFrame(ROIEyes);

			eyesCascade.detectMultiScale(frameROI, eyesv, 1.075, 1, 0,
										 Size(25, 10), Size(70, 50));
			eyes[0] = eyesv.at(0);
			eyes[1] = eyesv.at(1);
			for (unsigned short i = 1; i < 2; i++){
				Point p1(ROIEyes.x + eyes[i].x, ROIEyes.y + eyes[i].y);
				Point p2(ROIEyes.x + eyes[i].x + eyes[i].width,
						 ROIEyes.y + eyes[i].y + eyes[i].height);
				eyes[i] = Rect(p1, p2);
			}// for
		}// if
		processingTime = getTickCount() - tp0;

		// Write proccessing time in file
		if (proctimefile.is_open())
			proctimefile <<
			format("%.4f",
					static_cast<double>(processingTime) * 1000.0f /
					getTickFrequency())
			<< endl;

		// Write frame on video output
		if (writeVideo)
			outputVideo << frame;
		// Show frame on window in realtime
		imshow("OpenCV Facedetection", frame);
	}else
		done = true;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

// Parse command line arguments and initialize program
int init(int argc, char** argv);
// Print execution statistics throught stdout
void printExecStatistics(size_t nFrames, int64 processingTime, int64 *t0);
// Search for faces and/or noses and draw their location
void trackAndDraw(Mat frame);

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
	"Easy facedetection v1\n"
	"This program is a facetracker concept implemented using haarcascades";

Ptr<CLAHE> clahe = createCLAHE(2.0);// Gray range equalizer
CascadeClassifier faceCascade;		// Face haarclassifier
bool searchEyes = false;			// Track eyes
CascadeClassifier eyesCascade;		// Eyes haarclassifier
bool searchNose = false;			// Track nose
CascadeClassifier noseCascade;		// Nose cascade
VideoCapture capture;				// Video where the face is tracked
bool writeVideo = false;			// Write output video
VideoWriter outputVideo;			// Video output stream
bool gray = false;					// Output gray video instead of coloured
bool noseLoc = false;				// Output nose location
bool eyesLoc = false;				// Output eyes location
ofstream proctimefile;				// File stream to store process time
									// invested in tracking
int main(int argc, char** argv){
	// Parse command line arguments and init global variables 
	if (init(argc, argv) != 0)
		return 1;

	Mat frame;	// Frame to read from video source and to draw in
	size_t nFrames = 0;	// Number of frames readed on every iteration.
	int64 t0 = cv::getTickCount();	// Ticks at the begining of execution
	int64 processingTime = 0;	// Proccesing time used in detection
	// While the video source has frames and the user doesn't hit ESC
	while (capture.read(frame) && !(waitKey(1) == 27/*ESC*/)){
		// Check frame integrity
		if (frame.empty()){
			cerr << "ERROR::FRAME::EMPTY_FRAME_READED" << endl;
			return 1;
		}

		// Print execution statistics if nose/eyes location is not required
		nFrames++;
		if (!(noseLoc || eyesLoc) && nFrames % 30 == 0)
			printExecStatistics(nFrames, processingTime, &t0);

		// Measure time used in detecting faces and noses
		int64 tp0 = getTickCount();
		trackAndDraw(frame);
		processingTime = getTickCount() - tp0;

		// Write proccessing time in file
		if (proctimefile.is_open())
			proctimefile <<
			format("%.4f",
					static_cast<double>(processingTime) * 1000.0f /
					getTickFrequency())
			<< endl;

		// Write frame on video output
		if (writeVideo)
			outputVideo << frame;
		// Show frame on window in realtime
		imshow("OpenCV Facedetection", frame);
	}
   
	// Release resources
	if (proctimefile.is_open())
		proctimefile.close();
	if (capture.isOpened())
		capture.release();
	if (outputVideo.isOpened())
		outputVideo.release();

	return nFrames > 0 ? 0 : 1;
}


// Parse command line arguments and initialize program
int init(int argc, char** argv){
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

	// Load cascade
	String face_cascade_name = parser.get<String>(0);
	if (!faceCascade.load(face_cascade_name)){
		cerr << "ERROR::CASCADE::FAILURE_LOADING_FACE_CASCADE_FILE" << endl;
		return 1;
	}
	
	// Check eyes cascade
	String eyes_cascade_name = parser.get<String>("eyes_cascade");
	if(!eyes_cascade_name.empty()){
		searchEyes = true;
		if (!eyesCascade.load(eyes_cascade_name)){
			cerr <<
			"ERROR::CASCADE::FAILURE_LOADING_EYES_CASCADE_FILE"
			<< endl;
		}
	}

	// Check nose cascade
	String nose_cascade_name = parser.get<String>("nose_cascade");
	if (!nose_cascade_name.empty()){
		searchNose = true;
		if (!noseCascade.load(nose_cascade_name)){
			cerr <<
			"ERROR::CASCADE::FAILURE_LOADING_NOSE_CASCADE_FILE"
			<< endl;
			return 1;
		}
	}

	// Open video stream
	String camera_input = parser.get<String>("camera");
	String video_input = parser.get<String>("video");
	if (!video_input.empty()){			// Open video file
		const String name = video_input;
		capture.open(name);
	}else if (camera_input.size() == 1)	// Open specific camera
		capture.open(stoi(camera_input));
	else								// Open default system camera
		capture.open(0);

	if (!capture.isOpened()){
		cerr << "ERROR::VIDEO::FAILURE_OPENING_VIDEO_STREAM_INPUT" << endl;
		return 1;
	}

	// Open video output stream
	String dest = parser.get<String>("out");
	if (!dest.empty()){
		writeVideo = true;
		const string name = dest + ".avi";
		int codec = VideoWriter::fourcc('X', 'V', 'I', 'D'); // mp4 encoding
		// Get input video stream info
		int srcFrameWidth, srcFrameHeight, srcfps;
		srcFrameWidth = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
		srcFrameHeight = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
		srcfps = static_cast<int>(capture.get(CAP_PROP_FPS));
		outputVideo.open(name, codec, srcfps,
							Size(srcFrameWidth, srcFrameHeight), true);
		if (!outputVideo.isOpened()){
			cerr <<
			"ERROR:VIDEO::FAILURE_OPENING_VIDEO_STREAM_OUTPUT"
			<< endl;
			return 1;
		}
	}

	// FLAGS
	// Color output format
	if (parser.has("gray"))
		gray = true;
	// Write nose location
	if (parser.has("nose_location"))
		noseLoc = true;
	// Write eyes location
	if (parser.has("eyes_location"))
		eyesLoc = true;
	// Open output file
	if (parser.has("proc_time")){
		if (!video_input.empty()){
			const string name = video_input.substr(0, video_input.size()-4)\
								+ "proctime";
			proctimefile.open(name, ios::trunc);
		}else
			proctimefile.open("camproctime", ios::trunc);
	}

	return 0;
}


// Print execution statistics throught stdout
void printExecStatistics(size_t nFrames, int64 processingTime, int64 *t0){
	const int N = 30;
	int64 t1 = cv::getTickCount();
	cout << "Frames captured: " <<
	format("%5lld", static_cast<long long int>(nFrames))
	<< "    Average FPS: " <<
	format("%9.1f",
			static_cast<double>(getTickFrequency()) * N / (t1 - *t0))
	<< "    Average time per frame: " << 
	format("%9.2f ms",
			static_cast<double>(t1 - *t0) * 1000.0f / (N*getTickFrequency()))
	<< "    Processing time: " <<
	format("%9.2f ms",
			static_cast<double>(processingTime) * 1000.0f /
			getTickFrequency())
	<< endl;
	*t0 = t1;
}


// Search for faces, eyes and noses and draw their location
void trackAndDraw(Mat frame){
	// Convert the frame to grayscale
	Mat grayFrame;
	cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
	clahe->apply(grayFrame, grayFrame);

	// Set the output frame as gray too
	if (gray)
		cvtColor(grayFrame, frame, COLOR_GRAY2BGR);

	// Detect faces on the frame and draw their location 
	vector<Rect> faces, eyes, noses;
	Rect ROINose, ROIEyes;
	faceCascade.detectMultiScale(grayFrame, faces, 1.2,	2, 0, Size(100, 150),
									Size(300, 450));
	for (size_t i = 0; i < faces.size(); i++){
		rectangle(frame, faces[i], Scalar(255, 0, 0));

		// Use a 3/4 frame size as Region Of Interest to search the nose
		if (searchNose){
			ROINose = Rect(Point(faces[i].x+faces[i].width*0.2,
							faces[i].y+faces[i].height*0.35),
							Size(faces[i].width*0.6, faces[i].height*0.5));
			rectangle(frame, ROINose, Scalar(0, 0, 255));
			Mat frameROI = grayFrame(ROINose);

			noseCascade.detectMultiScale(frameROI, noses, 1.1, 2, 0,
											Size(20, 15), Size(90, 80));
			for (size_t j = 0; j < noses.size(); j++){
				// Use the ROINose as reference for location and dimension
				Point p1(ROINose.x + noses[j].x, ROINose.y + noses[j].y);
				Point p2(ROINose.x + noses[j].x + noses[j].width,
							ROINose.y + noses[j].y + noses[j].height);
				rectangle(frame, p1, p2, Scalar(0, 255, 0));
			}// for
		}// if

		// Same as nose for eyes
		if (searchEyes){
			ROIEyes = Rect(Point(faces[i].x, faces[i].y),
							Size(faces[i].width, faces[i].height/1.7));
			rectangle(frame, ROIEyes, Scalar(0, 125, 125));
			Mat frameROI = grayFrame(ROIEyes);

			eyesCascade.detectMultiScale(frameROI, eyes, 1.075, 1, 0,
											Size(25, 10), Size(70, 50));
			for (size_t j = 0; j < eyes.size(); j++){
				Point p1(ROIEyes.x + eyes[j].x, ROIEyes.y + eyes[j].y);
				Point p2(ROIEyes.x + eyes[j].x + eyes[j].width,
							ROIEyes.y + eyes[j].y + eyes[j].height);
				rectangle(frame, p1, p2, Scalar(125, 125, 0));
			}// for
		}// if
	}// for

	// Write features locations to stdout if detected
	if (noseLoc || eyesLoc){
		if (eyesLoc){
			if(eyes.size() >= 1){
				short x = ((ROIEyes.x + eyes[0].x) * 2 + eyes[0].width)/2;
				short y = ((ROIEyes.y + eyes[0].y) * 2 + eyes[0].height)/2;
				cout << x << ", " << y;
				if(eyes.size() >= 2){
					x = ((ROIEyes.x + eyes[1].x)*2 + eyes[1].width)/2;
					y = ((ROIEyes.y + eyes[1].y)*2 + eyes[1].height)/2;
					cout << ", " << x << ", " << y;
				}
			}else
				cout << "-1, -1, -1, -1";
		}
		if (noseLoc && eyesLoc)
			cout << ", ";
		if (noseLoc){
			if (!noses.empty()){
				short x = ((ROINose.x + noses[0].x) * 2 + noses[0].width)/2;
				short y = ((ROINose.y + noses[0].y) * 2 + noses[0].height)/2;
				cout << x << ", " << y;
			}else
				cout << "-1, -1";
		}

		cout << endl;
	}
}

