#ifndef _AT89S52_H_
#define _AT89S52_H_


/*******************************************************************
 *
 *      AT89S52 MCU COMMANDS
 *
 ******************************************************************/
#define  READ_LOCK_BIT           0xA4000000UL
#define  PROGRAM_ENABLE          0xAC530060UL
#define  CHIP_ERASE              0xAC800000UL
#define  WRITE_LOCK_BIT          0xACE00000UL
#define  READ_BYTE_MEM           0x20000000UL
#define  READ_SIGNATURE_BYTE1    0x28000000UL
#define  READ_SIGNATURE_BYTE2    0x28010000UL
#define  READ_SIGNATURE_BYTE3    0x28020000UL
#define  READ_PAGE_MEM           0x30000000UL
#define  WRITE_BYTE_MEM          0x40000000UL
#define  WRITE_PAGE_MEM          0x50000000UL


#endif /* _AT89S52_H_ */
