#ifndef __UART__
#define __UART__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

void UART0_init(void);
char UART0_read(void);
void UART0_clearInt(void);
void UART0_writeText(char* txt);

#endif
