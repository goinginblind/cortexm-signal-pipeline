#include <stdint.h>

/* note that this is a generic mock
 * in reality you need to configure
 * your sensor on your own 
 * for your specific needs
 */

uint16_t
sensor_read(void) {
    static uint16_t v;
    return v & 0x3FF;
}
