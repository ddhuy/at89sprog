#include <Arduino.h>

#include "protocol.h"
#include "crc16.h"


#define  LED_01    10
#define  LED_02    9
#define  LED_03    8


uint8_t  data_buffer[MESSAGE_SIZE] = { 0 };
uint16_t data_size = 0;
uint16_t crc = 0x0000;
ISP_EID  eid = EID_OK;


IspMessage ispmsg;


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
//        digitalWrite(LED_01, HIGH);
//        delay(400);
//        digitalWrite(LED_01, LOW);
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

    // prepare acknowledge message
    ispmsg.hdr.typ = MSGT_ACK;
    ispmsg.hdr.len = 2;
    ispmsg.hdr.crc = 0x0000;
    ispmsg.msg.ack.resp = eid;
    // encode message
    if (encode_message(&ispmsg, data_buffer) == EID_OK)
    {
        // send message
        eid = send_message(data_buffer, MESSAGE_SIZE);
    }
}

void
setup ( void )
{
    pinMode(LED_01, OUTPUT);
    pinMode(LED_02, OUTPUT);
    pinMode(LED_03, OUTPUT);

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
 
    Serial.begin(9600);
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
