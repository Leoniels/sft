#!/usr/bin/env python3
import sys
import math as mt
import ast

if len(sys.argv) < 1:
    print("Usage: evaltracking.py <file_with_locations>")
    exit(1)

with open(sys.argv[1]) as file:
    numFrames = 0
    eyesNotDetected = 0
    noseNotDetected = 0
    oneEyeDetected = 0
    smthNotDetected = 0
    location  = []
    for line in file:
        location = ast.literal_eval(line)

#        print(location)
        numFrames += 1

        if len(location) > 4:
            if location[0] == -1:
                eyesNotDetected += 1
            
            if location[4] == -1:
                noseNotDetected += 1

            if location[0] == -1 or location[4] == -1:
                smthNotDetected += 1

        else:
            oneEyeDetected += 1
            smthNotDetected += 1

            if location[2] == -1:
                noseNotDetected += 1

    print("Number of frames " + str(numFrames))
    print("Eyes not detected " + str(eyesNotDetected))
    print("Nose not detected " + str(noseNotDetected))
    print("Only one eye detected " + str(oneEyeDetected))
    print("Something not detected " + str(smthNotDetected))

    print("Failure percentage %" + str(smthNotDetected / float(numFrames) * 100.0))
    print("Eyes not detected percentage %" + str((eyesNotDetected + oneEyeDetected) / float(numFrames) * 100))
    print("Nose not detected percentage %" + str(noseNotDetected / float(numFrames) * 100))

