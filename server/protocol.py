''' This module contains only protocol constants.
    It should never contain socket or parser logic.
 It must explicitly enforce Network Byte Order (Big Endian) via the > symbol.
 Header Format String: >BHH (1-byte Message Type, 2-byte Sequence Number, 2-byte Payload Length).
 Constants: Mirror your C enums exactly (MSG_SENSOR_DATA = 0x01, MSG_ACK = 0x04).
 
 '''


 
# Header
 
HEADER_SIZE = 5
MAX_PAYLOAD_SIZE = 256


# ==========================================================
# Message Ranges
# ==========================================================
SYSTEM_RANGE_START  = 0x10
SYSTEM_RANGE_END    = 0x1F

SENSOR_RANGE_START  = 0x20
SENSOR_RANGE_END    = 0x2F

DEVICE_RANGE_START  = 0x30
DEVICE_RANGE_END    = 0x3F

CONTROL_RANGE_START = 0x40
CONTROL_RANGE_END   = 0x4F
# ==========================================================#
#                         msg_type                          #
# ==========================================================#

# System Messages (0x10 - 0x1F)
MSG_ACK         = 0x10
MSG_ERROR       = 0x11
MSG_HEARTBEAT   = 0x12

# Sensor Messages (0x20 - 0x2F)
MSG_SENSOR_DATA = 0x20
 
# Device Messages (0x30 - 0x3F)
 
# Control Messages (0x40 - 0x4F)
MSG_COMMAND     = 0x40


# Message Names
MESSAGE_NAMES = {
    MSG_ACK: "ACK",
    MSG_ERROR: "ERROR",
    MSG_HEARTBEAT: "HEARTBEAT",
    MSG_SENSOR_DATA: "SENSOR_DATA",
    MSG_COMMAND: "COMMAND",
}

# ==========================================================#
#                         classifier                        #
# ==========================================================#

def is_system(msg_type: int) -> bool:
    return SYSTEM_RANGE_START <= msg_type <= SYSTEM_RANGE_END

def is_sensor(msg_type: int) -> bool:
    return SENSOR_RANGE_START <= msg_type <= SENSOR_RANGE_END

def is_device(msg_type: int) -> bool:
    return DEVICE_RANGE_START <= msg_type <= DEVICE_RANGE_END

def is_control(msg_type: int) -> bool:
    return CONTROL_RANGE_START <= msg_type <= CONTROL_RANGE_END