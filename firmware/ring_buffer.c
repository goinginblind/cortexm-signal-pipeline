#include <stdint.h>

#define BUF_SIZE 256 /* 1000 samples/s * 0.25 sec = 250 samples */

/* ring buffer: the oldest samples are overwritten */
volatile uint16_t buffer[BUF_SIZE];
volatile uint32_t write_idx = 0;
volatile uint32_t read_idx = 0;
volatile uint32_t dropped_samples = 0;

void
ringbuf_init(void) {
    write_idx = read_idx = dropped_samples = 0;
}

void
ringbuf_push(uint16_t s) {
    uint32_t next = (write_idx + 1) & (BUF_SIZE - 1);

    if (next == read_idx) {
        read_idx = (read_idx + 1) & (BUF_SIZE - 1);
        dropped_samples++;
    }

    buffer[write_idx] = s;
    write_idx = next;
}

int
ringbuf_pop(uint16_t* out) {
    if (read_idx == write_idx) {
        return 0; /* empty */
    }
    *out = buffer[read_idx];
    read_idx = (read_idx + 1) & (BUF_SIZE - 1);
    return 1;
}

uint32_t
ringbuf_dropped(void) {
    return dropped_samples;
}
