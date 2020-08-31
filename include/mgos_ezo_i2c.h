#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned char byte;

    enum mgos_ezo_i2c_errors
    {
        EZO_SUCCESS,
        EZO_FAIL,
        EZO_NOT_READY,
        EZO_NO_DATA,
        EZO_NOT_READ_CMD
    };
    typedef enum mgos_ezo_i2c_errors Ezo_Errors;

    struct mgos_ezo_board
    {
        struct mgos_i2c *i2c;
        uint8_t i2c_address;
        const char *name;
        float reading;
        bool issued_read;
        enum mgos_ezo_i2c_errors error;
    };
    typedef struct mgos_ezo_board Ezo_Board;

    Ezo_Board *mgos_ezo_board_init(uint8_t address);
    Ezo_Board *mgos_ezo_board_init_with_name(uint8_t address, const char *name);

    void mgos_ezo_send_cmd(Ezo_Board *board, const char *command);
    void mgos_ezo_send_read_cmd(Ezo_Board *board);
    int mgos_ezo_send_cmd_with_num(Ezo_Board *board, const char *cmd, float num);
    int mgos_ezo_send_read_with_temp_comp(Ezo_Board *board, float temperature);

    Ezo_Errors mgos_ezo_receive_read_cmd(Ezo_Board *board);
    Ezo_Errors mgos_ezo_receive_cmd(Ezo_Board *board, char *sensor_data);

    bool mgos_ezo_is_read_poll(Ezo_Board *board);
    float mgos_ezo_get_last_received_reading(Ezo_Board *board);
    Ezo_Errors mgos_ezo_get_error(Ezo_Board *board);
    uint8_t mgos_ezo_get_address(Ezo_Board *board);

#ifdef __cplusplus
}
#endif