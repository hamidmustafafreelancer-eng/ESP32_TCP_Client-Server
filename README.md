# ESP32 TCP Client–Server Communication Framework

![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-ESP--IDF-orange)
![Language](https://img.shields.io/badge/Language-C%20%7C%20Python-green)
![Protocol](https://img.shields.io/badge/Protocol-TCP/IP-success)
![License](https://img.shields.io/badge/License-MIT-yellow)

> A complete TCP/IP communication framework implementing a custom binary protocol between an ESP32 client (ESP-IDF) and a Python TCP server.

---

# Overview

This project demonstrates how to build a professional TCP client-server communication system for Embedded and IoT applications.

The ESP32 firmware connects to Wi-Fi, establishes a TCP connection using the lwIP socket API, serializes application data into custom binary packets, and exchanges messages with a Python TCP server.

The server validates incoming packets, parses the application protocol, and replies with structured binary ACK packets. The ESP32 then parses and verifies the received acknowledgements before continuing communication.

The project emphasizes modular software architecture, reusable components, packet-oriented protocol design, and reliable TCP communication.

---

# Features

- Wi-Fi Station Management
- Automatic Wi-Fi Reconnection
- TCP Client using lwIP Sockets
- Python TCP Server
- Custom Binary Application Protocol
- Packet Builder
- Packet Parser (ESP32)
- Packet Parser (Python)
- Binary ACK Messages
- ACK Verification
- Sequence Number Tracking
- Automatic Reconnection
- Modular Software Architecture
- Clean Separation of Networking and Protocol Logic

---

# System Architecture

```text
                ESP32 Firmware

                     │

             Application Layer

                     │

              Packet Builder

                     │

              TCP Client (lwIP)

                     │

                Wi-Fi Driver

                     │

               TCP/IP Network

                     │

              Python TCP Server

                     │

              Packet Parser

                     │

           Application Processing

                     │

            Binary ACK Generator

                     │

             ESP32 Packet Parser
```

---

# Repository Structure

```text
ESP32_TCP_Client_Server/
│
├── firmware/
│   ├── main/
│   ├── components/
│   │   ├── wifi_manager/
│   │   ├── tcp_client/
│   │   └── packet/
│   └── README.md
│
├── server/
│   ├── server.py
│   ├── parser.py
│   ├── protocol.py
│   └── README.md
│
├── docs/
│   ├── Architecture.png
│   ├── Protocol.md
│   ├── PacketFormat.md
│   ├── StateMachine.md
│   ├── TestCases.md
│   └── Images/
│
├── LICENSE
├── README.md
└── .gitignore
```

---

# Communication Protocol

## Packet Format

```text
+------------+-------------+---------------+--------------+
| Type (1B)  | Sequence(2B)| Length (2B)   | Payload (N)  |
+------------+-------------+---------------+--------------+
```

## Supported Message Types

| Type | Description |
|------|-------------|
| SENSOR_DATA | Sensor measurements |
| HEARTBEAT | Connection keep-alive |
| COMMAND | Server command |
| ACK | Acknowledgement |
| ERROR | Error message |

---

# Communication Flow

```text
ESP32

Build Packet

↓

TCP Send

↓

Python Server

↓

Parse Packet

↓

Validate

↓

Build ACK

↓

TCP Send

↓

ESP32

↓

Parse ACK

↓

Verify Sequence

↓

Next Packet
```

---

# Getting Started

## Hardware

- ESP32 Development Board
- USB Cable
- Wi-Fi Network

## Software

- ESP-IDF
- Python 3.x

---

## Firmware

```bash
git clone <repository>

cd firmware

idf.py build

idf.py flash

idf.py monitor
```

---

## Python Server

```bash
cd server

python server.py
```

---

# Demo

The project demonstrates:

- ESP32 successfully joins the Wi-Fi network
- TCP connection establishment
- Packet serialization
- Binary packet transmission
- Python packet parsing
- Binary ACK generation
- ACK verification on ESP32
- Automatic reconnection after disconnect

---

# Test Results

| Test Case | Status |
|------------|--------|
| Wi-Fi Connection | ✅ PASS |
| DHCP Assignment | ✅ PASS |
| TCP Socket Creation | ✅ PASS |
| TCP Connection | ✅ PASS |
| Packet Serialization | ✅ PASS |
| Packet Parsing (ESP32) | ✅ PASS |
| Packet Parsing (Python) | ✅ PASS |
| ACK Generation | ✅ PASS |
| ACK Verification | ✅ PASS |
| Sequence Verification | ✅ PASS |
| Continuous Communication | ✅ PASS |
| Server Disconnect Recovery | ✅ PASS |
| Wi-Fi Reconnection | ✅ PASS |

---

# Future Improvements

- [ ] CRC Packet Validation
- [ ] Heartbeat Mechanism
- [ ] Packet Retransmission
- [ ] Configuration File Support
- [ ] Multiple Client Support
- [ ] TLS Encryption
- [ ] Device Authentication
- [ ] MQTT Gateway Integration
- [ ] FreeRTOS Queue Integration
- [ ] OTA Update Support

---

# Lessons Learned

This project reinforced several important networking concepts:

- TCP is a byte-stream protocol, requiring application-level packet framing.
- Serialization and deserialization should remain independent of socket handling.
- Separating networking logic from protocol logic improves maintainability.
- Sequence numbers enable reliable application-level acknowledgement.
- Modular firmware architecture simplifies testing and future extension.

---

# License

This project is licensed under the MIT License. See the `LICENSE` file for details.