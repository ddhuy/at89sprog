#ifndef _HEX_IO_H_
#define _HEX_IO_H_

#include <stdint.h>
#include <types.h>

ISP_EID
read_hexfile ( char* filename,
               uint8_t* buffer,
               uint8_t* length );


#endif /* _HEX_IO_H_ */
