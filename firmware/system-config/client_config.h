#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

/*WIFI-CONFIG*/
#define WIFI_SSID "MyFi"       // Change to Your SSID or AP name 
#define WIFI_PASS "Net@#@#42"        // password 
#define MAX_WIFI_RETRIES 10         // MAX TRY TO RECONNECT 

/*TCP-CONFIG*/
#define SERVER_IP           "10.47.106.115" //Replace with your local IP
#define SERVER_PORT          5000
#define TCP_RX_BUFFER_SIZE   1024
#define RECONNECT_MS         3000

/*FREERTOS BOUNDARY PARAMETERS*/

// Expand to 40 slots (40 slots * 2s sampling = 80 seconds of offline capacity)
#define NETWORK_QUEUE_LEN     40  #define QUEUE_TIMEOUT_MS     50
// Hard safety limit: don't block the sensor task for more than 500ms if queue clips
#define QUEUE_SEND_TIMEOUT_MS 500 

#define RECEIVE_TIMEOUT_MS   10000
#define RECONNECT_MS         3000


#endif