import socket
import json
import math
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
import threading
from sklearn import linear_model
from sklearn.neighbors import NearestNeighbors

##create tcp socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##address found from the ip seen on phone hotspot
server_address = ('0.0.0.0', 50)
print("starting up on: ", server_address)
sock.bind(server_address)
sock.listen(10)

x_end = 2000
y_end = 1000

## Initialised position
obstx_L = [[0, 0]]
obstx_R = [[0, 0]]
obsty_L = [[0, 0]]
obsty_R = [[0, 0]]
posx = [0, 0]
posy = [0, 0]

def checkquadrant(yaw, margin):
    while(yaw<(-3*math.pi/4)):
        yaw = yaw + 2*math.pi
    while yaw > (3*math.pi/4):
        yaw = yaw- 2*math.pi
    if(yaw<(math.pi/4)+margin and yaw > -(math.pi/4)-margin):
        return 0
    elif (yaw<-(math.pi/4)-margin and yaw > -(3*math.pi/4)-margin):
        return 1
    elif(yaw>(math.pi/4)+margin and yaw<(3*math.pi/4+margin)):
        return 3
    elif(yaw>(3*math.pi)/4 or yaw<(-3*math.pi)/4):
        return 2

def findObstaclePosition(obst, yaw):
    offset_left = 140
    offset_right = 50
    ##The ToF sensors are in a 45 degree angle so the cartesian coordinates are calculated below
    ## L = 0.1*distance (in cm)*cos(45+yaw), 0.1*distance*sin(45+yaw)
    obstx_L[-1].append(posx[-1]+1*obst[0]*math.cos(((offset_left*math.pi)/180)-yaw))
    obsty_L[-1].append(posy[-2]+1*obst[0]*math.sin(((offset_left*math.pi)/180)-yaw))
    obstx_R[-1].append(posx[-1]+1*obst[1]*math.cos(((offset_right*math.pi)/180)-yaw))
    obsty_R[-1].append(posy[-2]+1*obst[1]*math.sin(((offset_right*math.pi)/180)-yaw))

def findPosition(segment, yaw):
    posx.append(posx[-1]+segment*math.sin(yaw))
    posy.append(posy[-1]+segment*math.cos(yaw))

def updatePlots(frame):
    global wall_R, wall_L, posgraph
    posgraph.set_offsets(list(zip(posx, posy)))
    filterOutliers((obstx_L[-1][-1], obstx_L[-1][-1]), (obstx_L[-1][-2], obsty_L[-1][-2]), "L")
    filterOutliers((obstx_R[-1][-1], obsty_R[-1][-1]), (obstx_R[-1][-2], obsty_R[-1][-2]), "R")
    ax.plot([posx[-2], posx[-1]], [posy[-2], posy[-1]], c='r')

def filterOutliers(pt_i, pt_i_min_1, side):
    # dist2pts = math.dist(pt_i, pt_i_min_1)
    # if(dist2pts > 1 and dist2pts < 250):
    if(side == "L"):
        wall_L.set_offsets(list(zip(obstx_L[-1], obsty_L[-1])))
    elif(side=="R"):
        wall_R.set_offsets(list(zip(obstx_R[-1], obsty_R[-1])))

def sendCommand(connection, char):
    if connection:
        try:
            connection.sendall(char.encode())
            print(f"Sent Command: {char}")
        except Exception as e:
            print(f"Error: {e}")
    else:
        print("No rover connected")
        
def RANSAC(obstx_L, obsty_L, obstx_R, obsty_R): 
    threshold = 6
    angle = math.pi/6
    lowangle = math.tan(angle)
    highangle = math.tan((math.pi/2) - angle)
    for i in range(len(obstx_L)):
        if(len(obstx_L[i])>threshold or len(obsty_L[i])>threshold):
            # print(obstx_L[i][2:])
            # print(obsty_L[i][2:])
            arrX_L = np.array(obstx_L[i][2:])
            arrY_L = np.array(obsty_L[i][2:])
            lr_L = linear_model.LinearRegression()
            lr_L.fit(arrX_L.reshape(-1, 1), arrY_L.reshape(-1, 1))
            if((lr_L.coef_[0]<lowangle and lr_L.coef_[0]>-lowangle) or lr_L.coef_[0] > highangle or lr_L.coef_[0]<-highangle):
                print(lr_L.coef_[0])
                line_y_L = lr_L.predict(arrX_L.reshape(-1, 1))
                plt.plot(arrX_L, line_y_L, label = "Left Wall"+str(i), color='red')
            else:
                line_y_L = lr_L.predict(arrX_L.reshape(-1, 1))
                plt.plot(arrX_L, line_y_L, label="Left Wall" + str(i), color='blue')
    for i in range(len(obstx_R)):
        if(len(obstx_R[i])>threshold or len(obsty_R[i])>threshold):
            # print(obstx_R[i][2:])
            # print(obsty_R[i][2:])
            arrX_R = np.array(obstx_R[i][2:])
            arrY_R = np.array(obsty_R[i][2:])
            lr_R = linear_model.LinearRegression()
            lr_R.fit(arrX_R.reshape(-1, 1), arrY_R.reshape(-1, 1))
            if((lr_R.coef_[0]<lowangle and lr_R.coef_[0]>-lowangle) or lr_R.coef_[0] > highangle or lr_R.coef_[0]<-highangle):
                print(lr_R.coef_[0])
                line_y_R = lr_R.predict(arrX_R.reshape(-1, 1))
                plt.plot(arrX_R, line_y_R, label="Right Wall" + str(i), color='red')
            else:
                line_y_R = lr_R.predict(arrX_R.reshape(-1, 1))
                plt.plot(arrX_R, line_y_R, label="Right Wall" + str(i), color='blue')
    plt.title("Mapping data using linear regression")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.xlim(-x_end, x_end)
    plt.ylim(-y_end, y_end)
    plt.legend(loc = "lower right")
    plt.show()

def filterPoints(points, radius, min_neighbours):
    neigh = NearestNeighbors(radius=radius)
    neigh.fit(points)
    neighbours = neigh.radius_neighbors(points, return_distance=False)

    inliers = [pt for i, pt in enumerate(points) if len(neighbours[i]) >= min_neighbours]
    return np.array(inliers)

def showpointcloud(obstx_L, obsty_L, obstx_R, obsty_R, posx, posy):
    fig, ax = plt.subplots()
    plt.xlim(-x_end, x_end)
    plt.ylim(-y_end, y_end)
    wall_L = ax.scatter(obstx_L[-1], obsty_L[-1], marker='.', s=1, label = "left side", c='blue')
    wall_R = ax.scatter(obstx_R[-1], obsty_R[-1], marker = '.', s=1, label = "right side", c='green') 

    ax.plot(posx, posy, label='path', c='red')
    pointLx = []
    pointLy = []
    pointRx = []
    pointRy = []
    for i in range(len(obstx_L)):
        for point in obstx_L[i]:
            pointLx.append(point)
    for i in range(len(obsty_L)):
        for point in obsty_L[i]:
            pointLy.append(point)
    for i in range(len(obstx_R)):
        for point in obstx_R[i]:
            pointRx.append(point)
    for i in range(len(obsty_R)):
        for point in obsty_R[i]:
            pointRy.append(point)
    wall_L.set_offsets(list(zip(pointLx, pointLy)))
    wall_R.set_offsets(list(zip(pointRx, pointRy)))
    plt.title("Raw data of the time of flight sensor")
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend(loc="lower right")
    plt.show()


def createGrid(obstx_L, obsty_L, obstx_R, obsty_R):
    resolution = 5
    points = []
    for i in range(len(obstx_L)):
        for x, y in zip(obstx_L[i], obsty_L[i]):
            points.append((x, y))
    for i in range(len(obsty_R)):
        for x, y in zip(obstx_R[i], obsty_R[i]):
            points.append((x, y))
    points = np.array(points)
    points = filterPoints(points, 40, 5)
    x_min, x_max, y_min, y_max = points[:, 0].min(), points[:, 0].max(), points[:, 1].min(), points[:, 1].max()
    x_size = int(np.ceil((x_max-x_min)/resolution))
    y_size = int(np.ceil((y_max-y_min)/resolution))
    grid = np.zeros((y_size, x_size))
    

    for x, y in points:
        gx = int((x - x_min)/resolution)
        gy = int((y - y_min)/resolution)
        grid[gy, gx] = 1
    print("Occupied cells:", np.count_nonzero(grid))

    
    plt.imshow(grid[::-1], cmap='binary', extent=[x_min, x_max, y_min, y_max])
    plt.title("Filtered data of time of flight data")
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.show()

    

def receiveData():
    quadrant = 0    
    prevQuadrant = 0
    prevTravelled = 0
    yaw_scaler = 15.7
    startFlag = True
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
                        yaw = yaw_scaler*data["yaw"]
                        prevQuadrant = quadrant
                        quadrant = checkquadrant(yaw, 0.15)
                        if prevQuadrant != quadrant:
                            obstx_L.append([0, 0])
                            obsty_L.append([0, 0])
                            obstx_R.append([0, 0])
                            obsty_R.append([0, 0])
                        travelled = (data["encoder"][0]+data["encoder"][1])/2
                        segment = travelled-prevTravelled
                        prevTravelled = travelled
                        findPosition(segment, yaw)
                        findObstaclePosition(data["distance"], yaw)
                        updatePlots(frame=None)
                    except json.JSONDecodeError as e:
                        print("JSON Decode Error:", e)
        finally:
            connection.close()    


thread = threading.Thread(target=receiveData, daemon=True)
thread.start()

fig, ax = plt.subplots()
plt.xlim(-x_end, x_end)
plt.ylim(-y_end, y_end)
wall_L = ax.scatter(obstx_L[-1], obsty_L[-1], marker='.', s=1, c='blue')
wall_R = ax.scatter(obstx_R[-1], obsty_R[-1], marker = '.', s=1, c='green')
posgraph = ax.scatter(posx, posy, marker='.', s=1, c='red')
anim = FuncAnimation(fig, updatePlots, frames=None)
plt.show()  
showpointcloud(obstx_L, obsty_L, obstx_R, obsty_R, posx, posy)
createGrid(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  

