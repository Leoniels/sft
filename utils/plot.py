import sys
import math as mt
import ast
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import numpy as np

frames = 1200

def ecm(sample):
    sum = 0
    for i in sample:
        sum += i**2
    
    return sum / len(sample)

def distance(reftrace, trace):
    distances = []
    for i in range(0, len(trace)):
        xdiff = reftrace[i][0] - trace[i][0]
        ydiff = reftrace[i][1] - trace[i][1]
        distances.append(mt.sqrt(xdiff**2+ydiff**2))
    return distances

def routing(traces=[]):
    distance1 = distance(traces[0], traces[1])
    distance2 = distance(traces[0], traces[2])
    err = []
    err += [round(ecm(distance1), 2)]
    err += [round(ecm(distance2), 2)]

    x = np.arange(0, len(traces[0]))

    plt.xlabel('Frame')
    plt.ylabel('Error')
    plt.title('Controller Samples')
    red_patch = mpatches.Patch(color='red', label='Trace1')
    blue_patch = mpatches.Patch(color='blue', label='Trace2')
    plt.legend(handles=[red_patch, blue_patch])
    plt.text(20, 15, r'ECM=' + str(err[0]), color='red')
    plt.text(20, 13, r'ECM=' + str(err[1]), color='blue')
    plt.plot(x, distance1, 'r', x, distance2, 'b')
    plt.show()

    print(err[0])
   
def main():
    traces = []
    for i in range(1, len(sys.argv)):
        trace = []
        with open(sys.argv[i]) as file:
            for line in file:
                trace.append(ast.literal_eval(line))
        traces.append(trace)

    routing(traces)
    

if __name__ == '__main__':
    if len(sys.argv) >= 3:
        main()
    else:
        print("Usage: plot.py <ref_file> <file_1> [<file_2>]")
