#!/usr/bin/env python3
import cv2, sys

img = cv2.imread(sys.argv[1], 0)

clahedef = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
clahecust = cv2.createCLAHE(clipLimit=float(sys.argv[2]), tileGridSize=(int(sys.argv[3]), int(sys.argv[3])))
outclahedef = clahedef.apply(img)
outclahecust = clahecust.apply(img)

outhe = cv2.equalizeHist(img)

out = cv2.hconcat([img, outhe, outclahedef, outclahecust])
cv2.imshow('OpenCV histogram equalize', out)
cv2.waitKey(0)

