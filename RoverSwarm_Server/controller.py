import websocket as web
import threading
import visualiser as v
import dataProcessing as dProc

bound = [300, 300]

sock = web.startTCP()
R = web.initialiseData()
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
    dProc.createGrid(bound, allPoints)

# LinReg(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  
