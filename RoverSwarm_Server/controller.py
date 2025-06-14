import matplotlib.pyplot as plt
import websocket as web
import threading
import visualiser as v

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


# showpointcloud(obstx_L, obsty_L, obstx_R, obsty_R, posx, posy)
# createGrid(obstx_L, obsty_L, obstx_R, obsty_R, posx, posy)
# LinReg(obstx_L, obsty_L, obstx_R, obsty_R)
# RANSAC(obstx_L, obsty_L, obstx_R, obsty_R)  
