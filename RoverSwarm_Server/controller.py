import socket
import json
import asyncio
import math
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import threading

##create tcp socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##address found from the ip seen on phone hotspot
server_address = ('0.0.0.0', 50)
print('starting up on ', server_address)
sock.bind(server_address)
sock.listen(10)

x_end = 700
y_end = 700

## Initialised position
obstx = [0, 0] #In the order of L R
obsty = [0, 0]
posx = [0, 0]
posy = [0, 0]

def findObstaclePosition(obst, yaw):
    ##The ToF sensors are in a 45 degree angle so the cartesian coordinates are calculated below
    ## L = 0.1*distance (in cm)*cos(45+yaw), 0.1*distance*sin(45+yaw)
    obstx.append(posx[-1]+0.1*obst[0]*math.cos((3*math.pi/4)+yaw))
    obsty.append(posy[-2]+0.1*obst[0]*math.sin((3*math.pi/4+yaw)))
    obstx.append(posx[-1]+0.1*obst[0]*math.cos((math.pi/4)+yaw))
    obsty.append(posy[-2]+0.1*obst[0]*math.sin((math.pi/4+yaw)))

def findPosition(segment, yaw):
    posx.append(posx[-1]+segment*math.sin(yaw))
    posy.append(posy[-1]+segment*math.cos(yaw))

def updatePlots(frame):
    global graph, posgraph
    graph.set_offsets(list(zip(obstx,obsty)))
    posgraph.set_offsets(list(zip(posx, posy)))
    pt1 = (posx[-2], posy[-2])
    pt2 = (posx[-1],posy[-1])
    dist2pts = math.dist(pt1, pt2)

    if(dist2pts > 1 and dist2pts < 100):
        ax.plot([pt1[0], pt2[0]], [pt1[1], pt2[1]], c='r')
    


def receive_data():
    prevTravelled = 0

    while True:
        print('waiting for connection')
        connection, client_address = sock.accept()
        print('connection from', client_address)  
        try:  
            with connection.makefile('r') as f:
                for line in f:
                    try:
                        line = line.strip()
                        data = json.loads(line)
                        print("Parsed JSON:", data)
                        yaw = 15.7*data["yaw"]
                        segment = data["travelled"]-prevTravelled
                        prevTravelled = data["travelled"]
                        findPosition(segment, yaw)
                        findObstaclePosition(data["distance"], yaw)
                        updatePlots(frame=None)
                    except json.JSONDecodeError as e:
                        print("JSON Decode Error:", e)
        finally:
            connection.close()    

thread = threading.Thread(target=receive_data, daemon=True)
thread.start()

fig, ax = plt.subplots()
plt.xlim(-x_end, x_end)
plt.ylim(-y_end, y_end)
graph = ax.scatter(obstx, obsty, marker='.', s=1, c='blue')
posgraph = ax.scatter(posx, posy, marker='.', s=1, c='red')
anim = FuncAnimation(fig, updatePlots, frames=None)
plt.show()    

