#include <stdint.h>
#include "protocol.h"
#include "ring_buffer.h"

/* 
 * A frame consists of:
 *  uint16_t magic;                   // 0xABCD 
 *  uint16_t count;                   // number of samples 
 *  uint32_t seq;                     // frame counter 
 *  uint16_t samples[FRAME_SAMPLES];  // payload 
 *
 * !!! The format of the wire is little-endian !!!
 */

#define FRAME_MAGIC   0xABCD
#define FRAME_SAMPLES 32

#define UART1_BASE    0x40005000
#define UART1_DATA    (*(volatile uint32_t*)(UART1_BASE + 0x00))
#define UART1_STATE   (*(volatile uint32_t*)(UART1_BASE + 0x04))
#define UART1_CTRL    (*(volatile uint32_t*)(UART1_BASE + 0x08))
#define UART1_BAUD    (*(volatile uint32_t*)(UART1_BASE + 0x0C))

#define UART_DR       (*(volatile uint32_t*)0x40004000)
#define UART_FR       (*(volatile uint32_t*)0x40004018)

#define UART_FR_TXFF  (1 << 5)

static void
uart_send_byte(uint8_t c) {
    while (UART_FR & UART_FR_TXFF) {
        /* wait: if the uart is busy */
    }
    UART_DR = c;
}

static uint32_t frame_seq;

static void
uart_send_u16(uint16_t v) {
    uart_send_byte(v & 0xFF);
    uart_send_byte((v >> 8) & 0xFF);
}

static void
uart_send_u32(uint32_t v) {
    uart_send_byte(v & 0xFF);
    uart_send_byte((v >> 8) & 0xFF);
    uart_send_byte((v >> 16) & 0xFF);
    uart_send_byte((v >> 24) & 0xFF);
}

void
protocol_send_batch(void) {
    uint16_t batch[FRAME_SAMPLES];
    uint16_t sample;

    for (int i = 0; i < FRAME_SAMPLES; i++) {
        if (!ringbuf_pop(&sample)) {
            return;
        }
        batch[i] = sample;
    }

    uart_send_u16(FRAME_MAGIC);
    uart_send_u16(FRAME_SAMPLES);
    uart_send_u32(frame_seq++);

    for (int i = 0; i < FRAME_SAMPLES; i++) {
        uart_send_u16(batch[i]);
    }
}
