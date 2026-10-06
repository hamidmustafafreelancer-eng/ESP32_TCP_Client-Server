#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include <stdbool.h>
/* Core FreeRTOS dependencies for EventGroupHandle_t */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#ifdef __cplusplus
extern "C" {
#endif

//Expose the Event Group handle to the system cleanly
EventGroupHandle_t wifi_manager_get_event_group(void);

/**
 * @brief Initializes the Wi-Fi Manager state machine.
 * 
 * Configures and starts NVS, the network interface layer (ESP-NETIF),
 * the default system event loop, and the Wi-Fi subsystem in Station mode.
 */
void wifi_manager_init(void);

/**
 * @brief Disconnects from the current Access Point and stops reconnect attempts.
 */
void wifi_manager_disconnect(void);

/**
 * @brief Checks if the station is currently connected and has an active IP address.
 * 
 * @return true Connected with IP assigned
 * @return false Disconnected or connecting
 */
bool wifi_manager_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_ */
