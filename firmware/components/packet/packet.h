#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Message type ranges (Protocol v2)
 */
#define MSG_TELEMETRY_MIN   0x10
#define MSG_TELEMETRY_MAX   0x1F

#define MSG_COMMAND_MIN     0x20
#define MSG_COMMAND_MAX     0x2F

#define MSG_CONFIG_MIN      0x30
#define MSG_CONFIG_MAX      0x3F

#define MSG_DEVICE_MIN      0x40
#define MSG_DEVICE_MAX      0x4F

#define MSG_ERROR_MIN       0x50
#define MSG_ERROR_MAX       0x5F

#define MSG_FIRMWARE_MIN    0x60
#define MSG_FIRMWARE_MAX    0x6F

#define PACKET_HEADER_SIZE  5U
#define MAX_PAYLOAD_SIZE    256U

typedef enum
{
    /* Telemetry (0x10 - 0x1F) */
    MSG_SENSOR_DATA     = 0x10,
    MSG_STATUS          = 0x11,
    MSG_EVENT           = 0x12,
    MSG_LOG             = 0x13,

    /* Commands (0x20 - 0x2F) */
    MSG_COMMAND         = 0x20,
    MSG_COMMAND_RESULT  = 0x21,
    MSG_COMMAND_ACK     = 0x22,

    /* Configuration (0x30 - 0x3F) */
    MSG_CONFIG_GET      = 0x30,
    MSG_CONFIG_SET      = 0x31,
    MSG_CONFIG_RESPONSE = 0x32,

    /* Device management (0x40 - 0x4F) */
    MSG_HEARTBEAT       = 0x40,
    MSG_PING            = 0x41,
    MSG_PONG            = 0x42,
    MSG_DEVICE_INFO     = 0x43,
    MSG_VERSION         = 0x44,
    MSG_ACK             = 0x45,

    /* Errors (0x50 - 0x5F) */
    MSG_ERROR           = 0x50,
    MSG_WARNING         = 0x51,

    /* Firmware update (0x60 - 0x6F) */
    MSG_OTA_BEGIN       = 0x60,
    MSG_OTA_DATA        = 0x61,
    MSG_OTA_END         = 0x62,
    MSG_OTA_STATUS      = 0x63,

} msg_type_t;

typedef struct
{
    uint8_t type;
    uint16_t sequence;
    uint16_t length;
    const uint8_t *payload;

} ParsedPacket_t;

static inline bool Packet_IsTelemetry(uint8_t type)
{
    return (type >= MSG_TELEMETRY_MIN && type <= MSG_TELEMETRY_MAX);
}

static inline bool Packet_IsCommand(uint8_t type)
{
    return (type >= MSG_COMMAND_MIN && type <= MSG_COMMAND_MAX);
}

static inline bool Packet_IsConfig(uint8_t type)
{
    return (type >= MSG_CONFIG_MIN && type <= MSG_CONFIG_MAX);
}

static inline bool Packet_IsDevice(uint8_t type)
{
    return (type >= MSG_DEVICE_MIN && type <= MSG_DEVICE_MAX);
}

static inline bool Packet_IsError(uint8_t type)
{
    return (type >= MSG_ERROR_MIN && type <= MSG_ERROR_MAX);
}

static inline bool Packet_IsFirmware(uint8_t type)
{
    return (type >= MSG_FIRMWARE_MIN && type <= MSG_FIRMWARE_MAX);
}

size_t Build_Packet(
    msg_type_t msg_t,
    uint16_t seq,
    const uint8_t *payload,
    uint16_t payload_length,
    uint8_t *tx_buffer,
    size_t tx_buffer_size);

bool Parse_Packet(
    const uint8_t *buffer,
    size_t buffer_len,
    ParsedPacket_t *packet);

#endif
