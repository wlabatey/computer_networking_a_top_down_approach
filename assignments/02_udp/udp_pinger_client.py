#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import datetime
import socket
import time


HOST = "127.0.0.1"
PORT = 12000
MAX_ATTEMPTS = 10
SOCKET_TIMEOUT = 1  # Seconds
LOOP_TIME = .150  # Milliseconds


for i in range(MAX_ATTEMPTS):
    t1 = datetime.datetime.now()
    msg = "Ping {}  {}".format(i+1, t1.strftime("%H:%M:%S"))

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.settimeout(1)
        s.sendto(msg.encode("utf-8"), (HOST, PORT))

        try:
            data, addr = s.recvfrom(1024)
            t2 = datetime.datetime.now()
        except socket.timeout:
            print("Request {} timed out".format(i+1))
        else:
            delta = (t2 - t1) / datetime.timedelta(microseconds=1000)
            print("Ping {}  {}  {}ms".format(i+1,
                                               t1.strftime("%H:%M:%S"),
                                               str(delta)))

    time.sleep(LOOP_TIME)
