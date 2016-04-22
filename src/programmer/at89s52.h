#ifndef _AT89S52_H_
#define _AT89S52_H_


#include "at89s.h"



/*******************************************************************
 *
 *      AT89S52 MCU COMMANDS
 *
 ******************************************************************/
#define  READ_LOCK_BIT           0xA4000000UL
#define  PROGRAM_ENABLE          0xAC530000UL
#define  CHIP_ERASE              0xAC800000UL
#define  WRITE_LOCK_BIT          0xACE00000UL
#define  READ_BYTE_MEM           0x20000000UL
#define  READ_SIGNATURE_BYTE1    0x28000000UL
#define  READ_SIGNATURE_BYTE2    0x28010000UL
#define  READ_SIGNATURE_BYTE3    0x28020000UL
#define  READ_PAGE_MEM           0x30000000UL
#define  WRITE_BYTE_MEM          0x40000000UL
#define  WRITE_PAGE_MEM          0x50000000UL


/*******************************************************************
 *
 *      AT89S52 CONSTANTS
 *
 ******************************************************************/
#define  MCU_PROGRAM_ENA_OK        0x69


/*******************************************************************
 *
 *      AT89S52 ISP TIMING
 *
 ******************************************************************/
#define  OSC_FREQ    (11.0592) // in MHz
#define  T_CLCL      (0) // Nanosecond
#define  T_SHSL      (8 * T_CLCL) // Nanosecond
#define  T_SLSH      (8 * T_CLCL) // Nanosecond
#define  T_OVSH      T_CLCL // Nanosecond
#define  T_SHOX      (2 * T_CLCL) // Nanosecond
#define  T_SLIV      (16) // Nanosecond
#define  T_ERASE     (500000) // 500000 microsecond = 500 Millisecond
#define  T_SWC       (64 * T_CLCL + 400) // Microsecond
#define  T_RESET     (1000) // Microsecond


#endif /* _AT89S52_H_ */
