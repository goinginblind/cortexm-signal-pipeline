#include "systick.h"
#include <stdint.h>
#include "ring_buffer.h"
#include "sensor.h"

#define SYST_CSR      (*(volatile uint32_t*)0xE000E010)
#define SYST_RVR      (*(volatile uint32_t*)0xE000E014)
#define SYST_CVR      (*(volatile uint32_t*)0xE000E018)

#define COUNTER_RESET 0 /* to reset counter */
#define CTRL_BITS     7 /* start timer, enable interrupts, use cpu clock */

void
systick_init(uint32_t reload) {
    SYST_RVR = reload;
    SYST_CVR = COUNTER_RESET;
    SYST_CSR = CTRL_BITS;
}

extern void sample_push(uint16_t sample);

void
SysTick_Handler(void) {
    uint16_t sample = sensor_read();
    ringbuf_push(sample);
}
