#!/usr/bin/env python3
"""
videocrop.py Simple video cropper.
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
import cv2, sys

if len(sys.argv) < 2:
    print("Usage videocrop.py <sourcevid> <destvid>")

videoCapture = cv2.VideoCapture(sys.argv[1])

# get video properties
fps = videoCapture.get(cv2.CAP_PROP_FPS)
size = (int(videoCapture.get(cv2.CAP_PROP_FRAME_WIDTH)),
        int(videoCapture.get(cv2.CAP_PROP_FRAME_HEIGHT)))

# open video writer with same properties as video input
videoWriter = cv2.VideoWriter(sys.argv[2],
        cv2.VideoWriter_fourcc('X', 'V', 'I', 'D'), fps, size)

ret, frame = videoCapture.read()
write = False
while(ret):
    cv2.imshow('OpenCV Video Capture', frame)
 
    keycode = cv2.waitKey(150) & 0xff
    if keycode == ord('q'):
        break
    # change writing status
    if keycode == ord('c'): 
        write = not write
    # fill video output with frames
    if write:
        videoWriter.write(frame)

    ret, frame = videoCapture.read()

videoCapture.release()
cv2.destroyAllWindows()

