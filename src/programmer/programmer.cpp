/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <Arduino.h>

#include "at89sprog.h"
#include "at89s52.h"
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
static unsigned long
send_mcu_cmd ( unsigned long command );

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

/*
 *
 */
static AT89S_EID
process_write_flash ( void );


/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/

unsigned char data_buf[AT89S_MESSAGE_SIZE] = { 0 };
int           data_len = 0;
uint16_t      crc = 0x0000;

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
    else if (eid == AT89S_EID_PROT_LEN)
    {
        digitalWrite(LED_01, HIGH);
        digitalWrite(LED_02, LOW);
        digitalWrite(LED_03, LOW);
    }
    else if (eid == AT89S_EID_PROT_CRC)
    {
        digitalWrite(LED_01, LOW);
        digitalWrite(LED_02, HIGH);
        digitalWrite(LED_03, LOW);
    }
    else if (eid == AT89S_EID_PROT_CMD)
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
static unsigned long
send_mcu_cmd ( unsigned long command )
{
    unsigned long res = 0;
    signed char i = 0;

    for (i = 31; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);

        if ((command >> i) & 0x01)
            digitalWrite(PIN_MOSI, HIGH);
        else
            digitalWrite(PIN_MOSI, LOW);

        // pull high SCK to start write
        digitalWrite(PIN_SCK, HIGH); 

        res |= ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    // turn SCK back to default
    digitalWrite(PIN_SCK, HIGH);

    return res;
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

    while (len > 0)
    {
        byte_sent = Serial.write(buf, len);
        if (byte_sent > 0)
        {
            buf += byte_sent;
            len -= byte_sent;
        }
        else
            break;
    }

    if (len != 0)
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
process_write_flash ( void )
{
    unsigned char len = 0, i = 0;
    unsigned long data = 0;

    // reset target to reprogramming mode
    digitalWrite(PIN_RST, HIGH);
    delay(10);

    // program enable
    data = send_mcu_cmd(PROGRAM_ENABLE);
    at89s_msg.data[len++] = ((unsigned char *) &data)[0];
    // write hex data to MCU flash
    for (i = 0; i < at89s_msg.len; ++i)
    {
        // always erase first

        // then writing

    }
    
    // prepare acknowledge message
    at89s_msg.cmd = AT89S_CMD_WRITE_FLS;
    at89s_msg.len = len;
    at89s_msg.crc = gen_crc16(at89s_msg.data, at89s_msg.len);
    // encode message
    eid = enc_message(&at89s_msg, data_buf, &data_len);
    if (eid == AT89S_EID_OK)
    {
        // send message
        eid = send_response(data_buf, AT89S_MESSAGE_SIZE);
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
process_read_signature ( void )
{
    unsigned char len = 0;
    unsigned long res = 0x00000000;

    // reset target to reprogramming mode
    digitalWrite(PIN_RST, HIGH);
    delay(10);

    // read device signature
    res = send_mcu_cmd(PROGRAM_ENABLE);
    at89s_msg.data[len++] = ((unsigned char *) &res)[0];
    res = send_mcu_cmd(READ_SIGNATURE_BYTE1);
    at89s_msg.data[len++] = ((unsigned char *) &res)[0];
    res = send_mcu_cmd(READ_SIGNATURE_BYTE2);
    at89s_msg.data[len++] = ((unsigned char *) &res)[0];
    res = send_mcu_cmd(READ_SIGNATURE_BYTE3);
    at89s_msg.data[len++] = ((unsigned char *) &res)[0];

    // prepare acknowledge message
    at89s_msg.cmd = AT89S_CMD_READ_SIGN;
    at89s_msg.len = len;
    at89s_msg.crc = gen_crc16(at89s_msg.data, at89s_msg.len);
    // encode message
    eid = enc_message(&at89s_msg, data_buf, &data_len);
    if (eid == AT89S_EID_OK)
    {
        // send message
        eid = send_response(data_buf, AT89S_MESSAGE_SIZE);
    }

    return eid; 
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

    // decode & process message
    if (data_len >= AT89S_MESSAGE_SIZE)
    {
        eid = dec_message(data_buf, data_len, &at89s_msg);
        if (eid == AT89S_EID_OK)
        {
            switch (at89s_msg.cmd)
            {
                case AT89S_CMD_READ_SIGN:
                    eid = process_read_signature();
                    break;

                case AT89S_CMD_WRITE_FLS:
                    eid = process_write_flash();
                    break;

                default:
                    eid = AT89S_EID_PROT_CMD;
                    break;
            }
        }
        // toggle LED to display error
        error_handling(eid);
        // process done, continue receiving data
        data_len = 0;
    }
}
