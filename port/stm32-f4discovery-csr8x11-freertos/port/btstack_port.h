#ifndef __PORT_H
#define __PORT_H

#ifdef BTSTACK_FREERTOS_ENABLE
void port_main(void *arg);
#else
void port_main(void);
#endif
#endif
