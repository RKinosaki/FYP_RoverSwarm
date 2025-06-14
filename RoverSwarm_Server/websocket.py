import socket
import json
import numpy as np
import math
import visualiser as v
import threading

dataLock = threading.Lock()
newData = False
RovID = 0


"""
This file is meant to connect to the rover, receive the raw data to find the position, and its obstacle
"""

class roverData:
    def __init__(self, id = 1, startx = 0, starty = 0, yaw_0 = 0):
        self.id = id
        self.pos = np.array([(startx, starty)])
        self.yaw0 = yaw_0
        self.yaw = yaw_0
        self.obstL = np.array(np.zeros((1, 2)))
        self.obstR = np.array(np.zeros((1, 2)))

def startTCP():
    ##create tcp socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ##address found from the ip seen on phone hotspot
    server_address = ('0.0.0.0', 50)
    print("starting up on: ", server_address)
    sock.bind(server_address)
    sock.listen(10)
    return sock

def findPosition(coord, segment, yaw):
    x = coord[-1][0] + segment*math.sin(yaw)
    y = coord[-1][1] + segment*math.cos(yaw)
    nextPos = np.array((x, y))
    return np.vstack((coord, nextPos))

def findObstaclePosition(coord, obst, yaw, side, bound=500):
    offset_left = 140
    offset_right = 50
    ##The ToF sensors are in a 45 degree angle so the cartesian coordinates are calculated below
    ## L = 0.1*distance (in cm)*cos(45+yaw), 0.1*distance*sin(45+yaw)
    if(side=="L"):
        if(obst<bound):
            x = coord[-1][0]+obst*math.cos(((offset_left*math.pi)/180)-yaw)
            y = coord[-1][1]+obst*math.sin(((offset_left*math.pi)/180)-yaw)
        else:
            x = 0
            y = 0
        nextObst = np.array((x, y))
    elif(side=="R"):
        if(obst<bound):
            x = coord[-1][0]+obst*math.cos(((offset_right*math.pi)/180)-yaw)
            y = coord[-1][1]+obst*math.sin(((offset_right*math.pi)/180)-yaw)
        else:
            x = 0
            y = 0
        nextObst = np.array((x, y))
    else:
        print("Side not correct!")
        x = 0
        y = 0
        nextObst = np.array((x, y))
    return np.vstack((coord, nextObst))
    
def initialiseData():
    R1 = roverData(1, 0, 0, 0)
    R2 = roverData(2, 1, 2, 3)
    return (R1, R2)

def formatData(line):
    print(line)
    line.strip()
    data = json.loads(line)
    print("Parsed JSON:", data)
    return data

def receiveData(sock, R, graphs, axFig):
    global newData, RovID
    prevTravelled = np.array([0, 0, 0])
    travelled = np.array([0, 0, 0])
    yaw_scaler = 15.7
    while True:
        print('waiting for connection')
        connection, client_address = sock.accept()
        print('connection from', client_address)
        try:  
            with connection.makefile('r') as f:
                for line in f:
                    try:
                        data = formatData(line)
                        id = int(data["id"])-1
                        ##Picks the rover from the id data
                        Rov = R[id]
                        ##Take the average of the encoder
                        travelled = (data["encoder"][0]+data["encoder"][1])/2
                        segment = travelled-prevTravelled[id]
                        ## Yaw = Yaw0 + Measured Yaw
                        Rov.yaw = Rov.yaw0 + yaw_scaler*data["yaw"]
                        ##Calculate position and add to np list
                        Rov.pos = findPosition(Rov.pos, segment, Rov.yaw)
                        prevTravelled[id] = travelled
                        ##Find the obstacle positions
                        Rov.obstL = findObstaclePosition(Rov.pos, data["distance"][0], Rov.yaw, "L")
                        Rov.obstR = findObstaclePosition(Rov.pos, data["distance"][1], Rov.yaw, "R")
                        with dataLock:
                            newData = True
                            RovID = id
                    except json.JSONDecodeError as e:
                        print("JSON Decode Error:", e)
        finally:
            connection.close()    