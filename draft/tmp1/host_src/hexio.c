#include <stdio.h>

#include "hexio.h"


ISP_EID
read_hexfile ( char* filename,
               uint8_t* buffer,
               uint8_t* length )
{
    FILE* pfile = NULL;
    uint32_t len = 0, ipos = 0;

    if (filename == NULL
        || buffer == NULL
        || length == NULL)
    {
        return EID_ARG_NULL;
    }

    pfile = fopen(filename, "rb");
    if (pfile == NULL)
    {
        return EID_HEX_OPEN;
    }

    len = *length;
    while (!feof(pfile))
    {
        ipos = fread(buffer, 1, len, pfile);
        buffer += ipos;
        len    -= ipos;
        if (len <= 0)
            break;
    }

    if (feof(pfile))
    {
        *length = *length - len;
        fclose(pfile);
        return EID_OK;
    }
    else
    {
        fclose(pfile);
        return EID_SMALL_BUFFER;
    }

    return EID_NOK;
}


