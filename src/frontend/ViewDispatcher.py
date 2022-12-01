import socket

BUFFER_SIZE = 1024
DISPATCHER_UDP_PORT = 8080


class ViewDispatcher:
  """ Dispatch user commands to the backend """

  def __init__(self, udp_ip, udp_port=DISPATCHER_UDP_PORT):
    """ Initialize the dispatcher

    Args:
        udp_ip (_type_): the ip address of the backend
        udp_port (_type_, optional): the port of the backend. Defaults to DISPATCHER_UDP_PORT.
    """
    print(f"Starting ViewDispatcher on {udp_ip}:{udp_port}")
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.udp_ip = udp_ip
    self.udp_port = udp_port

  def __enter__(self):
    return self

  def __exit__(self, exc_type, exc_val, exc_tb):
    self.sock.close()

  def send_message(self, message):
    """ Send a message to the backend

    Args:
        message (_type_): the message to send
    """
    self.sock.sendto(bytes(message, "utf-8"), (self.udp_ip, self.udp_port))

  def receive_message(self):
    """ Receive a message from the backend

    Returns:
        message, server_address: the message and the server address
    """
    data, addr = self.sock.recvfrom(BUFFER_SIZE)
    return data.decode(), addr

  def start_loop(self):
    """ Start the loop to receive messages from stdin and send to the backend """

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
