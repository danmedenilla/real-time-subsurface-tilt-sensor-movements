# -*- coding: utf-8 -*-
"""
Created on Tue Sep 21 18:24:15 2021

@author: GGRDD-8
"""
import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

arduino = serial.Serial(port='COM3', baudrate=9600)

xy = 0
xz = 0
xy2 = 0
xz2 = 0
xa1 = 0
ya1 = 0
za1 = 0
xa2 = 0
ya2 = 0
za2 = 0
seg_len = 1
x = 0
y = 0
z = 0
u = 0
v = 0
w = -1
x2 = 0
y2 = 0
z2 = 0
u2 = 0
v2 = 0
w2 = -1
xyc = 0
xzc = 0
xy2c = 0
xz2c = 0

def center():
    global xy, xz, xy2, xz2, xyc, xzc, xy2c, xz2c
    
    while True:
        fetch()
        xyc = xy
        xzc = xz
        xy2c = xy2
        xz2c = xz2
        if (xyc != 0 and xzc != 0 and xy2c != 0 and xz2c != 0):
            break
    
def read_data():
    data = arduino.readline()
    return data

def accel_to_lin_xz_xy(seg_len,xa,ya,za):
#DESCRIPTION
#converts accelerometer data (xa,ya,za) to corresponding tilt expressed as horizontal linear displacements values, (xz, xy)
#INPUTS
#seg_len; float; length of individual column segment
#xa,ya,za; array of integers; accelerometer data (ideally, -1024 to 1024)
#OUTPUTS
#xz, xy; array of floats; horizontal linear displacements along the planes defined by xa-za and xa-ya, respectively; units similar to seg_len
    theta_xz = np.arctan2(za,(np.sqrt(xa*2 + ya*2)))
    theta_xy = np.arctan2(ya,(np.sqrt(xa*2 + za*2)))
    xz = seg_len * np.sin(theta_xz)
    xy = seg_len * np.sin(theta_xy)
    return xz, xy

def fetch():
    global xz, xy, xz2, xy2
    value = str(read_data()); # Taking input from serial
    value = value.replace('b', '')
    value = value.replace('\'', '')
    value = value.replace('\\', '')
    value = value.replace('r', '')
    value = value.replace('n', '')
    tilt1 = value.split() # printing the value
    print(tilt1)
    try:
        xa1 = int(tilt1[0])
        ya1 = int(tilt1[1])
        za1 = int(tilt1[2])
        print (xa1, ya1, za1)
        try:
            xz, xy = accel_to_lin_xz_xy(seg_len,xa1,ya1,za1)
            xz = xz - xzc
            xy = xy - xyc
        except:
            pass    
        print (xz)
        print (xy)
    except:
        pass
    try:
        xa2 = int(tilt1[3])
        ya2 = int(tilt1[4])
        za2 = int(tilt1[5])
        print (xa2, ya2, za2)
        try:
            xz2, xy2 = accel_to_lin_xz_xy(seg_len,xa2,ya2,za2)
            xz2 = xz2 - xz2c
            xy2 = xy2 - xy2c
        except:
            pass    
        print (xz2)
        print (xy2)
    except:
        pass

#center() 
fig, ax = plt.subplots(subplot_kw=dict(projection="3d"))

def get_arrow(zx, xy):
    x = 0
    y = 0
    z = 0
    u = xy
    v = xz
    w = -1
    return x,y,z,u,v,w
def get_arrow2(xz, xy, xz2, xy2):
    x2 = xy
    y2 = xz
    z2 = -1
    u2 = xy2 #re-consider addition
    v2 = xz2 #re-consider addition
    w2 = -1
    return x2,y2,z2,u2,v2,w2

quiver = ax.quiver(*get_arrow(xz, xy), pivot='middle')
quiver2 = ax.quiver(*get_arrow(xz2, xy2), pivot='middle')

ax.set_xlim(-3, 3)
ax.set_ylim(-3, 3)
ax.set_zlim(-3, 1)
ax.set_xlabel('xy')
ax.set_ylabel('xz')
ax.set_zlabel('depth')

def update(i):
    fetch()
    global quiver, quiver2, xz, xy, xz2, xy2
    quiver.remove()
    quiver = ax.quiver(*get_arrow(xz, xy),color ='blue', label = 'node 1')
    quiver2.remove()
    quiver2 = ax.quiver(*get_arrow2(xz, xy, xz2, xy2),color ='red', label = 'node 2')
    ax.legend(loc='upper left')


ani = FuncAnimation(fig, update, frames=1000, interval=50)
plt.show()



    
