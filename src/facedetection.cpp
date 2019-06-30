/*
 * This program is a facetracker concept implemented using haarcascades
 *
 * Author: Leonardo Niels Pardi
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

// Parse command line arguments and initialize program
int init(int argc, char** argv);
// Print execution statistics throught stdout
void printExecStatistics(size_t nFrames, int64 processingTime, int64 *t0);
// Search for faces and/or noses and draw their location
void detectAndDisplay(Mat frame);

const String keys =
	"{help h|| Print help message}"
	"{camera c|0| Camera device number as video stream input}"
	"{video v|| Video file as video stream input}"
	"{@face_cascade f|/usr/share/opencv4/haarcascades/haarcascade_frontalface"
	"_default.xml|.xml file}"
	"{nose_cascade n|| .xml nose cascade file}"
	"{out o|| Write a video file -o <name>.avi}"
	"{gray g|| Output video in grayscale}"
	"{nose_location l|| Writes to stdout the nose location if detected, "
	"otherwise \'-1, -1\'}"
	"{proc_time p|| Write the processing time of detection in a file "
	"(camproctime or <vid>proctime)}";
const String about =
	"Easy facedetection v1\n"
	"This program is a facetracker concept implemented using haarcascades";

Ptr<CLAHE> clahe = createCLAHE();
CascadeClassifier faceCascade;
bool searchNose = false;
CascadeClassifier noseCascade;
VideoCapture capture;
bool writeVideo = false;
VideoWriter outputVideo;
bool gray = false;
bool noseLoc = false;
ofstream proctimefile;

int main(int argc, char** argv){
	if (init(argc, argv) != 0)
		return 1;

	Mat frame;	// Frame to read from video source and to draw in
	size_t nFrames = 0;	// Number of frames readed on every iteration.
	int64 t0 = cv::getTickCount();	// Ticks at the begining of execution
	int64 processingTime = 0;	// Proccesing time used in detection
	// While the video source has frames and the user don't hit ESC
	while (capture.read(frame) && !(waitKey(1) == 27/*ESC*/)){
		// Check frame integrity
		if (frame.empty()){
			cerr << "ERROR::FRAME::EMPTY_FRAME_READED" << endl;
			return 1;
		}

		nFrames++;
		if (!noseLoc && nFrames % 30 == 0)
			printExecStatistics(nFrames, processingTime, &t0);

		// Measure time used in detecting faces and noses
		int64 tp0 = getTickCount();
		detectAndDisplay(frame);
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

	//std::cout << "Number of captured frames: " << nFrames << endl;
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
		outputVideo.open(name,
							codec,
							srcfps,
							Size(srcFrameWidth, srcFrameHeight),
							true);
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


// Search for faces and/or noses and draw their location
void detectAndDisplay(Mat frame){
	// Convert the frame to grayscale
	Mat grayFrame;
	cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
	// Apply contrast limiterd adaptative histogram equalization
	clahe->apply(grayFrame, grayFrame);

	// Set the output frame as gray too
	if (gray)
		cvtColor(grayFrame, frame, COLOR_GRAY2BGR);

	// Detect faces on the frame and draw their location with a blue rectangle
	vector<Rect> faces;
	vector<Rect> noses;
	Rect ROI;
	faceCascade.detectMultiScale(grayFrame,
									faces,
									1.2,
									3,
									0,
									Size(100, 150),
									Size(300, 450));
	for (size_t i = 0; i < faces.size(); i++){
		rectangle(frame, faces[i], Scalar(255, 0, 0));

		// Use a 3/4 frame size as Region Of Interest to search for noses
		if (searchNose){
			ROI = Rect(Point(faces[i].x+faces[i].width*0.2,
						faces[i].y+faces[i].height*0.35),
						Size(faces[i].width*0.6, faces[i].height*0.5));
			rectangle(frame, ROI, Scalar(0, 0, 255));
			Mat frameROI = grayFrame(ROI);

			noseCascade.detectMultiScale(frameROI,
											noses,
											1.1,
											2,
											0,
											Size(35, 30),
											Size(90, 80));
			for (size_t j = 0; j < noses.size(); j++){
				// Use the ROI as reference for location and dimension
				Point p1(ROI.x + noses[j].x, ROI.y + noses[j].y);
				Point p2(ROI.x + noses[j].x + noses[j].width,
							ROI.y + noses[j].y + noses[j].height);
				rectangle(frame, p1, p2, Scalar(0, 255, 0));
			}// for
		}// if
	}// for

	// Write nose location to stdout if detected
	if (noseLoc){
		if (!noses.empty()){
			int x = ((ROI.x + noses[0].x) * 2 + noses[0].width) / 2;
			int y = ((ROI.y + noses[0].y) * 2 + noses[0].height) / 2;
			cout << x << ", " << y << endl;
		}else
			cout << "-1, -1" << endl;
	}
}

