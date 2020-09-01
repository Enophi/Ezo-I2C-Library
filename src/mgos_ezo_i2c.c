#include "mgos_ezo_i2c.h"

#include "mgos.h"
#include "mgos_i2c.h"

#define BUFFER_LEN 32

Ezo_Board *mgos_ezo_board_init(uint8_t address) {
    return mgos_ezo_board_init_with_name(address, NULL);
}

Ezo_Board *mgos_ezo_board_init_with_name(uint8_t address, const char *name) {
    struct mgos_i2c *i2c = mgos_i2c_get_bus(0);
    if (i2c == NULL) {
        LOG(LL_ERROR, ("Could not get I2C instance. Be sure to activate it in mos.yml : i2c.enable"));
        return NULL;
    }

    struct mgos_ezo_board *ezo_board = (struct mgos_ezo_board *)calloc(1, sizeof(*ezo_board));
    if (ezo_board == NULL) {
        return NULL;
    }

    ezo_board->i2c = i2c;
    ezo_board->i2c_address = address;
    ezo_board->name = name;
    ezo_board->reading = 0;
    ezo_board->issued_read = false;

    return ezo_board;
}

void mgos_ezo_send_cmd(Ezo_Board *board, const char *command) {
    mgos_i2c_write(board->i2c, board->i2c_address, command, strlen(command), true);
    board->issued_read = false;
}

void mgos_ezo_send_read_cmd(Ezo_Board *board) {
    mgos_ezo_send_cmd(board, "r");
    board->issued_read = true;
}

// TODO Handle the decimal_amount like the original library uint8_t decimal_amount
int mgos_ezo_send_cmd_with_num(Ezo_Board *board, const char *cmd, float num) {
    char buffer[10];  // TODO Check if it's enough (read all EZO board docs)
    int res = snprintf(buffer, sizeof buffer, "%.2f", num);
    if (res < 0) {
        LOG(LL_ERROR, ("num param is not a number! Unexpected behaviour will happend!"));
        return -1;
    }

    char fcmd[20];  // TODO Check if it's enough (read all EZO board docs)
    strcpy(fcmd, cmd);

    mgos_ezo_send_cmd(board, strcat(fcmd, buffer));
    return 1;
}

int mgos_ezo_send_read_with_temp_comp(Ezo_Board *board, float temperature) {
    int res = mgos_ezo_send_cmd_with_num(board, "rt,", temperature);
    if (res < 0) {
        LOG(LL_ERROR, ("Error while reading temperature!"));
        return -1;
    }

    board->issued_read = true;
    return 1;
}

Ezo_Errors mgos_ezo_receive_read_cmd(Ezo_Board *board) {
    char sensor_data[BUFFER_LEN];
    board->error = mgos_ezo_receive_cmd(board, sensor_data);

    if (board->error == EZO_SUCCESS) {
        if (board->issued_read) {
            board->reading = atof(sensor_data);
        } else {
            board->error = EZO_NOT_READ_CMD;
        }
    }
    return board->error;
}

Ezo_Errors mgos_ezo_receive_cmd(Ezo_Board *board, char *sensor_data) {
    uint8_t code = 1;
    memset(sensor_data, 0, BUFFER_LEN);  // Clear sensor data array

    if (!mgos_i2c_read(board->i2c, board->i2c_address, sensor_data, BUFFER_LEN, true)) {
        return EZO_FAIL;
    }

    code = sensor_data[0];
    if (sensor_data[0] != '\0') {
        memmove(sensor_data, sensor_data + 1, strlen(sensor_data));
    }

    switch (code) {
        case 1:
            board->error = EZO_SUCCESS;
            break;
        case 2:
            board->error = EZO_FAIL;
            break;
        case 254:
            board->error = EZO_NOT_READY;
            break;
        case 255:
            board->error = EZO_NO_DATA;
            break;
    }

    return board->error;
}

bool mgos_ezo_is_read_poll(Ezo_Board *board) {
    return board->issued_read;
}

float mgos_ezo_get_last_received_reading(Ezo_Board *board) {
    return board->reading;
}

Ezo_Errors mgos_ezo_get_error(Ezo_Board *board) {
    return board->error;
}

uint8_t mgos_ezo_get_address(Ezo_Board *board) {
    return board->i2c_address;
}

bool mgos_ezo_i2c_init(void) {
    return true;
}