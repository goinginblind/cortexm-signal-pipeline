#pragma once
#include <stdint.h>

void ringbuf_init(void);
void ringbuf_push(uint16_t s);
int ringbuf_pop(uint16_t* out);
uint32_t ringbuf_dropped(void);
