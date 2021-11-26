import os
from math import *
from typing import Text
import matplotlib.pyplot as plt

from browseTerminal import terminalBrowser
import numpy as np

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

threshold = 0.000018
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
for i, coord in enumerate(coord_list[2:]):
    if i%3 == 0:
        continue
    vec = Vector(p1=coord_list[i-3], p2=coord)
    vectors.append(vec)
    p1 = vec.position.get_xy()
    X.append(p1[0])
    Y.append(p1[1])
    p2 = vec.get_xy()
    U.append(cos(vec.angle))
    V.append(sin(vec.angle))

plt.quiver(X,Y,U,V,angles="uv", scale=18)
plt.show()
