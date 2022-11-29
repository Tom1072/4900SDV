import socket

#UDP_IP = "192.168.56.101"
#UDP_IP = "192.168.2.226"
UDP_PORT = 5003
BUFFER_SIZE = 1024


class ViewDispatcher:
  def __init__(self):
    print("UDP target IP:", UDP_IP)
    print("UDP target port:", UDP_PORT)
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_val, exc_tb):
    self.sock.close()

  def send_message(self, message):
    self.sock.sendto(bytes(message, "utf-8"), (UDP_IP, UDP_PORT))

  def receive_message(self):
    data, addr = self.sock.recvfrom(BUFFER_SIZE)
    return data.decode(), addr

  def start_loop(self):
    while True:
      message = input("Enter command: ")
      self.send_message(message)
      data, addr = self.receive_message()
      print(f"Received response: \'{data}\' from {addr}\n")
      if (message == "stop"):
        break


if __name__ == "__main__":
  with ViewDispatcher() as dispatcher:
    dispatcher.start_loop()
