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

device_ip = "your_device_ip_here"
port = 49780
check_port(device_ip, port)