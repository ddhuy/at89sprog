#ifndef _SERIAL_H_
#define _SERIAL_H_

/*******************************************************************
 *
 *      HEADERS
 *
 ******************************************************************/
#include "at89s.h"



/*******************************************************************
 *
 *      PREPROCESSOR & CONSTANT
 *
 ******************************************************************/
#define  IHEX_START_CODE    ":"

#define  IHEX_REC_DATA             0x00
#define  IHEX_REC_EOF              0x01
#define  IHEX_REC_EXT_SEG_ADDR     0x02
#define  IHEX_REC_START_SEG_ADDR   0x03
#define  IHEX_REC_EXT_LINE_ADDR    0x04
#define  IHEX_REC_START_LINE_ADDR  0x05

/*******************************************************************
 *
 *      DATA TYPE
 *
 ******************************************************************/
/*
 * Intel Hex Record structure
 */
typedef struct IHex_Record_t
{
    uint8_t   length;
    uint8_t   rectype;
    uint8_t   chksum;
    uint16_t  address;
    uint8_t*  data;

    struct IHex_Record_t* pnext;

} IHex_Record_t;

/*
 * Intel Hex file structure
 */
typedef struct IHex_File_t
{
    uint8_t  no_record;
    IHex_Record_t*  record_phead;
    IHex_Record_t*  record_ptail;

} IHex_File_t;



/*******************************************************************
 *
 *      API DECLARATION
 *
 ******************************************************************/
AT89S_EID
ihex_load ( char* filename,
            IHex_File_t* hexfile_ptr );

AT89S_EID
ihex_clean ( IHex_File_t* hexfile_ptr );


#endif /* _SERIAL_H_ */
