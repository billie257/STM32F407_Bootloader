#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <stdint.h>

void console_init(uint32_t baundrate);
void console_write(const char str[], uint32_t length);

#endif /* __CONSOLE_H__ */
