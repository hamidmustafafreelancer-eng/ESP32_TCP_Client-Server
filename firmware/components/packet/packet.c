/*@brief:
INPUT
APP_DATA
│
▼
Build_Packet()
│
▼
Raw Byte Buffer
│
▼
TCP send()

This module follows the Single Responsibility Principle (SRP).

Responsibilities:
- Build protocol packets
- Parse protocol packets
- Validate packet structure
- Validate protocol ranges

It does NOT process application messages.
*/

#include "packet.h"
#include <string.h>

static bool packet_type_is_valid(uint8_t type)
{
    return (Packet_IsTelemetry(type) ||
            Packet_IsCommand(type)   ||
            Packet_IsConfig(type)    ||
            Packet_IsDevice(type)    ||
            Packet_IsError(type)     ||
            Packet_IsFirmware(type));
}

size_t Build_Packet(msg_type_t msg_t,
                    uint16_t seq,
                    const uint8_t *payload,
                    uint16_t payload_length,
                    uint8_t *tx_buffer,
                    size_t tx_buffer_size)
{
    if (tx_buffer == NULL)
    {
        return 0;
    }

    if (tx_buffer_size < PACKET_HEADER_SIZE)
    {
        return 0;
    }

    if ((payload == NULL) && (payload_length > 0))
    {
        return 0;
    }

    if (payload_length > MAX_PAYLOAD_SIZE)
    {
        return 0;
    }

    if ((PACKET_HEADER_SIZE + payload_length) > tx_buffer_size)
    {
        return 0;
    }

    uint8_t type = (uint8_t)msg_t;

    if (!packet_type_is_valid(type))
    {
        return 0;
    }

    tx_buffer[0] = type;

    tx_buffer[1] = (uint8_t)(seq >> 8);
    tx_buffer[2] = (uint8_t)(seq & 0xFF);

    tx_buffer[3] = (uint8_t)(payload_length >> 8);
    tx_buffer[4] = (uint8_t)(payload_length & 0xFF);

    if (payload_length > 0)
    {
        memcpy(&tx_buffer[PACKET_HEADER_SIZE],
               payload,
               payload_length);
    }

    return PACKET_HEADER_SIZE + payload_length;
}

bool Parse_Packet(const uint8_t *buffer,
                  size_t buffer_len,
                  ParsedPacket_t *packet)
{
    if ((buffer == NULL) || (packet == NULL))
    {
        return false;
    }

    if (buffer_len < PACKET_HEADER_SIZE)
    {
        return false;
    }

    uint8_t type = buffer[0];

    if (!packet_type_is_valid(type))
    {
        return false;
    }

    uint16_t sequence =
        ((uint16_t)buffer[1] << 8) |
        (uint16_t)buffer[2];

    uint16_t length =
        ((uint16_t)buffer[3] << 8) |
        (uint16_t)buffer[4];

    if (length > MAX_PAYLOAD_SIZE)
    {
        return false;
    }

    if ((PACKET_HEADER_SIZE + length) > buffer_len)
    {
        return false;
    }

    packet->type = type;
    packet->sequence = sequence;
    packet->length = length;

    if (length == 0)
    {
        packet->payload = NULL;
    }
    else
    {
        packet->payload = &buffer[PACKET_HEADER_SIZE];
    }

    return true;
}
