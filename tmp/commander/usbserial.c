/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "usbserial.h"


/*******************************************************************
 *
 *      FUNCTION DEFINITION
 *
 ******************************************************************/
/*
 * Purpose:
 * - Open connection to Usb Serial device
 *
 * Input:
 * - [IN]  dev_name: name of device (e.g.: /dev/ttyACM0)
 * - [OUT] dev_ptr: pointer to UsbSerialDevice, used to store its termios
 * - [IN]  baudrate: serial data bit rate
 * - [IN]  config: data bit, parity, stop bits configurations
 *
 * Output:
 * - Return OK, if connection is opened successfully, else return error code
 */
AT89S_EID
usbserial_open ( char* dev_name,
                 UsbSerialDevice* dev_ptr,
                 int baudrate,
                 int config )
{
    speed_t io_baudrate = B9600;

    if (dev_name == NULL || dev_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    if (baudrate != US_BAUDRATE_9600
        && baudrate != US_BAUDRATE_19200
        && baudrate != US_BAUDRATE_115200)
    {
        return AT89S_EID_SERIAL_BAUDRATE_INVALID;
    }

    if (config != US_CONFIG_8N1
        && config != US_CONFIG_7E1
        && config != US_CONFIG_7O1)
    {
        return AT89S_EID_SERIAL_CONFIG_INVALID;
    }

    // store device name
    strcpy(dev_ptr->name, dev_name);

    // open device
    dev_ptr->fd = open (dev_ptr->name,
                        O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (dev_ptr->fd <= 0)
    {
        return AT89S_EID_SERIAL_OPEN;
    }

    // get current termios settings
    if (tcgetattr(dev_ptr->fd, &dev_ptr->tios) < 0)
    {
        return AT89S_EID_SERIAL_GET_ATTR;
    }

    // set input/output baudrdate
    if (baudrate == US_BAUDRATE_9600)
        io_baudrate = B9600;
    else if (baudrate == US_BAUDRATE_19200)
        io_baudrate = B19200;
    else if (baudrate == US_BAUDRATE_115200)
        io_baudrate = B115200;

    if (cfsetispeed(&dev_ptr->tios, io_baudrate) != 0
        || cfsetospeed(&dev_ptr->tios, io_baudrate) != 0)
    {
        return AT89S_EID_SERIAL_SET_IOSPEED;
    }

    // enable receiver, ignore status line
    dev_ptr->tios.c_cflag |= (CREAD | CLOCAL);
    // set config
    if (config == US_CONFIG_8N1)
    {
        dev_ptr->tios.c_cflag &= ~PARENB;
        dev_ptr->tios.c_cflag &= ~CSTOPB;
        dev_ptr->tios.c_cflag &= ~CSIZE;
        dev_ptr->tios.c_cflag |= CS8;
    }
    else if (config == US_CONFIG_7E1)
    {
        dev_ptr->tios.c_cflag |= PARENB;
        dev_ptr->tios.c_cflag &= ~PARODD;
        dev_ptr->tios.c_cflag &= ~CSTOPB;
        dev_ptr->tios.c_cflag &= ~CSIZE;
        dev_ptr->tios.c_cflag |= CS7;
    }
    else if (config == US_CONFIG_7O1)
    {
        dev_ptr->tios.c_cflag |= PARENB;
        dev_ptr->tios.c_cflag |= PARODD;
        dev_ptr->tios.c_cflag &= ~CSTOPB;
        dev_ptr->tios.c_cflag &= ~CSIZE;
        dev_ptr->tios.c_cflag |= CS7;
    }

    // no HW flow control
    dev_ptr->tios.c_cflag &= ~CRTSCTS;

    // no input processing (raw input)
    dev_ptr->tios.c_iflag &= ~(IXON | IXOFF | IXANY);
    // other input settings
    dev_ptr->tios.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // no output processing (raw output)
    dev_ptr->tios.c_oflag &= ~OPOST;

    // control character settings
    dev_ptr->tios.c_cc[VMIN]  = 1; // wait for 1 minimum chacacter received
    dev_ptr->tios.c_cc[VTIME] = 10; // no timeout when waiting for charater

    // commit new settings
    if (tcsetattr(dev_ptr->fd, TCSANOW, &dev_ptr->tios) < 0)
    {
        return AT89S_EID_SERIAL_SET_ATTR;
    }

    // wait for device reset & sync up
    usleep(1500 * 1000);

    // clean all bytes in buffer
    tcflush(dev_ptr->fd, TCIOFLUSH);

    return AT89S_EID_OK;
} /* usbserial_open */


/*
 * Purpose:
 * - Close an opened connection to Usb Serial device
 *
 * Input:
 * - [OUT] dev_ptr: pointer to UsbSerialDevice, used to store its termios
 *
 * Output:
 * - Return OK, if connection is closed successfully, else return error code
 */
AT89S_EID
usbserial_close ( UsbSerialDevice* dev_ptr )
{
    if (dev_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    if (close(dev_ptr->fd) < 0)
    {
        return AT89S_EID_SERIAL_CLOSE;
    }

    return AT89S_EID_OK;
} /* usbserial_close */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
AT89S_EID
usbserial_send ( UsbSerialDevice* dev_ptr,
                 unsigned char* data_ptr,
                 int data_len )
{
    int byte_sent = 0;

    if (dev_ptr == NULL || data_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    // send data
    while (byte_sent < data_len)
    {
        byte_sent = write(dev_ptr->fd, data_ptr, data_len);
        if (byte_sent > 0)
        {
            data_ptr += byte_sent;
            data_len -= byte_sent;
        }
        else
        {
            if (errno == EAGAIN)
                continue;
            else
                return AT89S_EID_SERIAL_SEND;
        }
    }

    return AT89S_EID_OK;
} /* usbserial_send */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
AT89S_EID
usbserial_recv ( UsbSerialDevice* dev_ptr,
                 unsigned char* data_ptr,
                 int data_len )
{
    int byte_read = 0;
    char b[1];

    if (dev_ptr == NULL
        || data_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    // start receiving data
    while (byte_read < data_len)
    {
        if (read(dev_ptr->fd, b, 1) > 0)
        {
            data_ptr[byte_read++] = *b;
        }
        else
        {
            if (errno == EAGAIN)
                continue;
            else
                return AT89S_EID_SERIAL_RECV;
        }
    }

    return AT89S_EID_OK;
} /* usbserial_recv */
