/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "hexio.h"


/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/



/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/




/*******************************************************************
 *
 *      LOCAL FUNCTION DECLARATION
 *
 ******************************************************************/
/*
 *
 */
static AT89S_EID
create_record ( IHex_Record_t** record_pptr );

/*
 *
 */
static void
destroy_record ( IHex_Record_t* record_ptr );

/*
 *
 */
static AT89S_EID
read_record ( FILE* pfile, IHex_Record_t* record_ptr );

/*
 *
 */
static AT89S_EID
insert_record_tail ( IHex_File_t* hexfile_ptr, IHex_Record_t* record_ptr );

/*
 *
 */
static AT89S_EID
detach_record_head ( IHex_File_t* hexfile_ptr, IHex_Record_t** record_pptr );


/*******************************************************************
 *
 *      FUNCTIONS DEFINITION
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
AT89S_EID
ihex_load ( char* filename,
            IHex_File_t* hexfile_ptr )
{
    AT89S_EID eid = EID_NOK;
    IHex_Record_t* record_ptr = NULL;
    FILE* pfile = NULL;

    if (filename == NULL || hexfile_ptr == NULL)
        return EID_ARG_NULL;

    pfile = fopen(filename, "rb");
    if (pfile != NULL)
    {
        while (!feof(pfile))
        {
            eid = create_record(&record_ptr);
            if (eid == EID_OK)
            {
                eid = read_record(pfile, record_ptr);
                if (eid == EID_OK)
                {
                    eid = insert_record_tail(hexfile_ptr, record_ptr);
                    if (eid == EID_OK)
                    {
                        // do nothing
                    }
                    else
                    {
                        destroy_record(record_ptr);
                        break;
                    }
                }
                else
                {
                    destroy_record(record_ptr);
                    break;
                }
            }
            else
            {
                break;
            }
        }
        fclose(pfile);
    }
    else
    {
        eid = EID_HEX_OPEN;
    }

    return eid;

} /* ihex_read */


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
AT89S_EID
ihex_clean ( IHex_File_t* hexfile_ptr )
{
    IHex_Record_t* record_ptr = NULL;

    if (hexfile_ptr == NULL)
        return EID_ARG_NULL;

    while (EID_OK == detach_record_head(hexfile_ptr, &record_ptr))
    {
        destroy_record(record_ptr);
    }

    if (hexfile_ptr->no_record != 0)
        return EID_REC_LEAK;
    return EID_OK;

} /* ihex_clean */


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
create_record ( IHex_Record_t** record_pptr )
{
    IHex_Record_t* rec_ptr = NULL;
    AT89S_EID eid = EID_OK;

    if (record_pptr == NULL)
        return EID_ARG_NULL;

    rec_ptr = (IHex_Record_t *) malloc(sizeof(IHex_Record_t));
    if (rec_ptr != NULL)
    {
        memset(rec_ptr, 0, sizeof(IHex_Record_t));
    }
    else
    {
        eid = EID_MEM_ALLOC;
    }

    *record_pptr = rec_ptr;

    return eid;
} /* read_record */


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static void
destroy_record ( IHex_Record_t* record_ptr )
{
    if (record_ptr != NULL)
    {
        if (record_ptr->data != NULL)
            free(record_ptr->data);
        free(record_ptr);
    }

} /* destroy_record */

/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
read_record ( FILE* pfile, IHex_Record_t* record_ptr )
{
    uint32_t nitem = 0, i = 0;
    uint32_t bcount, haddr, laddr, rectype, read_byte;
    uint8_t crc = 0;

    if (pfile == NULL || record_ptr == NULL)
        return EID_ARG_NULL;

    // read record header
    nitem = fscanf(pfile,
                   IHEX_START_CODE "%02x%02x%02x%02x",
                   &bcount, &haddr, &laddr, &rectype);
    if (nitem != 4)
    {
        return EID_REC_FMT;
    }

    // parse & assign record header
    record_ptr->length  = bcount;
    record_ptr->address = ((haddr << 8) | laddr);
    record_ptr->rectype = rectype;

    // read record data
    record_ptr->data = (uint8_t *) malloc(bcount);
    if (record_ptr->data == NULL)
        return EID_MEM_ALLOC;

    for (i = 0; i < bcount; ++i)
    {
        nitem = fscanf(pfile, "%02x", &read_byte);
        if (nitem == 1)
        {
            record_ptr->data[i] = read_byte;
            crc += record_ptr->data[i];
        }
        else
        {
            return EID_REC_FMT;
        }
    }

    // read record CRC
    nitem = fscanf(pfile, "%02x\n", &read_byte);
    if (nitem != 1)
    {
        return EID_REC_FMT;
    }

    record_ptr->chksum = read_byte;

    // calculate & verify CRC
    crc += bcount + haddr + laddr + rectype;
    crc = ~crc + 1;
    if (crc != record_ptr->chksum)
    {
        return EID_REC_CRC;
    }

    return EID_OK;
} /* read_record */


/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
insert_record_tail ( IHex_File_t* hexfile_ptr, IHex_Record_t* record_ptr )
{
    if (hexfile_ptr == NULL || record_ptr == NULL)
        return EID_ARG_NULL;

    if (hexfile_ptr->record_phead == NULL && hexfile_ptr->record_ptail == NULL)
    {
        hexfile_ptr->record_phead = record_ptr;
        hexfile_ptr->record_ptail = record_ptr;
    }
    else
    {
        hexfile_ptr->record_ptail->pnext = record_ptr;
        hexfile_ptr->record_ptail = record_ptr;
    }

    hexfile_ptr->no_record += 1;

    return EID_OK;
} /* insert_record_tail */



/*
 * Description:
 *
 * Input:
 *
 * Output:
 *
 */
static AT89S_EID
detach_record_head ( IHex_File_t* hexfile_ptr, IHex_Record_t** record_pptr )
{
    if (hexfile_ptr == NULL || record_pptr == NULL)
        return EID_ARG_NULL;

    *record_pptr = NULL;

    if (hexfile_ptr->no_record == 0)
    {
        return EID_REC_EMPTY;
    }

    // get head
    *record_pptr = hexfile_ptr->record_phead;

    // adjust list
    if (hexfile_ptr->record_phead == hexfile_ptr->record_ptail)
    {
        // last record in list
        hexfile_ptr->record_phead = NULL;
        hexfile_ptr->record_ptail = NULL;
    }
    else
    {
        // move head to next element
        hexfile_ptr->record_phead = hexfile_ptr->record_phead->pnext;
    }

    // detach old head
    (*record_pptr)->pnext = NULL;
    hexfile_ptr->no_record -= 1;

    return EID_OK;

} /* detach_record_head */
