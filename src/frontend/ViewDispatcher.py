import socket

BUFFER_SIZE = 1024
UDP_PORT = 5003


class ViewDispatcher:
  def __init__(self, udp_ip):
    print("UDP target IP:", udp_ip)
    print("UDP target port:", UDP_PORT)
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    self.udp_ip = udp_ip

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_val, exc_tb):
    self.sock.close()

  def send_message(self, message):
    self.sock.sendto(bytes(message, "utf-8"), (self.udp_ip, UDP_PORT))

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
