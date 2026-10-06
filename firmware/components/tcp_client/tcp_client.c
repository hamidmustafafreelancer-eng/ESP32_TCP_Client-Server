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

/* Core FreeRTOS Foundations (MUST BE FIRST) */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/* Standard C Libraries */
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* LwIP Socket Networking */
#include "lwip/sockets.h"
#include "lwip/err.h"

/* Project Components */
#include "esp_log.h"
#include "wifi_manager.h"
#include "packet.h"
#include "protocol_handler.h"
#include "tcp_client.h"

#include "client_config.h"  //Centralized System Config File

static const char *TAG = "TCP-client";

/* Thread-safe state tracking variables */
static TaskHandle_t s_tcp_task_handle = NULL;
static volatile bool s_is_connected = false;
static volatile bool s_run_task = false;

/* NEW: Queue handle */
static QueueHandle_t network_queue = NULL;

/* ───────────────────────────────────────────────
 *  NEW: Queue API
 * ─────────────────────────────────────────────── */
QueueHandle_t network_queue_init(void)
{
    network_queue = xQueueCreate(20, sizeof(network_message_t));
    if (network_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create network queue");
    }
    return network_queue;
}

bool network_send(const network_message_t *msg, TickType_t wait)
{
    if (network_queue == NULL || msg == NULL) {
        return false;
    }
    return (xQueueSend(network_queue, msg, wait) == pdPASS);
}

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

    // 1. Fetch the absolute Event Group handle from the Wi-Fi Manager module
    EventGroupHandle_t net_events = wifi_manager_get_event_group();
    if (net_events == NULL) {
        ESP_LOGE(TAG, "Critical error: Network Event Group handle is null!");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in dest_addr;
    // Configure Target Server Address Structure
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    // Read s_run_task atomically if atomics are wired, or standard check
    while (s_run_task) {
        /*creat a scockt & connect*/
             ESP_LOGI(TAG, "Waiting for Wi-Fi association and valid DHCP IP lease...");
            
             // BLOCK INDEFINITELY until both bits are set at the OS level
            EventBits_t bits = xEventGroupWaitBits(
                net_events,                                    // The shared group handle
                WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT,      // The exact bits we are waiting for
                pdFALSE,                                      // Do NOT clear the bits on exit
                pdTRUE,                                    // Wait for BOTH bits to be set
                portMAX_DELAY                                 // Block forever (0% CPU execution)
            );

            // Double check sanity guard to confirm we unblocked due to valid bits
            if ((bits & (WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT)) != (WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT)) {
                continue; 
            }
            ESP_LOGI(TAG, "Network layer verified ready. Creating TCP socket...");


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

            while (s_run_task)
            {
                // 3. Fast link integrity bit check before draining frames
                bits = xEventGroupGetBits(net_events);
                if ((bits & (WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT)) != (WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT)) {
                    ESP_LOGW(TAG, "Event bits dropped mid-session. Tearing down socket.");
                    break;
                }
                /*implement queue communication*/

                network_message_t msg;
                bool has_msg = (xQueueReceive(network_queue, &msg, pdMS_TO_TICKS(50)) == pdTRUE); // chanage the non-blocking queue read  to 50 

                if (has_msg) {
                    size_t tx_size = Build_Packet(msg.type, sequence,
                                                msg.payload, msg.payload_len,
                                                tx_buffer, sizeof(tx_buffer));
                    if (tx_size == 0) {
                        ESP_LOGE(TAG, "Packet build failed");
                    } else if (!tcp_send_all(sock, tx_buffer, tx_size)) {
                        ESP_LOGE(TAG, "Send failed — will reconnect");
                        break;
                    } else {
                        ESP_LOGI(TAG, "Sent type=0x%02X seq=%u", msg.type, sequence);
                        sequence++; // Increment on every send to preserve unique IDs
                    }
                
                }

                


                /* CHANGED: Non-blocking recv check */
                ssize_t rx_size = recv(sock, rx_buffer, sizeof(rx_buffer), MSG_DONTWAIT);//chanage the non-blocking recv to MSG_DONTWAIT to avoid deadlock scenarios

                if (rx_size < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        /* No data available — loop back to queue receive */
                        continue;
                    }
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                }
                else if (rx_size == 0) {
                    ESP_LOGW(TAG, "Connection closed by remote.");
                    break;
                }
            
                //Parse Incoming Packet
                ParsedPacket_t packet;
                if (!Parse_Packet(rx_buffer, (size_t)rx_size, &packet))
                    {
                        ESP_LOGW(TAG, "Received invalid packet");
                        continue;
                    }
                if (packet.type == MSG_ACK)
                {
                    ESP_LOGI(TAG, "Server ACK received");
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
                    /* CHANGED: Removed vTaskDelay(3000) — queue timeout paces the loop */                 
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