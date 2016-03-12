#ifndef _HEX_IO_H_
#define _HEX_IO_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/

#include "at89sprog.h"


/*******************************************************************
 *
 *      CONSTANT & PREPROCESSOR
 *
 ******************************************************************/
#define  MAX_RECORDS    16




/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Intel HEX record structure
 */
typedef struct hex_record_t
{
    unsigned char   length;
    unsigned short  address;
    unsigned char   rectype;
    unsigned char*  data;
    unsigned char   crc;

} hex_record;

/*
 * Intel HEX file structure
 */
typedef struct hex_file_t
{
    char  name[256];
    FILE* pfile;
    unsigned char no_record;
    hex_record    records[MAX_RECORDS];

} hex_file;



/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
/*
 *
 */
AT89S_EID
hexio_read ( char*  filename,
             hex_file* hfile_ptr );

/*
 *
 */
AT89S_EID
hexio_destroy ( hex_file hfile_ptr );

#endif /* _HEX_IO_H_ */
