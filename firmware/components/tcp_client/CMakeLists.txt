idf_component_register(SRCS "tcp_client.c"
                       INCLUDE_DIRS "."
                       REQUIRES 
                       system-config
                       wifi_manager packet protocol_handler) # Add your custom wifi_manager component here
