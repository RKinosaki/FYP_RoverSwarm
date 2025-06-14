import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from functools import partial

def initVisualiser(roverClass, bound=[300, 300]):
    plt.ion()
    fig, ax = plt.subplots()
    allGraphs = []
    axFig = [fig, ax]
    for rov in roverClass:
        plt.xlim(-bound[0], bound[0])
        plt.ylim(-bound[1], bound[1])
        try:
            wallL = ax.scatter(rov.obstL[:,0], rov.obstL[:,1], marker = '.', s=1, c='blue')
            wallR = ax.scatter(rov.obstR[:, 0], rov.obstR[:,1], marker = '.', s=1, c='green')
            posgraph = ax.scatter(rov.pos[:, 0], rov.pos[:, 1], marker='.', s=100, c='red')
        except ValueError:
            print("Data not found!")
        graphAttr = [wallL, wallR, posgraph]
        allGraphs.append(graphAttr)
    plt.show()

    return allGraphs, axFig

def updatePlots(roverClass, graphs, axFig):
    fig, ax = axFig[0], axFig[1]
    ax.plot(roverClass.pos[-2:-1, 0], roverClass.pos[-2:-1, 1], marker='.', c='red')
    graphs[2].set_offsets(roverClass.pos)
    graphs[1].set_offsets(roverClass.obstR[-1]) 
    graphs[0].set_offsets(roverClass.obstL[-1])
    fig.canvas.draw_idle()
    fig.canvas.flush_events()

    