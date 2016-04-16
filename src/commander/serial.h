#ifndef _SERIAL_H_
#define _SERIAL_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <termios.h>

#include "at89s.h"



/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
#define  NAME_MAX    256



/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Serial Device
 */
typedef struct SerialDevice_t
{
    int devfd;
    struct termios termios;

} SerialDevice_t;

/*
 * Bit rate & Parity mode
 */
typedef enum SerialMode_t
{
    SERIAL_MODE_8N1 = 0,
    SERIAL_MODE_7E1,
    SERIAL_MODE_7O1,
    SERIAL_MODE_7S1,

} SerialMode_t;


/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
/*
 * Open serial connection
 */
AT89S_EID
serial_open ( char* ttyfile,
              SerialDevice_t* serial_ptr,
              int baudrate );

/*
 * Close serial connection
 */
AT89S_EID
serial_close ( SerialDevice_t* serial_ptr );


/*
 * Send data through serial
 */
AT89S_EID
serial_send ( SerialDevice_t* serial_ptr,
              char* data_buf,
              int data_len );


/*
 * Get data from serial
 */
AT89S_EID
serial_recv ( SerialDevice_t* serial_ptr,
              char* data_buf,
              int data_len );



#endif /* _SERIAL_H_ */
