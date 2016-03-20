#include <Arduino.h>

#include "protocol.h"
#include "crc16.h"


#define  LED_01    10
#define  LED_02    9
#define  LED_03    8

#define  PIN_RST   49
#define  PIN_MISO  50
#define  PIN_MOSI  51
#define  PIN_SCK   52


uint8_t  data_buffer[MESSAGE_SIZE] = { 0 };
uint16_t data_size = 0;
uint16_t crc = 0x0000;
ISP_EID  eid = EID_OK;


IspMessage ispmsg;


static unsigned long
isp_send_cmd (unsigned long command)
{
    unsigned long ibyte = 0;
    char i;

    for (i = 31; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        delayMicroseconds(2);

        if ((command >> i) & 0x01)
            digitalWrite(PIN_MOSI, HIGH);
        else
            digitalWrite(PIN_MOSI, LOW);

        digitalWrite(PIN_SCK, HIGH); // pull high SCK to start write
        delayMicroseconds(2);

        ibyte |= (digitalRead(PIN_MISO) & 0x01) << i;

    }
    // turn SCK back to default
    digitalWrite(PIN_SCK, HIGH);

    return ibyte;
}

static ISP_EID
send_message ( uint8_t* buffer,
               uint16_t length )
{
    uint16_t byte_sent = 0;

    if (buffer != NULL)
    {
        while (byte_sent < length)
        {
            byte_sent = Serial.write(buffer, length);
            if (byte_sent > 0)
            {
                buffer += byte_sent;
                length -= byte_sent;
            }
            else
                break;
        }
    }

    if (byte_sent != length)
        return EID_COM_SEND;
    return EID_OK;
}


static void
error_handling (ISP_EID eid)
{
    if (eid == EID_OK)
    {
        digitalWrite(LED_01, HIGH);
        delay(400);
        digitalWrite(LED_01, LOW);
        delay(400);
    }
    else if (eid == EID_MSG_BAD_LEN)
    {
        digitalWrite(LED_01, HIGH);
        digitalWrite(LED_02, LOW);
        digitalWrite(LED_03, LOW);
    }
    else if (eid == EID_MSG_BAD_CRC)
    {
        digitalWrite(LED_01, LOW);
        digitalWrite(LED_02, HIGH);
        digitalWrite(LED_03, LOW);
    }
    else
    {
        digitalWrite(LED_01, HIGH);
        digitalWrite(LED_02, HIGH);
        digitalWrite(LED_03, LOW);
    }
}

void
resp_ack(ISP_EID eid)
{
    // prepare acknowledge message
    ispmsg.hdr.typ = MSGT_ACK;
    ispmsg.hdr.len = 2;
    ispmsg.hdr.crc = 0x0000;
    ispmsg.msg.ack.resp = eid;
    // encode message
    if (encode_message(&ispmsg, data_buffer) == EID_OK)
    {
        // send message
        send_message(data_buffer, MESSAGE_SIZE);
    }
}

ISP_EID
process_read_signature ( void )
{
    unsigned tmp;
    unsigned char len = 0;
    unsigned long ibyte = 0;

    // reset target to reprogramming mode
    digitalWrite(PIN_RST, HIGH);
    delay(10);

//    // program enable
//    ispmsg.msg.data[len++] = isp_send_cmd(0xAC);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x53);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x69);
//    // read signature byte 1
//    ispmsg.msg.data[len++] = isp_send_cmd(0x28);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    // read signature byte 2
//    ispmsg.msg.data[len++] = isp_send_cmd(0x28);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x01);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    // read signature byte 3
//    ispmsg.msg.data[len++] = isp_send_cmd(0x28);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x02);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
//    ispmsg.msg.data[len++] = isp_send_cmd(0x00);
    
    ibyte = isp_send_cmd(0xAC53FF69);
    ispmsg.msg.data[len++] = ((unsigned char *) &ibyte)[0];

    ibyte = isp_send_cmd(0x28000000);
    ispmsg.msg.data[len++] = ((unsigned char *) &ibyte)[0];

    ibyte = isp_send_cmd(0x28010000);
    ispmsg.msg.data[len++] = ((unsigned char *) &ibyte)[0];

    ibyte = isp_send_cmd(0x28020000);
    ispmsg.msg.data[len++] = ((unsigned char *) &ibyte)[0];


    // prepare acknowledge message
    ispmsg.hdr.typ = MSGT_SIG_READ;
    ispmsg.hdr.len = len;
    ispmsg.hdr.crc = gen_crc16(ispmsg.msg.data, ispmsg.hdr.len);

    // encode message
    if (encode_message(&ispmsg, data_buffer) == EID_OK)
    {
        // send message
        send_message(data_buffer, MESSAGE_SIZE);
    }

    return EID_OK; 
}

void
setup ( void )
{
    Serial.begin(9600);
 
    // setup AT89S pins
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_SCK, OUTPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_MISO, INPUT);
    // set pin voltage to default
    digitalWrite(PIN_RST, LOW);
    digitalWrite(PIN_SCK, HIGH);
    digitalWrite(PIN_MOSI, HIGH);
    
    // setup status LED
    pinMode(LED_01, OUTPUT);
    pinMode(LED_02, OUTPUT);
    pinMode(LED_03, OUTPUT);

    // blink status LED after setup done
    digitalWrite(LED_01, HIGH);
    delay(100);
    digitalWrite(LED_02, HIGH);
    delay(100);
    digitalWrite(LED_03, HIGH);
    delay(100);
    digitalWrite(LED_01, LOW);
    delay(100);
    digitalWrite(LED_02, LOW);
    delay(100);
    digitalWrite(LED_03, LOW);
}

void
loop ( void )
{
    int nbytes = Serial.available();
    while (data_size < MESSAGE_SIZE && nbytes > 0)
    {
        nbytes--;
        data_buffer[data_size++] = Serial.read();
    }

    if (data_size >= MESSAGE_SIZE)
    {
        // decode message
        eid = decode_message(data_buffer, &ispmsg);
        // process message
        if (eid == EID_OK)
        {
            switch(ispmsg.hdr.typ)
            {
                case MSGT_MEM_WRITE:
                    // check crc
                    crc = gen_crc16(ispmsg.msg.data, ispmsg.hdr.len);
                    if (crc != ispmsg.hdr.crc)
                        eid = EID_MSG_BAD_CRC;
                    else
                        eid = EID_OK;
                    // send ack to host
                    resp_ack(eid);
                    break;

                case MSGT_SIG_READ:
                    eid = process_read_signature();
                    break;

                default:
                    break;
            }
        }

        // toggle LED to display error
        error_handling(eid);
        // process done, continue receiving data
        data_size = 0;
    }
}
