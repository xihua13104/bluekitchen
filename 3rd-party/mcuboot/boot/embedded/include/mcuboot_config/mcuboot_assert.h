#ifndef __MCUBOOT_ASSERT_H__
#define __MCUBOOT_ASSERT_H__

#include "stdio.h"

#ifdef assert
#undef assert
#endif
#define assert(arg)                                                          \
    do {                                                                     \
        if (!(arg)) {                                                        \  
            while(1) {                                                       \
                printf("assert:%s %d %s\r\n" __FILE__, __LINE__, __func__ ); \
            }                                                                \
        }                                                                    \
    } while(0)
#endif