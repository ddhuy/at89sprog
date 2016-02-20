/******************************************************************************
 *
 *      HEADERS
 *
 *****************************************************************************/
#include <stdio.h>

#include "protocol.h"


/******************************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 *****************************************************************************/





/******************************************************************************
 *
 *      DATA TYPE
 *
 *****************************************************************************/





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
                IspMessage* outmsg_ptr)
{
    uint16_t i = 0;

    if (inbuf_ptr == NULL || outmsg_ptr == NULL)
        return EID_ARG_NULL;

    // decode message header
    outmsg_ptr->hdr.msgtyp = inbuf_ptr[0];
    outmsg_ptr->hdr.msglen = inbuf_ptr[1];
    outmsg_ptr->hdr.msgcrc = (inbuf_ptr[2] << 8) | (inbuf_ptr[3] & 0x00FF);

    // decode data content
    for (i = 0; i < outmsg_ptr->hdr.msglen; ++i)
        outmsg_ptr->data[i] = inbuf_ptr[i + HEADER_SIZE];

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
ISP_EID
encode_message (IspMessage* inmsg_ptr,
                uint8_t* outbuf_ptr)
{
    uint16_t i = 0, len = 0;

    if (inmsg_ptr == NULL || outbuf_ptr == NULL)
        return EID_ARG_NULL;

    // encode header
    outbuf_ptr[len++] = inmsg_ptr->hdr.msgtyp;
    outbuf_ptr[len++] = inmsg_ptr->hdr.msglen;
    outbuf_ptr[len++] = (inmsg_ptr->hdr.msgcrc & 0xFF00) >> 8;
    outbuf_ptr[len++] = (inmsg_ptr->hdr.msgcrc & 0x00FF);

    // encode data
    for (i = 0; i < inmsg_ptr->hdr.msglen; ++i)
        outbuf_ptr[len++] = inmsg_ptr->data[i];

    // insert padding data
    while (len < MESSAGE_SIZE)
        outbuf_ptr[len++] = PADDING_BYTE;

    return EID_OK;
}

