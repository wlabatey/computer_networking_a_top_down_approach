#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import datetime
import socket

HOST = "127.0.0.1"
PORT = 12000
MAX_ATTEMPTS = 10
SOCKET_TIMEOUT = 1  # Seconds


def get_time():
    time_obj = datetime.datetime.now()
    return time_obj


def get_time_diff(time1, time2):
    delta = time2 - time1
    return delta


for i in range(MAX_ATTEMPTS):
    t1 = get_time()
    msg = "Ping {} {}".format(i+1, t1.strftime("%H:%M:%S"))

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.settimeout(1)
        s.sendto(msg.encode("utf-8"), (HOST, PORT))
        try:
            data, addr = s.recvfrom(1024)
            t2 = get_time()
        except socket.timeout:
            print("Request {} timed out".format(i+1))
        else:
            delta = (t2 - t1) / datetime.timedelta(microseconds=1000)
            print("Ping {}   {}   {}ms".format(i+1, t1.strftime("%H:%M:%S"), str(delta)))
