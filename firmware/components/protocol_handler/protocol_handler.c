/*@brief :
    This module works at the Application Layer, not the Transport Layer.
    It receives parsed packets and applies business logic.
*/
#include "protocol_handler.h"
#include "esp_log.h"

static const char *TAG = "PROTO_HANDLER";

void Handle_Telemetry_Message(const ParsedPacket_t *packet)
{
    switch (packet->type)
    {
        case MSG_SENSOR_DATA:
            ESP_LOGI(TAG, "Sensor data received (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_STATUS:
            ESP_LOGI(TAG, "Status update (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_EVENT:
            ESP_LOGI(TAG, "Event received (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_LOG:
            ESP_LOGI(TAG, "Log message (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        default:
            ESP_LOGW(TAG, "Unknown telemetry message: 0x%02X", packet->type);
            break;
    }
}

void Handle_Command_Message(const ParsedPacket_t *packet)
{
    switch (packet->type)
    {
        case MSG_COMMAND:
            ESP_LOGI(TAG, "Command received (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_COMMAND_RESULT:
            ESP_LOGI(TAG, "Command result (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_COMMAND_ACK:
            ESP_LOGI(TAG, "Command ACK (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        default:
            ESP_LOGW(TAG, "Unknown command message: 0x%02X", packet->type);
            break;
    }
}

void Handle_Config_Message(const ParsedPacket_t *packet)
{
    ESP_LOGI(TAG, "Config message: 0x%02X (seq=%u, len=%u)",
             packet->type, packet->sequence, packet->length);
}

void Handle_Device_Message(const ParsedPacket_t *packet)
{
    switch (packet->type)
    {
        case MSG_ACK:
            ESP_LOGI(TAG, "ACK received (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            if (packet->length > 0)
            {
                ESP_LOGI(TAG, "ACK payload: %.*s",
                         packet->length, packet->payload);
            }
            break;

        case MSG_HEARTBEAT:
            ESP_LOGI(TAG, "Heartbeat (seq=%u)", packet->sequence);
            break;

        case MSG_PING:
            ESP_LOGI(TAG, "Ping (seq=%u)", packet->sequence);
            break;

        case MSG_PONG:
            ESP_LOGI(TAG, "Pong (seq=%u)", packet->sequence);
            break;

        case MSG_DEVICE_INFO:
            ESP_LOGI(TAG, "Device info (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_VERSION:
            ESP_LOGI(TAG, "Version info (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        default:
            ESP_LOGW(TAG, "Unknown device message: 0x%02X", packet->type);
            break;
    }
}

void Handle_Error_Message(const ParsedPacket_t *packet)
{
    switch (packet->type)
    {
        case MSG_ERROR:
            ESP_LOGW(TAG, "Error packet (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        case MSG_WARNING:
            ESP_LOGW(TAG, "Warning packet (seq=%u, len=%u)",
                     packet->sequence, packet->length);
            break;

        default:
            ESP_LOGW(TAG, "Unknown error message: 0x%02X", packet->type);
            break;
    }
}

void Handle_Firmware_Message(const ParsedPacket_t *packet)
{
    ESP_LOGI(TAG, "Firmware message: 0x%02X (seq=%u, len=%u)",
             packet->type, packet->sequence, packet->length);
}
