#!/usr/bin/env python3
import cv2, argparse

# argument options
ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video", required=True, help="Path to the video")
ap.add_argument("-o", "--out", required=True, help="Output filename")
args = vars(ap.parse_args())
 
# open file and video source
videocap = cv2.VideoCapture(args["video"])
outfile = open(args["out"], 'w')

# get video properties
frcount = videocap.get(cv2.CAP_PROP_FRAME_COUNT)
fps = videocap.get(cv2.CAP_PROP_FPS)

clicked = False
point = []

# write mouse pointition in file
def mousepoint(event, x, y, flags, param):
    global clicked, point
    if event == cv2.EVENT_LBUTTONDOWN:
        point = [x, y]
        clicked = True

# init window context and mouse callback
cv2.namedWindow("OpenCV frame")
cv2.setMouseCallback("OpenCV frame", mousepoint)

ret,frame = videocap.read()
# wait until mouse pointition is catched 
def catchpoint(frame):
    global clicked
    clicked = False
    while not clicked:
        cv2.imshow("OpenCV frame", frame)
        cv2.waitKey(50)

# catch first point 
catchpoint(frame)
prevpoint = point
# frame index to show every midsecond frame
fridx = int(fps/2)
videocap.set(cv2.CAP_PROP_POS_FRAMES, fridx-1)
retval, frame = videocap.read()
while retval:
    catchpoint(frame)
    # offset of the two points
    diffx = point[0] - prevpoint[0]
    diffy = point[1] - prevpoint[1]
    # offset per frame
    incx = diffx / float(fps/2)
    incy = diffy / float(fps/2)
    offset = [0.0, 0.0]
    # frame point
    intpt = [int(prevpoint[0]), int(prevpoint[1])]
    for i in range(0, int(fps/2)-1):
        outfile.write(str(intpt[0]) + ", " + str(intpt[1]) + "\n")
        # increase offset for next frame
        offset[0] += incx
        offset[1] += incy
        intpt = [int(prevpoint[0]+offset[0]), int(prevpoint[1]+offset[1])]
    outfile.write(str(point[0]) + ", " + str(point[1]) + "\n")
    prevpoint = point
    fridx += int(fps/2)
    videocap.set(cv2.CAP_PROP_POS_FRAMES, fridx-1)
    retval, frame = videocap.read()

# last frame
videocap.set(cv2.CAP_PROP_POS_FRAMES, frcount-1)
retval, frame = videocap.read()
catchpoint(frame)
# offset from the two points
diffx = point[0] - prevpoint[0]
diffy = point[1] - prevpoint[1]
# offset per frame
incx = diffx / float(frcount-1-fridx)
incy = diffy / float(frcount-1-fridx)
offset = [0.0, 0.0]
# frame point
intpt = [int(prevpoint[0]), int(prevpoint[1])]
for i in range(0, int(frcount)-1-fridx-1):
    outfile.write(str(intpt[0]) + ", " + str(intpt[1]) + "\n")
    # increase offset for next frame
    offset[0] += incx
    offset[1] += incy
    intpt = [int(prevpoint[0]+offset[0]), int(prevpoint[1]+offset[1])]
outfile.write(str(point[0]) + ", " + str(point[1]) + "\n")
 
# close video, window and file
videocap.release()
cv2.destroyAllWindows()
outfile.close()

