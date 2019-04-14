#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# A very simple, threaded http server

import argparse
import socket
import threading

SERVER_IP = "127.0.0.1"
SERVER_PORT = 8080

parser = argparse.ArgumentParser(description="A simple threaded http server")
parser.add_argument("server_ip", nargs="?", default=SERVER_IP,
                    help="The address to listen on. Defaults to 127.0.0.1")
parser.add_argument("server_port", nargs="?", default=SERVER_PORT,
                    help="The port to bind to. Defaults to 8080")
args = parser.parse_args()

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serverSocket.bind((SERVER_IP, SERVER_PORT))
serverSocket.listen(4)


def serve(sock, addr):

    print("\nReceived a request from: {}".format(addr))

    try:
        message = sock.recv(2048)

        request = message.decode().split("\r\n")[0]

        filename = request.split()[1]
        filename = "index.html" if filename == "/" else filename[1:]
        print("request: {}".format(message.decode().strip("\r\n\n")))

        with open(filename, "r") as f:
            outputdata = f.read()

        response = ("HTTP/1.1 200 OK\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        sock.send(response.encode())

        for i in range(0, len(outputdata)):
            sock.send(outputdata[i].encode())

        sock.send("\r\n".encode())
        sock.close()

    except IOError:
        # Send response message for file not found
        response = ("HTTP/1.1 404 Not Found\n"
                    "Server: Python 3.7.2\n"
                    "Content-Type: text/html; charset=utf-8\r\n\n")
        sock.send(response.encode())
        sock.send("\r\n".encode())
        sock.close()


print("HTTP server listening on {}:{}".format(args.server_ip,
                                              args.server_port))

while True:
    cli_sock, addr = serverSocket.accept()
    threading.Thread(target=serve, args=(cli_sock, addr)).start()
