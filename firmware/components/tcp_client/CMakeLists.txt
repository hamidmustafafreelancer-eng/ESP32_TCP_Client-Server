idf_component_register(SRCS "tcp_client.c"
                       INCLUDE_DIRS "."
                       REQUIRES wifi_manager packet) # Add your custom wifi_manager component here
