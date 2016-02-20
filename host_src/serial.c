#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "serial.h"


ISP_EID
serial_open ( char* devname,
              SerialDevice* dev_ptr,
              SerialConfig* cfg_ptr )
{

    if (devname == NULL
        || cfg_ptr == NULL
        || dev_ptr == NULL)
    {
        return EID_ARG_NULL;
    }

    // store TTY device path name
    strcpy(dev_ptr->name, devname);

    // open connection to TTY device
    dev_ptr->fd = open(dev_ptr->name,
                       O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if (dev_ptr->fd <= 0)
    {
        return EID_COM_OPEN;
    }

    // get currently set options for the TTY
    if (tcgetattr(dev_ptr->fd, &dev_ptr->termios) != 0)
    {
        return EID_COM_GETATT;
    }

    // set input/output baudrate
    if (cfsetispeed(&dev_ptr->termios, cfg_ptr->ispeed) != 0
        || cfsetospeed(&dev_ptr->termios, cfg_ptr->ospeed) != 0)
    {
        return EID_COM_SETATT;
    }

    // enable receiver, ignore status line
    dev_ptr->termios.c_cflag |= (CREAD | CLOCAL);
    // 8N1
    dev_ptr->termios.c_cflag &= ~PARENB;
    dev_ptr->termios.c_cflag &= ~CSTOPB;
    dev_ptr->termios.c_cflag &= ~CSIZE;
    dev_ptr->termios.c_cflag |= CS8;
    // no HW flow control
    dev_ptr->termios.c_cflag &= ~CRTSCTS;

    // no input/output flow control
    // disable restart charater
    dev_ptr->termios.c_iflag &= ~(IXON | IXOFF | IXANY);
    // disable cannonical input, echo, visually erase chars,
    // terminal-generated signals
    dev_ptr->termios.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // no output processing
    dev_ptr->termios.c_oflag &= ~OPOST;

    // set control chacraters
    dev_ptr->termios.c_cc[VMIN]  = 1; // wait for minimum chars come
    dev_ptr->termios.c_cc[VTIME] = 0; // dont wait for read returnsi

    // commit new options
    if (tcsetattr(dev_ptr->fd, TCSANOW, &dev_ptr->termios) != 0)
    {
        return EID_COM_SETATT;
    }

    // flush anything already in serial buffer
    tcflush(dev_ptr->fd, TCIOFLUSH);

    return EID_OK;
}

ISP_EID
serial_send ( SerialDevice* dev_ptr,
              uint8_t* data_ptr,
              uint16_t  data_len )
{
    ISP_EID eid = EID_OK;
    uint32_t nbyte = 0;

    if (dev_ptr == NULL || data_ptr == NULL)
    {
        eid = EID_ARG_NULL;
        return eid;
    }

    nbyte = write(dev_ptr->fd, data_ptr, data_len);
    if (nbyte != data_len)
    {
        eid = EID_COM_SEND;
        return eid;
    }

    return eid;
}

ISP_EID
serial_recv ( SerialDevice* dev_ptr,
              uint8_t* data_ptr,
              uint16_t* data_len )
{
    ISP_EID eid = EID_OK;
    int n = 0, len = 0;

    if (dev_ptr == NULL
        || data_ptr == NULL
        || data_len == NULL)
    {
        return EID_ARG_NULL;
    }

    // sync reading
    fcntl(dev_ptr->fd, F_SETFL, 0);

    // start reading
    while (len < *data_len)
    {
        n = read(dev_ptr->fd, &data_ptr[len], *data_len);
        if (n <= 0)
        {
            if (errno == EAGAIN)
                continue;

            eid = EID_COM_RECV;
            break;
        }
        else
        {
            len += n;
            data_ptr[len] = 0;

            printf("----------------------\n");
            printf("Received: %d bytes\n", n);
            printf("Data: %s\n", data_ptr);
            printf("----------------------\n");
        }
    }

    if (eid == EID_OK)
    {
        *data_len = len;
    }

    return eid;
}

ISP_EID
serial_close ( SerialDevice* dev_ptr )
{
    ISP_EID eid = EID_OK;

    if (dev_ptr == NULL)
    {
        eid = EID_ARG_NULL;
        return eid;
    }

    if (close(dev_ptr->fd) != 0)
    {
        eid = EID_COM_CLOSE;
        return eid;
    }

    return eid;
}

