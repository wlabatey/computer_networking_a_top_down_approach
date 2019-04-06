#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import base64
import getpass
import socket
import ssl

HOST = "smtp.gmail.com"
PORT = 465
EMAIL = "testuser01987@gmail.com"
PASSWORD = str(getpass.getpass(prompt="Enter password for {}: ".format(EMAIL)))
RECIPIENT = str(input("Enter receipient's email: "))
SUBJECT = str(input("Enter subject: "))
MESSAGE = str(input("Enter message: "))
DEBUG_MODE = True

context = ssl.create_default_context()


class Smtp:
    def __init__(self, socket):
        self.socket = socket

    def cmd(self, *args, size=1024, status=None):
        if args:
            cmd = args[0] + "\r\n"
            self.socket.send(cmd.encode())

        recv = self.socket.recv(size).decode()

        if DEBUG_MODE:
            print(recv)

        if status and recv[:3] != str(status):
            print("{} reply not received from server.".format(status))


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    with context.wrap_socket(sock, server_hostname=HOST) as s_sock:
        s_sock.connect((HOST, PORT))

        smtp = Smtp(s_sock)

        smtp.cmd(status=220)

        smtp.cmd("EHLO {}".format(HOST), status=250)

        smtp.cmd("AUTH LOGIN", status=334)

        user = base64.b64encode(EMAIL.encode()).decode()
        smtp.cmd(user, status=334)

        pw = base64.b64encode(PASSWORD.encode()).decode()
        smtp.cmd(pw, status=235)

        smtp.cmd("MAIL FROM: <{}>".format(EMAIL), status=250)

        smtp.cmd("RCPT TO: <{}>".format(RECIPIENT, status=250))

        smtp.cmd("DATA", status=354)

        msg = ("From: Test Example <{0}>\n"
              "To: <{1}>\n"
              "Subject: {2}\n"
              "{3}\n"
              "\r\n.")
        msg.format(EMAIL, RECIPIENT, SUBJECT, MESSAGE)
        smtp.cmd(msg, status=250)

        smtp.cmd("QUIT", status=221)
