/* 
 * File: LCD.h 
 * Author: electroSome & Embedded life
 * Bibliogra:
 * //https://electrosome.com/lcd-pic-mplab-xc8/
 * //https://embeddedlifehelp.blogspot.com/2015/03/c-code-to-convert-floating-point-value.html
 */
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//Se definen los puertos de la LCD
#define RS PORTEbits.RE0
#define EN PORTEbits.RE1
#define RW PORTEbits.RE2
#define D0 PORTDbits.RD0
#define D1 PORTDbits.RD1
#define D2 PORTDbits.RD2
#define D3 PORTDbits.RD3
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7
//Autor electrosome
void Lcd_Port(char a);
void Lcd_Cmd(char a);
void Lcd_Clear(void);
void Lcd_Set_Cursor(char a, char b);
void Lcd_Init(void);
void Lcd_Write_Char(char a);
void Lcd_Write_String(char *a);
void Lcd_Shift_Right(void);
void Lcd_Shift_Left(void);
//Autor embedded life
void convert(char *data,float a, int place);

#endif	/* LCD_H */

