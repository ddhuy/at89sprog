#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <hexio.h>
#include <serial.h>
#include <protocol.h>
#include <crc16.h>


uint8_t send_buffer[MESSAGE_SIZE] = { 0 };
uint16_t send_size = 0;

uint8_t  recv_buffer[MESSAGE_SIZE] = { 0 };
uint16_t recv_size = 0;

pthread_t tid_receiving = -1;

SerialConfig serial_cfg =
{
    .ispeed = B9600,
    .ospeed = B9600,
};

SerialDevice serial_dev;


void *
get_data_routine ( void* parg )
{
    ISP_EID eid = EID_OK;

    recv_size = 64;
    eid = serial_recv(&serial_dev,
                      recv_buffer,
                      &recv_size);
    
    return (void *)(eid);
}


int
main ( int argc,
       char** argv )
{
    char* hexfile  = NULL;
    char* devname  = NULL;
    ISP_EID eid = EID_NOK;
    IspMessage ispmsg;

    /*
     * TODO: enhance the argument parsing
     */
    if (argc < 5)
    {
        printf("[Error Usage]:\n"
                "at892prog -f <hex file name>\n"
                "          -d <serial device>\n");
        return -1;
    }
    else
    {
        hexfile = argv[2];
        devname = argv[4];
    }

    // init
    memset(&serial_dev, 0, sizeof(SerialDevice));

    // prepare message header
    ispmsg.hdr.msgtyp = MSGT_MEM_WRITE;
    ispmsg.hdr.msglen = DATA_SIZE;
    ispmsg.hdr.msgcrc = 0x0000;

    // message data content
    eid = read_hexfile(hexfile, ispmsg.data, &ispmsg.hdr.msglen);
    if (eid != EID_OK)
    {
        printf("Could not read hex file: eid=%d errno=%s\n",
                eid,
                strerror(errno));
        goto EXIT_PROGRAM;
    }
    printf("Read hex file successfully: %d bytes\n", ispmsg.hdr.msglen);

    // data content crc
    ispmsg.hdr.msgcrc = gen_crc16(ispmsg.data, ispmsg.hdr.msglen);
    printf("Data content CRC: %04x\n", ispmsg.hdr.msgcrc);

    // encode message
    eid = encode_message(&ispmsg, send_buffer);
    if (eid != EID_OK)
    {
        printf("Encode message failed: eid=%d errno=%s\n",
                eid,
                strerror(errno));
        goto EXIT_PROGRAM;
    }
    printf("Message was encoded successfully\n");

    // open connection to TTY device
    eid = serial_open(devname,
                      &serial_dev,
                      &serial_cfg);
    if (eid != EID_OK)
    {
        printf("Open TTY device failed: eid=%d errno=%s\n",
                eid,
                strerror(errno));
        goto EXIT_PROGRAM;
    }
    printf("Connect to %s successfully\n", devname);

    // create TTY receiving thread
    if (pthread_create(&tid_receiving,
                       NULL,
                       get_data_routine,
                       NULL) < 0)
    {
        printf("Cannot create thread: %s\n", strerror(errno));
        goto EXIT_PROGRAM;
    }
 
    // send message to device
    eid = serial_send(&serial_dev,
                      send_buffer,
                      MESSAGE_SIZE);
    if (eid != EID_OK)
    {
        printf("Message sent failed: eid=%d errno=%s\n",
                eid,
                strerror(errno));
        goto EXIT_PROGRAM;
    }
    printf("Sent %d bytes\n", MESSAGE_SIZE);

    
EXIT_PROGRAM:

    // waiting for thread done
    pthread_join(tid_receiving, NULL);
    if (eid != EID_OK)
    {
        printf("Received serial data failed: eid=%d errno=%s\n",
                eid,
                strerror(errno));
    }
    else
    {
        printf("Received len=%d  str=\'%s\'\n",
                recv_size,
                recv_buffer);
    }


    eid = serial_close(&serial_dev);
    if (eid != EID_OK)
    {
        printf("Device closed fail: eid=%d errno=%s\n",
                eid,
                strerror(errno));
    }
    printf("Device closed\n");

    return 0;
}
