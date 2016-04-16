#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "serial.h"


#define BUF_SIZE   32

int main ( int argc,
           char** argv )
{
    AT89S_EID eid = EID_OK;
    SerialDevice_t serial_dev;

    char data_buf[BUF_SIZE] = { 0 };

    if (argc < 2)
        return -1;

    eid = serial_open(argv[1], &serial_dev, 9600);
    while (eid == EID_OK)
    {
        memset(data_buf, 0, BUF_SIZE);
        serial_recv(&serial_dev, data_buf, BUF_SIZE);
        if (eid == EID_OK)
            printf("%s", data_buf);
    }

    serial_close(&serial_dev);

    return EID_OK;
}
