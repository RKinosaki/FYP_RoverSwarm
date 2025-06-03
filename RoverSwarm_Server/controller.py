import socket
import json
import asyncio
import math

##create tcp socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##address found from the ip seen on phone hotspot
server_address = ('0.0.0.0', 50)
print('starting up on ', server_address)
sock.bind(server_address)
sock.listen(10)

def findObstaclePosition(obst):
    obstL = [-obst[0]/math.sqrt(2), obst[0]/math.sqrt(2)]
    obstR = [obst[1]/math.sqrt(2), obst[1]/math.sqrt(2)]
    return [obstL, obstR]

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
                    distance = findObstaclePosition(data["distance"])
                    print("Obstacle position: ", distance[0], ", ", distance[1])
                except json.JSONDecodeError as e:
                    print("JSON Decode Error:", e)
    finally:
        connection.close()        

