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
    IspMessage ispmsg;

    recv_size = MESSAGE_SIZE;
    eid = serial_recv(&serial_dev,
                      recv_buffer,
                      &recv_size);
    printf("Received %d bytes\n", recv_size);

    if (eid == EID_OK)
    {
        eid = decode_message(recv_buffer, &ispmsg);
        if (eid == EID_OK)
        {
            // process message
            switch (ispmsg.hdr.typ)
            {
                case MSGT_ACK:
                    printf("Message sent successfully: %d\n", ispmsg.msg.ack.resp);
                    break;

                case MSGT_SIG_READ:
                    for (int i = 0; i < recv_size; ++i)
                        printf("%02X ", recv_buffer[i]);
                    printf("\n");
                    printf("-------------\n");
                    printf("Length: %4d\n", ispmsg.hdr.len);
                    printf("CRC: %04x\n", ispmsg.hdr.crc);
                    printf("Signature: ");
                    for (int i = 0; i < ispmsg.hdr.len; ++i)
                        printf("%02X ", ispmsg.msg.data[i]);
                    printf("\n");
                    break;

                default:
                    printf("Invalid message type: %d\n", ispmsg.hdr.typ);
                    break;
            }
        }
    }

    return (void *)(eid);
}


int
main ( int argc,
       char** argv )
{
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
        devname = argv[2];
    }

    // init
    memset(&serial_dev, 0, sizeof(SerialDevice));

    // prepare message header
    ispmsg.hdr.typ = 0x07;
    ispmsg.hdr.len = 0;
    ispmsg.hdr.crc = 0x0000; 

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
    else
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




#if 0
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
    ispmsg.hdr.typ = MSGT_MEM_WRITE;
    ispmsg.hdr.len = DATA_SIZE;
    ispmsg.hdr.crc = 0x0000; 
 
    // message data content
    eid = read_hexfile(hexfile, ispmsg.msg.data, &ispmsg.hdr.len);
    if (eid != EID_OK)
    {
        printf("Could not read hex file: eid=%d errno=%s\n",
                eid,
                strerror(errno));
        goto EXIT_PROGRAM;
    }
    printf("Read hex file successfully: %d bytes\n", ispmsg.hdr.len);

    // data crc
    ispmsg.hdr.crc = gen_crc16(ispmsg.msg.data, ispmsg.hdr.len);
    printf("Data CRC: %04x\n", ispmsg.hdr.crc);

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
    else
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
#endif 
