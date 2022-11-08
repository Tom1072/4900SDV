# This file serves a server that sends the user input to QNX via UDP
# The data from here is to be directed to QNX server control system

import socket
import os
import sys

# Need to know the function name which will transfer data from the user
def getSensorReadings():
    # receive the readings as a tuple from the display
    # sensor_data = (acc_state, abs_state, speed, distance, friction, etc)
    sensor_data = (1,2,3,4,5,6)
    return sensor_data

def send_sensor_readings(hostn, portn):
    # Assign IP address and port number to socket
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    buffer = 2048
    message = str(getSensorReadings())
    qnx_server = "192.168.2.211"
    # Create a message with a time sent
    # Send message to the server
    clientSocket.sendto(message.encode(), (hostn, portn))
    # sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))

def main():
    print("Please, use Python 3.7 to run this code.")
    if len(sys.argv) > 2:
        hostn = str(sys.argv[1])
        try:
            portn = int(sys.argv[2])
        except Exception as e:
            portn = 5001
        while(True):
            send_sensor_readings(hostn, portn)
    else:
        print("\n***Usage: qnx_vm_ip port_number\n")
        send_sensor_readings("127.0.0.1", 12000)


if __name__ == '__main__':
    main()

'''
UDP_IP = "192.168.2.211"
UDP_PORT = 5001
MESSAGE = "Hello, World! Please work!!!!!!!"

print("UDP target IP:", UDP_IP)
print("UDP target port:", UDP_PORT)
print("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))
'''