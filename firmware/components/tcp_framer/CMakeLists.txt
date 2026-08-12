idf_component_register(SRCS "tcp_framer.c"
                       INCLUDE_DIRS "."
                       REQUIRES wifi_manager packet protocol_handler) # Add your custom wifi_manager component here
