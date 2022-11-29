import socket
import json

UDP_IP = "192.168.56.1"  # Tom's IP
# UDP_IP = "192.168.56.101"
# UDP_IP = "192.168.2.226" # Kate's IP
UDP_PORT = 8080

# Write a python socket server that listens on UDP_PORT 5004 and UDP_IP = "192.168.56.101" and print the response


class ViewListener:
    def __init__(self):
        print(f"Starting ViewListener on {UDP_IP}:{UDP_PORT}")
        self.sock = socket.socket(
            family=socket.AF_INET, type=socket.SOCK_DGRAM, proto=socket.IPPROTO_UDP)
        self.sock.bind((UDP_IP, UDP_PORT))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.sock.close()

    def start_loop(self):
        while (True):
            bytes_address_pair = self.sock.recvfrom(1024)
            message = bytes_address_pair[0]
            address = bytes_address_pair[1]

            if (message == b'stop'):
                print("Stopping ViewListener")
                break

            data = json.loads(message)
            self.display_data(data)

            self.sock.sendto(bytes("Hello from server", "utf-8"), address)

    def display_data(self, data):
        print("VIEW: throttle={}, brake=({}, {}), speed={:.2f}, distance={:.2f}, obj=({}, {:.2f})\n".format(
            data["throttle"],
            data["brake"], data["skid"],
            data["speed"],
            min(data["distance"], 9999.99),
            data["obj"], min(data["obj-speed"], 9999.99)))


if __name__ == "__main__":
    with ViewListener() as listener:
        listener.start_loop()
