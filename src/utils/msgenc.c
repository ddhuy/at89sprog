/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include "at89s.h"




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
typedef  AT89S_EID  (*Encoder) ( AT89S_Msg_t* atmsg,
                                 char* data_buf, 
                                 int* data_len );
typedef struct MsgEncoder_t
{
    uint8_t msgtype;
    Encoder msgenc_fptr;

} MsgEncoder;

/*******************************************************************
 *
 *      PRIVATE FUNCTION DELCARATION
 *
 ******************************************************************/
static AT89S_EID
enc_c_rsig ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len );

static AT89S_EID
enc_r_rsig ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len );

static AT89S_EID
enc_c_wmem ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len );

static AT89S_EID
enc_r_wmem ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len );



/*******************************************************************
 *
 *      FILE VARIABLE
 *
 ******************************************************************/
MsgEncoder  msg_encoders[] = 
{
    { CMD_W_MEM, enc_c_wmem },
    { CMD_R_MEM, NULL },
    { CMD_E_MEM, NULL },
    { CMD_R_SIG, enc_c_rsig },
    { CMD_R_USIG, NULL },
    { CMD_W_USIG, NULL },

    { RES_W_MEM, enc_r_wmem },
    { RES_R_MEM, NULL },
    { RES_E_MEM, NULL },
    { RES_R_SIG, enc_r_rsig },
    { RES_R_USIG, NULL },
    { RES_W_USIG, NULL },

    { CMD_NULL, NULL },
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
AT89S_EID
msg_encode ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len )
{
    MsgEncoder* msgenc_ptr = NULL;
    AT89S_EID eid = EID_NOK;
    int i = 0;

    if (atmsg == NULL || data_buf == NULL || data_len == NULL)
        return EID_ARG_NULL;

    // find message encoder
    for (msgenc_ptr = &msg_encoders[0];
         msgenc_ptr->msgtype != CMD_NULL;
         msgenc_ptr++)
    {
          if (msgenc_ptr->msgtype == atmsg->msgt)
              break;
    }

    if (msgenc_ptr->msgtype == CMD_NULL)
    {
        eid = EID_ENC_NOT_FOUND;
    }
    else if (msgenc_ptr->msgenc_fptr != NULL)
    {
        // initialize
        *data_len = 0;
        // encode message header
        data_buf[(*data_len)++] = atmsg->msgt;
        // encode message content
        eid = msgenc_ptr->msgenc_fptr(atmsg, data_buf, data_len);
        // insert Padding Byte
        if (eid == EID_OK)
        {
            for (i = *data_len; i < MSG_SIZE; ++i)
            {
                data_buf[i] = PADDING_BYTE;
            }
        }
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
static AT89S_EID
enc_c_rsig ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len )
{
    if (atmsg == NULL || data_buf == NULL || data_len == NULL)
        return EID_ARG_NULL;

    data_buf[(*data_len)++] = atmsg->data.cmd_read_sign.signature_type;

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
static AT89S_EID
enc_r_rsig ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len )
{
    if (atmsg == NULL || data_buf == NULL || data_len == NULL)
        return EID_ARG_NULL;

    data_buf[(*data_len)++] = atmsg->data.res_read_sign.signature[0];
    data_buf[(*data_len)++] = atmsg->data.res_read_sign.signature[1];
    data_buf[(*data_len)++] = atmsg->data.res_read_sign.signature[2];

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
static AT89S_EID
enc_c_wmem ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len )
{
    int i = 0;

    if (atmsg == NULL || data_buf == NULL || data_len == NULL)
        return EID_ARG_NULL;

    data_buf[(*data_len)++] = atmsg->data.cmd_write_mem.memtype;
    data_buf[(*data_len)++] = ((atmsg->data.cmd_write_mem.address & 0xFF00) >> 8);
    data_buf[(*data_len)++] = (atmsg->data.cmd_write_mem.address & 0x00FF);
    data_buf[(*data_len)++] = atmsg->data.cmd_write_mem.size;
    data_buf[(*data_len)++] = ((atmsg->data.cmd_write_mem.crc & 0xFF00) >> 8);
    data_buf[(*data_len)++] = (atmsg->data.cmd_write_mem.crc & 0X00FF);

    for (i = 0; i < atmsg->data.cmd_write_mem.size; ++i)
    {
        data_buf[(*data_len)++] = atmsg->data.cmd_write_mem.data[i];
    }


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
static AT89S_EID
enc_r_wmem ( AT89S_Msg_t* atmsg,
             char* data_buf,
             int* data_len )
{
    if (atmsg == NULL || data_buf == NULL || data_len == NULL)
        return EID_ARG_NULL;

    return EID_OK;
}


