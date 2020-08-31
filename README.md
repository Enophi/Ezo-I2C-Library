# EZO I2C C Library for Mongoose OS

## Overview

This is a port of Atlas Scientific's EZO I2C library for Mongoose OS. See the [official library](https://github.com/Atlas-Scientific/Ezo_I2c_lib).

### Palteform support
Currently only build on ESP8266/ESP32 plateforms.

## Example
The i2c configuration must be enabled : ```i2c.enable=true```

```
#include "mgos.h"
#include "mgos_timers.h"
#include "mgos_ezo_i2c.h"

#define DEFAULT_TEMPERATURE 25.0f

struct devices
{
  Ezo_Board *ph;
  Ezo_Board *rtd;
};
typedef struct devices Devices;

static void loop_cb(void *args)
{
  Devices *devices = (Devices *)args;

  mgos_ezo_send_read_cmd(devices->rtd);

  mgos_usleep(300);

  Ezo_Errors temp_status = mgos_ezo_receive_read_cmd(devices->rtd);
  float temp_reading = mgos_ezo_get_last_received_reading(devices->rtd);
  if ((temp_status == EZO_SUCCESS) && temp_reading > -1000.0f)
  {
    mgos_ezo_send_read_with_temp_comp(devices->ph, temp_reading);
  }
  else
  {
    mgos_ezo_send_read_with_temp_comp(devices->ph, DEFAULT_TEMPERATURE);
  }

  mgos_usleep(300);

  Ezo_Errors ph_status = mgos_ezo_receive_read_cmd(devices->ph);
  float ph_reading = mgos_ezo_get_last_received_reading(devices->ph);
  if (ph_status == EZO_SUCCESS)
  {
    LOG(LL_INFO, ("PH value %f (C° = %f)", ph_reading, temp_reading));
    // DO SOMETHING
  }
}

enum mgos_app_init_result mgos_app_init(void)
{
  Ezo_Board *PH = mgos_ezo_board_init_with_name(99, "PH");
  Ezo_Board *RTD = mgos_ezo_board_init_with_name(102, "RTD");

  Devices devices = {PH, RTD};

  mgos_set_timer(5000, true, loop_cb, &devices);

  return MGOS_APP_INIT_SUCCESS;
}


```

## Author
- David Cruchon