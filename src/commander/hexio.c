/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <malloc.h>

#include "hexio.h"


/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
static int
fsize ( FILE* pfile );


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
AT89S_EID
hexio_readall ( char* filename,
                unsigned char* data_ptr,
                int* data_len_ptr )
{
    FILE* pfile = NULL;
    int ipos = 0;

    if (filename == NULL
        || data_ptr == NULL
        || data_len_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

    // open file
    pfile = fopen(filename, "rb");
    if (pfile == NULL)
    {
        return AT89S_EID_HEXIO_OPEN;
    }

    // get file length
    *data_len_ptr = fsize(pfile);
    if (*data_len_ptr < 0)
    {
        fclose(pfile);
        return AT89S_EID_HEXIO_LENGTH;
    }

    // check buffer size
    if (*data_len_ptr >= AT89S_DATA_SIZE)
    {
        fclose(pfile);
        return AT89S_EID_BUF_SMALL;
    }

    // read whole file to new allocated memory
    ipos = fread(data_ptr, 1, *data_len_ptr, pfile);
    if (ipos <= 0)
    {
        fclose(pfile);
        return AT89S_EID_HEXIO_READALL;
    }

    // end of function
    fclose(pfile);
    return AT89S_EID_OK;
}

/*
 * Purpose:
 * - Return size of file in byte
 */
static int
fsize ( FILE* pfile )
{
    int prev_pos = -1;
    int file_size = -1;

    if (pfile == NULL)
        return -1;

    prev_pos = ftell(pfile);
    fseek(pfile, 0L, SEEK_END);
    file_size = ftell(pfile);
    fseek(pfile, prev_pos, SEEK_SET);

    return file_size;
}
