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
x = [0]
y= [0]

def findObstaclePosition(obst, travelled):
    ##The ToF sensors are in a 45 degree angle so the cartesian coordinates are calculated below
    ##TODO: Implement addition of IMU yaw
    obstL = [travelled - obst[0]/math.sqrt(2),  travelled + obst[0]/math.sqrt(2)]
    obstR = [travelled + obst[1]/math.sqrt(2), travelled + obst[1]/math.sqrt(2)]
    return [0.1*obstL, 0.1*obstR]


def updatePlots(frame):
    global graph
    graph.set_offsets(list(zip(x,y)))


def receive_data():
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
                        distance = findObstaclePosition(data["distance"], data["travelled"])
                        print("Obstacle position: ", distance[0], ", ", distance[1])
                        x.extend([distance[0][0], distance[1][0]])
                        y.extend([distance[0][1], distance[1][1]])
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
graph = ax.scatter(x, y)
anim = FuncAnimation(fig, updatePlots, frames=None)
plt.show()    

