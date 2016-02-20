#ifndef _ISP_PROTOCOL_H_
#define _ISP_PROTOCOL_H_

/******************************************************************************
 *
 *      HEADERS
 *
 *****************************************************************************/
#include <stdint.h>

#include "types.h"


/******************************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 *****************************************************************************/
#define  MESSAGE_SIZE    256
#define  HEADER_SIZE     4
#define  DATA_SIZE      (MESSAGE_SIZE - HEADER_SIZE)

#define  PADDING_BYTE    0x13

/*
 * Message type
 */
#define  MSGT_EOC            0x00
#define  MSGT_ACK            0xFF

#define  MSGT_MEM_WRITE      0x31
#define  MSGT_MEM_READ       0x32
#define  MSGT_MEM_ERASE      0x33
#define  MSGT_MEM_VERIFY     0x34
#define  MSGT_MCU_LOCK_BIT   0x35
#define  MSGT_MCU_READ       0x36



/******************************************************************************
 *
 *      DATA TYPE
 *
 *****************************************************************************/
/*
 * Message header
 */
typedef struct MessageHeader_t
{
    uint8_t    msgtyp;
    uint8_t    msglen;
    uint16_t   msgcrc;
} MsgHdr;


/*
 * Message content
 */
typedef struct IspMessage_t
{
    MsgHdr     hdr;
    uint8_t    data[DATA_SIZE];

} IspMessage;


/******************************************************************************
 *
 *      FUNCTION DECLARATION
 *
 *****************************************************************************/
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
