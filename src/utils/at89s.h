#ifndef _AT89S_TYPES_H_
#define _AT89S_TYPES_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <stdint.h>




/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
/*
 * Protocol between Programmer & Commander
 */
#define  CMD_NULL      0x00
#define  CMD_W_MEM     0x01
#define  CMD_R_MEM     0x02
#define  CMD_E_MEM     0x03
#define  CMD_R_SIG     0x04
#define  CMD_R_USIG    0x05
#define  CMD_W_USIG    0x06

#define  RES_W_MEM     0x10
#define  RES_R_MEM     0x20
#define  RES_E_MEM     0x30
#define  RES_R_SIG     0x40
#define  RES_R_USIG    0x50
#define  RES_W_USIG    0x60

#define  CMD_MASK    0x0F
#define  RES_MASK    0xF0

#define  PADDING_BYTE  0x21

#define  MEM_FLS       0x00
#define  MEM_ROM       0x01
#define  MEM_RAM       0x02

#define  SIGN_DEV      0x00
#define  SIGN_USR      0x01


/*
 * Maximum packet size
 */
#define  MSG_SIZE    256
#define  DATA_SIZE   (MSG_SIZE - 1)


/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Error ID
 */
typedef enum AT89S_EID
{
    EID_OK = 0,
    EID_NOK,

    EID_ARG_NULL,
    EID_ARG_INVALID,

    EID_MEM_ALLOC,

    EID_SERIAL_OPEN,
    EID_SERIAL_CLOSE,
    EID_SERIAL_SEND,
    EID_SERIAL_RECV,
    EID_SERIAL_ATTR_SET,
    EID_SERIAL_ATTR_GET,
    EID_SERIAL_IOSPEED_SET,
    EID_SERIAL_BAUDRATE_INVALID,
    EID_SERIAL_CONFIG_INVALID,

    EID_HEX_OPEN,
    EID_HEX_CLOSE,
    EID_HEX_READ,
    EID_HEX_WRITE,
    EID_HEX_EOF,

    EID_REC_CRC,
    EID_REC_FMT,
    EID_REC_TYPE,
    EID_REC_CORRUPT,
    EID_REC_EMPTY,
    EID_REC_LEAK,

    EID_CMD_LEN,
    EID_CMD_BAD,
    EID_CMD_CRC,
    EID_CMD_DATA,

    EID_ENC_NOT_FOUND,

    EID_DEC_NOT_FOUND,

} AT89S_EID;

/*
 * Command Read Signature
 */
typedef struct Msg_ReadSign_t
{
    uint8_t  type;
    uint8_t  signature[3];

} Msg_ReadSign_t;

/*
 * Command Write Signature
 */
typedef struct Msg_WriteSign_t
{
    uint8_t signature[3];

} Msg_WriteSign_t;

/*
 * Command Read Memory
 */
typedef struct Msg_ReadMem_t
{
    uint16_t  address;
    uint8_t   memtype;
    uint8_t   size;
    uint8_t   data[DATA_SIZE - 4];

} Msg_ReadMem_t;

/*
 * Command Write Memory
 */
typedef struct Msg_WriteMem_t
{
    uint16_t  address;
    uint8_t   memtype;
    uint8_t   size;
    uint8_t   crc;
    uint8_t   data[DATA_SIZE - 5];

} Msg_WriteMem_t;

/*
 * Command Erase Memory
 */
typedef struct Msg_EraseMem_t
{
    uint16_t  address;
    uint8_t   memtype;
    uint8_t   size;

} Msg_EraseMem_t;


/*
 * AT89S Command Message
 */
typedef struct AT89S_Msg_t
{
    uint8_t msgt;

    /*
     * Message Data
     */
    union MsgData_t
    {
        Msg_ReadMem_t   msg_read_mem;
        Msg_WriteMem_t  msg_write_mem;
        Msg_EraseMem_t  msg_erase_mem;
        Msg_ReadSign_t  msg_read_sign;
        Msg_WriteSign_t msg_write_sign;

        uint8_t d[DATA_SIZE];

    } data;

} AT89S_Msg_t;

/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
AT89S_EID decode_msg ( char* data_buf,
                       int   data_len,
                       AT89S_Msg_t* atmsg );

AT89S_EID encode_msg ( AT89S_Msg_t* atmsg,
                       char* data_buf,
                       int* data_len );


#endif /* _AT89S_TYPES_H_ */
