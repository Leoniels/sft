#!/usr/bin/env python3
import cv2, argparse

def getpos(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print ("x " + str(x) + ", y " + str(y))

# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required=True, help="Path to the image")
args = vars(ap.parse_args())
 
# load the image, clone it, and setup the mouse callback function
image = cv2.imread(args["image"])
cv2.namedWindow("OpenCV image")
cv2.setMouseCallback("OpenCV image", getpos)
  
# keep looping until the 'q' key is pressed
while True:
    # display the image and wait for a keypress
    cv2.imshow("OpenCV image", image)
    key = cv2.waitKey(20) & 0xFF
                 
    # if the 'r' key is pressed, reset the cropping region
    if key == ord("q"):
        break
                                                                     
# close all open windows
cv2.destroyAllWindows()

