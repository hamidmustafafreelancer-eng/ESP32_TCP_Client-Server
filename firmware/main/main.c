#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <wifi_manager.h>
#include <tcp_client.h>



void app_main(void)
{
    wifi_manager_init();
    tcp_client_init();



}
