import socket
import json
import select

UDP_PORT = 8080

class ViewListener:
    def __init__(self, udp_ip):
        print(f"Starting ViewListener on {udp_ip}:{UDP_PORT}")
        self.sock = socket.socket(
            family=socket.AF_INET, type=socket.SOCK_DGRAM, proto=socket.IPPROTO_UDP)
        self.sock.bind((udp_ip, UDP_PORT))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.sock.close()

    def start_loop(self):
        while True:
            bytes_address_pair = self.sock.recvfrom(1024)
            message = bytes_address_pair[0]
            address = bytes_address_pair[1]

            if (message == b'stop'):
                print("Stopping ViewListener")
                break

            data = json.loads(message)
            self.display_data(data)

            self.sock.sendto(bytes("Hello from server", "utf-8"), address)

    def listen(self):
        ready = select.select([self.sock], [], [], 0)
        if ready[0]:
            bytes_address_pair = self.sock.recvfrom(1024)
            message = bytes_address_pair[0]
            address = bytes_address_pair[1]

            if (message == b'stop'):
                print("Stopping ViewListener")
                return "stop"

            data = json.loads(message)
            self.sock.sendto(bytes("Hello from server", "utf-8"), address)

            return data

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
