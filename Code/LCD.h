#ifndef __LCD_H__
#define __LCD_H__

void LCD_init(void);

void LCD_secondLine(void);
void LCD_clearScreen(void);

void LCD_writeWord(char * word);

#endif