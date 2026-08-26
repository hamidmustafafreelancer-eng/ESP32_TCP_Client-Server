/*
*tcp_client.c
*The TCP client should only handle TCP communication.
*/

/* state machine
IDLE
 │
 ▼
WAIT_WIFI
 │
 ▼
CREATE_SOCKET
 │
 ▼
CONNECT
 │
 ▼
CONNECTED
 │
 ▼
SEND / RECEIVE
 │
 ▼
ERROR
 │
 ▼
CLOSE_SOCKET
 │
 ▼
WAIT_WIFI

*/



/*dev -

TCP Client

↓

Parse

↓

Route msg types 

↓

Handler*/


#include "tcp_client.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "errno.h"

#include "packet.h"
#include "protocol_handler.h"
static const char *TAG ="TCP_CLIENT";

/* config */
#define SERVER_IP           "10.114.229.115" //Replace with your local IP
#define SERVER_PORT          5000
#define TCP_RX_BUFFER_SIZE   1024
#define RECONNECT_MS         3000


/* Thread-safe state tracking variables */
static TaskHandle_t s_tcp_task_handle = NULL;
static volatile bool s_is_connected = false;
static volatile bool s_run_task = false;

/*Helper */
static bool tcp_send_all(int sock,const uint8_t *data,size_t length)
{   
    if (data == NULL || length == 0)
    {
        return false;
    }

   size_t total_sent = 0;

    while (total_sent < length)
    {
        ssize_t bytes_sent = send(
            sock,
            data + total_sent,
            length - total_sent,
            0
        );

        if (bytes_sent < 0)
        {
            ESP_LOGE(
                TAG,
                "send() failed: errno %d",
                errno
            );

            return false;
        }

        /*
         * Defensive check:
         * send() should not normally return 0 for
         * a non-zero length, but treat it as failure.
         */
        if (bytes_sent == 0)
        {
            ESP_LOGE(
                TAG,
                "send() returned 0"
            );

            return false;
        }

        total_sent += (size_t)bytes_sent;
    }

    return true;
}

/*Main TCP Client processing thread*/

static void tcp_client_task(void *pvParameters){
    uint8_t tx_buffer[PACKET_HEADER_SIZE + MAX_PAYLOAD_SIZE];
    uint8_t rx_buffer[PACKET_HEADER_SIZE + TCP_RX_BUFFER_SIZE];
    uint16_t sequence = 0;

    struct sockaddr_in dest_addr;
    // Configure Target Server Address Structure
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);
    
    while (s_run_task) {
        /*creat a scockt & connect*/
            // 1. Wait until Wi-Fi Manager confirms network layer is fully ready
            if (!wifi_manager_is_connected()) {
                ESP_LOGW(TAG, "Wi-Fi not ready. Waiting for network connection...");
                vTaskDelay(pdMS_TO_TICKS(2000));
                continue;
            }

            ESP_LOGI(TAG, "Network link active. Creating socket...");

            //create TCP socket IPV4 
            int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
            if (sock < 0) {
                ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            // Set socket timeout to prevent permanent blocking on reading dead lines
            struct timeval timeout = { .tv_sec = 10, .tv_usec = 0 };
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

            // 3. Attempt Connection to Host
            ESP_LOGI(TAG, "Connecting to server %s:%d...", SERVER_IP, SERVER_PORT);

            int err_connect =connect(sock,(struct sockaddr*)&dest_addr,sizeof(dest_addr));
            if (err_connect != 0) {
                ESP_LOGE(TAG, "Socket unable to connect: errno %d. Retrying...", errno);
                shutdown(sock, SHUT_RDWR);
                close(sock);
                vTaskDelay(pdMS_TO_TICKS(5000)); // Dynamic retry delay
                continue;
            }
            ESP_LOGI(TAG, "TCP Connected");
            ESP_LOGI(TAG,"Remote:%s:%d",SERVER_IP,SERVER_PORT);
            s_is_connected = true;
        /*  Communication Loop  */
        const uint8_t payload[] = "sensor data";//dev-owned by app 
        ParsedPacket_t packet;

            while (s_run_task)
            {
                // Check link state dynamically before expecting packet returns
                if (!wifi_manager_is_connected()) {
                    ESP_LOGW(TAG, "Wi-Fi Link dropped mid-session.");
                    break;
                }
                //prepare payload 
                size_t tx_packet_size = Build_Packet(MSG_SENSOR_DATA, sequence, payload, sizeof(payload) - 1, tx_buffer, sizeof(tx_buffer));
                if (tx_packet_size == 0) {
                    ESP_LOGE(TAG, "Packet build failed");
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    continue;
                }
                //send packet 
               if (!tcp_send_all(sock,tx_buffer,tx_packet_size))
                {
                    ESP_LOGE(TAG, "Failed to send complete packet");
                    break;
                }


                //recive ack
                ssize_t rx_packet_size = recv(sock, rx_buffer, sizeof(rx_buffer), 0);
                //proccess 
                if ( rx_packet_size< 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        ESP_LOGD(TAG, "recv timeout occurred, polling next cycle...");
                        vTaskDelay(pdMS_TO_TICKS(500)); // Rate limit your transmission rate
                        continue;
                    }
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                } 
                else if (rx_packet_size == 0) {
                    ESP_LOGW(TAG, "Connection cleanly closed by remote server endpoint.");
                    break;
                } 
            
                //Parse Incoming Packet
                if (!Parse_Packet(rx_buffer, rx_packet_size, &packet))
                    {
                        ESP_LOGW(TAG, "Received invalid packet");
                        continue;
                    }
                if (packet.type == MSG_ACK)
                {
                    sequence++;
                }

                if (Packet_IsTelemetry(packet.type))
                {
                    Handle_Telemetry_Message(&packet);
                }
                else if (Packet_IsCommand(packet.type))
                {
                    Handle_Command_Message(&packet);
                }
                else if (Packet_IsConfig(packet.type))
                {
                    Handle_Config_Message(&packet);
                }
                else if (Packet_IsDevice(packet.type))
                {
                    Handle_Device_Message(&packet);
                }
                else if (Packet_IsError(packet.type))
                {
                    Handle_Error_Message(&packet);
                }
                else if (Packet_IsFirmware(packet.type))
                {
                    Handle_Firmware_Message(&packet);
                }
                else
                {
                    ESP_LOGW(TAG, "Unknown message type: 0x%02X", packet.type);
                }



                vTaskDelay(pdMS_TO_TICKS(500)); // Standard simulation delay between transmissions
                    
                 
            }
            
    // Cleanup socket sequence before looping back to recovery layer
        s_is_connected = false; 
        ESP_LOGW(TAG, "Closing socket and preparing reconnect cycle.");
        shutdown(sock, SHUT_RDWR);
        close(sock);
    
        if (s_run_task) {
            vTaskDelay(pdMS_TO_TICKS(RECONNECT_MS));
        }
    }
    // Safety Guard: FreeRTOS tasks must never allowed to run off the end
    s_tcp_task_handle = NULL;
    vTaskDelete(NULL);
}


void tcp_client_init(void){

    s_run_task = true;
   BaseType_t ret = xTaskCreatePinnedToCore(tcp_client_task,"tcp_client",4096,NULL,5,&s_tcp_task_handle,1);

    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create TCP task");
    }
}




void tcp_client_stop(void) {
    if (s_tcp_task_handle == NULL) {
        return;
    }
    
    ESP_LOGI(TAG, "Stopping TCP Client task...");
    s_run_task = false;
    
    /* Wait for task to naturally self-terminate and clear its handle */
    while(s_tcp_task_handle != NULL) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    ESP_LOGI(TAG, "TCP Client task stopped cleanly");
}

bool tcp_client_is_connected(void) {
    return s_is_connected;
}