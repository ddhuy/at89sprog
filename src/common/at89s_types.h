#ifndef _AT89S_TYPES_H_
#define _AT89S_TYPES_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/




/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/







/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 *
 */
typedef enum AT89S_EID_t
{
    AT89S_EID_OK = 0,
    AT89S_EID_NOK,

    AT89S_EID_ARG_NULL,
    AT89S_EID_ARG_INVALID,

    AT89S_EID_MEM_MALLOC,

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

} AT89S_EID;


/*
 *
 */
typedef enum AT89S_Command_t
{
    AT89S_CMD_NULL = 0,

    AT89S_CMD_WRITE_FLS,
    AT89S_CMD_READ_FLS,
    AT89S_CMD_ERASE_FLS,

    AT89S_CMD_WRITE_ROM,
    AT89S_CMD_READ_ROM,
    AT89S_CMD_ERASE_ROM,

    AT89S_CMD_READ_SIGNATURE,
    AT89S_CMD_READ_USER_SIGNATURE,
    AT89S_CMD_WRITE_USER_SIGNATURE,

} AT89S_Command;



#endif /* _AT89S_TYPES_H_ */
