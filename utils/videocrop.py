#!/usr/bin/env python3
"""
Crop a secuence of a video
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

