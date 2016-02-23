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
typedef enum AT89S_EID_t
{
    AT89S_EID_OK = 0,
    AT89S_EID_NOK,

    AT89S_EID_ARG_NULL,

    AT89S_EID_SERIAL_OPEN,
    AT89S_EID_SERIAL_CLOSE,
    AT89S_EID_SERIAL_SEND,
    AT89S_EID_SERIAL_RECV,
    AT89S_EID_SERIAL_SET_ATTR,
    AT89S_EID_SERIAL_GET_ATTR,
    AT89S_EID_SERIAL_SET_IOSPEED,
    AT89S_EID_SERIAL_INVALID_BAUDRATE,
    AT89S_EID_SERIAL_INVALID_CONFIG,


} AT89S_EID;





#endif /* _AT89S_TYPES_H_ */
