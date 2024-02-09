import socket
import threading

# Server function
def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('192.168.1.2', 0))
    server_socket.listen(1)
    print("Server started, waiting for connections...")
    conn, addr = server_socket.accept()
    print(f"Connected to {addr}")

    while True:
        data = conn.recv(1024)
        if not data:
            break
        print("Received from client:", data.decode())
        conn.sendall(b"Data received from server")

    conn.close()
    server_socket.close()

# Client function
def start_client():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('localhost', 8080))

    while True:
        message = input("Enter message to send: ")
        if message.lower() == 'exit':
            break
        client_socket.sendall(message.encode())
        data = client_socket.recv(1024)
        print("Server response:", data.decode())

    client_socket.close()

# Start both server and client in separate threads
server_thread = threading.Thread(target=start_server)
client_thread = threading.Thread(target=start_client)

server_thread.start()
client_thread.start()

# Wait for both threads to finish
server_thread.join()
client_thread.join()
