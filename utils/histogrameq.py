#!/usr/bin/env python3
import cv2, sys

img = cv2.imread(sys.argv[1], 0)

#img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
clahedef = cv2.createCLAHE()
clahecust = cv2.createCLAHE(clipLimit=float(sys.argv[2]), tileGridSize=(int(sys.argv[3]), int(sys.argv[3])))
outclahedef = clahedef.apply(img)
outclahecust = clahecust.apply(img)

outhe = cv2.equalizeHist(img)

cv2.imwrite("resgray.png", img)
cv2.imwrite("reseq.png", outhe)
cv2.imwrite("resclahe.png", outclahedef)
cv2.imwrite("rescustclahe.png", outclahecust)

"""
out = cv2.hconcat([img, outhe, outclahedef, outclahecust])
cv2.imshow('OpenCV histogram equalize', out)
cv2.waitKey(0)
"""

