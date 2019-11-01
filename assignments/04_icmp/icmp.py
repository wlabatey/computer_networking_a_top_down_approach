#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# ICMP Pinger program. Needs root privileges to use SOCK_RAW.


import argparse
import binascii
import os
import select
import signal
import socket
import statistics
import struct
import sys
import time


ICMP_ECHO_REQUEST = 8


parser = argparse.ArgumentParser()
parser.add_argument("ping_target", help="The hostname or IP of machine to ping")
parser.add_argument("-t", "--timeout", type=int, default=1, help="Specify the time to wait until a ping reply is received.")
args = parser.parse_args()


class Pinger:
    def __init__(self, target_ip, timeout):
        self.target_ip = socket.gethostbyname(target_ip)

        try:
            self.target_host = socket.gethostbyaddr(target_ip)[0]
        except socket.herror:
            self.target_host = self.target_ip

        self.timeout = timeout
        self.icmp_seq = 1
        self.icmp_id = os.getpid() & 0xFFFF
        self.packet_sent_count = 0
        self.packet_received_count = 0
        self.packet_loss = 0.0
        self.rtt_list = []
        self.rtt_min = 0.0
        self.rtt_avg = 0.0
        self.rtt_max = 0.0
        self.rtt_mdev = 0.0

    def calculate_statistics(self, sig_num, stack_frame):
        self.rtt_min = min(self.rtt_list)
        self.rtt_max = max(self.rtt_list)
        self.rtt_avg = statistics.median(self.rtt_list)

        # Set mdev to 0 when less than 2 data points in self.rtt_list
        try:
            self.rtt_mdev = statistics.stdev(self.rtt_list)
        except statistics.StatisticsError:
            self.rtt_mdev = 0

        print("\n--- {} ping statistics ---".format(self.target_ip))
        packet_stats = ("{} packets transmitted, "
                        "{} packets received, "
                        "{:.0f}% packet loss".format(self.packet_sent_count,
                                                     self.packet_received_count,
                                                     (1 - (self.packet_received_count / self.packet_sent_count)) * 100))
        print(packet_stats)

        rtt_stats = ("rtt min/avg/max/mdev = {:.3f}/{:.3f}/{:.3f}/{:.3f} ms".format(self.rtt_min,
                                                                    self.rtt_avg,
                                                                    self.rtt_max,
                                                                    self.rtt_mdev))
        print(rtt_stats)
        sys.exit(0)

    def checksum(self, packet_bytes):
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


    def receive_ping(self, my_socket, dest_addr):
        time_left = self.timeout

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

        ip_src = socket.inet_ntoa(ip_src)
        icmp_checksum = struct.unpack("!H", icmp_checksum)[0]
        icmp_id = struct.unpack("<H", icmp_id)[0]
        icmp_seq = struct.unpack("<H", icmp_seq)[0]
        icmp_date = struct.unpack("<d", icmp_date)[0]
        time_diff = (time_received - icmp_date) * 1000
        self.rtt_list.append(time_diff)

        # Ignore reply if the ID field doesn't match ours.
        if icmp_id != self.icmp_id:
            return

        self.packet_received_count += 1

        # Zero the original ICMP checksum value so we can
        # calculate our own checksum for comparison
        icmp_packet_ba[2:4] = b"\x00\x00"

        if sys.platform == "darwin":
            reply_checksum = icmp_checksum & 0xffff
        else:
            reply_checksum = icmp_checksum

        local_checksum = self.checksum(bytes(icmp_packet_ba))

        # Ignore replies with incorrect checksums
        if reply_checksum != local_checksum:
            return

        print("{} bytes from {} ({})".format(icmp_length, self.target_host, ip_src), end=": ")
        print("icmp_seq={}".format(icmp_seq), end=" ")
        print("ttl={}".format(ip_ttl), end=" ")
        print("time={0:.3g}ms".format(time_diff))


    def send_ping(self, my_socket, dest_addr):
        # Header is type (8), code (8), checksum (16), id (16), sequence (16)
        packet_checksum = 0

        # Make a dummy header with a 0 checksum
        # struct interpret strings as packed binary data
        header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, self.icmp_id, self.icmp_seq)
        data = struct.pack("d", time.time())

        # Calculate the checksum on the data and the dummy header.
        packet = header + data
        packet_checksum = self.checksum(packet)

        # Get the right checksum, and put in the header
        if sys.platform == 'darwin':
        # Convert 16bit integers from host to network byte order
            packet_checksum = socket.htons(packet_checksum) & 0xffff
        else:
            packet_checksum = socket.htons(packet_checksum)

        header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, packet_checksum, self.icmp_id, self.icmp_seq)
        packet = header + data
        my_socket.sendto(packet, (dest_addr, 1)) # socket.AF_INET address must be tuple, not str

        self.icmp_seq += 1
        self.packet_sent_count += 1

    def start_ping(self, dest_addr):
        icmp = socket.getprotobyname("icmp")

        # SOCK_RAW is a powerful socket type. For more details: http://sockraw.org/papers/sock_raw
        my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)

        self.send_ping(my_socket, dest_addr)

        delay = self.receive_ping(my_socket, dest_addr)
        my_socket.close()

        return delay


    def ping(self):
        ver = sys.version_info
        print("PING {} ({}) with Python {}.{}.{}".format(self.target_host, self.target_ip, ver.major, ver.minor, ver.micro))

        # Send ping requests to a server separated by approximately one second
        while True:
            delay = self.start_ping(self.target_ip)
            time.sleep(1)  # one second

        return delay


def main():
    p = Pinger(args.ping_target, args.timeout)
    signal.signal(signal.SIGINT, p.calculate_statistics)
    p.ping()


if __name__ == "__main__":
    main()
