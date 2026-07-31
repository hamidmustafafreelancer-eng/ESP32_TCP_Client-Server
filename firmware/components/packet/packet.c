/*@brief:
INPUT
APP_DATA
│
▼
build_packet()
|
OUTPUT
Raw Byte Buffer
│
▼
TCP send()

*This follows the Single Responsibility Principle (SRP). 
It has one responsibility:
Build/parse  a valid packet.

*/


/*Include*/
#include "packet.h"
#include <string.h> 


/*implemetation*/

//packet builder 
size_t Build_Packet(msg_type_t msg_t,
                    uint16_t seq,
                    const uint8_t *payload,
                    uint16_t payload_length,
                    uint8_t *tx_buffer,
                    size_t tx_buffer_size){

    // 1. Initial Pointer & Safety Checks
    if (tx_buffer == NULL) {
        return 0;
    }
    if (tx_buffer_size < PACKET_HEADER_SIZE) {
        return 0; // Buffer cannot even hold the header
    }
    if ((payload == NULL) && (payload_length > 0U)) {
        return 0;
    }
    if (payload_length > MAX_PAYLOAD_SIZE) {
        return 0;
    }
    // 2. Strict Destination Buffer Boundary Check
    if (PACKET_HEADER_SIZE + payload_length > tx_buffer_size) {
        return 0; // Prevent out-of-bounds memory writes
    }
    // 3. Message Type Validation
    switch(msg_t)
    {
        case MSG_SENSOR_DATA:
        case MSG_HEARTBEAT:
        case MSG_COMMAND:
        case MSG_ACK:
        case MSG_ERROR:
            break;

        default:
            return 0;
    }

    
    /* Serialize packet bytes into tx buffer (Big Endian) */
    tx_buffer[0] = (uint8_t)msg_t;
    tx_buffer[1] = (uint8_t)(seq>>8);
    tx_buffer[2] = (uint8_t)(seq&0xFF);
    tx_buffer[3] = (uint8_t)(payload_length>>8);
    tx_buffer[4] = (uint8_t)(payload_length&0xFF);
    // Safely Copy Payload
    if (payload_length > 0U) {
        memcpy(&tx_buffer[PACKET_HEADER_SIZE], payload, payload_length);
    }
    

    return (PACKET_HEADER_SIZE+payload_length);
}

//packet parser 

bool Parse_Packet(const uint8_t *buffer,
    size_t buffer_len,
    ParsedPacket_t *packet){

    // 1. Parameter Validation
    if (buffer == NULL || packet == NULL) {
        return false;
    }

    // 2. Bound Check: Minimum size for a header
    if (buffer_len < PACKET_HEADER_SIZE) {
        return false;
    }

    // 3. Decode Message Type and Validate
    uint8_t msg_type = buffer[0];
    switch ((msg_type_t)msg_type) {
        case MSG_SENSOR_DATA:
        case MSG_HEARTBEAT:
        case MSG_COMMAND:
        case MSG_ACK:
        case MSG_ERROR:
            break;
        default:
            return false; // Reject unknown message types instantly
    }

    // 4. Decode Header Fields (Big Endian)
    uint16_t sequence = ((uint16_t)buffer[1] << 8) | (uint16_t)buffer[2];
    uint16_t length   = ((uint16_t)buffer[3] << 8) | (uint16_t)buffer[4];

    //  Verify payload doesn't exceed maximum protocol expectations
    if (length > MAX_PAYLOAD_SIZE) {
        return false;
    }

    // 5. Bound Check: Verify the complete packet size fits inside the received slice
    if ((PACKET_HEADER_SIZE + length) > buffer_len) {
        return false;
    }
    // 6. Commit Parsed Fields to Output Struct
    packet->type = msg_type;
    packet->sequence = sequence;
    packet->length = length;

    // Map Payload Pointer safely within current buffer space
    if (packet->length == 0) {
        packet->payload = NULL;
    } else {
        packet->payload = &buffer[PACKET_HEADER_SIZE];
    } 
    
    return true;
}
