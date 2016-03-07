#ifndef _AT89S_TYPES_H_
#define _AT89S_TYPES_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdint.h>




/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
#define  AT89S_MESSAGE_SIZE    256
#define  AT89S_HEADER_SIZE     4
#define  AT89S_DATA_SIZE       (AT89S_MESSAGE_SIZE - AT89S_HEADER_SIZE)


/*
 * Command between Commander & Programmer
 */
#define  AT89S_CMD_NULL              0x00    
#define  AT89S_CMD_WRITE_FLS         0x01
#define  AT89S_CMD_READ_FLS          0x02
#define  AT89S_CMD_ERASE_FLS         0x03
#define  AT89S_CMD_WRITE_ROM         0x04
#define  AT89S_CMD_READ_ROM          0x05
#define  AT89S_CMD_ERASE_ROM         0x06
#define  AT89S_CMD_READ_SIGN         0x07
#define  AT89S_CMD_READ_USER_SIGN    0x08
#define  AT89S_CMD_WRITE_USER_SIGN   0x09

#define  AT89S_PADDING_BYTE    0x13



/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Error ID
 */
typedef enum AT89S_EID_t
{
    AT89S_EID_OK = 0,
    AT89S_EID_NOK,

    AT89S_EID_ARG_NULL,
    AT89S_EID_ARG_INVALID,

    AT89S_EID_MEM_MALLOC,
    AT89S_EID_BUF_SMALL,

    AT89S_EID_SERIAL_OPEN,
    AT89S_EID_SERIAL_CLOSE,
    AT89S_EID_SERIAL_SEND,
    AT89S_EID_SERIAL_RECV,
    AT89S_EID_SERIAL_SET_ATTR,
    AT89S_EID_SERIAL_GET_ATTR,
    AT89S_EID_SERIAL_SET_IOSPEED,
    AT89S_EID_SERIAL_BAUDRATE_INVALID,
    AT89S_EID_SERIAL_CONFIG_INVALID,

    AT89S_EID_HEXIO_OPEN,
    AT89S_EID_HEXIO_CLOSE,
    AT89S_EID_HEXIO_READALL,
    AT89S_EID_HEXIO_LENGTH,
    AT89S_EID_HEXIO_SMALL_BUFFER,

    AT89S_EID_PROT_INVALID_LEN,
    AT89S_EID_PROT_INVALID_CMD,
    AT89S_EID_PROT_INVALID_CRC,
    AT89S_EID_PROT_BAD_MSG,

    AT89S_MAX_EID,

} AT89S_EID;


/*
 * Protocol between Commander & Programmer
 */
typedef struct AT89S_Message_t
{
    unsigned char  cmd;
    unsigned char  len;
    unsigned short crc;
    unsigned char  data[AT89S_DATA_SIZE];

} AT89S_Message;


/*******************************************************************
 *
 *      GLOBAL AT89S VARIABLES
 *
 ******************************************************************/
extern const char* const AT89S_ERROR_MESSAGE[AT89S_MAX_EID];


/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
/*
 * Purpose:
 *
 * Input
 *
 * Output:
 *
 */
AT89S_EID
enc_message ( AT89S_Message* at89s_msg,
              unsigned char* data_buf,
              int* data_len );


/*
 * Purpose:
 *
 * Input
 *
 * Output:
 *
 */
AT89S_EID
dec_message ( unsigned char* data_buf,
              int  data_len,
              AT89S_Message* at89s_msg );



#endif /* _AT89S_TYPES_H_ */
