''' Only handles networking.

   Responsability 
    │
    ├── Create socket
    ├── Bind
    ├── Listen
    ├── Accept client
    ├── Receive bytes
    ├── Call parser
    ├── Process packet
    ├── Send ACK
    └── Handle disconnect

'''

''' Sercer WorkFlow :

    Receive Bytes
    ↓
    parse_packet()
    ↓
    Packet Valid?
    ↓
    YES
    ↓
    Print
    ↓
    Send ACK

'''

''' 
    data = client.recv(1024)

    packet = parse_packet(data)

    if packet is None:
        # Handle invalid packet
    else:
        # Process packet
        # Build and send ACK



'''

from datetime import datetime 
import socket

import protocol
import struct  # Required for building clean binary frames matching your C structs
from parser import parse_packet




# State: Configure Connection Parameters
# Use 0.0.0.0 to bind to all local interfaces. If you need a specific interface,
# replace this with an IP address assigned to this machine.
HOST = "10.47.106.115" 
PORT = 5000
BUFFER_SIZE = 1024
ACK_MESSAGE = b"ACK"

# ===========================
# Logger
# ===========================
def log(message: str):
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {message}")

# ===========================
# build ACK
# ===========================

def build_binary_ack(sequence_num: int) -> bytes:
    """
    Constructs a strict binary response packet matching your C protocol layout:
    [1 Byte Type] [2 Bytes Sequence Number] [2 Bytes Payload Length] + [Optional Payload]
    """
    # Define an optional small payload string
    payload = b"ACK"
    payload_len = len(payload)
    
    # '>' = Big Endian (Network Byte Order)
    # 'B' = 1 Byte unsigned char (Message Type)
    # 'H' = 2 Byte unsigned short (Sequence)
    # 'H' = 2 Byte unsigned short (Length)
    header = struct.pack(">BHH", protocol.MSG_ACK, sequence_num, payload_len)
    return header + payload



# ===========================
# main server 
# =========================== 

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        # Allow quick restart after stopping the server
        server_socket.setsockopt(
            socket.SOL_SOCKET,
            socket.SO_REUSEADDR,
            1
        )
        #  Bind 
        server_socket.bind((HOST, PORT))
        #  Listen
        server_socket.listen()
        log(f"TCP Server listening on {HOST}:{PORT}")
        # Outer Loop: Back to accept() after client disconnects
        while True:
            try:
                 # State: Accept Client
                log("Waiting for client...")
                client_socket, client_address = server_socket.accept()
                log(
                    f"Client Connected -> "
                    f"{client_address[0]}:{client_address[1]}"
                )

                #Enter communication loop 
                with client_socket :
                     log("### Communication loop ###")

                     while True :
                        log("Waiting for recv()...")
                        data =client_socket.recv(BUFFER_SIZE)
                        if not data:
                            log(f"Client Disconnected -> {client_address[0]}")
                            break

                        log(f"Parsing received data ({len(data)} raw bytes)...")
                        parsed_packet = parse_packet(data)
                        if parsed_packet is None:
                            log("Invalid packet")
                            continue
                        msg_type = parsed_packet["type"]

                        if protocol.is_telemetry(msg_type):
                            log("Telemetry Message")
                        elif protocol.is_command(msg_type):
                            log("Command Message")
                        elif protocol.is_config(msg_type):
                            log("Config Message")
                        elif protocol.is_device(msg_type):
                            log("Device Message")
                        elif protocol.is_error(msg_type):
                            log("Error Message")
                        elif protocol.is_firmware(msg_type):
                            log("Firmware Message")
                        else:
                            log("Unknown Message")
                            continue



                        # Generate and transmit a structured binary ACK packet
                        # We echo the sequence number back so the client knows WHICH message was processed
                        ack_packet = build_binary_ack(parsed_packet['sequence'])
                        client_socket.sendall(ack_packet)
                        log(f"Binary ACK transmitted back for Sequence {parsed_packet['sequence']}.")


                        #Print Humman Readed Data
                        print("Packet Received:")
                        log(f"Type     : {parsed_packet['type_name']}")
                        log(f"Sequence : {parsed_packet['sequence']}")
                        log(f"Length   : {parsed_packet['length']}")
                        log(f"Payload  : {parsed_packet['payload'].decode('utf-8', errors='replace')}")
                        print("====================")

            except ConnectionResetError:
                log("Connection reset by client.")

            except KeyboardInterrupt:
                    log("Server stopped.")
                    break

            except OSError as error:
                    log(f"Socket Error: {error}")


if __name__ == "__main__":
    main()