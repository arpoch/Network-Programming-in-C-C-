# TFTP Server

# Introduction
Trivial File Transfer Protocol (TFTP) is a very simple protocol used to transfer files. It is implemented in C++ programming language using Berkeley sockets on Linux Kernel.

# Implementation

The mplementation is completely based on specification of the TFTP protocol behaviour as defined in RFC 1350.

* ### Handling of RRQ / WRQ's mode

Two modes for file transfer used-

1. *netascii* : Modified version of ascii contains 8 bits.
2. *octet* : raw 8 bit bytes.

Both client and server can send and receive files.

# Usage

`./tftp [port]`

* The port parameter is used to specify the UDP port the server should listen on.



