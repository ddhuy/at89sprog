/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include "at89sprog.h"

/*******************************************************************
 *
 *      GLOBAL AT89S VARIABLES
 *
 ******************************************************************/
const char* const AT89S_ERROR_MESSAGE[AT89S_MAX_EID] = 
{
    "OK",
    "NOT OK",
    "ARGUMENT NULL",
    "ARGUMENT INVALID",
    "MEMORY ALLOCATION",
    "SERIAL OPEN",
    "SERIAL CLOSE",
    "SERIAL SEND",
    "SERIAL RECEIVE",
    "SERIAL GET ATTRIBUTE",
    "SERIAL SET ATTRIBUTE",
    "SERIAL BAUDRATE INVALID",
    "SERIAL CONFIGUGRATION INVALID",
    "HEX FILE OPEN",
    "HEX FILE CLOSE",
    "HEX FILE READ ALL",
    "HEX FILE LENGTH",
    "HEX FILE SMALL BUFFER",
    "PROTOCOL LENGTH INVALID",
    "PROTOCOL COMMAND INVALID",
    "PROTOCOL CRC INVALID",
    "PROTOCOL MESSAGE BAD",
};


