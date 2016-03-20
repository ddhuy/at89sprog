#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "at89s.h"
#include "crc16.h"

int main ( int argc,
           char** argv )
{
    AT89S_Msg_t atmsg;
    AT89S_EID eid = EID_OK;
    char data_buf[MSG_SIZE] = { 0 };
    int  data_len = 0;

    // init data
    memset(&atmsg, 0, sizeof(AT89S_Msg_t));

    /*
     * data sample
     */
    atmsg.msgt = CMD_W_MEM;
    atmsg.data.cmd_write_mem.memtype = MEM_RAM;
    atmsg.data.cmd_write_mem.size    = 10;
    atmsg.data.cmd_write_mem.address = 0xABBA;
    atmsg.data.cmd_write_mem.data[0] = 0x01;
    atmsg.data.cmd_write_mem.data[1] = 0x02;
    atmsg.data.cmd_write_mem.data[2] = 0x03;
    atmsg.data.cmd_write_mem.data[3] = 0x04;
    atmsg.data.cmd_write_mem.data[4] = 0x05;
    atmsg.data.cmd_write_mem.data[5] = 0x06;
    atmsg.data.cmd_write_mem.data[6] = 0x07;
    atmsg.data.cmd_write_mem.data[7] = 0x08;
    atmsg.data.cmd_write_mem.data[8] = 0x09;
    atmsg.data.cmd_write_mem.data[9] = 0x0A;
    atmsg.data.cmd_write_mem.crc     = gen_crc16(atmsg.data.cmd_write_mem.data, 10);

    printf("Raw data:\n");
    for (int i = 0; i < MSG_SIZE; ++i)
        printf("%04X ", ((unsigned char *) &atmsg)[i]);
    printf("\n\n");


    // test encoder
    eid = encode_msg(&atmsg, data_buf, &data_len);
    if (eid != EID_OK)
    {
        printf("Encode error: %d\n", eid);
        return -1;
    }

    printf("Encoded data len: %d\n", data_len);
    for (int i = 0; i < MSG_SIZE; ++i)
        printf("%04X ", data_buf[i]);
    printf("\n\n");

    // test decoder
    memset(&atmsg, 0, sizeof(AT89S_Msg_t));

    eid = decode_msg(data_buf, data_len, &atmsg);
    if (eid != EID_OK)
    {
        printf("Decode error: %d\n", eid);
        return -1;
    }

    printf("Decoded data len: %d\n", data_len);
    printf("MsgType : %04X\n", atmsg.msgt);
    printf("MemType : %04X\n", atmsg.data.cmd_write_mem.memtype);
    printf("MemSize : %04X\n", atmsg.data.cmd_write_mem.size);
    printf("Address : %04X\n", atmsg.data.cmd_write_mem.address);
    printf("CRC     : %04X\n", atmsg.data.cmd_write_mem.crc);
    for (int i = 0; i < atmsg.data.cmd_write_mem.size; ++i)
        printf("%04X ", atmsg.data.cmd_write_mem.data[i]);
    printf("\n");

    return 0;
}
