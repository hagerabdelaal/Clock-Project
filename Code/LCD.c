#include "tm4c123gh6pm.h"
#include "LCD.h"

#include "FreeRTOS.h"
#include "task.h"

// LCD Intructions
#define LCD_CLEAR 			 			0x01			// Clears the screen and returns home
#define LCD_RETURN_HOME  			0x02			// Points cursor to the beginning of the first line
#define LCD_ENTRY_MODE				0x06			// Incrememnt DDRAM address
#define LCD_DISPLAY_ON	 			0x0C 			// Turning on display, no cursor or blinking
#define LCD_FUNCTION_SET 			0x3C 		  // 8-bit mode, 2 lines, 5x11 dots
#define LCD_CUR_SEC_LINE			0xC0


// LCD Control Pins
#define LCD_RS  (1 << 0)
#define LCD_RW  (1 << 1)
#define LCD_EN  (1 << 2)

// LCD ports
#define LCD_DATA GPIO_PORTB_DATA_R
#define LCD_CTRL GPIO_PORTE_DATA_R

// Internal function prototypes
void LCD_dataPortInit(void);
void LCD_ctrlPortInit(void);
void LCD_writeChar(unsigned char data);
void LCD_instruction(unsigned char);


void LCD_dataPortInit(void){
		SYSCTL_RCGCGPIO_R |= (1 << 1);
		GPIO_PORTB_DIR_R = 0xFF;
		GPIO_PORTB_DEN_R = 0xFF;
		GPIO_PORTB_DATA_R = 0x0;
}

void LCD_ctrlPortInit(void){
		SYSCTL_RCGCGPIO_R |= (1 << 4);
		GPIO_PORTE_DIR_R = 0x07;
		GPIO_PORTE_DEN_R = 0x07;
		GPIO_PORTE_DATA_R = 0x0;
}

void LCD_init(void){
		// Initializing ports
		LCD_dataPortInit();
		LCD_ctrlPortInit();
		
		// Configuring the LCD
		LCD_instruction(LCD_FUNCTION_SET);
		LCD_instruction(LCD_ENTRY_MODE);
		LCD_instruction(LCD_CLEAR);
		LCD_instruction(LCD_DISPLAY_ON);
}

// Puts the cursor at the beginning of the second line
void LCD_secondLine(void){
		LCD_instruction(LCD_CUR_SEC_LINE);
}

// Clears the screen and puts the cursor at the beginning of the first line
void LCD_clearScreen(void){
		LCD_instruction(LCD_CLEAR);
}

// Sends an instruction to the LCD
void LCD_instruction(unsigned char instr){
		LCD_CTRL = 0;
		LCD_DATA = instr;
		LCD_CTRL |= LCD_EN;
		vTaskDelay(pdMS_TO_TICKS(1));
		LCD_CTRL = 0;
		vTaskDelay(pdMS_TO_TICKS(2));
}

// Writes a character where the LCD's cursor is at
void LCD_writeChar(unsigned char data){
		LCD_CTRL = LCD_RS;
		LCD_DATA = data;
		LCD_CTRL |= LCD_EN;
		vTaskDelay(pdMS_TO_TICKS(1));
		LCD_CTRL = 0;
		vTaskDelay(pdMS_TO_TICKS(1));
}

// Writes a string where the LCD's cursor is at
void LCD_writeWord(char * word){
		while(*word){
				LCD_writeChar(*word);
				word++;
		}
}