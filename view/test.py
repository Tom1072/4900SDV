import socket


UDP_IP = "172.16.41.129"
UDP_PORT = 5001
MESSAGE = "Hello, World! Please work!!!!!!!"

print("UDP target IP:", UDP_IP)
print("UDP target port:", UDP_PORT)
print("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP, UDP_PORT))