#ifndef _ISP_PROTOCOL_H_
#define _ISP_PROTOCOL_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdint.h>

#include "types.h"


/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
#define  MESSAGE_SIZE    128
#define  HEADER_SIZE     4
#define  DATA_SIZE      (MESSAGE_SIZE - HEADER_SIZE)

#define  PADDING_BYTE    0x13

/*
 * Message type
 */
#define  MSGT_NULL           0x00
#define  MSGT_ACK            0xFF

#define  MSGT_MEM_WRITE      0x31
#define  MSGT_MEM_READ       0x32
#define  MSGT_MEM_ERASE      0x33
#define  MSGT_MEM_VERIFY     0x34
#define  MSGT_MCU_LOCK_BIT   0x35
#define  MSGT_MCU_READ       0x36


/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Message header
 */
typedef struct MessageHeader_t
{
    uint8_t    typ;
    uint8_t    len;
    uint16_t   crc;
} MsgHdr;

/*
 * Acknowledge message
 */
typedef struct MsgAck_t
{
    ISP_EID  resp;
} MsgAck;


/*
 * Message data content
 */

typedef union MsgData_t
{
    MsgAck  ack;
    uint8_t data[DATA_SIZE];

} MsgCnt;


/*
 * Message content
 */
typedef struct IspMessage_t
{
    MsgHdr   hdr;
    MsgCnt   msg;

} IspMessage;


/*******************************************************************
 *
 *      FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
ISP_EID
decode_message (uint8_t* inbuf_ptr,
                IspMessage* outmsg_ptr);


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
ISP_EID
encode_message (IspMessage* inmsg_ptr,
                uint8_t* outbuf_ptr);


#endif /* _ISP_PROTOCOL_H_ */
