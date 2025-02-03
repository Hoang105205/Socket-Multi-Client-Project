# File Download Server

## Overview

This project is a **file download system** that allows multiple clients to connect to a server and request specific files using **socket programming**. The server handles client requests, retrieves the requested files, and sends them over the network.

## Features

- **Multi-client support**: Multiple users can connect to the server simultaneously.
- **File selection**: Clients can request specific files from the server.
- **Efficient data transfer**: Uses socket programming to ensure reliable file transmission.
- **Error handling**: Detects and handles errors such as missing files or connection issues.

## Technologies Used

- **Programming Language**: C++ 
- **Networking**: Sockets (TCP)
- **File Handling**: Server retrieves and sends files upon request.

## Usage

1.  Start the server.
2.  Clients connect using the provided IP and port.
3.  Clients send a request for a file.
4.  The server processes the request and sends the file.
5.  The client receives and saves the file.  
