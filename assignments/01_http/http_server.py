#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# A very simple, single threaded http web server

import socket
import sys


HOST = "127.0.0.1"
PORT = 8080

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Prepare a sever socket
serverSocket.bind((HOST, PORT))
serverSocket.listen(1)

while True:
    # Establish the connection
    print("Ready to serve...")
    connectionSocket, addr = serverSocket.accept()

    try:
        message = connectionSocket.recv(2048)

        filename = message.split()[1].decode()
        filename = "index.html" if filename == "/" else filename[1:]
        print("filename: {}".format(filename))

        with open(filename, "r") as f:
            outputdata = f.read()

        response = ("HTTP/1.1 200 OK\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        connectionSocket.send(response.encode())

        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())

        connectionSocket.send("\r\n".encode())
        connectionSocket.close()

    except IOError:
        # Send response message for file not found
        response = ("HTTP/1.1 404 Not Found\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        connectionSocket.send(response.encode())
        connectionSocket.send("\r\n".encode())
        connectionSocket.close()

serverSocket.close()
sys.exit(0)  # Terminate the program after sending the corresponding data
