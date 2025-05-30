import socket

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
        while True:
                ##wasd bind is used for movement command that is sent to the rover
                data = connection.recv(1024)
                try:
                    print(data)
                    print(int(data.hex(), 16))
                except ValueError as e:
                     print(e)
    finally:
        connection.close()        

