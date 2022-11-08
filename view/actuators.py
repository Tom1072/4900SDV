# This file serves a server that listens to QNX data via UDP
# The data from here is to be directed to display

import socket

# Create UDP socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(('', 12000))

while True:

    # Receive the client packet along with the address it is coming from
    message, address = serverSocket.recvfrom(1024)

    # Read the data from the message into tuple
    # data = (state_acc, speed, distance)
    data = message.decode()
    print(data)
    # Send the data to display
    # display(data) - a function from the display pygame


