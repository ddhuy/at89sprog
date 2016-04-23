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
/*
 *
 */
static AT89S_EID
start_reprogram ( void );

/*
 *
 */
static void
reset_mcu( void );

/*
 *
 */
static unsigned char
send_mcu_cmd ( unsigned long command );

/*
 *
 */
static unsigned char
send_mcu_byte ( unsigned char command );

/*
 *
 */
static unsigned char
read_mcu_byte ( void );

/*
 *
 */
static void
erase_chip ( void );

/*
 *
 */
static AT89S_EID
process_message ( AT89S_Msg_t* atmsg_ptr );

/*
 *
 */
static AT89S_EID
read_device_signature ( Msg_Signature_t * signature_msg_ptr );

/*
 *
 */
static AT89S_EID
write_data ( Msg_Memmory_t * mem_msg_ptr );

/*
 *
 */
static AT89S_EID
read_data ( Msg_Memmory_t * mem_msg_ptr );

/*
 *
 */
static AT89S_EID
read_lock_bit ( Msg_LockBit_t* msg_lbit_ptr );

/*
 *
 */
static AT89S_EID
write_lock_bit ( Msg_LockBit_t* msg_lbit_ptr );


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
/*
 *
 */
static AT89S_EID 
start_reprogram ( void )
{
    uint32_t resp = 0;

    digitalWrite(PIN_RST, HIGH);
    digitalWrite(PIN_SCK, LOW);
    delayMicroseconds(T_RESET);

    resp = send_mcu_cmd(PROGRAM_ENABLE);

    if (resp == MCU_PROGRAM_ENA_OK)
        return EID_OK;
    return EID_NOK;
}

/*
 *
 */
static void
reset_mcu( void )
{
    digitalWrite(PIN_RST, LOW);
}


/*
 *
 */
static void
erase_chip ( void )
{
    send_mcu_cmd(CHIP_ERASE);
    delay(T_ERASE);
}

/*
 *
 */
static unsigned char
send_mcu_cmd ( unsigned long command )
{
    unsigned long resp = 0;
    signed char i = 0;

    for (i = 31; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        digitalWrite(PIN_MOSI, ((command >> i) & 0x01));
        digitalWrite(PIN_SCK, HIGH);
        resp |= ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    digitalWrite(PIN_SCK, HIGH);

    return ((unsigned char) resp);
}


/*
 *
 */
static unsigned char
send_mcu_byte ( unsigned char command )
{
    unsigned long resp = 0;
    signed char i = 0;

    for (i = 7; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, LOW);
        digitalWrite(PIN_MOSI, ((command >> i) & 0x01));
        digitalWrite(PIN_SCK, HIGH);
        resp |= ((digitalRead(PIN_MISO) & 0x01) << i);
    }
    digitalWrite(PIN_SCK, LOW);

    return resp;
}

/*
 *
 */
static unsigned char
read_mcu_byte ( void )
{
    signed char i = 0;
    unsigned char read_byte = 0;

    for (i = 7; i >= 0; --i)
    {
        digitalWrite(PIN_SCK, HIGH);
        read_byte |= ((digitalRead(PIN_MISO) & 0x01) << i);
        digitalWrite(PIN_SCK, LOW);
    }

    return read_byte;
}


/*
 *
 */
static AT89S_EID
process_message ( AT89S_Msg_t* atmsg_ptr )
{
    AT89S_EID eid = EID_OK;

    if (atmsg_ptr == NULL)
    {
        eid = EID_ARG_NULL;
    }
    else
    {
        switch (atmsg_ptr->msgt)
        {
            case CMD_W_MEM:
                eid = write_data(&atmsg_ptr->data.msg_memory);
                break;

            case CMD_R_MEM:
                eid = read_data(&atmsg_ptr->data.msg_memory);
                break;

            case CMD_E_MEM:
                eid = erase_chip();
                break;

            case CMD_R_SIG:
                eid = read_device_signature(&atmsg_ptr->data.msg_signature);
                break;

            case CMD_R_LBIT:
                eid = read_lock_bit(&atmsg_ptr->data.msg_lbit);
                break;

            case CMD_W_LBIT:
                eid = write_lock_bit(&atmsg_ptr->data.msg_lbit);
                break;

            default:
                break;
        }
    }

    return eid;
}

/*
 *
 */
static AT89S_EID
read_device_signature ( Msg_Signature_t * signature_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    unsigned long res = 0;

    if (signature_msg_ptr->type == SIGN_DEV)
    {
        // start reprogram mode
        start_reprogram();
        // read device signature
        res = send_mcu_cmd(READ_SIGNATURE_BYTE1);
        signature_msg_ptr->signature[0] = ((unsigned char *) &res)[0];
        res = send_mcu_cmd(READ_SIGNATURE_BYTE2);
        signature_msg_ptr->signature[1] = ((unsigned char *) &res)[0];
        res = send_mcu_cmd(READ_SIGNATURE_BYTE3);
        signature_msg_ptr->signature[2] = ((unsigned char *) &res)[0];
        // reset when done
        reset_mcu();
    }
    else
    {
        eid = EID_ARG_INVALID;
    }

    return eid;
}


/*
 *
 */
static AT89S_EID
write_data ( Msg_Memmory_t * mem_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    uint8_t i = 0, resp = 0;

    if (mem_msg_ptr)
    {
        // start reprogram mode
        start_reprogram();
        // erase flash first
        erase_chip();

        // send data
        if (mem_msg_ptr->mode == BYTE_MODE)
        {
            for (i = 0; i < mem_msg_ptr->size; ++i)
            {
                send_mcu_byte(0x40);
                send_mcu_byte(0x00);
                send_mcu_byte(i);
                resp = send_mcu_byte(mem_msg_ptr->data[i]);
                delayMicroseconds(T_SWC);
            }
        }

        // flash data done, reset target MCU
        reset_mcu();


    }
    else
    {
        eid = EID_ARG_INVALID;
    }


    return eid;
}

/*
 *
 */
static AT89S_EID
read_data ( Msg_Memmory_t * mem_msg_ptr )
{
    AT89S_EID eid = EID_OK;
    uint32_t i = 0, j = 0;


    if (mem_msg_ptr)
    {
        // start reprogram mode
        start_reprogram();

        // read data;
        for (i = 0; i < mem_msg_ptr->size; ++i)
        {
            send_mcu_byte(0x20);
            send_mcu_byte(0x00);
            send_mcu_byte(i);
            mem_msg_ptr->data[i] = (uint8_t) read_mcu_byte();
            delayMicroseconds(T_SWC);
        }

        // flash data done, reset target MCU
        reset_mcu();
    }
    else
    {
        eid = EID_ARG_INVALID;
    }


    return eid;
}

/*
 *
 */
static AT89S_EID
read_lock_bit ( Msg_LockBit_t* msg_lbit_ptr )
{
    AT89S_EID eid = EID_OK;

    // program enable
    eid = start_reprogram();
    // read lock bit
    if (eid == EID_OK)
    {
        msg_lbit_ptr->lock_bit[0] = send_mcu_cmd(READ_LOCK_BIT);
        msg_lbit_ptr->lock_bit[1] = send_mcu_cmd(READ_LOCK_BIT);
        msg_lbit_ptr->lock_bit[2] = send_mcu_cmd(READ_LOCK_BIT);
    }
    // reset MCU
    reset_mcu();

    return eid;
}


/*
 *
 */
static AT89S_EID
write_lock_bit ( Msg_LockBit_t* msg_lbit_ptr )
{
    AT89S_EID eid = EID_OK;

    // program enable
    eid = start_reprogram();
    // read lock bit
    if (eid == EID_OK)
    {
        msg_lbit_ptr->lock_bit[0] = send_mcu_cmd(READ_LOCK_BIT);
        msg_lbit_ptr->lock_bit[1] = send_mcu_cmd(READ_LOCK_BIT);
        msg_lbit_ptr->lock_bit[2] = send_mcu_cmd(READ_LOCK_BIT);
    }
    // reset MCU
    reset_mcu();

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
//    // get data from commander
//    int nbytes = Serial.available();
//    while (g_data_len < MSG_SIZE && nbytes > 0)
//    {
//        nbytes--;
//        g_data_buf[g_data_len++] = Serial.read();
//    }
//
//    // decode message from commander
//    if (g_data_len >= MSG_SIZE)
//    {
//        g_eid = decode_msg(g_data_buf, g_data_len, &g_at89msg);
//        if (g_eid == EID_OK)
//        {
//            g_eid = process_message(&g_at89msg);
//            if (g_eid == EID_OK)
//            {}
//        }
//
//        // reset value for next message
//        g_data_len = 0;
//    }

    char cmd;
    char resp[256] = { 0 };
    AT89S_EID eid = EID_NOK;
    Msg_Signature_t signature_msg;
    Msg_Memory_t memory_msg;

    static unsigned char sample_data[] = { 0x75, 0xA0, 0xAA, 0x12,
                                           0x00, 0x0E, 0x75, 0xA0,
                                           0x55, 0x12, 0x00, 0x0E,
                                           0x80, 0xF2, 0x79, 0x00,
                                           0x78, 0x00, 0xD8, 0xFE,
                                           0x00, 0x00, 0x00, 0x00,
                                           0x00, 0xD9, 0xF7, 0x22 };

    if (Serial.available())
    {
        cmd = Serial.read();

        switch(cmd)
        {
            case 's':
                signature_msg.type = SIGN_DEV;
                eid = read_device_signature(&signature_msg);
                if (eid == EID_OK)
                {
                    snprintf(resp, sizeof(resp),
                             "%02x %02x %02x",
                             signature_msg.signature[0],
                             signature_msg.signature[1],
                             signature_msg.signature[2]);
                    Serial.println(resp);
                }
                break;

            case 'w':
                memory_msg.size    = 0x1C;
                memory_msg.address = 0x0000;
                memory_msg.rectype = 0x00;
                memory_msg.crc     = 0xF9;
                memory_msg.mode    = BYTE_MODE;
                memcpy(memory_msg.data, sample_data, memory_msg.size);

                eid = write_data(&memory_msg);
                snprintf(resp, sizeof(resp), "\nWrite data %d !", eid);
                Serial.println(resp);

                break;

            case 'r':
                memory_msg.size = 0x1C;
                eid = read_data(&memory_msg);
                snprintf(resp, sizeof(resp), "\nRead data %d !", eid);
                Serial.println(resp);
                break;

            default:
                break;
        }

    }
}
