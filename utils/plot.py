import sys
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

def trimming(lst):
    print("lst: " + str(lst))
    for i in range(len(lst)):
        if len(lst[i]) > frames:
            lst[i] = lst[i][:frames]

def routing(route=[]):
    trimming(route)
    for i in range(len(route)):
        route[i] = list(map(abs, route[i]))

    err = []
    for i in route:
        err += [round(ecm(i), 2)]

    x = np.arange(0, frames)

    plt.xlabel('Frame')
    plt.ylabel('Error')
    plt.title('Controller Samples')
    red_patch = mpatches.Patch(color='red', label='P')
    blue_patch = mpatches.Patch(color='blue', label='PID')
    plt.legend(handles=[red_patch, blue_patch])
    plt.text(200, 190, r'ECM=' + str(err[0]), color='red')
    plt.text(200, 176, r'ECM=' + str(err[1]), color='blue')
    plt.plot(x, route[0], 'r', x, route[1], 'b')
    plt.show()
   
def main():
    if len(sys.argv) > 3:
        route = []
        for i in range(2, len(sys.argv)):
            with open(sys.argv[i]) as file:
                for line in file:
                    route += [ast.literal_eval(line)]
        print("str: " + str(route))
        
        routing(route)
    else:
        print("ERROR: please specify the number of frames and two text files.")
    

if __name__ == '__main__':
    if len(sys.argv) > 1:
        frames = ast.literal_eval(sys.argv[1])
        main()
    else:
        print("Usage: <number of frames> <file_PID> <file_FuzzyLogic>")
