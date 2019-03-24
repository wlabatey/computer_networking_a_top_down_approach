#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# A very simple, threaded http web server

import socket
import threading


HOST = '127.0.0.1'
PORT = 8080

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Prepare a sever socket
serverSocket.bind((HOST, PORT))
serverSocket.listen(4)

def serve(con_socket):
    try:
        message = con_socket.recv(2048)

        filename = message.split()[1].decode()
        filename = "index.html" if filename == "/" else filename[1:]
        print("filename: {}".format(filename))

        with open(filename, "r") as f:
            outputdata = f.read()

        response = ("HTTP/1.1 200 OK\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        con_socket.send(response.encode())

        for i in range(0, len(outputdata)):
            con_socket.send(outputdata[i].encode())

        con_socket.send("\r\n".encode())
        con_socket.close()

    except IOError:
        # Send response message for file not found
        response = ("HTTP/1.1 404 Not Found\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        con_socket.send(response.encode())
        con_socket.send("\r\n".encode())
        con_socket.close()


while True:
    # Establish the connection
    print('Ready to serve...')
    connectionSocket, _ = serverSocket.accept()
    threading.Thread(target=serve, args=[connectionSocket]).start()
