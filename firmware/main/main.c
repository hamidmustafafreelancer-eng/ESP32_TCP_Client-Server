#include "packet.h"
#include "client_config.h"
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <string.h>
#include <wifi_manager.h>
#include <tcp_client.h>

static const char *TAG = "MAIN_APP";

/* ───────────────────────────────────────────────────────────────────────────
 *  APPLICATION LAYER: Sensor Simulation Task (The Producer)
 * ─────────────────────────────────────────────────────────────────────────── */
void sensor_producer_task(void *pvParameters) {
    uint32_t seconds_counter = 0;
    
    // Concrete message container defined inside your tcp_client.h
    network_message_t tx_msg; 

    ESP_LOGI(TAG, "Sensor producer task started.");

    while (1) {
        seconds_counter++;

        // 1. Define message type according to Specification v2 (0x10 = SENSOR_DATA)
        tx_msg.type = MSG_SENSOR_DATA; 

        // 2. Populate the payload array with mock data
        // Simulating a dummy string or binary measurement frame "TEMP:2X.X"
        int written = snprintf((char*)tx_msg.payload, sizeof(tx_msg.payload), 
                               "TEMP:2%lu.5", (seconds_counter % 5));
        
        tx_msg.payload_len = (uint16_t)written;

        ESP_LOGI(TAG, "[Sensor Task] Generated telemetry data point (len=%u): '%s'", 
                 tx_msg.payload_len, (char*)tx_msg.payload);

        // 3. Drop data off into the thread-safe queue boundary. 
        // Replace the 0 timeout with our new global timeout macro
        if (network_send(&tx_msg, pdMS_TO_TICKS(QUEUE_SEND_TIMEOUT_MS))) {
            ESP_LOGI(TAG, "[Sensor Task] Successfully pushed message to Network Queue.");
        } else {
            // If we hit this, the network has been dead for > 80 seconds. 
            // The sensor intentionally drops or delays this frame to protect timing integrity.
            ESP_LOGW(TAG, "[Sensor Task] Backpressure alert: Queue saturated! Telemetry delayed.");
        }

        // Sample data point once every 2 seconds for clean logging output
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "Initializing System Architecture...");

    /* 1. Your existing Wi-Fi init */
    wifi_manager_init();

    /* 2. NEW: Create queue BEFORE starting TCP task */
    QueueHandle_t network_queueH = network_queue_init();
    if (network_queueH==NULL){
        ESP_LOGE(TAG, "Critical Failure: Could not initialize network queue!");
        return;
    }

    /* 3. Start TCP client */
    tcp_client_init();

    xTaskCreatePinnedToCore(
        sensor_producer_task,   // Task function
        "sensor_producer",      // Text name
        3072,                   // Stack size (bytes)
        NULL,                   // Parameters
        5,                      // Priority 
        NULL,                   // Task handle
        0                       // Pin to Core 0 (Keep communication on Core 1)
    );


}
