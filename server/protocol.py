''' This module contains only protocol constants.
    It should never contain socket or parser logic.
 It must explicitly enforce Network Byte Order (Big Endian) via the > symbol.
 Header Format String: >BHH (1-byte Message Type, 2-byte Sequence Number, 2-byte Payload Length).
 Constants: Mirror your C enums exactly (MSG_SENSOR_DATA = 0x01, MSG_ACK = 0x04).
 
 '''


# ==========================================================
# Header
# ==========================================================
HEADER_SIZE = 5
MAX_PAYLOAD_SIZE = 256

# ==========================================================
# MSG_TYPE
# ==========================================================
MSG_SENSOR_DATA = 0x01
MSG_HEARTBEAT   = 0x02
MSG_COMMAND     = 0x03
MSG_ACK         = 0x04
MSG_ERROR       = 0x05
# ==========================================================
# Message Names
# ==========================================================
MESSAGE_NAMES = {
    MSG_SENSOR_DATA: "SENSOR_DATA",
    MSG_HEARTBEAT:   "HEARTBEAT",
    MSG_COMMAND:     "COMMAND",
    MSG_ACK:         "ACK",
    MSG_ERROR:       "ERROR",
}
