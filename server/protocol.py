''' Protocol constants (Protocol v2).

This module contains only protocol constants.
It must not contain socket or parser logic.
All multi-byte fields use network byte order (big endian).
'''

HEADER_SIZE = 5
MAX_PAYLOAD_SIZE = 256

# Message ranges
TELEMETRY_RANGE_START = 0x10
TELEMETRY_RANGE_END   = 0x1F

COMMAND_RANGE_START   = 0x20
COMMAND_RANGE_END     = 0x2F

CONFIG_RANGE_START    = 0x30
CONFIG_RANGE_END      = 0x3F

DEVICE_RANGE_START    = 0x40
DEVICE_RANGE_END      = 0x4F

ERROR_RANGE_START     = 0x50
ERROR_RANGE_END       = 0x5F

FIRMWARE_RANGE_START  = 0x60
FIRMWARE_RANGE_END    = 0x6F

# Telemetry (0x10 - 0x1F)
MSG_SENSOR_DATA = 0x10
MSG_STATUS      = 0x11
MSG_EVENT       = 0x12
MSG_LOG         = 0x13

# Commands (0x20 - 0x2F)
MSG_COMMAND         = 0x20
MSG_COMMAND_RESULT  = 0x21
MSG_COMMAND_ACK     = 0x22

# Configuration (0x30 - 0x3F)
MSG_CONFIG_GET      = 0x30
MSG_CONFIG_SET      = 0x31
MSG_CONFIG_RESPONSE = 0x32

# Device management (0x40 - 0x4F)
MSG_HEARTBEAT   = 0x40
MSG_PING        = 0x41
MSG_PONG        = 0x42
MSG_DEVICE_INFO = 0x43
MSG_VERSION     = 0x44
MSG_ACK         = 0x45

# Errors (0x50 - 0x5F)
MSG_ERROR   = 0x50
MSG_WARNING = 0x51

# Firmware update (0x60 - 0x6F)
MSG_OTA_BEGIN  = 0x60
MSG_OTA_DATA   = 0x61
MSG_OTA_END    = 0x62
MSG_OTA_STATUS = 0x63

MESSAGE_NAMES = {
    MSG_SENSOR_DATA: "SENSOR_DATA",
    MSG_STATUS: "STATUS",
    MSG_EVENT: "EVENT",
    MSG_LOG: "LOG",
    MSG_COMMAND: "COMMAND",
    MSG_COMMAND_RESULT: "COMMAND_RESULT",
    MSG_COMMAND_ACK: "COMMAND_ACK",
    MSG_CONFIG_GET: "CONFIG_GET",
    MSG_CONFIG_SET: "CONFIG_SET",
    MSG_CONFIG_RESPONSE: "CONFIG_RESPONSE",
    MSG_HEARTBEAT: "HEARTBEAT",
    MSG_PING: "PING",
    MSG_PONG: "PONG",
    MSG_DEVICE_INFO: "DEVICE_INFO",
    MSG_VERSION: "VERSION",
    MSG_ACK: "ACK",
    MSG_ERROR: "ERROR",
    MSG_WARNING: "WARNING",
    MSG_OTA_BEGIN: "OTA_BEGIN",
    MSG_OTA_DATA: "OTA_DATA",
    MSG_OTA_END: "OTA_END",
    MSG_OTA_STATUS: "OTA_STATUS",
}


def is_telemetry(msg_type: int) -> bool:
    return TELEMETRY_RANGE_START <= msg_type <= TELEMETRY_RANGE_END


def is_command(msg_type: int) -> bool:
    return COMMAND_RANGE_START <= msg_type <= COMMAND_RANGE_END


def is_config(msg_type: int) -> bool:
    return CONFIG_RANGE_START <= msg_type <= CONFIG_RANGE_END


def is_device(msg_type: int) -> bool:
    return DEVICE_RANGE_START <= msg_type <= DEVICE_RANGE_END


def is_error(msg_type: int) -> bool:
    return ERROR_RANGE_START <= msg_type <= ERROR_RANGE_END


def is_firmware(msg_type: int) -> bool:
    return FIRMWARE_RANGE_START <= msg_type <= FIRMWARE_RANGE_END
