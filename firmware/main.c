#include "protocol.h"
#include "ring_buffer.h"
#include "systick.h"

#define TICK_RELOAD 10000 /* 10 ms */

int
main(void) {
    ringbuf_init();
    systick_init(TICK_RELOAD);
    while (1) {
        protocol_send_batch();
    }
}
