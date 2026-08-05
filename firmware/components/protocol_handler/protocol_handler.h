#ifndef POROTOCOL_HANDLER_H
#define POROTOCOL_HANDLER_H

#include "packet.h"

/* Public API*/

void Handle_System_Message(const ParsedPacket_t *packet);

void Handle_Sensor_Message(const ParsedPacket_t *packet);

void Handle_Device_Message(const ParsedPacket_t *packet);

void Handle_Control_Message(const ParsedPacket_t *packet);


#endif