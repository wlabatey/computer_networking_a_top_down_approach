#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# A very simple cli http client. Doesn't handle redirects or TLS.

import argparse
import socket


parser = argparse.ArgumentParser(description="Simple HTTP CLI")
parser.add_argument("host", nargs="?", default="127.0.0.1", help="The host to connect to")
parser.add_argument("port", nargs="?", default=80, type=int, help="The host port to connect to")
parser.add_argument("filename", nargs="?", default="index.html", help="The file on the host to retrieve")
args = parser.parse_args()


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((args.host, args.port))
    http_req = "GET /{} HTTP/1.1".format(args.filename)
    s.sendall(http_req.encode("utf-8"))

    response = ""
    while True:
        data = s.recv(4096)
        if not data:
            break
        else:
            response += data.decode("utf-8")

    print(response)
