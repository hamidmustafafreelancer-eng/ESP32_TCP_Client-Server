#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "packet.h"

void Handle_Telemetry_Message(const ParsedPacket_t *packet);
void Handle_Command_Message(const ParsedPacket_t *packet);
void Handle_Config_Message(const ParsedPacket_t *packet);
void Handle_Device_Message(const ParsedPacket_t *packet);
void Handle_Error_Message(const ParsedPacket_t *packet);
void Handle_Firmware_Message(const ParsedPacket_t *packet);

#endif
