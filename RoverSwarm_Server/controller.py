import websocket as web
import threading
import visualiser as v
import dataProcessing as dProc
import math

bound = [300, 300]

sock = web.startTCP()
R1 = web.roverData(1, 150, -150, 0, 'red') #ID, startx, starty, yaw_0, path color
R2 = web.roverData(2, -150, 150, math.pi/2, 'black')
R = [R1, R2]
web.newDataFlags = [False, False]
graphs, axFig = v.initVisualiser(R, bound)

thread = threading.Thread(target=web.receiveData, args=(sock, R), daemon=True)
thread.start()

try:
    while True:
        with web.dataLock:
            for i in range(len(R)):
                if web.newDataFlags[i]:
                    v.updatePlots(R[i], graphs[i], axFig=axFig)
                    web.newDataFlags[i] = False
except KeyboardInterrupt:
    print("Shutting down...")
    web.closeAllConnections()


dProc.showpointcloud(R, bound)
allPoints = dProc.showFilteredPointCloud(R, bound)
gridPoints = dProc.createGrid(allPoints)
gridPoints = dProc.createFilteredGrid(gridPoints)

# LinReg(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  
