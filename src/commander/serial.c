/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "serial.h"



/*******************************************************************
 *
 *      FUNCTION DEFINITION
 *
 ******************************************************************/
/*
 * Description:
 * - Open serial connection
 *
 * Input:
 *
 * Output:
 *
 */
AT89S_EID
serial_open ( char* ttyfile,
              SerialDevice_t* serial_ptr,
              int baudrate )
{
    int n;
    speed_t io_speed;

    if (ttyfile == NULL && serial_ptr == NULL)
        return EID_ARG_NULL;

    if (baudrate != 9600
        && baudrate != 115200)
    {
        return EID_SERIAL_BAUDRATE_INVALID;
    }

    serial_ptr->devfd = open(ttyfile, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_ptr->devfd < 0)
    {
        return EID_SERIAL_OPEN;
    }
    else
    {
        // cancel the O_NDELAY flag
        n = fcntl(serial_ptr->devfd, F_GETFL, 0);
        fcntl(serial_ptr->devfd, F_SETFL, n & ~O_NDELAY);
    }

    // get the current settings for the port 
    if (tcgetattr(serial_ptr->devfd, &serial_ptr->termios) < 0)
    {
        return EID_SERIAL_ATTR_GET;
    }

    // config the baudrate setting
    if (baudrate == 9600)
        io_speed = B9600;
    else
        io_speed = B115200;

    if ((cfsetispeed(&serial_ptr->termios, io_speed) != 0)
        || cfsetospeed(&serial_ptr->termios, io_speed) != 0)
    {
        return EID_SERIAL_IOSPEED_SET;
    }

    // enable receiver & ignore status line
    serial_ptr->termios.c_cflag |= (CREAD | CLOCAL);

    // hardcode 8N1
//    if (mode == SERIAL_MODE_8N1)
    {
        serial_ptr->termios.c_cflag &= ~PARENB;
        serial_ptr->termios.c_cflag &= ~CSTOPB;
        serial_ptr->termios.c_cflag &= ~CSIZE;
        serial_ptr->termios.c_cflag |= CS8;
    }
    // no flow control
    serial_ptr->termios.c_cflag &= ~CRTSCTS;

    // raw input
    serial_ptr->termios.c_iflag &= ~(IXON | IXOFF | IXANY);

    // raw output
    serial_ptr->termios.c_oflag &= ~OPOST;

    // time out settings
    serial_ptr->termios.c_cc[VMIN]  = 1;
    serial_ptr->termios.c_cc[VTIME] = 0;

    // Raw processing
    serial_ptr->termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // commit new settings
    if (tcsetattr(serial_ptr->devfd, TCSANOW, &serial_ptr->termios) < 0)
    {
        return EID_SERIAL_ATTR_SET;
    }

    // wait for device reset & sync up
    usleep(1500 * 1000);
    // clean up buffer
    tcflush(serial_ptr->devfd, TCIOFLUSH);

    return EID_OK;

} /* serial_open */


/*
 * Description:
 * - Close serial connection
 *
 * Input:
 *
 * Output:
 *
 */
AT89S_EID
serial_close( SerialDevice_t* serial_ptr )
{
    int ret = -1;

    if (serial_ptr == NULL)
        return EID_ARG_NULL;
    ret = close(serial_ptr->devfd);
    if (ret == 0)
        return EID_OK;
    return EID_SERIAL_CLOSE;
} /* serial_close */


/*
 * Description:
 * - Send data through serial
 *
 * Input:
 *
 * Output:
 *
 */
AT89S_EID
serial_send( SerialDevice_t* serial_ptr,
             char* data_buf,
             int data_len )
{
    int sent_byte = 0;

    if (serial_ptr == NULL || data_buf == NULL)
        return EID_ARG_NULL;

    while (sent_byte < data_len)
    {
        sent_byte = write(serial_ptr->devfd, data_buf, data_len);
        if (sent_byte > 0)
        {
            data_buf += sent_byte;
            data_len -= sent_byte;
        }
        else if (errno != EAGAIN)
        {
            return EID_SERIAL_SEND;
        }
    }

    return EID_OK;

} /* serial_send */


/*
 * Description:
 * - Get data from serial
 *
 * Input:
 *
 * Output:
 *
 */
AT89S_EID
serial_recv( SerialDevice_t* serial_ptr,
             char* data_buf,
             int data_len )
{
    int read_byte = 0;
    char b[1];

    if (serial_ptr == NULL || data_buf == NULL)
        return EID_ARG_NULL;

    while (read_byte < data_len)
    {
        read_byte += read(serial_ptr->devfd, data_buf, data_len);
        if (read_byte == data_len)
//        if (read(serial_ptr->devfd, b, 1) == 1)
        {
//            data_buf[read_byte++] = *b;
        }
        else if (errno != EAGAIN)
        {
            return EID_SERIAL_RECV;
        }
    }

    return EID_OK;

} /* serial_recv */

