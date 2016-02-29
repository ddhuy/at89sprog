/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>

#include "at89s_types.h"
#include "usbserial.h"



/*******************************************************************
 *
 *      FILE DATA TYPE
 *
 ******************************************************************/
/*
 * Argument commands
 */
typedef enum ArgCmd_t
{
    ARG_CMD_NULL = 0,

    ARG_CMD_WRITE_FLS,
    ARG_CMD_READ_FLS,
    ARG_CMD_ERASE_FLS,

    ARG_CMD_WRITE_ROM,
    ARG_CMD_READ_ROM,
    ARG_CMD_ERASE_ROM,

    ARG_CMD_READ_SIGN,
    ARG_CMD_READ_USER_SIGN,
    ARG_CMD_WRITE_USER_SIGN,

} ArgCmd;


/*
 *
 */
typedef struct AT89S_Type_t
{
    ArgCmd  arg_cmd;
    char* hex_file;
    char* buffer;
    int   buffer_len;

    UsbSerialDevice ttydev;

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
                 char** argv );

/*
 *
 */
static AT89S_EID
arg_psr_w ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_r ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_e ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_b ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_v ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_c ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_s ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_u ( AT89S_Type* pconf );

/*
 *
 */
static AT89S_EID
arg_psr_m ( AT89S_Type* pconf );


/*
 *
 */
static AT89S_EID
arg_psr_f ( AT89S_Type* pconf );


/*
 *
 */
static AT89S_EID
arg_psr_d ( AT89S_Type* pconf );




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
    { "write-flash",     no_argument,  NULL,  'w' },
    { "read-flash",      no_argument,  NULL,  'r' },
    { "erase-flash",     no_argument,  NULL,  'e' },

    { "write-eeprom",    no_argument,  NULL,  'b' },
    { "read-eeprom",     no_argument,  NULL,  'v' },
    { "erase-eeprom",    no_argument,  NULL,  'c' },

    { "read-sign",       no_argument,        NULL,  's' },
    { "read-user-sign",  no_argument,        NULL,  'u' },
    { "write-user-sign", required_argument,  NULL,  'm' },

    { "hex-file",  required_argument,  NULL,  'f' },
    { "device",    required_argument,  NULL,  'd' },

    { "help",            no_argument,  NULL, '?' },

    { 0, 0, 0, 0 },
};
// short options
static const char* v_sargs = "wrebvcsum:f:d:?";


/*
 * AT89S argument configurations
 */
AT89S_Type* v_configs_ptr = NULL;



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
                 char** argv )
{
    AT89S_EID eid = AT89S_EID_NOK;
    int opt_idx = 0, selected_opt = 0;

    if (argc < 1 || argv == NULL)
    {
        eid = AT89S_EID_ARG_NULL;
        return eid;
    }

    v_configs_ptr = (AT89S_Type *) malloc(sizeof(AT89S_Type));
    if (v_configs_ptr == NULL)
    {
        eid = AT89S_EID_MEM_MALLOC;
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
                printf("Read Flash\n");
                arg_psr_r(v_configs_ptr);
                break;

            case 'w':
                printf("Rrite flash\n");
                arg_psr_w(v_configs_ptr);
                break;

            case 'e':
                printf("erase flash\n");
                arg_psr_e(v_configs_ptr);
                break;

            case 'v':
                printf("read eeprom\n");
                arg_psr_v(v_configs_ptr);
                break;

            case 'b':
                printf("write eeprom\n");
                arg_psr_b(v_configs_ptr);
                break;

            case 'c':
                printf("erase eeprom\n");
                arg_psr_c(v_configs_ptr);
                break;

            case 's':
                printf("read signature\n");
                arg_psr_s(v_configs_ptr);
                break;

            case 'u':
                printf("read user signature\n");
                arg_psr_u(v_configs_ptr);
                break;

            case 'm':
                printf("write user signature\n");
                arg_psr_m(v_configs_ptr);
                break;

            case 'f':
                printf("Hex file\n");
                arg_psr_f(v_configs_ptr);
                break;

            case 'd':
                printf("Serial device\n");
                arg_psr_d(v_configs_ptr);
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
    printf("programg usage:\n");
} /* show_usage */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_w ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_WRITE_FLS;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_w */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_r ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_READ_FLS;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_r */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_e ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_ERASE_FLS;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_e */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_b ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_WRITE_ROM;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_b */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_v ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_READ_ROM;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_v */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_c ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_ERASE_ROM;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_c */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_s ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_READ_SIGN;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_s */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_u ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_READ_USER_SIGN;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_u */

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_m ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->arg_cmd  = ARG_CMD_WRITE_USER_SIGN;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_m */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_f ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    pconf->hex_file   = NULL;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_OK;
} /* arg_psr_f */


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
arg_psr_d ( AT89S_Type* pconf )
{
    if (pconf == NULL)
        return AT89S_EID_ARG_NULL;

    //pconf->ttydev     = NULL;
    pconf->buffer     = NULL;
    pconf->buffer_len = 0;

    return AT89S_EID_NOK;
} /* arg_psr_d */



/*******************************************************************
 *
 *      entry function
 *
 ******************************************************************/
int
main ( int argc, 
       char** argv )
{
    // try parsing the input argument
    if (parse_argument(argc, argv) != AT89S_EID_OK)
    {
        printf("Error Usage! Press help for more information\n");
        return -1;
    }

    // open connection to serial device

    // process argument commands

    return 0;
}

