import protocol


def parse_packet(data: bytes):
    """
    Parse a binary packet.

    Returns a dict on success, or None if the packet is invalid.
    """
    if len(data) < protocol.HEADER_SIZE:
        return None

    packet_type = data[0]

    valid = (
        protocol.is_telemetry(packet_type)
        or protocol.is_command(packet_type)
        or protocol.is_config(packet_type)
        or protocol.is_device(packet_type)
        or protocol.is_error(packet_type)
        or protocol.is_firmware(packet_type)
    )

    if not valid:
        return None

    sequence = int.from_bytes(data[1:3], byteorder="big")
    payload_length = int.from_bytes(data[3:5], byteorder="big")

    if payload_length > protocol.MAX_PAYLOAD_SIZE:
        return None

    expected_size = protocol.HEADER_SIZE + payload_length
    if len(data) < expected_size:
        return None

    payload = data[protocol.HEADER_SIZE:expected_size]

    return {
        "type": packet_type,
        "type_name": protocol.MESSAGE_NAMES.get(packet_type, "UNKNOWN"),
        "sequence": sequence,
        "length": payload_length,
        "payload": payload,
    }
