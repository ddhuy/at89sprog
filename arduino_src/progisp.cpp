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


IspMessage ispmsg = 
{
    {
        .msgtyp = 0,
        .msglen = 0,
        .msgcrc = 0x0000,
    },
    .data = { 0 },
};


void
error_handling (ISP_EID eid)
{
    if (eid == EID_OK)
    {
        digitalWrite(LED_01, HIGH);
        delay(400);
        digitalWrite(LED_01, LOW);
        delay(400);
        digitalWrite(LED_01, HIGH);
        delay(400);
        digitalWrite(LED_01, LOW);
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

    Serial.print("Received: ");
    Serial.print(data_size);
    Serial.println(" bytes");

    Serial.print("CRC: ");
    Serial.println(crc, HEX);

    Serial.print("MSGCRC: ");
    Serial.println(ispmsg.hdr.msgcrc, HEX);
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
        digitalWrite(LED_01, HIGH);
        delay(500);
        digitalWrite(LED_01, LOW);
        delay(500);
        digitalWrite(LED_01, HIGH);
        delay(500);
        digitalWrite(LED_01, LOW);
 
        // decode message
        eid = decode_message(data_buffer, &ispmsg);
        // check crc
        if (eid == EID_OK)
        {
            crc = gen_crc16(ispmsg.data, ispmsg.hdr.msglen);
            if (crc != ispmsg.hdr.msgcrc)
                eid = EID_MSG_BAD_CRC;
        }

        // toggle LED to display error
        error_handling(eid);
        // process done, continue receiving data
        data_size = 0;
    }
}
