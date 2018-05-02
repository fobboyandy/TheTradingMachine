import sys
import socket                                         
import time

# create a socket object
serversocket = socket.socket(
	        socket.AF_INET, socket.SOCK_STREAM) 

# get local machine name
host = "localhost"                           

port = 27015                                           

# bind to the port
serversocket.bind((host, port))                                  

# queue up to 5 requests
serversocket.listen(5)       

# establish a connection
print ("Block here until connection.");
clientsocket,addr = serversocket.accept()      
print("Got a connection from %s" % str(addr))


i = 0
while(True):
    tm = clientsocket.recv(1024)     
    if(len(tm) != 0):
        print (tm.decode('ascii'))
    else:
        break;
    
clientsocket.close()