#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <stdbool.h>
#include"stdint.h"
#
#include "freertos/queue.h"

/* ── NEW: Inter-task message ───────────────────── */
typedef struct {
    uint8_t  type;
    uint8_t  payload[64];
    size_t   payload_len;

} network_message_t;

void tcp_client_init(void);
void tcp_client_stop(void);
bool tcp_client_is_connected(void);

/* ── NEW: Queue API ────────────────────────────── */
QueueHandle_t network_queue_init(void);
bool network_send(const network_message_t *msg, TickType_t wait);

#endif /* TCP_CLIENT_H */
