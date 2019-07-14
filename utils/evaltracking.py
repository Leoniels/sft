#!/usr/bin/env python3
"""
evaltracking.py evaluates the simplefacetracker output giving simple
statistics results.
Copyright(C) Leonardo Niels Pardi

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
import sys, ast
import math as mt

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

