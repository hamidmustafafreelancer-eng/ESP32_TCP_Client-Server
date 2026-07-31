#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdbool.h>

void tcp_client_init(void);
void tcp_client_stop(void);
bool tcp_client_is_connected(void);

#endif /* TCP_CLIENT_H */
