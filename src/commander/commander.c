/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>
#include <memory.h>

#include "at89sprog.h"
#include "usbserial.h"
#include "hexio.h"
#include "crc16.h"


/*******************************************************************
 *
 *      MACRO & PREPROCESSOR
 *
 ******************************************************************/
/*
 * Argument commands
 */
#define  ARG_CMD_NULL               0x0000
#define  ARG_CMD_WRITE_FLS          0x0001
#define  ARG_CMD_READ_FLS           0x0002
#define  ARG_CMD_ERASE_FLS          0x0004
#define  ARG_CMD_WRITE_ROM          0x0008
#define  ARG_CMD_READ_ROM           0x0010
#define  ARG_CMD_ERASE_ROM          0x0020
#define  ARG_CMD_READ_SIGN          0x0040
#define  ARG_CMD_READ_USER_SIGN     0x0080
#define  ARG_CMD_WRITE_USER_SIGN    0x0100


/*******************************************************************
 *
 *      FILE DATA TYPE
 *
 ******************************************************************/
// type used for argument commands
typedef  unsigned int    ArgCmd;

/*
 *
 */
typedef struct AT89S_Type_t
{
    ArgCmd  arg_cmd;
    char* in_fls_file;
    char* out_fls_file;
    char* in_rom_file;
    char* out_rom_file;
    char* tty_filename;
    char* user_sign;

    unsigned char data_buf[AT89S_MESSAGE_SIZE];
    int  data_len;

    UsbSerialDevice tty_dev;
    AT89S_Message at89s_msg;

} AT89S_Type;



/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 *
 */
static void
show_usage ( void );

/*
 *
 */
static AT89S_EID
parse_argument ( int argc,
                 char** argv,
                 AT89S_Type* conf_ptr );

/*
 *
 */
static AT89S_EID
process_command ( AT89S_Type* conf_ptr );

/*
 *
 */
static void 
print_command ( AT89S_Type* conf_ptr );

/*
 *
 */
static AT89S_EID
receive_message ( AT89S_Type* conf_ptr );

/*
 *
 */
static void 
show_result ( AT89S_Type* conf_ptr );



/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/
/*
 * Main's arguments parsing
 */
// long options
static const struct option v_largs[] =
{
    { "write-flash",     required_argument,  NULL,  'w' },
    { "read-flash",      required_argument,  NULL,  'r' },
    { "erase-flash",     no_argument,  NULL,  'e' },

    { "write-eeprom",    required_argument,  NULL,  'b' },
    { "read-eeprom",     required_argument,  NULL,  'v' },
    { "erase-eeprom",    no_argument,  NULL,  'c' },

    { "read-sign",       no_argument,        NULL,  's' },
    { "read-user-sign",  no_argument,        NULL,  'u' },
    { "write-user-sign", required_argument,  NULL,  'm' },

    { "device",    required_argument,  NULL,  'd' },

    { "help",            no_argument,  NULL, '?' },

    { 0, 0, 0, 0 },
};
// short options
static const char* v_sargs = "w:r:eb:v:csum:d:?";



/*******************************************************************
 *
 *      function definition
 *
 ******************************************************************/
/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
parse_argument ( int argc,
                 char** argv,
                 AT89S_Type* conf_ptr )
{
    AT89S_EID eid = AT89S_EID_OK;
    int opt_idx = 0, selected_opt = 0;

    if (argc < 1 || argv == NULL || conf_ptr == NULL)
    {
        eid = AT89S_EID_ARG_NULL;
        return eid;
    }

    for (;;)
    {
        opt_idx = 0;
        selected_opt = getopt_long(argc, argv,
                                   v_sargs, v_largs,
                                   &opt_idx);
        if (selected_opt == -1)
            break;

        switch (selected_opt)
        {
            case 'r':
                conf_ptr->arg_cmd |= ARG_CMD_READ_FLS;
                conf_ptr->out_fls_file = optarg;
                break;

            case 'w':
                conf_ptr->arg_cmd |= ARG_CMD_WRITE_FLS;
                conf_ptr->in_fls_file = optarg;
                break;

            case 'e':
                conf_ptr->arg_cmd |= ARG_CMD_ERASE_FLS;
                break;

            case 'v':
                conf_ptr->arg_cmd |= ARG_CMD_READ_ROM;
                conf_ptr->out_rom_file = optarg;
                break;

            case 'b':
                conf_ptr->arg_cmd |= ARG_CMD_WRITE_ROM;
                conf_ptr->out_rom_file = optarg;
                break;

            case 'c':
                conf_ptr->arg_cmd |= ARG_CMD_ERASE_ROM;
                break;

            case 's':
                conf_ptr->arg_cmd |= ARG_CMD_READ_SIGN;
                break;

            case 'u':
                conf_ptr->arg_cmd |= ARG_CMD_READ_USER_SIGN;
                break;

            case 'm':
                conf_ptr->arg_cmd |= ARG_CMD_WRITE_USER_SIGN;
                conf_ptr->user_sign = optarg;
                break;

            case 'd':
                conf_ptr->tty_filename = optarg;
                break;

            case '?':
                show_usage();
                break;

            default:
                break;
        }
    }

    return eid;
} /* parse_argument */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
process_command ( AT89S_Type* conf_ptr )
{
    AT89S_EID eid = AT89S_EID_OK;

    if (conf_ptr == NULL)
    {
        eid = AT89S_EID_ARG_NULL;
        return eid;
    }

    // clean message
    memset(&conf_ptr->at89s_msg, 0, sizeof(AT89S_Message));
    //
    // prepare message
    if ((conf_ptr->arg_cmd & ARG_CMD_READ_FLS) == ARG_CMD_READ_FLS)
    {}
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_FLS) == ARG_CMD_WRITE_FLS)
    {
        // read hex file content
        eid = hexio_readall(conf_ptr->in_fls_file,
                            conf_ptr->at89s_msg.data,
                            &conf_ptr->data_len);
        if (eid == AT89S_EID_OK)
        {
            // prepare message header
            conf_ptr->at89s_msg.cmd = AT89S_CMD_WRITE_FLS;
            conf_ptr->at89s_msg.len = conf_ptr->data_len;
            conf_ptr->at89s_msg.crc = gen_crc16(conf_ptr->at89s_msg.data,
                                                conf_ptr->data_len);
            printf("Hex file length: %d\n", conf_ptr->at89s_msg.len);
            printf("Hex file CRC:    %04X\n", conf_ptr->at89s_msg.crc);
        }
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_ERASE_FLS) == ARG_CMD_ERASE_FLS)
    {
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_ROM) == ARG_CMD_READ_ROM)
    {}
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_ROM) == ARG_CMD_WRITE_ROM)
    {}
    else if ((conf_ptr->arg_cmd & ARG_CMD_ERASE_ROM) == ARG_CMD_ERASE_ROM)
    {}
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_SIGN) == ARG_CMD_READ_SIGN)
    {
        conf_ptr->at89s_msg.cmd = AT89S_CMD_READ_SIGN;
        conf_ptr->at89s_msg.len = 0;
        conf_ptr->at89s_msg.crc = 0x00;
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_USER_SIGN) == ARG_CMD_READ_USER_SIGN)
    {}
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_USER_SIGN) == ARG_CMD_WRITE_USER_SIGN)
    {}
    else
    {
        eid = AT89S_EID_ARG_INVALID;
    }

    if (eid == AT89S_EID_OK)
    {
        // open connection to device
        eid = usbserial_open(conf_ptr->tty_filename,
                             &conf_ptr->tty_dev,
                             US_BAUDRATE_9600,
                             US_CONFIG_8N1);
        if (eid == AT89S_EID_OK)
        {
            printf("Opened connection to device\n");
            
            eid = enc_message(&conf_ptr->at89s_msg, conf_ptr->data_buf, &conf_ptr->data_len);
            if (eid == AT89S_EID_OK)
            {
                printf("Encoded message successfully\n");

                eid = usbserial_send(&conf_ptr->tty_dev, conf_ptr->data_buf, conf_ptr->data_len);
                if (eid == AT89S_EID_OK)
                {
                    printf("Sent message successfully\n");
                    eid = receive_message(conf_ptr);
                }
            }
        }
    }

    return eid;

} /* process_argument */


AT89S_EID
receive_message ( AT89S_Type* conf_ptr )
{
    AT89S_EID eid;

    conf_ptr->data_len = AT89S_MESSAGE_SIZE;
    eid = usbserial_recv(&conf_ptr->tty_dev, conf_ptr->data_buf, conf_ptr->data_len);
    if (eid == AT89S_EID_OK)
    {
        eid = dec_message(conf_ptr->data_buf, conf_ptr->data_len, &conf_ptr->at89s_msg);
        printf("Received response: %s\n", AT89S_ERROR_MESSAGE[eid]);
    }

    return eid;
}

/*
 *
 */
static void 
show_result ( AT89S_Type* conf_ptr )
{
    if ((conf_ptr->arg_cmd & ARG_CMD_READ_SIGN) == ARG_CMD_READ_SIGN)
    {
        printf("Device Signature: ");
        for (int i = 0; i < conf_ptr->at89s_msg.len; ++i)
            printf("%04X ", conf_ptr->at89s_msg.data[i]);
        printf("\n");
    }
}


/*
 * purpose:
 *
 * input:
 *
 * output:
 *
 */
static void
show_usage ( void )
{
    printf("Programg Usage\n");
} /* show_usage */

/*
 *
 */
static void 
print_command ( AT89S_Type* conf_ptr )
{
    printf("Serial device:     %s\n", conf_ptr->tty_filename);
    printf("Argument command:  %d\n", conf_ptr->arg_cmd);

    if ((conf_ptr->arg_cmd & ARG_CMD_READ_FLS) == ARG_CMD_READ_FLS)
    {
        printf("Read flash. Output hex file: %s\n", conf_ptr->out_fls_file);
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_FLS) == ARG_CMD_WRITE_FLS)
    {
        printf("Write flash. Input hex file: %s\n", conf_ptr->in_fls_file);
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_ERASE_FLS) == ARG_CMD_ERASE_FLS)
    {
        printf("Erase flash.\n");
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_ROM) == ARG_CMD_READ_ROM)
    {
        printf("Read EPPROM. Output hex file: %s\n", conf_ptr->out_rom_file);
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_ROM) == ARG_CMD_WRITE_ROM)
    {
        printf("Write EPPROM. Input hex file: %s\n", conf_ptr->in_rom_file);
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_ERASE_ROM) == ARG_CMD_ERASE_ROM)
    {
        printf("Erase EPPROM.\n");
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_SIGN) == ARG_CMD_READ_SIGN)
    {
        printf("Read Signature\n");
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_READ_USER_SIGN) == ARG_CMD_READ_USER_SIGN)
    {
        printf("Read User Signature\n");
    }
    else if ((conf_ptr->arg_cmd & ARG_CMD_WRITE_USER_SIGN) == ARG_CMD_WRITE_USER_SIGN)
    {
        printf("Write User Signature.\n");
    }
    else
    {
        printf("Invalid command\n");
    }
}

/*******************************************************************
 *
 *      ENTRY FUNCTION
 *
 ******************************************************************/
int
main ( int argc, 
       char** argv )
{
    AT89S_EID   eid = AT89S_EID_NOK;
    AT89S_Type at89s_configs;

    // init
    memset(&at89s_configs, 0, sizeof(AT89S_Type));

    // try parsing the input argument
    eid = parse_argument(argc, argv, &at89s_configs);
    if (eid != AT89S_EID_OK)
    {
        printf("Error Usage: %s\n! Press help for more information\n",
                AT89S_ERROR_MESSAGE[eid]);
        goto CLEAN_EXIT;
    }

    // show command infos
    print_command(&at89s_configs);

    // process commands
    eid = process_command(&at89s_configs);
    if (eid != AT89S_EID_OK)
    {
        printf("Process command failed: %s\n",
                AT89S_ERROR_MESSAGE[eid]);
        goto CLEAN_EXIT;
    }

    // show result
    show_result(&at89s_configs);

CLEAN_EXIT:
    eid = usbserial_close(&at89s_configs.tty_dev);
    printf("Close connection: %s\n", AT89S_ERROR_MESSAGE[eid]);

    return 0;
}

