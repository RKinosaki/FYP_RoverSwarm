import websocket as web
import threading
import visualiser as v
import dataProcessing as dProc
import math

bound = [300, 300]

sock = web.startTCP()
R1 = web.roverData(1, 150, -150, 0, 'red')
R2 = web.roverData(2, -150, 150, 0, 'black')
R = [R1, R2]
graphs, axFig = v.initVisualiser(R, bound)

thread = threading.Thread(target=web.receiveData, args=(sock, R, graphs, axFig), daemon=True)
thread.start()

try:
    while True:
        with web.dataLock:
            if web.newData:
                v.updatePlots(R[web.RovID], graphs[web.RovID], axFig=axFig)
                web.newData = False
except KeyboardInterrupt:
    print("Shutting down...")


dProc.showpointcloud(R[0], bound)
allPoints = dProc.showFilteredPointCloud(R[0], bound)
gridPoints = dProc.createGrid(allPoints)
gridPoints = dProc.createFilteredGrid(gridPoints)

# LinReg(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  
