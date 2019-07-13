#!/usr/bin/env python3
#Script based on another from Alejandro Méndez Fernández and Leonardo Niels Pardi
#Author: Leonardo Niels Pardi
import argparse, ast    # Parse arguments and read files
import math as mt       # Math stuff
import matplotlib.patches as mpatches # Dem charts
import matplotlib.pyplot as plt
import numpy as np      # Arrays


# Argument options
ap = argparse.ArgumentParser()
ap.add_argument("-r", "--reference", required=True, help="File with reference points")
ap.add_argument("-s", "--simpleft", required=True, help="File with simplefacetracker's points")
ap.add_argument("-d", "--saftdef", required=True, help="File with default saragih-facetracker's points")
ap.add_argument("-c", "--saftcus", required=True, help="File with custom saragih-facetracker's points")


# Square mean error
def ecm(sample):
    sum = 0
    for i in sample:
        sum += i**2
    
    return sum / len(sample)


# Distance between points of reference and tracker points
def calcDistances(reftrace, trace):
    distances = []
    for i in range(0, len(trace)):
        xdiff = reftrace[i][0] - trace[i][0]
        ydiff = reftrace[i][1] - trace[i][1]
        distances.append(mt.sqrt(xdiff**2+ydiff**2))
    return distances


# Get distances between reference and traces, calc sme and draw the chart
def routing(setstraces=[]):
    setsdistances = []
    setsdistances.append(calcDistances(setstraces[0], setstraces[1]))
    setsdistances.append(calcDistances(setstraces[0], setstraces[2]))
    setsdistances.append(calcDistances(setstraces[0], setstraces[3]))

    errors = []
    errors += [round(ecm(setsdistances[0]), 3)]
    errors += [round(ecm(setsdistances[1]), 3)]
    errors += [round(ecm(setsdistances[2]), 3)]

    top = max([max(setsdistances[0]), max(setsdistances[1]), max(setsdistances[2])])
    step = top / 12

    x = np.arange(0, len(setstraces[0]))

    font = {'weight' : 'bold', 'size' : 22}
    plt.rc('font', **font)
    plt.xlabel('Frame')
    plt.ylabel('Error')
    plt.title('Test de Precisión')
    red_patch = mpatches.Patch(color='red', label='simple ft')
    blue_patch = mpatches.Patch(color='blue', label='default saragih ft')
    green_patch = mpatches.Patch(color='green', label='custom saragih ft')
    plt.legend(handles=[red_patch, blue_patch, green_patch])
    plt.text(0, int(top-step), r'ECM=' + str(errors[0]), color='red')
    plt.text(0, int(top-step*2), r'ECM=' + str(errors[1]), color='blue')
    plt.text(0, int(top-step*3), r'ECM=' + str(errors[2]), color='green')
    plt.plot(x, setsdistances[0], 'r', x, setsdistances[1], 'b', x, setsdistances[2], 'g')
    plt.show()

   
def main(args):
    filelist =[]
    filelist.append(args["reference"])
    filelist.append(args["simpleft"])
    filelist.append(args["saftdef"])
    filelist.append(args["saftcus"])

    setstraces = []
    for tracefile in filelist:
        traces = []
        with open(tracefile) as file:
            for line in file:
                traces.append(ast.literal_eval(line))
        setstraces.append(traces)

    routing(setstraces)
    

if __name__ == '__main__':
    args = vars(ap.parse_args())
    main(args)

