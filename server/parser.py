
''' parser.py
 Responsibilities:
    - Validate incoming packets
    - Decode header fields
    - Extract payload 


    Message Type : SENSOR_DATA
    Sequence     : 10
    Length       : 11
    Payload      : sensor data

    --> decode with Big Endian

'''

''' Header format
    Byte 0 : Message Type
    Byte 1 : Sequence High
    Byte 2 : Sequence Low
    Byte 3 : Length High
    Byte 4 : Length Low
'''

''' check Conditions (if any fai) --> reject packet
    Header complete?
    Payload length valid?
    Complete payload received?
    Known message type?

'''


''' Return a structured result.
    Conceptually:
    {
        "type": 1,
        "sequence": 10,
        "length": 11,
        "payload": b"sensor data"
    }
'''


import protocol

def parse_packet(data:bytes):

    """
    Parse a binary packet.

    Parameters
    ----------
    data : bytes
        Raw packet received from the TCP socket.
    
    
    Returns
    -------
    dict | None
        Parsed packet on success.
        None if the packet is invalid.

    """
    #check header size 
    if len(data) < protocol.HEADER_SIZE:
        return None

    #decode the header 
    packet_type = data[0]
    if packet_type not in protocol.MESSAGE_NAMES:
        return None
    
    sequence    = int.from_bytes(data[1:3] , byteorder="big")
    payload_length = int.from_bytes(data[3:5],byteorder="big")
    #check pay load 
    expected_size=protocol.HEADER_SIZE + payload_length
    if len(data)<(expected_size):
        return None
    
    #valid payload -->> parse payload data 
    payload=data[protocol.HEADER_SIZE:expected_size]

    #return struct data 
    return {
    "type": packet_type,
    "type_name": protocol.MESSAGE_NAMES[packet_type],
    "sequence": sequence,
    "length": payload_length,
    "payload": payload,
    }









