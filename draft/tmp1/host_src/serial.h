#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <stdint.h>
#include <termios.h>

#include <types.h>

#define NAME_MAX    256
#define BLCK_SIZE   64

typedef struct SerialConfig_t
{
    speed_t ispeed;
    speed_t ospeed;

} SerialConfig;

typedef struct SerialDevice_t
{
    int fd;
    char name[NAME_MAX];
    struct termios termios;

} SerialDevice;

ISP_EID
serial_open ( char* devname,
              SerialDevice* dev_ptr,
              SerialConfig* cfg_ptr );

ISP_EID
serial_send ( SerialDevice* dev_ptr,
              uint8_t* data_ptr,
              uint16_t data_len );

ISP_EID
serial_recv ( SerialDevice* dev_ptr,
              uint8_t* data_ptr,
              uint16_t* data_len );

ISP_EID
serial_close ( SerialDevice* dev_ptr );

#endif /* _SERIAL_H_ */
