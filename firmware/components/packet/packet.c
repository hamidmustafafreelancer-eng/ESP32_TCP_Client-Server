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

/* Includes */
#include "packet.h"
#include <string.h>

 
/* Packet Builder*/
 
size_t Build_Packet(msg_type_t msg_t,
                    uint16_t seq,
                    const uint8_t *payload,
                    uint16_t payload_length,
                    uint8_t *tx_buffer,
                    size_t tx_buffer_size)
{
    /*------------------------------------------------------
     * Parameter Validation
     *-----------------------------------------------------*/
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

    /*------------------------------------------------------
     * Message Type Validation (Protocol V2)
     *-----------------------------------------------------*/
    uint8_t type = (uint8_t)msg_t;

    if (!(Packet_IsSystem(type)  ||
          Packet_IsSensor(type)  ||
          Packet_IsDevice(type)  ||
          Packet_IsControl(type)))
    {
        return 0;
    }

    /*------------------------------------------------------
     * Serialize Header (Big Endian)
     *-----------------------------------------------------*/
    tx_buffer[0] = type;

    tx_buffer[1] = (uint8_t)(seq >> 8);
    tx_buffer[2] = (uint8_t)(seq & 0xFF);

    tx_buffer[3] = (uint8_t)(payload_length >> 8);
    tx_buffer[4] = (uint8_t)(payload_length & 0xFF);

    /*------------------------------------------------------
     * Copy Payload
     *-----------------------------------------------------*/
    if (payload_length > 0)
    {
        memcpy(&tx_buffer[PACKET_HEADER_SIZE],
               payload,
               payload_length);
    }

    return PACKET_HEADER_SIZE + payload_length;
}

 
 /* Packet Parser   */
 
bool Parse_Packet(const uint8_t *buffer,
                  size_t buffer_len,
                  ParsedPacket_t *packet)
{
    /*------------------------------------------------------
     * Parameter Validation
     *-----------------------------------------------------*/
    if ((buffer == NULL) || (packet == NULL))
    {
        return false;
    }

    /*------------------------------------------------------
     * Packet must contain a complete header
     *-----------------------------------------------------*/
    if (buffer_len < PACKET_HEADER_SIZE)
    {
        return false;
    }

    /*------------------------------------------------------
     * Decode Header
     *-----------------------------------------------------*/
    uint8_t type = buffer[0];

    /*------------------------------------------------------
     * Protocol Range Validation
     *-----------------------------------------------------*/
    if (!(Packet_IsSystem(type)  ||
          Packet_IsSensor(type)  ||
          Packet_IsDevice(type)  ||
          Packet_IsControl(type)))
    {
        return false;
    }

    uint16_t sequence =
        ((uint16_t)buffer[1] << 8) |
        (uint16_t)buffer[2];

    uint16_t length =
        ((uint16_t)buffer[3] << 8) |
        (uint16_t)buffer[4];

    /*------------------------------------------------------
     * Payload Validation
     *-----------------------------------------------------*/
    if (length > MAX_PAYLOAD_SIZE)
    {
        return false;
    }

    if ((PACKET_HEADER_SIZE + length) > buffer_len)
    {
        return false;
    }

    /*------------------------------------------------------
     * Populate Parsed Packet
     *-----------------------------------------------------*/
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