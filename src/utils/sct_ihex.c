#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "hexio.h"

int main ( int argc,
           char** argv )
{
    AT89S_EID eid = EID_OK;
    IHex_File_t hexfile;
    IHex_Record_t* record_ptr = NULL;

    memset(&hexfile, 0, sizeof(IHex_File_t));

    eid = ihex_load(argv[1], &hexfile);
    if (eid == EID_OK)
    {
        printf("Load Hex file successfully\n");
    }
    else
    {
        printf("Load Hex file failed: %d - %s\n", eid, strerror(errno));
        return -1;
    }

    for (record_ptr = hexfile.record_phead;
         record_ptr != NULL;
         record_ptr = record_ptr->pnext)
    {
        printf("--------------------\n");
        printf("Data length   : %d\n", record_ptr->length);
        printf("Start address : %04X\n", record_ptr->address);
        printf("Record type   : %d\n", record_ptr->rectype);
        printf("Checksum      : %02X\n", record_ptr->chksum);
    }

    eid = ihex_clean(&hexfile);
    if (eid == EID_OK)
    {
        printf("Clean Hex file successfully\n");
    }
    else
    {
        printf("Clean Hex file failed: %d - %s\n", eid, strerror(errno));
        return -2;
    }

    return 0;
}
