/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <string.h>

#include "at89s_types.h"



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




/*******************************************************************
 *
 *      API DEFINITION
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
enc_message ( AT89S_Message* msg_ptr,
              uint8_t* data_buf,
              uint8_t* data_len )
{
    int i = 0;

    if (msg_ptr == NULL
        || data_buf == NULL
        || data_len == NULL )
    {
        return AT89S_EID_ARG_NULL;
    }

    // encode header first
    *data_len = 0;
    data_buf[*data_len++] = msg_ptr->cmd;
    data_buf[*data_len++] = msg_ptr->len;
    data_buf[*data_len++] = (uint8_t) ((msg_ptr->crc & 0xFF00) >> 8);
    data_buf[*data_len++] = (uint8_t) (msg_ptr->crc & 0x00FF);

    // encode message data
    for (i = 0; i < msg_ptr->len; ++i)
        data_buf[*data_len++] = msg_ptr->data[i];

    // insert padding byte to the rest
    for (i = *data_len; i < AT89S_MESSAGE_SIZE; ++i)
        data_buf[*data_len++] = AT89S_PADDING_BYTE;

    return AT89S_EID_OK;
}


/*
 * Purpose:
 *
 * Input
 *
 * Output:
 *
 */
AT89S_EID
dec_message ( uint8_t* data_buf,
              uint8_t  data_len,
              AT89S_Message* msg_ptr )
{
    int i = 0;

    if (msg_ptr == NULL
        || data_buf == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    if (data_len < AT89S_HEADER_SIZE)
        return AT89S_EID_PROT_INVALID_LEN;

    // decode header
    msg_ptr->cmd  = data_buf[i++];
    msg_ptr->len  = data_buf[i++];
    msg_ptr->crc  = (uint16_t) ((data_buf[i++] << 8) & 0xFF00);
    msg_ptr->crc |= (uint16_t) (data_buf[i++] & 0x00FF);

    if (data_len < msg_ptr->len + AT89S_HEADER_SIZE)
        return AT89S_EID_PROT_BAD_MSG;

    // decode message content
    for (i = 0; i < msg_ptr->len; ++i)
        msg_ptr->data[i] = data_buf[i + AT89S_HEADER_SIZE];

    return AT89S_EID_OK;
}


