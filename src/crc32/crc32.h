#ifndef CRC32_H_
#define CRC32_H_

# include <stdio.h>
# include <stdlib.h>

//# define CRC_BUFFER_SIZE  1024

unsigned long compute_CRC_buffer(const void *buf, size_t bufLen);

#endif
