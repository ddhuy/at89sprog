/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

#include "at89scmd.h"
#include "hexio.h"
#include "serial.h"



/*******************************************************************
 *
 *      MACRO & PREPROCESSOR
 *
 ******************************************************************/





/*******************************************************************
 *
 *      FILE DATA TYPE
 *
 ******************************************************************/
/*
 * AT89S Commander structure
 */
typedef struct AT89S_Commander_t
{} AT89S_Commander_t;




/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 *
 */
static AT89S_EID
parse_args ( int argc,
             char** argv,
             AT89S_Commander_t* at89scmd_ptr );





/*******************************************************************
 *
 *      GLOBAL VARIABLES
 *
 ******************************************************************/
// short options
static const char* v_sargs = "w:r:esd:?";
// long options
static const struct option v_largs[] =
{
    { "write-fls",       required_argument,  NULL,  'w' },
    { "read-fls",        required_argument,  NULL,  'r' },
    { "erase-fls",       no_argument,        NULL,  'e' },
    { "read-signature",  no_argument,        NULL,  's' },
    { "device",          required_argument,  NULL,  'd' },
    { "help",            no_argument,        NULL,  '?' },
    { 0, 0, 0, 0 },
};






/*******************************************************************
 *
 *      FUNCTION DEFINITION
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
parse_args ( int argc,
             char** argv,
             AT89S_Commander_t* at89scmd_ptr )
{
    AT89S_EID eid = EID_OK;
    int optidx = 0, selopt = 0;

    if (argc < 1 || argv == NULL || at89scmd_ptr == NULL)
    {
        eid = EID_ARG_NULL;
    }
    else
    {
        for (;;)
        {
            optidx = 0;
            selopt = getopt_long(argc, argv, v_sargs, v_largs, &optidx);

            if (selopt < 0)
                break;

            switch (selopt)
            {
                case 'r':
                    break;

                case 'w':
                    break;

                case 'e':
                    break;

                case 's':
                    break;

                case 'd':
                    break;

                case '?':
                    break;

                default:
                    eid = EID_ARG_INVALID;
                    break;
            }
        }
    }

    return eid;
}




/*******************************************************************
 *
 *      PROGRAM ENTRY POINT
 *
 ******************************************************************/
int main ( int argc,
           char** argv )
{
    AT89S_Commander_t at89scmd_ptr[1];
    AT89S_EID eid = EID_NOK;

    // init data
    memset(at89scmd_ptr, 0, sizeof(AT89S_Commander_t));
    // parse input argument
    eid = parse_args(argc, argv, at89scmd_ptr);
    if (eid == EID_OK)
    {
        // process command & data
    }

    return eid;
}
