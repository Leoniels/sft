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

/* All flags set to false. CLAHE clip limit set to 2.0. Objects locations and
 * size set to zero. */
SimpleFacetracker::SimpleFacetracker()
	:
		searchEyes(false), searchNose(false),
		writeVideo(false), gray(false),
		done(false)
{
	clahe = createCLAHE(2.0);
	face = Rect(Point(0,0),Point(0,0));
	eyes[0] = Rect(Point(0,0),Point(0,0));
	eyes[1] = Rect(Point(0,0),Point(0,0));
	nose = Rect(Point(0,0),Point(0,0));
}
SimpleFacetracker::SimpleFacetracker(float claheClipLimit)
	:
		searchEyes(false), searchNose(false),
		writeVideo(false), gray(false),
		done(false)
{
	clahe = createCLAHE(claheClipLimit);
	face = Rect(Point(0,0),Point(0,0));
	eyes[0] = Rect(Point(0,0),Point(0,0));
	eyes[1] = Rect(Point(0,0),Point(0,0));
	nose = Rect(Point(0,0),Point(0,0));
}
SimpleFacetracker::~SimpleFacetracker(){}

/* Load cascade from .xml file */
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

/* Initialize video resources */
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
	if (!capture.isOpened()){
		cerr << "ERROR:VIDEO::VIDEO_INPUT_NOT_OPENED" << endl;
		return 1;
	}

	const String filename(name + ".avi");
	//name += ".avi";
	int codec = VideoWriter::fourcc('X', 'V', 'I', 'D'); // mp4 encoding

	// Get input video stream info
	int srcfps;
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

/* Output video in grayscale format */
void SimpleFacetracker::outputGray(){
	gray = true;
}

/* Write the processing time invested in the tracking */
void SimpleFacetracker::writeProcessingTime(){
	proctimefile.open("proctime", ios::trunc);
}

/* Return status of video source or user command */
bool SimpleFacetracker::finished(){
	return done;
}

/* Facetracking applies on the video source. Locates the objects detected
 * on the Rect attributes */
int SimpleFacetracker::track(){
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
		// Surfaces to operate with
		Mat grayFrame, frameROI;
		// Convert the frame to grayscale
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		clahe->apply(grayFrame, grayFrame);

		// Detect faces on the frame and draw their location 
		vector<Rect> faces, eyesv, noses;
		faceCascade.detectMultiScale(grayFrame, faces, 1.2,	2, 0, Size(100, 150),
									 Size(300, 450));
		if (!faces.empty()){
			face = faces.at(0);

			if (searchNose){
				// Use a 3/4 frame size as Region Of Interest to search the nose
				Rect ROINose = Rect(Point(face.x+face.width*0.2,
										  face.y+face.height*0.35),
									Size(face.width*0.6, face.height*0.5));
				frameROI = grayFrame(ROINose);

				noseCascade.detectMultiScale(frameROI, noses, 1.1, 2, 0,
											 Size(20, 15), Size(90, 80));
				if (!nose.empty()){
					nose = noses.at(0);
					// Use the ROINose as reference for location and dimension
					Point p1(ROINose.x + nose.x, ROINose.y + nose.y);
					Point p2(ROINose.x + nose.x + nose.width,
							 ROINose.y + nose.y + nose.height);
					nose = Rect(p1, p2);
				}// if
			}// if

			// Same as nose for eyes
			if (searchEyes){
				Rect ROIEyes = Rect(Point(face.x, face.y),
									Size(face.width, face.height/1.7));
				frameROI = grayFrame(ROIEyes);

				eyesCascade.detectMultiScale(frameROI, eyesv, 1.075, 1, 0,
											 Size(25, 10), Size(70, 50));
				if (eyesv.size () >= 2){
					eyes[0] = eyesv.at(0);
					eyes[1] = eyesv.at(1);
					for (unsigned short i = 1; i < 2; i++){
						Point p1(ROIEyes.x + eyes[i].x, ROIEyes.y + eyes[i].y);
						Point p2(ROIEyes.x + eyes[i].x + eyes[i].width,
								 ROIEyes.y + eyes[i].y + eyes[i].height);
						eyes[i] = Rect(p1, p2);
					}// for
				}// if
			}// if
		}//if
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
			writeFrame();
	}else
		done = true;

	return 0;
}

/* Outputs objects detected on the last frame captured */
void SimpleFacetracker::drawLocations(){
		// Set the output frame as gray too
		if (gray)
			cvtColor(frame, frame, COLOR_BGR2GRAY);

		rectangle(frame, face, Scalar(255, 0, 0));
		rectangle(frame, nose, Scalar(0, 255, 0));
		rectangle(frame, eyes[0], Scalar(0, 0, 255));
		rectangle(frame, eyes[1], Scalar(0, 0, 255));

		// Show frame on window in realtime
		imshow("OpenCV Facedetection", frame);
}

/* Writes the last frame with the objects detected on a video file */
void SimpleFacetracker::writeFrame(){
		// Set the output frame as gray too
		if (gray)
			cvtColor(frame, frame, COLOR_BGR2GRAY);

		rectangle(frame, face, Scalar(255, 0, 0));
		rectangle(frame, nose, Scalar(0, 255, 0));
		rectangle(frame, eyes[0], Scalar(0, 0, 255));
		rectangle(frame, eyes[1], Scalar(0, 0, 255));

		// Write frame on output video file
		outputVideo << frame;
}

/* Outputs locations of detected objects */
void SimpleFacetracker::faceLocation(float &x1, float &y1, float &x2, float &y2)
{
	x1 = (face.x - srcFrameWidth / 2) / srcFrameWidth;
	x2 = ((face.x + face.width) - srcFrameWidth / 2) / srcFrameWidth;
	y1 = (face.y - srcFrameHeight / 2) / srcFrameHeight;
	y2 = ((face.y + face.height) - srcFrameHeight / 2) / srcFrameHeight;
}
void SimpleFacetracker::noseLocation(float &x1, float &y1, float &x2, float &y2)
{
	x1 = (nose.x - srcFrameWidth / 2) / srcFrameWidth;
	x2 = ((nose.x + nose.width) - srcFrameWidth / 2) / srcFrameWidth;
	y1 = (nose.y - srcFrameHeight / 2) / srcFrameHeight;
	y2 = ((nose.y + nose.height) - srcFrameHeight / 2) / srcFrameHeight;
}
void SimpleFacetracker::eyesLocation(float &x11, float &y11,
									 float &x21, float &y21,
									 float &x12, float &y12,
									 float &x22, float &y22){
	x11 = (eyes[0].x - srcFrameWidth / 2) / srcFrameWidth;
	x21 = ((eyes[0].x + eyes[0].width) - srcFrameWidth / 2) / srcFrameWidth;
	y11 = (eyes[0].y - srcFrameHeight / 2) / srcFrameHeight;
	y21 = ((eyes[0].y + eyes[0].height) - srcFrameHeight / 2) / srcFrameHeight;
	x12 = (eyes[1].x - srcFrameWidth / 2) / srcFrameWidth;
	x22 = ((eyes[1].x + eyes[1].width) - srcFrameWidth / 2) / srcFrameWidth;
	y12 = (eyes[1].y - srcFrameHeight / 2) / srcFrameHeight;
	y22 = ((eyes[1].y + eyes[1].height) - srcFrameHeight / 2) / srcFrameHeight;
}

/* Release resources */
void SimpleFacetracker::release(){
	// Release resources
	if (proctimefile.is_open())
		proctimefile.close();
	if (capture.isOpened())
		capture.release();
	if (outputVideo.isOpened())
		outputVideo.release();
}

