#ifndef PACKET_H
#define PACKET_H

/*Include*/
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * dev-Message Type Ranges
 */
#define MSG_SYSTEM_MIN      0x00
#define MSG_SYSTEM_MAX      0x1F

#define MSG_SENSOR_MIN      0x20
#define MSG_SENSOR_MAX      0x3F

#define MSG_DEVICE_MIN      0x40
#define MSG_DEVICE_MAX      0x5F

#define MSG_CONTROL_MIN     0x60
#define MSG_CONTROL_MAX     0x7F



/*Constants*/
#define PACKET_HEADER_SIZE 5U
#define MAX_PAYLOAD_SIZE   256U

/*dev-Enum*/
typedef enum
{
    /*
     * System Messages (0x00 - 0x1F)
     */
    MSG_ACK            = 0x01,
    MSG_HEARTBEAT      = 0x02,
    MSG_ERROR          = 0x03,

    /*
     * Sensor Messages (0x20 - 0x3F)
     */
    MSG_SENSOR_DATA    = 0x20,
    MSG_SENSOR_CONFIG  = 0x21,

    /*
     * Device Messages (0x40 - 0x5F)
     */
    MSG_DEVICE_INFO    = 0x40,
    MSG_DEVICE_STATUS  = 0x41,

    /*
     * Control Messages (0x60 - 0x7F)
     */
    MSG_COMMAND        = 0x60,
    MSG_COMMAND_RESULT = 0x61,

} msg_type_t;

typedef struct
{
    uint8_t type;
    uint16_t sequence;
    uint16_t length;
    const uint8_t *payload;

} ParsedPacket_t;

/* Helper */
static inline bool Packet_IsSystem(uint8_t type)
{
    return (type >= MSG_SYSTEM_MIN &&
            type <= MSG_SYSTEM_MAX);
}

static inline bool Packet_IsSensor(uint8_t type)
{
    return (type >= MSG_SENSOR_MIN &&
            type <= MSG_SENSOR_MAX);
}

static inline bool Packet_IsDevice(uint8_t type)
{
    return (type >= MSG_DEVICE_MIN &&
            type <= MSG_DEVICE_MAX);
}

static inline bool Packet_IsControl(uint8_t type)
{
    return (type >= MSG_CONTROL_MIN &&
            type <= MSG_CONTROL_MAX);
}


/*Public API*/
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