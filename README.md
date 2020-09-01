# EZO I2C C Library for Mongoose OS

## Overview

This is a port of Atlas Scientific's EZO I2C library for Mongoose OS. See the [official library](https://github.com/Atlas-Scientific/Ezo_I2c_lib).

### Palteform support
Currently only build on ESP8266/ESP32 plateforms.

## Example
The i2c configuration must be enabled : ```i2c.enable=true```

```c
#include "mgos.h"
#include "mgos_ezo_i2c.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"

#define DEFAULT_TEMPERATURE 25.0f

static void loop_cb(void *args) {
    Ezo_Board *board = (Ezo_Board *)args;

    mgos_ezo_send_read_cmd(board);

    mgos_usleep(600000); // 600 ms processing delay

    Ezo_Errors status = mgos_ezo_receive_read_cmd(board);
    float temp_reading = mgos_ezo_get_last_received_reading(board);
    LOG(LL_INFO, (">>> %f", temp_reading));
    (void) status;
}

enum mgos_app_init_result mgos_app_init(void) {
    mgos_gpio_set_mode(15, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_write(15, true);

    Ezo_Board *RTD = mgos_ezo_board_init_with_name(102, "RTD");

    mgos_set_timer(5000, true, loop_cb, RTD);

    return MGOS_APP_INIT_SUCCESS;
}
```

## Author
- David Cruchon
