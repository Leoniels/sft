#!/usr/bin/env python3
import cv2, sys

img = cv2.imread(sys.argv[1], 0)

clahe2 = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
clahe4 = cv2.createCLAHE(clipLimit=3.0, tileGridSize=(16, 16))
outclahe2 = clahe2.apply(img)
outclahe4 = clahe4.apply(img)

outhe = cv2.equalizeHist(img)

out = cv2.hconcat([img, outhe, outclahe2, outclahe4])
cv2.imshow('OpenCV histogram equalize', out)
cv2.waitKey(0)

