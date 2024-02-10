import socket

def find_open_port(device_ip, start_port=1, end_port=65535):
    open_ports = []
    for port in range(start_port, end_port+1):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1)  # Adjust timeout as needed
        result = sock.connect_ex((device_ip, port))
        if result == 0:
            print(f"Port {port} is open")
            open_ports.append(port)
        sock.close()
    return open_ports

device_ip = "192.168.1.1"
open_ports = find_open_port(device_ip)
print("Open ports:", open_ports)