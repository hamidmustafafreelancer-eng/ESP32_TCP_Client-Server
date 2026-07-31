# Python TCP Server

![Language](https://img.shields.io/badge/Language-Python-blue)
![Protocol](https://img.shields.io/badge/Protocol-TCP/IP-success)
![Architecture](https://img.shields.io/badge/Architecture-Modular-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

> A lightweight Python TCP server implementing a custom binary protocol for communication with an ESP32 client.

---

# Overview

This server acts as the networking endpoint of the communication framework.

Its responsibilities are intentionally limited to:

- Accept TCP client connections
- Receive raw binary packets
- Parse packets using the application protocol
- Process received messages
- Generate binary ACK packets
- Send responses back to the ESP32
- Handle client disconnections

The server follows a modular architecture where networking, protocol definitions, and packet parsing are separated into independent modules.

---

# Features

- TCP Socket Server
- Single Client Communication
- Binary Packet Parsing
- Custom Application Protocol
- Binary ACK Generation
- Sequence Number Echo
- Modular Design
- Clean Logging
- Automatic Return to Listening State after Disconnect

---

# Software Architecture

```text
                 server.py
                     │
                     │
        Socket Communication Layer
                     │
                     ▼
             Receive Raw Bytes
                     │
                     ▼
              parser.parse_packet()
                     │
          ┌──────────┴──────────┐
          │                     │
      Invalid Packet        Valid Packet
          │                     │
          ▼                     ▼
      Ignore Packet      Process Message
                                │
                                ▼
                     build_binary_ack()
                                │
                                ▼
                        Send ACK Packet
```

---

# Module Responsibilities

## server.py

Responsible for networking only.

Responsibilities

- Create TCP socket
- Bind IP and Port
- Listen for connections
- Accept clients
- Receive bytes
- Call packet parser
- Send ACK packets
- Handle disconnects

---

## parser.py

Responsible for packet parsing only.

Responsibilities

- Validate packet length
- Decode binary header
- Extract payload
- Convert packet into a Python dictionary
- Reject malformed packets

---

## protocol.py

Protocol definition module.

Contains

- Message IDs
- Protocol constants
- Packet definitions
- Shared protocol values

This file intentionally contains **no networking logic** and **no parsing logic**.

---

# Communication Workflow

```text
ESP32

Build Packet

↓

TCP Send

↓

server.py

↓

Receive Bytes

↓

parser.py

↓

Packet Valid?

↓

YES

↓

Application Processing

↓

Build ACK Packet

↓

TCP Send

↓

ESP32
```

---

# Packet Format

```text
+------------+-------------+---------------+--------------+
| Type (1B)  | Sequence(2B)| Length (2B)   | Payload (N)  |
+------------+-------------+---------------+--------------+
```

---

# Directory Structure

```text
server/
│
├── server.py        # TCP networking
├── parser.py        # Packet parser
├── protocol.py      # Protocol definitions
├── README.md

```

---

# Running the Server

## Requirements

- Python 3.10 or newer

No external libraries are required.

---

## Start the Server

```bash
cd server

python server.py
```

Expected output

```text
TCP Server listening on 192.168.x.x:5000

Waiting for client...
```

---

# Example Communication

Client connects

```text
Client Connected -> 192.168.x.x:60321
```

Packet received

```text
Type      : SENSOR_DATA
Sequence  : 15
Length    : 11
Payload   : sensor data
```

ACK sent

```text
Binary ACK transmitted back for Sequence 15
```

Client disconnects

```text
Client Disconnected
Waiting for client...
```

---

# Design Principles

This server was developed following several software engineering principles:

- Single Responsibility Principle (SRP)
- Separation of Concerns
- Modular Architecture
- Reusable Protocol Layer
- Maintainable Code Structure

Each module has a single, clearly defined responsibility.

---

# Current Limitations

- Supports one client connection at a time
- No authentication
- No encryption
- No CRC validation
- No heartbeat monitoring
- No packet retransmission

These features are planned for future versions.

---

# Future Improvements

- [ ] Multi-client support
- [ ] Threaded client handling
- [ ] Async (asyncio) implementation
- [ ] Configuration file
- [ ] TLS encryption
- [ ] Device authentication
- [ ] CRC verification
- [ ] Heartbeat timeout detection
- [ ] Packet retransmission support
- [ ] Logging to file
- [ ] Unit tests

---

# Lessons Learned

Developing this server provided practical experience with:

- TCP socket programming
- Binary protocol implementation
- Packet serialization/deserialization
- Client-server architecture
- Modular software design
- Reliable embedded communication
- Application-layer acknowledgement handling

---

# License

This project is licensed under the MIT License. See the root `LICENSE` file for details.