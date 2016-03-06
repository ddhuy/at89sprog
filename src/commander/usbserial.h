#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <termios.h>

#include "at89sprog.h"

/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
// Serial Baudrate
#define  US_BAUDRATE_9600      9600
#define  US_BAUDRATE_19200     19200
#define  US_BAUDRATE_115200    115200

// 8N1
#define  US_CONFIG_8N1   0x0001
// 7E1
#define  US_CONFIG_7E1   0x0002
// 7O1
#define  US_CONFIG_7O1   0x0003

// Maximum characters of device name
#define  NAME_MAX    256

/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
typedef struct UsbSerialDevice_t
{
    int fd;
    char name[NAME_MAX];
    struct termios tios;

} UsbSerialDevice;



/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
/*
 *
 */
AT89S_EID
usbserial_open ( char* dev_name,
                 UsbSerialDevice* dev_ptr,
                 int baudrate,
                 int config );

/*
 *
 */
AT89S_EID
usbserial_send ( UsbSerialDevice* dev_ptr,
                 unsigned char* data_ptr,
                 int data_len );

/*
 *
 */
AT89S_EID
usbserial_recv ( UsbSerialDevice* dev_ptr,
                 unsigned char* data_ptr,
                 int data_len );

/*
 *
 */
AT89S_EID
usbserial_close ( UsbSerialDevice* dev_ptr );


#endif /* _USB_SERIAL_H_ */
