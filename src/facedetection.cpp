/*
 * Program that reads an input video (Camera 0 as default) and writes a video from that source with
 * no compression.
 */

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <vector>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

void detectAndDisplay(Mat frame, bool searchEyes);

const String keys =
   "{help h|| Print help message }"
   "{camera c|0| Camera device number as video stream input }"
	"{video v|| Video file as video stream input }"
   "{@face_cascade f|/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml|.xml file}"
   "{eyes_cascade e|| .xml eyes cascade file }"
   "{out o|| Write a video file -o <name>.avi}";
const String about =
	"Easy facedetection v1\n"
   "This program shows a video stream (camera or video file) and shows the faces detected on it.";

CascadeClassifier faceCascade;
CascadeClassifier eyesCascade;

int main(int argc, char** argv){
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

	bool searchEyes = false;
	String eyes_cascade_name = parser.get<String>("eyes_cascade");
	if (!eyes_cascade_name.empty()){
	searchEyes = true;
	if (!eyesCascade.load(eyes_cascade_name)){
			cerr << "ERROR::CASCADE::FAILURE_LOADING_EYES_CASCADE_FILE" << endl;
			return 1;
      }
	}

   // Open video stream
   VideoCapture capture;
   String camera_input = parser.get<String>("camera");
	String video_input = parser.get<String>("video");
   if (!video_input.empty()){				// Open video file
      const String name = video_input;
      capture.open(name);
	}else if (camera_input.size() == 1)	// Open specific camera
      capture.open(stoi(camera_input));
   else											// Open default system camera
      capture.open(0);

   if (!capture.isOpened()){
       cerr << "ERROR::VIDEO::FAILURE_OPENING_VIDEO_STREAM_INPUT" << endl;
       return 1;
   }

   // Display input video stream info
   int srcFrameWidth, srcFrameHeight, srcfps;
   srcFrameWidth = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
   srcFrameHeight = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
   srcfps = static_cast<int>(capture.get(CAP_PROP_FPS));
   cout << "Frame width: " << srcFrameWidth << endl;
   cout << "  \"  height: " << srcFrameHeight << endl;
   cout << "Capturing FPS: " << srcfps << endl;

   // Open video output stream
   String dest = parser.get<String>("out");
   bool writeVideo = false;
   VideoWriter outputVideo;
   if (!dest.empty()){
		writeVideo = true;
		const string name = dest + ".avi";
   	int codec = VideoWriter::fourcc('X', 'V', 'I', 'D');	// mp4 encoding
		outputVideo.open(name, codec, srcfps, Size(srcFrameWidth, srcFrameHeight), true);
		if (!outputVideo.isOpened()){
			cout << "ERROR:VIDEO::FAILURE_OPENING_VIDEO_STREAM_OUTPUT" << endl;
			return 1;
		}
	}

   Mat frame;
   size_t nFrames = 0;
   int64 t0 = cv::getTickCount();
	int64 processingTime = 0;
	while (capture.read(frame) && !(waitKey(1) == 27/*ESC*/)){
		if (frame.empty()){
			cerr << "ERROR::FRAME::EMPTY_FRAME_READED" << endl;
			return 1;
		}

		nFrames++;
		if (nFrames % 30 == 0){
         const int N = 30;
         int64 t1 = cv::getTickCount();
         cout << "Frames captured: " << cv::format("%5lld", (long long int)nFrames)
              << "    Average FPS: " <<
              cv::format("%9.1f", (double)getTickFrequency() * N / (t1 - t0))
              << "    Average time per frame: " << 
              cv::format("%9.2f ms", (double)(t1 - t0) * 1000.0f / (N * getTickFrequency()))
              << "    Processing time: " <<
				  cv::format("%9.2f ms", (double)(processingTime) * 1000.0f / getTickFrequency())
				  << std::endl;
         t0 = t1;
		}
 
		int64 tp0 = getTickCount();
		detectAndDisplay(frame, searchEyes);
		processingTime = getTickCount() - tp0;

		if (writeVideo)
      	outputVideo << frame;
		else
      	imshow("OpenCV Facedetection", frame);
   }
   
   std::cout << "Number of captured frames: " << nFrames << endl;
   return nFrames > 0 ? 0 : 1;
}

void detectAndDisplay(Mat frame, bool searchEyes){
	Mat grayFrame;
	cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
	equalizeHist(grayFrame, grayFrame);

	vector<Rect> faces;
	faceCascade.detectMultiScale(grayFrame, faces, 1.25, 2, 0/*| CASCADE_DO_ROUGH_SEARCH*/, Size(30, 30));

	for ( size_t i = 0; i < faces.size(); i++){
		rectangle(frame, faces[i], Scalar(255, 0, 0));

		if (searchEyes){
			Rect ROI(Point(faces[i].x, faces[i].y), Size(faces[i].width, faces[i].height/2));
			rectangle(frame, ROI, Scalar(0, 0, 255));
			Mat grayFrameROI = grayFrame(ROI);
			vector<Rect> eyes;
			eyesCascade.detectMultiScale(grayFrameROI, eyes);
			for (size_t j = 0; j < eyes.size(); j++){
				Point p1(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y);
				Point p2(faces[i].x + eyes[j].x + eyes[j].width, faces[i].y + eyes[j].y + eyes[j].height);
				rectangle(frame, p1, p2, Scalar(0, 255, 0));
			}// for
		}// if
	}// for
}

