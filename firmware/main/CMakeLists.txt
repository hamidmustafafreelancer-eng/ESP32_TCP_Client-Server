idf_component_register(
    SRCS "main.c"  
    INCLUDE_DIRS "."
    REQUIRES packet tcp_client wifi_manager
)
