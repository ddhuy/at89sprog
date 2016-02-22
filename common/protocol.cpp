/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>

#include "protocol.h"


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
typedef struct EncDecEntry_t
{
    uint8_t  msgtype;
    ISP_EID (*enc_func) (IspMessage* msg_ptr, uint8_t* data_ptr);
    ISP_EID (*dec_func) (uint8_t* data_ptr, IspMessage* msg_ptr);

} EncDecEntry;



/*******************************************************************
 *
 *      FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 * Decoding functions
 */
static ISP_EID
msgdec_ack ( uint8_t* data_ptr,
             IspMessage* msg_ptr );

static ISP_EID
msgdec_memw ( uint8_t* data_ptr,
              IspMessage* msg_ptr );


/*
 * Encoding functions
 */
static ISP_EID
msgenc_ack ( IspMessage* msg_ptr,
             uint8_t* data_ptr );

static ISP_EID
msgenc_memw ( IspMessage* msg_ptr,
              uint8_t* data_ptr );



/*******************************************************************
 *
 *      FILE VARIABLES
 *
 ******************************************************************/
static const EncDecEntry msgtype_table[] = 
{
    { MSGT_ACK,           msgenc_ack,  msgdec_ack  },
    { MSGT_MEM_WRITE,     msgenc_memw, msgdec_memw },
    { MSGT_MEM_READ,      NULL,        NULL       },
    { MSGT_MEM_ERASE,     NULL,        NULL       },
    { MSGT_MEM_VERIFY,    NULL,        NULL       },
    { MSGT_MCU_LOCK_BIT,  NULL,        NULL       },
    { MSGT_MCU_READ,      NULL,        NULL       },

    { MSGT_NULL, NULL, NULL }, // end of table
};




/*******************************************************************
 *
 *      FUNCTION DEFINITION
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
                IspMessage* msg_ptr)
{
    ISP_EID eid = EID_NOK;
    const EncDecEntry*  dec_ptr = NULL;

    if (inbuf_ptr == NULL || msg_ptr == NULL)
        return EID_ARG_NULL;

    // decode message header
    msg_ptr->hdr.typ = inbuf_ptr[0];
    msg_ptr->hdr.len = inbuf_ptr[1];
    msg_ptr->hdr.crc = (inbuf_ptr[2] << 8) | (inbuf_ptr[3] & 0x00FF);

    // decode message content
    dec_ptr = &msgtype_table[0];
    while (dec_ptr->msgtype != MSGT_NULL)
    {
        if (dec_ptr->msgtype == msg_ptr->hdr.typ
            && dec_ptr->dec_func != NULL)
        {
            eid = dec_ptr->dec_func(inbuf_ptr + HEADER_SIZE,
                                    msg_ptr);
            break;
        }
        dec_ptr++;
    }

    return eid;
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
ISP_EID
encode_message (IspMessage* msg_ptr,
                uint8_t* outbuf_ptr)
{
    uint16_t len = 0;
    ISP_EID eid = EID_MSG_BAD_MSGT;
    const EncDecEntry*  enc_ptr = NULL;

    if (msg_ptr == NULL || outbuf_ptr == NULL)
        return EID_ARG_NULL;

    // encode header
    outbuf_ptr[len++] = msg_ptr->hdr.typ;
    outbuf_ptr[len++] = msg_ptr->hdr.len;
    outbuf_ptr[len++] = (msg_ptr->hdr.crc & 0xFF00) >> 8;
    outbuf_ptr[len++] = (msg_ptr->hdr.crc & 0x00FF);

    // encode data
    enc_ptr = &msgtype_table[0];
    while (enc_ptr->msgtype != MSGT_NULL)
    {
        if (enc_ptr->msgtype == msg_ptr->hdr.typ
            && enc_ptr->enc_func != NULL)
        {
            eid = enc_ptr->enc_func(msg_ptr,
                                    outbuf_ptr + HEADER_SIZE);
            break;
        }
        enc_ptr++;
    }
    return eid;
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static ISP_EID 
msgdec_ack ( uint8_t* data_ptr,
             IspMessage* msg_ptr )
{
    ISP_EID eid;
    if (data_ptr == NULL || msg_ptr == NULL)
        return EID_ARG_NULL;

    eid = (ISP_EID) ((data_ptr[0] << 8) | (data_ptr[1] & 0x00FF));
    msg_ptr->msg.ack.resp = eid;

    return EID_OK;
}


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static ISP_EID
msgenc_ack ( IspMessage* msg_ptr,
             uint8_t* data_ptr )
{
    if (data_ptr == NULL || msg_ptr == NULL)
        return EID_ARG_NULL;

    data_ptr[0] = (uint8_t) ((msg_ptr->msg.ack.resp & 0xFF00) >> 8);
    data_ptr[1] = (uint8_t) (msg_ptr->msg.ack.resp & 0x00FF);

    return EID_OK;
}

/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static ISP_EID
msgdec_memw ( uint8_t* data_ptr,
              IspMessage* msg_ptr )
{
    uint16_t i = 0;

    if (data_ptr == NULL || msg_ptr == NULL)
        return EID_ARG_NULL;

    for (i = 0; i < msg_ptr->hdr.len; ++i)
        msg_ptr->msg.data[i] = data_ptr[i];

    return EID_OK;
}

/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static ISP_EID
msgenc_memw ( IspMessage* msg_ptr,
              uint8_t* data_ptr )
{
    int i = 0;

    if (msg_ptr == NULL || data_ptr == NULL)
        return EID_ARG_NULL;

    for (i = 0; i < msg_ptr->hdr.len; ++i)
        data_ptr[i] = msg_ptr->msg.data[i];

    return EID_OK;
}

