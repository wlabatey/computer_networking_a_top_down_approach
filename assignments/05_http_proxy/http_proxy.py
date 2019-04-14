#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# A simple http proxy with caching.

import argparse
import re
import socket
import threading

BUFF_SIZE = 4096
SERVER_IP = "127.0.0.1"
SERVER_PORT = 8081
CACHE_DIR = "/tmp/python_proxy/"
HTTP_VERSION = "HTTP/1.1"

parser = argparse.ArgumentParser(
            description="A simple http proxy with caching.")
parser.add_argument("server_ip", nargs="?", default=SERVER_IP,
                    help="The address to listen on. Defaults to 127.0.0.1")
parser.add_argument("server_port", nargs="?", type=int, default=SERVER_PORT,
                    help="The port to bind to. Defaults to 8081.")
args = parser.parse_args()


def read_from_cache(sock, filename):
    # Check whether the file exists in the cache
    try:
        with open(CACHE_DIR + filename, "rb") as f:
            data = f.read()

        sock.send(data)

        print("Read {} from cache".format(filename))
    # Make it explicit that we are catching IOError exceptions
    except IOError:
        raise


def request_from_server(cli_sock, method, host, port, resource, filename):
    # Create a socket on the proxyserver
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s_sock:
        s_sock.connect((host, port))
        proxy_request = (method + " " + resource + " " + HTTP_VERSION + "\n"
                         "Host: " + host + "\n"
                         "User-Agent: Python Proxy 0.1.0\n"
                         "Accept: */*\r\n\n")
        s_sock.sendall(proxy_request.encode("utf-8"))

        response = bytearray()
        while True:
            part = s_sock.recv(BUFF_SIZE)
            response += part
            if not part:
                break

        cli_sock.sendall(response)

    # Create a new file in the cache for the requested file.
    # Also send the response in the buffer to client socket and the corresponding file in the cache
    if resource == "/":
        resource = "/index.html"

    with open(CACHE_DIR + filename, "wb") as tmp_file:
        tmp_file.write(response)

    print("Cached {}{} to {}".format(host, resource, CACHE_DIR + filename))


def serve(sock, addr):

    print("\nReceived a request from: {}".format(addr))

    message = sock.recv(BUFF_SIZE).decode()

    request = message.split("\r\n")[0]
    method = request.split()[0]
    resource = request.split()[1]

    print("request: {}".format(request))

    reg = re.compile(r"(https?)*(?:://)*([^/\r\n:]+)(?::?)([0-9]*)(/[^:\r\n]*)?")
    matches = reg.findall(resource)

    host = matches[0][1]

    if matches[0][2]:
        port = int(matches[0][2])
    else:
        port = 80

    if matches[0][3]:
        resource = matches[0][3]
    else:
        resource = "/"

    filename = method.lower() + "_" + host + resource.replace("/", "_")

    try:
        read_from_cache(sock, filename)
    # Error handling for file not found in cache
    except IOError:
        request_from_server(sock, method, host, port, resource, filename)

    # Close the client and the server sockets
    sock.close()


# Create a server socket, bind it to a port and start listening
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((args.server_ip, args.server_port))
    s.listen(1)
    print("HTTP Proxy listening on {}:{}".format(args.server_ip, args.server_port))

    while True:
        cli_sock, addr = s.accept()
        threading.Thread(target=serve, args=(cli_sock, addr)).start()
