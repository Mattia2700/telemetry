import os
from math import *
from typing import Text
import matplotlib.pyplot as plt

from browseTerminal import terminalBrowser
import numpy as np
from numpy.core.fromnumeric import sort
from pandas.core import base

class Vector:
    mag = 0.0
    angle = 0.0
    position = None
    def __init__(self, mag_=None, angle_=None, position_ = None, p1=None, p2=None):
        if not mag_ == None and not angle_ == None:
            self.mag = mag_
            self.angle = angle_
        if type(position_) == list:
            self.position = Vector(p1=position_, p2=[0,0])
        else:
            self.position = position_
        if not p1 == None and not p2 == None:
            self.angle = atan2(p2[1]-p1[1],p2[0]-p1[0])
            self.mag = sqrt((p2[0]-p1[0])**2+(p2[1]-p1[1])**2)
            if position_ == None:
                self.position = Vector(
                    mag_=sqrt((p1[0])**2+(p1[1])**2),
                    angle_=atan2(p1[1],p1[0]),
                    position_=None
                )
    
    def get_xy(self):
        x = self.mag * cos(self.angle)
        y = self.mag * sin(self.angle)
        if not self.position == None:
            p = self.position.get_xy()
            x += p[0]
            y += p[1]
        return [x, y]

    def dist(self, p1):
        pos1 = self.get_xy()
        pos2 = p1.get_xy()
        dist = sqrt((pos2[0]-pos1[0])**2 + (pos2[1]-pos1[1])**2)
        return dist


def findAllFiles(path, extension):
    dirs = os.listdir(path)
    paths = []
    for dir in dirs:
        if (os.path.isdir(path + "/" + dir)):
            paths += findAllFiles(path + "/" + dir, extension)
        else:
            if (extension in dir):
                paths.append(path + "/" + dir)
    return paths



paths = "/home/filippo/Desktop/cristian_logs/20211122_153009_Mirco_TrackDrive"

import pandas as pd

csv = pd.read_csv(paths+"/GPS1.csv", sep=";|,")
values = csv.values
indexes = np.where(values[:,1] == "GGA")[0]
latitude = []
lognitude = []
origin = None
prev_coord = [0,0]

threshold = 0.0000072
angle_threshold = 0.52
x, y = 0, 0
current_speed = 0
angle = 0
X = []
Y = []
U = []
V = []
coord_list = []
speed_list = []
for i in range(values.shape[0]):
    if values[i,1] == "GGA":
        coord = [
            values[i,3],
            values[i,4]
        ]
        coord_list.append(coord)
        speed_list.append(speed_list)
    elif values[i,1] == "VTG":
        current_speed = values[i,13]

vectors = []
for i, coord in enumerate(coord_list[1:]):
    vec = Vector(p1=coord_list[i-1], p2=coord)
    vectors.append(vec)
    p1 = vec.position.get_xy()
    X.append(p1[0])
    Y.append(p1[1])
    p2 = vec.get_xy()
    U.append(cos(vec.angle))
    V.append(sin(vec.angle))

plt.quiver(X,Y,U,V,angles="uv", scale=18)
plt.show()


loops = []
for i in range(len(vectors)):
    reference = vectors[i]
    for j in range(i+1, len(vectors)):
        if not vectors[j].mag > threshold:
            continue
        if reference.dist(vectors[j]) < threshold:
            test = Vector(mag_=None, angle_=None, position_=None,
                            p1=reference.get_xy(), p2=vectors[j].get_xy())
            # print(test.angle, vectors[j].angle)
            # if abs(test.angle - vectors[j].angle) < angle_threshold:
            if j - i > 50:
                loops.append(vectors[i:j])
                break
loops = sorted(loops, key=len, reverse=True)
print(len(loops))

coord_list = np.array(coord_list).T
originalX = coord_list[0]
originalY = coord_list[1]
base_color = np.full((len(originalX), 3), (0.8,1,0.3))
coord = []
for i in range(len(loops)):
    coord.append([])
    X=[]
    Y=[]
    
    color = np.full((len(loops[i]), 3), (0.0,1.0,0.0))
    for j in range(len(loops[i])):
        pos = loops[i][j].get_xy()
        coord[i].append(pos)
        X.append(pos[0])
        Y.append(pos[1])
        color[j] = [0,float(j)/float(len(loops[i])), float(j)/float(len(loops[i]))]
    color = np.concatenate((base_color, color), axis=0)
    c1 = np.concatenate((originalX, X),axis=0)
    c2 = np.concatenate((originalY, Y),axis=0)
    # base_color[360:400] = [0,0,0]
    plt.scatter(c1, c2  , s=1, c=color)
    # plt.scatter(originalX, originalY, s=1, c=base_color)
    plt.show()

