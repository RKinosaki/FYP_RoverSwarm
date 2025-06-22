import websocket as web
import threading
import visualiser as v
import dataProcessing as dProc
import math

bound = [400, 400]

sock = web.startTCP()
R1 = web.roverData(1, -300, 300, math.pi, 'red') #ID, startx, starty, yaw_0, path color
R2 = web.roverData(2, 0, -300, math.pi/2, 'black')
R3 = web.roverData(3, 300, 250, -math.pi/2, 'blue')
R = [R1, R2, R3]
web.newDataFlags = [False, False, False]
graphs, axFig = v.initVisualiser(R, bound)

clientThread = threading.Thread(target=web.receiveData, args=(sock, R), daemon=True)
clientThread.start()

inputThread = threading.Thread(target=web.monitor_input, daemon=True)
inputThread.start()



while web.running:
    with web.dataLock:
        for i in range(len(R)):
            if web.newDataFlags[i]:
                v.updatePlots(R[i], graphs[i], axFig=axFig)
                web.newDataFlags[i] = False
clientThread.join()
inputThread.join()
sock.close()
web.closeAllConnections()





dProc.showpointcloud(R, bound)
allPoints = dProc.showFilteredPointCloud(R, bound)
print(allPoints)
gridPoints = dProc.createGrid(allPoints)
gridPoints = dProc.createFilteredGrid(gridPoints)

# LinReg(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  
