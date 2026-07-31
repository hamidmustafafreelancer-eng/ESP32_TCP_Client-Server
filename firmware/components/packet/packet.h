#ifndef PACKET_H
#define PACKET_H

/*Include*/
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/*Constants*/
#define PACKET_HEADER_SIZE 5U
#define MAX_PAYLOAD_SIZE   256U

/*Enum*/
typedef enum {
    MSG_SENSOR_DATA = 0x01,
    MSG_HEARTBEAT   = 0x02,
    MSG_COMMAND     = 0x03,
    MSG_ACK         = 0x04,
    MSG_ERROR       = 0x05
}msg_type_t;


typedef struct
{
    uint8_t type;
    uint16_t sequence;
    uint16_t length;
    const uint8_t *payload;

} ParsedPacket_t;

/* Structures */


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