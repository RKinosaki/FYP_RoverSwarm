from sklearn import linear_model
from sklearn.linear_model import RANSACRegressor
from sklearn.neighbors import NearestNeighbors
import matplotlib.pyplot as plt
import numpy as np
import math


"""
def LinReg(obstx_L, obsty_L, obstx_R, obsty_R): 
    threshold = 6
    angle = math.pi/4
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
                plt.plot(arrX_L, line_y_L, label = "Left Wall"+str(i), color='blue')
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
    plt.title("Mapping data using linear regression")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.xlim(-x_end, x_end)
    plt.ylim(-y_end, y_end)
    plt.legend(loc = "lower right")
    plt.show()

def RANSAC(obstx_L, obsty_L, obstx_R, obsty_R): 
    threshold = 6
    angle = math.pi/4
    lowangle = math.tan(angle)
    highangle = math.tan((math.pi/2) - angle)
    for i in range(len(obstx_L)):
        if(len(obstx_L[i])>threshold or len(obsty_L[i])>threshold):
            # print(obstx_L[i][2:])
            # print(obsty_L[i][2:])
            arrX_L = np.array(obstx_L[i][2:])
            arrY_L = np.array(obsty_L[i][2:])
            R_L = RANSACRegressor()
            R_L.fit(arrX_L.reshape(-1, 1), arrY_L.reshape(-1, 1))
            line_y_L = R_L.predict(arrX_L.reshape(-1, 1))
            plt.plot(arrX_L, line_y_L, label="Left Wall" + str(i), color='blue')
    for i in range(len(obstx_R)):
        if(len(obstx_R[i])>threshold or len(obsty_R[i])>threshold):
            # print(obstx_R[i][2:])
            # print(obsty_R[i][2:])
            arrX_R = np.array(obstx_R[i][2:])
            arrY_R = np.array(obsty_R[i][2:])
            R_R = RANSACRegressor()
            R_R.fit(arrX_R.reshape(-1, 1), arrY_R.reshape(-1, 1))
            line_y_R = R_R.predict(arrX_R.reshape(-1, 1))
            plt.plot(arrX_R, line_y_R, label="Right Wall" + str(i), color='red')
    plt.title("Mapping data using linear RANSAC")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.xlim(-x_end, x_end)
    plt.ylim(-y_end, y_end)
    plt.legend(loc = "lower right")
    plt.show()
"""

def filterLonePoints(points, radius, min_neighbours):
    neigh = NearestNeighbors(radius=radius)
    neigh.fit(points)
    neighbours = neigh.radius_neighbors(points, return_distance=False)

    inliers = [pt for i, pt in enumerate(points) if len(neighbours[i]) >= min_neighbours]
    return np.array(inliers)


def removePointsInPath(points, path, radius):
    filteredPoints = []
    for pts in points:
        inPath = False
        for p in path:
            if(math.dist(p, pts) <= radius):
                inPath = True
                break
        if not inPath:
            filteredPoints.append(pts)
    return np.array(filteredPoints)

def showpointcloud(R, bound):
    fig, ax = plt.subplots()
    plt.xlim(-bound[0], bound[0])
    plt.ylim(-bound[1], bound[1])
    allgraph=[]
    # for rov in R:
    rov = R
    wall_L = ax.scatter(rov.obstL[:, 0], rov.obstL[:, 1], marker='.', s=1, label = "left side", c='blue')
    wall_R = ax.scatter(rov.obstR[:, 0], rov.obstR[:, 1], marker = '.', s=1, label = "right side", c='green') 
    posgraph = ax.plot(rov.pos[:, 0], rov.pos[:, 1], label='path', c=rov.pathColor)
    allgraph.append([wall_L, wall_R, posgraph])
    plt.title("Raw data of the time of flight sensor")
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend(loc="lower right")
    plt.show()
    input("Press enter to continue...")

def showFilteredPointCloud(R, bound):
    allgraph=[]
    fig, ax = plt.subplots()
    plt.xlim(-bound[0], bound[0])
    plt.ylim(-bound[1], bound[1])
    # for rov in R:
    rov = R
    allPoints = np.vstack((rov.obstL, rov.obstR))
    allPoints = filterLonePoints(allPoints, 50, 5) ##Neighbourhood search
    allPoints = removePointsInPath(allPoints, rov.pos, 20) ##Removes any point in path
    try:
        wall = ax.scatter(allPoints[:, 0], allPoints[:, 1], marker='.', s=1, label = "Rover "+str(rov.id), c='blue')
        posgraph = ax.plot(rov.pos[:, 0], rov.pos[:, 1], label='path', c=rov.pathColor)
        allgraph.append([wall, posgraph])
    except IndexError as e:
        print("Error Indexing: ", e)
    plt.title("Filtered data of the time of flight sensor")
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.legend(loc="lower right")
    plt.show()
    input("Press enter to continue...")
    return allPoints


def createGrid(allPoints):
    resolution = 10
    try:
        x_min, x_max, y_min, y_max = allPoints[:, 0].min(), allPoints[:, 0].max(), allPoints[:, 1].min(), allPoints[:, 1].max()
    except IndexError as e:
        print("Index Error: ", e)
    try:
        x_size = int(np.ceil((x_max-x_min)/resolution))
        y_size = int(np.ceil((y_max-y_min)/resolution))
        grid = np.zeros((y_size, x_size))
        for x, y in allPoints:
            gx = int((x - x_min)/resolution)
            gy = int((y - y_min)/resolution)
            grid[gy, gx] = 1

        
        plt.imshow(grid[::-1], cmap='binary', extent=[x_min, x_max, y_min, y_max])
        plt.title("Filtered data of time of flight data")
        plt.xlabel('X')
        plt.ylabel('Y')
        plt.grid(True)
        plt.show()
        input("Press enter to continue...")
    except UnboundLocalError as e:
        print("No value!")
    return grid