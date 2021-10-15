#include "uart.h"

void UART0_init(void)
{
 // Enabling clock to UART 0 and Port A
  SYSCTL_RCGCUART_R |= 0x01;
  SYSCTL_RCGCGPIO_R |= 0x01;
  
  // Enabling alterantive functions for port A
  GPIO_PORTA_AFSEL_R |= 0x3; 
  GPIO_PORTA_PCTL_R |= 0x11;
  GPIO_PORTA_DEN_R |= 0x3;
  
  // Configuring the UART registers
  UART0_CTL_R &= ~0x1;          // Disable
	
	// Assuming Frequceny of 50MHz
  UART0_IBRD_R = 325;           // Integer portion of the divisor
  UART0_FBRD_R = 33;            // Fractional portion of the divisor
  UART0_LCRH_R = 0x70;          // Enabling FIFO, setting word length to 8, no parity, and one stop bit
  UART0_CC_R = 0x0;             // Setting the clock source
	
	// Enabling interrupts
	UART0_IM_R = (1 << 5);				
	NVIC_EN0_R |= (1 << 5);
	
  UART0_CTL_R |= 0x301;         // Enabling Tx and Rx
}

// Clear the interrupt
void UART0_clearInt(void){
	UART0_ICR_R |= (1 << 5);
}


// Reads one character
char UART0_read(void)
{
	while((UART0_FR_R & 0x10) != 0);
  return (char)UART0_DR_R;
}

// Prints a given string
void UART0_writeText(char* txt)
{
	while(*txt != 0)
	{
		while((UART0_FR_R & 0x20) != 0);
    UART0_DR_R = *txt;
    txt++;
  }
}
