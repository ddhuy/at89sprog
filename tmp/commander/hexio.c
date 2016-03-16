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
 *      CONSTANT & PREPROCESSOR
 *
 ******************************************************************/
#define  HEADER_FORMAT    ":%2x%2x%2x%2x"
#define  HEADER_NO_ITEM    3

/*
 * Intel HEX standard record types
 */
#define  REC_TYPE_DATA              0x00
#define  REC_TYPE_EOF               0x01
#define  REC_TYPE_EXT_SEG_ADDR      0x02
#define  REC_TYPE_START_SEC_ADDR    0x03
#define  REC_TYPE_EXT_LINE_ADDR     0x04
#define  REC_TYPE_START_LINE_ADDR   0x05




/*******************************************************************
 *
 *      PRIVATE FUNCTION DECLARATION
 *
 ******************************************************************/
static int
fsize ( FILE* pfile );


static AT89S_EID
read_record ( hex_file* hfile_ptr );




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
hexio_read ( char* filename,
             hex_file* hfile_ptr )
{
    FILE* pfile = NULL;
    int ipos = 0;

    if (filename == NULL
        || hfile_ptr == NULL)
    {
        return AT89S_EID_ARG_NULL;
    }

#if 0

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
#endif

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


/*
 * Purpose:
 * - Read one record from Intel HEX file
 */
static AT89S_EID
read_record ( hex_file* hfile_ptr )
{
    int i = 0, res = 0, record_idx = 0;
    unsigned char len, rec, data, laddr, haddr, crc;
    hex_record* record = NULL;

    if (hfile_ptr == NULL)
        return AT89S_EID_ARG_NULL;
    if (hfile_ptr->pfile == NULL)
        return AT89S_EID_ARG_INVALID;
    if (feof(hfile_ptr->pfile))
        return AT89S_EID_HEXIO_EOF;

    // init
    len = rec = data = laddr = haddr = crc = 0x00;

    // read header of record
    do
    {
        res = fscanf(hfile_ptr->pfile,
                     HEADER_FORMAT,
                     &len, &haddr, &laddr, &rec);
        if (res < HEADER_NO_ITEM + 1)
            return AT89S_EID_HEXIO_RECORD_FORMAT;
        if (rec > REC_TYPE_START_LINE_ADDR)
            return AT89S_EID_HEXIO_RECORD_TYPE;

    } while (rec == REC_TYPE_EXT_SEG_ADDR);

    if (rec != REC_TYPE_EOF)
    {
        record_idx = hfile_ptr->no_record;
        if (record_idx == MAX_RECORDS - 1)
            return AT89S_EID_HEXIO_RECORD_MAX;
        record = &hfile_ptr->records[record_idx];

        record->length  = len;
        record->address = ((haddr << 8) | laddr);
        record->rectype = rec;
        record->crc += len + haddr + laddr + rec;

        // read record data content
        record->data = (unsigned char *) malloc(sizeof(char) * len);
        if (record->data == NULL)
            return AT89S_EID_MEM_ALLOC;
        for (i = 0; i < len; ++i)
        {
            res = fscanf(hfile_ptr->pfile, "%2x", &record->data[i]);
            if (res < 1)
                return AT89S_EID_HEXIO_RECORD_CORRUPT;
            record->crc += record->data[i];
        }

        // verify checksum
        res = fscanf(hfile_ptr->pfile, "%2x", &crc);
        if (res < 1)
            return AT89S_EID_HEXIO_RECORD_CORRUPT;
        if (crc != record->crc)
            return AT89S_EID_HEXIO_RECORD_CHECKSUM;
    }

    return AT89S_EID_OK;
}

