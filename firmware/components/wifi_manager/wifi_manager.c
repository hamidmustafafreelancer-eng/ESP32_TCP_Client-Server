/* wifi manager state machine: 
   Boot │ ▼ NVS Init │ ▼ Network Init │ ▼ Wi-Fi Driver Init │ ▼ Start Wi-Fi │ ▼ Connect │ ▼ Wi-Fi Connected │ ▼ DHCP │ ▼ Got IP │ ▼ Application Notified 
*/ 

#include "stdio.h" 
#include "string.h" 
#include "client_config.h"  //Centralized System Config File
#include "wifi_manager.h" 
#include "freertos/event_groups.h"
#include "esp_log.h" 
#include "esp_err.h" 
#include "esp_wifi.h" 
#include "nvs_flash.h" 
#include "esp_netif.h" 
#include "esp_event.h" 

#include "client_config.h"  //Centralized System Config File

static const char *TAG = "WiFi-Manager"; 

/* Module Private State Variables */ 
static bool s_connected = false; 
static uint8_t s_retry_num = 0; 
static bool s_allow_reconnect = true; 

//Private Event Group Handle
static EventGroupHandle_t s_network_event_group = NULL;

/* Getter Function API to expose the handle cleanly */
EventGroupHandle_t wifi_manager_get_event_group(void) {
    return s_network_event_group;
}
/* Wifi Event Handler */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) { 
    if (event_base == WIFI_EVENT) { 
        switch (event_id) { 
            case WIFI_EVENT_STA_START: 
                ESP_LOGI(TAG, "Wi-Fi Station started. Connecting to AP..."); 
                s_allow_reconnect = true; 
                ESP_ERROR_CHECK(esp_wifi_connect()); 
                break; 

            case WIFI_EVENT_STA_CONNECTED: 
                ESP_LOGI(TAG, "Connected to Access Point. Waiting for DHCP lease..."); 
                //Set the Wi-Fi Bit
                if (s_network_event_group) {
                    xEventGroupSetBits(s_network_event_group, WIFI_CONNECTED_BIT);
                }
                break; 

            case WIFI_EVENT_STA_DISCONNECTED: 
                s_connected = false; 
                //Clear BOTH Bits instantly on network drop
                if (s_network_event_group) {
                    xEventGroupClearBits(s_network_event_group, WIFI_CONNECTED_BIT | IP_ASSIGNED_BIT);
                }
                if (s_allow_reconnect) { 
                    if (s_retry_num < MAX_WIFI_RETRIES) { 
                        s_retry_num++; 
                        ESP_LOGW(TAG, "Retry %d/%d", s_retry_num, MAX_WIFI_RETRIES); 
                        ESP_ERROR_CHECK(esp_wifi_connect()); 
                    } else {
                        ESP_LOGE(TAG, "Max reconnect retries reached.");
                    }
                } else {
                    ESP_LOGI(TAG, "Disconnected intentionally by user application request.");
                } 
                break; 

            default: 
                break; 
        } 

    } 
    else if (event_base == IP_EVENT) { 
        switch (event_id) { 
            case IP_EVENT_STA_GOT_IP: { 
                ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data; 
                s_retry_num = 0; 
                s_connected = true; 
                //Set the IP Assigned Bit
                if (s_network_event_group) {
                    xEventGroupSetBits(s_network_event_group, IP_ASSIGNED_BIT);
                }
                ESP_LOGI(TAG, "===================================="); 
                ESP_LOGI(TAG, "Wi-Fi Connection Established Successfully"); 
                ESP_LOGI(TAG, "IP Address : " IPSTR, IP2STR(&event->ip_info.ip)); 
                ESP_LOGI(TAG, "Gateway    : " IPSTR, IP2STR(&event->ip_info.gw)); 
                ESP_LOGI(TAG, "Netmask    : " IPSTR, IP2STR(&event->ip_info.netmask)); 
                ESP_LOGI(TAG, "===================================="); 
                break; 
            } 
            default: 
                break; 
        } 
    } 
} 




void wifi_manager_init(void) { 
    ESP_LOGI(TAG, "Initializing Wi-Fi Manager..."); 

    // 1. Initialize NVS (Required by Wi-Fi Driver to store configurations) 
    esp_err_t ret = nvs_flash_init(); 
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) { 
        ESP_ERROR_CHECK(nvs_flash_erase()); 
        ret = nvs_flash_init(); 
    } 
    ESP_ERROR_CHECK(ret); 

    // 2. Initialize the Underlying Network Interface Layer 
    ESP_ERROR_CHECK(esp_netif_init()); 
    
    //Allocate Event Group memory before registering events
    s_network_event_group = xEventGroupCreate();
    if (s_network_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create Network Event Group!");
        return;
    }
    // 3. Create System Default Event Loop if missing 
    esp_err_t loop_ret = esp_event_loop_create_default(); 
    if (loop_ret != ESP_OK && loop_ret != ESP_ERR_INVALID_STATE) { 
        ESP_ERROR_CHECK(loop_ret); 
    } 

    // 4. Create default Wi-Fi Station 
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta(); 
    assert(sta_netif); 

    // 5. Initialize Wi-Fi Driver with Default Allocations 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); 

    // 6. Register Private Event Handler Hooks 
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL)); 
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL)); 

    // 7. Define parameters for Target Access Point 
    wifi_config_t wifi_config = { 
        .sta = { 
            .ssid = WIFI_SSID, 
            .password = WIFI_PASS, 
            .threshold.authmode = WIFI_AUTH_WPA2_PSK, 
        }, 
    }; 

    // 8. Enforce Station Mode and Apply Network Config Structure 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA)); 
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config)); 

    // 9. Start Wi-Fi Driver Thread Control Loop 
    ESP_ERROR_CHECK(esp_wifi_start()); 
} 

void wifi_manager_disconnect(void) { 
    ESP_LOGI(TAG, "Requesting Wi-Fi disconnect..."); 
    s_allow_reconnect = false; 
    s_connected = false; 
    esp_wifi_disconnect(); 
} 

bool wifi_manager_is_connected(void) { 
    return s_connected; 
}
