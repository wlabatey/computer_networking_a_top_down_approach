#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# ICMP Pinger program. Needs root privileges to use SOCK_RAW.

# TODO: Add trap for SIGINT (ctrl + c) and calculate statistics, to match usual ping program behaviour.

import argparse
import binascii
import itertools
import os
import socket
import sys
import struct
import select
import time


ICMP_ECHO_REQUEST = 8


parser = argparse.ArgumentParser()
parser.add_argument("ping_target", help="The hostname or IP of machine to ping")
parser.add_argument("-t", "--timeout", type=int, default=1, help="Specify the time to wait until a ping reply is received.")
args = parser.parse_args()


def checksum(packet_bytes):
    csum = 0
    count_to = (len(packet_bytes) // 2) * 2
    count = 0
    while count < count_to:
        this_val = packet_bytes[count+1] * 256 + packet_bytes[count]
        csum = csum + this_val
        csum = csum & 0xffffffff
        count = count + 2

    if count_to < len(packet_bytes):
        csum = csum + packet_bytes[len(packet_bytes) - 1]
        csum = csum & 0xffffffff

    csum = (csum >> 16) + (csum & 0xffff)
    csum = csum + (csum >> 16)
    answer = ~csum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)

    return answer


def receive_ping(my_socket, ID, timeout, dest_addr):
    time_left = timeout

    while True:
        started_select = time.time()
        status = select.select([my_socket], [], [], time_left)
        select_length = (time.time() - started_select)

        if not status[0]: # Timeout
            return "Request timed out."
        else:
            break

    time_received = time.time()
    recv_packet, addr = my_socket.recvfrom(1024)

    socket_data = bytearray(recv_packet)
    ip_packet_ba = socket_data[0:21]
    icmp_packet_ba = socket_data[20:]

    ip_src = ip_packet_ba[12:16]
    ip_ttl = ip_packet_ba[8]
    icmp_length = len(icmp_packet_ba)
    icmp_checksum = icmp_packet_ba[2:4]
    icmp_id = icmp_packet_ba[4:6]
    icmp_seq = icmp_packet_ba[6:8]
    icmp_date = icmp_packet_ba[-8:]


    ip_src = ".".join(str(x) for x in ip_src)
    icmp_checksum = struct.unpack("!H", icmp_checksum)[0]
    icmp_id = struct.unpack("<H", icmp_id)[0]
    icmp_seq = struct.unpack("<H", icmp_seq)[0]
    icmp_date = struct.unpack("<d", icmp_date)[0]
    time_diff = (time_received - icmp_date) * 1000

    host_rev_lookup = socket.gethostbyaddr(ip_src)

    # Zero the original ICMP checksum value so we can
    # calculate our own checksum for comparison
    icmp_packet_ba[2:4] = b"\x00\x00"

    if sys.platform == "darwin":
        reply_checksum = icmp_checksum & 0xffff
    else:
        reply_checksum = icmp_checksum

    local_checksum = checksum(bytes(icmp_packet_ba))

    if reply_checksum != local_checksum:
        return "Bad checksum."

    print("{} bytes from {} ({})".format(icmp_length, host_rev_lookup[0], ip_src), end=": ")
    print("icmp_seq={}".format(icmp_seq), end=" ")
    print("ttl={}".format(ip_ttl), end=" ")
    print("time={:.0f}ms".format(time_diff))


def send_ping(my_socket, dest_addr, ID, seq):
    # Header is type (8), code (8), checksum (16), id (16), sequence (16)
    packet_checksum = 0

    # Make a dummy header with a 0 checksum
    # struct interpret strings as packed binary data
    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, ID, seq)
    data = struct.pack("d", time.time())

    # Calculate the checksum on the data and the dummy header.
    packet = header + data
    packet_checksum = checksum(packet)

    # Get the right checksum, and put in the header
    if sys.platform == 'darwin':
    # Convert 16bit integers from host to network byte order
        packet_checksum = socket.htons(packet_checksum) & 0xffff
    else:
        packet_checksum = socket.htons(packet_checksum)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, ID, seq)
    packet = header + data
    my_socket.sendto(packet, (dest_addr, 1)) # socket.AF_INET address must be tuple, not str

    seq += 1


def start_ping(dest_addr, timeout, seq):
    icmp = socket.getprotobyname("icmp")

    # SOCK_RAW is a powerful socket type. For more details: http://sockraw.org/papers/sock_raw
    my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)
    my_pid = os.getpid() & 0xFFFF # Return the current process id

    send_ping(my_socket, dest_addr, my_pid, seq)
    delay = receive_ping(my_socket, my_pid, timeout, dest_addr)
    my_socket.close()

    return delay


def ping(host, timeout):
    # timeout: If the value in seconds goes by without a reply from the server,
    # the client assumes that either the client's ping or the server's pong is lost
    dest = socket.gethostbyname(host)
    ver = sys.version_info
    print("Pinging {} ({}) with Python {}.{}.{}".format(host, dest, ver.major, ver.minor, ver.micro))

    # Send ping requests to a server separated by approximately one second
    for i in itertools.count(start=1):
        delay = start_ping(dest, timeout, i)
        time.sleep(1)  # one second

    return delay

ping(args.ping_target, args.timeout)
