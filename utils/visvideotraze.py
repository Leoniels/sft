#!/usr/bin/env python3
"""
visvideotrace.py Prints the trace builded by hand while playing the video
Copyright (C) Leonardo Niels Pardi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""
import cv2, argparse

# argument options
ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video", required=True, help="Path to the video traced")
ap.add_argument("-f", "--file", required=True, help="Path to the file with traces")
args = vars(ap.parse_args())

# open video stream and file
videoCapture = cv2.VideoCapture(args["video"])
f = open(args["file"], 'r')

ret, frame = videoCapture.read()
while(ret):
    # get point from file
    line = f.readline()
    strpoint = line.split(", ")
    point = (int(strpoint[0]), int(strpoint[1]))

    # draw the point in the image
    frame = cv2.circle(frame, point, 3, (0, 255, 0))
    cv2.imshow('OpenCV Video Capture', frame)
    keycode = cv2.waitKey(50) & 0xff
    
    if keycode == ord('q'):
        break

    ret, frame = videoCapture.read()

# release resources
f.close()
videoCapture.release()
cv2.destroyAllWindows()

