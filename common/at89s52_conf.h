#ifndef _AT89S52_H_
#define _AT89S52_H_

/*
 * AT89S serial programming timing
 */
#define  F_AT89S_CPU      11.0592

#define  T_CLCL     (1 / double(F_AT89S_CPU))
#define  T_RESET    (64 * T_CLCL)
#define  T_SHSL     (8 * T_CLCL)
#define  T_SLSH     (8 * T_CLCL)
#define  T_OVSH     (1 * T_CLCL)
#define  T_SHOX     (2 * T_CLCL)
#define  T_SLIV     (0.32)
#define  T_SWC      (64 * T_CLCL + 400)

#define  T_CHIP_ERASE   500


/*
 * AT89S programming command
 */
#define  C_PROG_ENABLE   0xAC530069L
#define  C_CHIP_ERASE    0xAC800000L
#define  C_MEM_READ_B    0x20000000L
#define  C_MEM_READ_P    0x30000000L
#define  C_MEM_WRITE_B   0x40000000L
#define  C_MEM_WRITE_P   0x50000000L
#define  C_LBIT_READ     0x24000000L
#define  C_LBIT_WRITE    0xACE00000L
#define  C_SIG_READ      0x28000000L


#endif /* _AT89S52_H_ */
