# sft
Simple Face Tracker based on the haarcascades object detector. Tracks the face, eyes and nose location in a video stream.

## Requirements
· g++

· OpenCV 4.1

## Usage
You can use the help flag to see all options available.
```bash
$ bin/sft -h
```

A more complete example:
```bash
$ bin/sft -e=cascades/haarcascade_eye_tree_eyeglasses.xml -n=cascades/haarcascade_mcs_nose.xml -v=video.avi -o=outputvid
```
