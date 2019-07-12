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
print("Number of frames " + str(frcount))
print("Frames per seconds " + str(fps))

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
# frame index to show next (every 0.5 sec)
frjump = int(fps/2)
fridx = frjump
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
    # estimated point 0.5 sec's frame
    estpt = [int(prevpoint[0]), int(prevpoint[1])]
    for i in range(0, frjump-1):
        outfile.write(str(estpt[0]) + ", " + str(estpt[1]) + "\n")
        # increase offset for next frame
        offset[0] += incx
        offset[1] += incy
        estpt = [int(prevpoint[0]+offset[0]), int(prevpoint[1]+offset[1])]
    outfile.write(str(point[0]) + ", " + str(point[1]) + "\n")
    prevpoint = point
    fridx += frjump
    videocap.set(cv2.CAP_PROP_POS_FRAMES, fridx-1)
    retval, frame = videocap.read()

# frame index jump overshooted
if fridx > frcount:
    fridx -= frjump
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
    # estimated point 0.5 sec's frame
    estpt = [int(prevpoint[0]), int(prevpoint[1])]
    for i in range(0, int(frcount)-fridx-1):
        outfile.write(str(estpt[0]) + ", " + str(estpt[1]) + "\n")
        # increase offset for next frame
        offset[0] += incx
        offset[1] += incy
        estpt = [int(prevpoint[0]+offset[0]), int(prevpoint[1]+offset[1])]
    outfile.write(str(point[0]) + ", " + str(point[1]) + "\n")
 
# close video, window and file
videocap.release()
cv2.destroyAllWindows()
outfile.close()

