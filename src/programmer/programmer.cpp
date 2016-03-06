/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <Arduino.h>

#include "at89sprog.h"
#include "crc16.h"



/*******************************************************************
 *
 *      PREPROCESSORS
 *
 ******************************************************************/
#define  LED_01    10
#define  LED_02    9
#define  LED_03    8

#define  PIN_RST   49
#define  PIN_MISO  50
#define  PIN_MOSI  51
#define  PIN_SCK   52



/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 *
 */
static void
error_handling ( AT89S_EID eid );

/*
 *
 */
static char
send_mcu_cmd ( char command );

/*
 *
 */
static AT89S_EID
send_response ( unsigned char* buf,
                int len );

/*
 *
 */
static AT89S_EID
process_read_signature ( void );


/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/

unsigned char data_buf[AT89S_MESSAGE_SIZE] = { 0 };
int           data_len = 0;
uint16_t  crc = 0x0000;

AT89S_EID     eid = AT89S_EID_NOK;
AT89S_Message at89s_msg;


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
static void
error_handling (AT89S_EID eid)
{
    if (eid == AT89S_EID_OK)
    {
        digitalWrite(LED_01, HIGH);
        delay(400);
        digitalWrite(LED_01, LOW);
        delay(400);
    }
    else if (eid == AT89S_EID_PROT_INVALID_LEN)
    {
        digitalWrite(LED_01, HIGH);
        digitalWrite(LED_02, LOW);
        digitalWrite(LED_03, LOW);
    }
    else if (eid == AT89S_EID_PROT_INVALID_CRC)
    {
        digitalWrite(LED_01, LOW);
        digitalWrite(LED_02, HIGH);
        digitalWrite(LED_03, LOW);
    }
    else if (eid == AT89S_EID_PROT_INVALID_CMD)
    {
        digitalWrite(LED_01, LOW);
        digitalWrite(LED_02, LOW);
        digitalWrite(LED_03, HIGH);
    }
    else
    {
        digitalWrite(LED_01, HIGH);
        digitalWrite(LED_02, HIGH);
        digitalWrite(LED_03, LOW);
    }
}

/*
 * Description:
 *
 *
 * Input:
 *
 *
 * Output:
 * 
 */
static char
send_mcu_cmd ( char command )
{
    unsigned char ibyte = 0;
    char i = 0;

    for (i = 7; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        delayMicroseconds(2);

        if ((command >> i) & 0x01)
            digitalWrite(PIN_MOSI, HIGH);
        else
            digitalWrite(PIN_MOSI, LOW);

        // pull high SCK to start write
        digitalWrite(PIN_SCK, HIGH); 
        delayMicroseconds(2);

        ibyte |= (unsigned char) ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    return ibyte;
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
send_response ( unsigned char* buf,
                int len )
{
    int byte_sent = 0;

    if (buf == NULL)
        return AT89S_EID_ARG_NULL;

    while (byte_sent < len)
    {
        byte_sent = Serial.write(buf, len);
        Serial.flush();
        if (byte_sent > 0)
        {
            buf += byte_sent;
            len -= byte_sent;
        }
        else
            break;
    }

    if (byte_sent < len)
        return AT89S_EID_SERIAL_SEND;
    return AT89S_EID_OK;
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
process_read_signature ( void )
{
    unsigned char len = 0;

    // reset target to reprogramming mode
    digitalWrite(PIN_RST, HIGH);
    delay(5);

    // program enable
    at89s_msg.data[len++] = send_mcu_cmd(0xAC);
    at89s_msg.data[len++] = send_mcu_cmd(0x53);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    at89s_msg.data[len++] = send_mcu_cmd(0x69);
    // read signature byte 1
    at89s_msg.data[len++] = send_mcu_cmd(0x28);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    // read signature byte 2
    at89s_msg.data[len++] = send_mcu_cmd(0x28);
    at89s_msg.data[len++] = send_mcu_cmd(0x01);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    // read signature byte 3
    at89s_msg.data[len++] = send_mcu_cmd(0x28);
    at89s_msg.data[len++] = send_mcu_cmd(0x02);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    at89s_msg.data[len++] = send_mcu_cmd(0x00);
    
//    ibyte = send_mcu_command(0xAC53FF69);
//    at89s_msg.data[len++] = ((unsigned char *) &ibyte)[0];
//    ibyte = send_mcu_command(0x28000000);
//    at89s_msg.data[len++] = ((unsigned char *) &ibyte)[0];
//    ibyte = send_mcu_command(0x28010000);
//    at89s_msg.data[len++] = ((unsigned char *) &ibyte)[0];
//    ibyte = send_mcu_command(0x28020000);
//    at89s_msg.data[len++] = ((unsigned char *) &ibyte)[0];


    // prepare acknowledge message
    at89s_msg.cmd = AT89S_CMD_READ_SIGN;
    at89s_msg.len = len;
    at89s_msg.crc = gen_crc16(at89s_msg.data, at89s_msg.len);

    // encode message
    eid = enc_message(&at89s_msg, data_buf, &data_len);
    if (eid == AT89S_EID_OK)
    {
        // send message
        eid = send_response(data_buf, data_len);
    }

    return AT89S_EID_OK; 
}


/*******************************************************************
 *
 *      ARDUINO SKELETON
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


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
void
loop ( void )
{
    int nbytes = Serial.available();
    while (data_len < AT89S_MESSAGE_SIZE && nbytes > 0)
    {
        nbytes--;
        data_buf[data_len++] = Serial.read();
    }

    if (data_len >= AT89S_MESSAGE_SIZE)
    {
        // decode message
        eid = dec_message(data_buf, data_len, &at89s_msg);
        // process message
        if (eid == AT89S_EID_OK)
        {
            switch(at89s_msg.cmd)
            {
                case AT89S_CMD_WRITE_FLS:
                    // check crc
                    crc = gen_crc16(at89s_msg.data, at89s_msg.len);
                    if (crc == at89s_msg.crc)
                        eid = AT89S_EID_OK;
                    else
                        eid = AT89S_EID_PROT_INVALID_CRC;
                    // send ack to host
                    break;

                case AT89S_CMD_READ_SIGN:
                    eid = process_read_signature();
                    break;

                default:
                    break;
            }
        }

        // toggle LED to display error
        error_handling(eid);
        // process done, continue receiving data
        data_len = 0;
    }
}
