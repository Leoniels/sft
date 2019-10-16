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


/*
 * SimpleFacetracker object to configure, initialize and run the tracker.
 */
class SimpleFacetracker{
	public:
		SimpleFacetracker();
		SimpleFacetracker(float claheClipLimit);
		~SimpleFacetracker();

		int initFaceCascade(String faceCascade);
		int initEyesCascade(String eyesCascade);
		int initNoseCascade(String NoseCascade);

		int videoSource(const String name);
		int cameraSource(int camera);
		int videoOutput(const String name);

		void outputGray();
		void writeProcessingTime();

		bool finished();

		void track();

		void faceLocation(float &x, &y);

		void release();

	private:
		Ptr<CLAHE> clahe;				// Gray range equalizer

		CascadeClassifier faceCascade;	// Face haarclassifier
		CascadeClassifier eyesCascade;	// Eyes haarclassifier
		CascadeClassifier noseCascade;	// Nose cascade

		bool searchEyes;				// Track eyes
		bool searchNose;				// Track nose

		VideoCapture capture;			// Video where the face is tracked
		VideoWriter outputVideo;		// Video output stream
		bool writeVideo;				// Write output video
		bool gray;						// Output gray scale video

		ofstream proctimefile;			// File stream to store process time

		bool done;

		Mat frame;						// Frame to read from video source
		size_t nFrames;					// Number of frames read

		double avgFPS;
		double avgTimePerFr;
		double processingTime;

		Rect face;
		Rect eyes[2];
		Rect nose;
}

