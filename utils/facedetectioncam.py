#!/usr/bin/env python3
import cv2, sys

videoCapture = cv2.VideoCapture(sys.argv[1])

ret, frame = videoCapture.read()
while(ret):
    cv2.imshow('OpenCV Video Capture', frame)
 
    keycode = cv2.waitKey(10) & 0xff
    if keycode == ord('q'):
        break
    if keycode == ord('c'): 
        cv2.imwrite('screenshot.png', frame)

    ret, frame = videoCapture.read()

videoCapture.release()
cv2.destroyAllWindows()

