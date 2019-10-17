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

/*
 * SimpleFacetracker object to configure, initialize and run the tracker.
 */
class SimpleFacetracker{
	public:
		/* All flags set to false. CLAHE clip limit set to 2.0 */
		SimpleFacetracker();
		SimpleFacetracker(float claheClipLimit);
		~SimpleFacetracker();

		/* Load cascade from .xml file */
		int initFaceCascade(String faceCascade);
		int initEyesCascade(String eyesCascade);
		int initNoseCascade(String NoseCascade);

		/* Initialize video resources */
		int videoSource(const String name);
		int cameraSource(int camera);
		int videoOutput(const String name);

		/* Output video in grayscale format */
		void outputGray();
		/* Write the processing time invested in the tracking */
		void writeProcessingTime();

		/* Return status of video source or user command */
		bool finished();

		/* Facetracking applies on the video source. Locates the objects
		 * detected on the Rect attributes */
		int track();
		/* Outputs objects detected on the last frame captured */
		void drawLocations();

		/* Outputs locations of detected objects in normalized range (-1, 1) */
		void faceLocation(float &x1, float &y1, float &x2, float &y2);
		void noseLocation(float &x1, float &y1, float &x2, float &y2);
		void eyesLocation(float &x11, float &y11, float &x21, float &y21,
						  float &x12, float &y12, float &x22, float &y22);

		/* Release resources */
		void release();

	private:
		Ptr<CLAHE> clahe;				// Gray range equalizer

		CascadeClassifier faceCascade;	// Face haarclassifier
		CascadeClassifier eyesCascade;	// Eyes haarclassifier
		CascadeClassifier noseCascade;	// Nose cascade

		bool searchEyes;				// Track eyes
		bool searchNose;				// Track nose

		VideoCapture capture;			// Video where the face is tracked
		int srcFrameWidth;				// Video source frame width in pixels
		int srcFrameHeight;				// Video source frame height in pixsels
		VideoWriter outputVideo;		// Video output stream
		bool writeVideo;				// Write output video
		bool gray;						// Output gray scale video

		ofstream proctimefile;			// File stream to store process time

		bool done;						// Status of video source or user
										// command
		Mat frame;						// Frame to load from video source
		size_t nFrames;					// Number of frames read

		double processingTime;			// Processing time of detection

		Rect face;						// Bounding box face location
		Rect eyes[2];					// Two bounding boxes of each eye
		Rect nose;						// BB nose 
	
	private:
		/* Writes the last frame with the objects detected on a video file */
		void writeFrame();
};

