/*@brief :
    This module works at the Application Layer, not the Transport Layer.
    so it received parsed packet    <--
    and then applay bussines logic  -->

*/
/*INCLUDE*/
#include "protocol_handler.h"
#include "esp_log.h"

/*CONSTANTS*/
static const char *TAG = "PROTO_HANDLER";


/*IMPLEMENTATION*/
void Handle_System_Message(const ParsedPacket_t *packet){
     switch (packet->type){
        
        case MSG_ACK:

            ESP_LOGI(TAG, "ACK Received");
            ESP_LOGI(TAG, "Sequence : %u", packet->sequence);
            ESP_LOGI(TAG, "Length   : %u", packet->length);

            if (packet->length > 0)
            {
                ESP_LOGI(TAG,"Payload  : %.*s", packet->length,packet->payload);
            }
            break;

        case MSG_HEARTBEAT:
            ESP_LOGI(TAG, "Heartbeat");
            break;

        case MSG_ERROR:
            ESP_LOGW(TAG, "Error Packet");
            break;

        default:
            ESP_LOGW(TAG, "Unknown System Message");
            break;
    }
}




void Handle_Sensor_Message(const ParsedPacket_t *packet)
{
    ESP_LOGI(TAG, "Sensor Message : 0x%02X", packet->type);
}

void Handle_Device_Message(const ParsedPacket_t *packet)
{
    ESP_LOGI(TAG, "Device Message : 0x%02X", packet->type);
}

void Handle_Control_Message(const ParsedPacket_t *packet)
{
    ESP_LOGI(TAG, "Control Message : 0x%02X", packet->type);
}