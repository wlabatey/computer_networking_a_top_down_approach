#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# ICMP Pinger program. Needs root privileges to use SOCK_RAW.

import argparse
import os
import socket
import sys
import struct
import time
import select
import binascii


ICMP_ECHO_REQUEST = 8


parser = argparse.ArgumentParser()
parser.add_argument("ping_target")
parser.add_argument("-t", "--timeout", type=int, default=1, help="Specify the time to wait until a ping reply is received.")
args = parser.parse_args()


def checksum(string):
    csum = 0
    count_to = (len(string) // 2) * 2
    count = 0
    while count < count_to:
        thisVal = ord(string[count+1]) * 256 + ord(string[count])
        csum = csum + thisVal
        csum = csum & 0xffffffff
        count = count + 2

    if count_to < len(string):
        csum = csum + ord(string[len(string) - 1])
        csum = csum & 0xffffffff

    csum = (csum >> 16) + (csum & 0xffff)
    csum = csum + (csum >> 16)
    answer = ~csum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)

    return answer


def receive_ping(my_socket, ID, timeout, dest_addr):
    time_left = timeout

    while 1:
        started_select = time.time()
        status = select.select([my_socket], [], [], time_left)
        select_length = (time.time() - started_select)
        if status[0] == []: # Timeout
            return "Request timed out."


    time_received = time.time()
    recv_packet, addr = my_socket.recvfrom(1024)

    #Fill in start

    #Fetch the ICMP header from the IP packet

    #Fill in end

    time_left = time_left - select_length

    if time_left <= 0:
        return "Request timed out."


def send_ping(my_socket, dest_addr, ID):
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    packet_checksum = 0

    # Make a dummy header with a 0 checksum
    # struct interpret strings as packed binary data
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, ID, 1)
    data = struct.pack("d", time.time())

    # Calculate the checksum on the data and the dummy header.
    packet_checksum = checksum(str(header + data))

    # Get the right checksum, and put in the header
    if sys.platform == 'darwin':
    # Convert 16bit integers from host to network byte order
        packet_checksum = socket.htons(packet_checksum) & 0xffff
    else:
        packet_checksum = socket.htons(packet_checksum)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, ID, 1)
    packet = header + data
    my_socket.sendto(packet, (dest_addr, 1)) # socket.AF_INET address must be tuple, not str

    # Both LISTS and TUPLES consist of a number of objects
    # which can be referenced by their position number within the object.


def start_ping(dest_addr, timeout):
    icmp = socket.getprotobyname("icmp")

    # SOCK_RAW is a powerful socket type. For more details: http://sockraw.org/papers/sock_raw
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)
    my_pid = os.getpid() & 0xFFFF # Return the current process id
    send_ping(my_socket, dest_addr, my_pid)
    delay = receive_ping(my_socket, my_pid, timeout, dest_addr)
    my_socket.close()

    return delay


def ping(host, timeout):
    # timeout=1 means: If one second goes by without a reply from the server,
    # the client assumes that either the client's ping or the server's pong is lost

    dest = socket.gethostbyname(host)
    print("Pinging " + dest + " using Python:")
    print("")

    # Send ping requests to a server separated by approximately one second
    while 1:
        delay = start_ping(dest, timeout)
        print(delay)
        time.sleep(1)# one second

    return delay

ping(args.ping_target, args.timeout)
