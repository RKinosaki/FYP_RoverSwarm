import socket
import json
import asyncio

##create tcp socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##address found from the ip seen on phone hotspot
server_address = ('0.0.0.0', 50)
print('starting up on ', server_address)
sock.bind(server_address)
sock.listen(10)

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
                except json.JSONDecodeError as e:
                    print("JSON Decode Error:", e)
    finally:
        connection.close()        

