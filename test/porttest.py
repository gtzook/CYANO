import socket

def check_port(device_ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(1)  # Adjust timeout as needed
    result = sock.connect_ex((device_ip, port))
    if result == 0:
        print(f"Port {port} is open")
    else:
        print(f"Port {port} is closed")
    sock.close()

device_ip = "192.168.1.1"
port = 8888
check_port(device_ip, port)