/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include "Arduino.h"
#include "at89s52.h"
#include "crc.h"



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



#define  DEFAULT_BAUDRATE    9600


/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/





/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/
/*
 * Global raw data message
 */
char      g_data_buf[MSG_SIZE] = { 0 };
uint16_t  g_data_len = 0;
uint8_t   g_data_crc = 0;

/*
 * Global AT89S Message
 */
AT89S_Msg_t  g_at89msg;


/*
 * Global Error ID
 */
AT89S_EID  g_eid = EID_OK;


/*******************************************************************
 *
 *      FUNCTION DEFINITION
 *
 ******************************************************************/





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
    Serial.begin(DEFAULT_BAUDRATE);

    // setup AT89s pin
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_SCK, OUTPUT);
    pinMode(PIN_MOSI, OUTPUT);
    pinMode(PIN_MISO, INPUT);

    // reset pin voltage to default
    digitalWrite(PIN_RST, LOW);
    digitalWrite(PIN_SCK, HIGH);
    digitalWrite(PIN_MOSI, HIGH);
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
    // get data from commander
    int nbytes = Serial.available();
    while (g_data_len < MSG_SIZE && nbytes > 0)
    {
        nbytes--;
        g_data_buf[g_data_len++] = Serial.read();
    }

    // decode message from commander
    if (g_data_len >= MSG_SIZE)
    {
        g_eid = decode_msg(g_data_buf, g_data_len, &g_at89msg);
    }
}
