/*
 * File:   LCD.c
 * Author: electroSome & Embedded life
 * Created on 4 de febrero de 2021, 04:27 PM
 */
#define _XTAL_FREQ 4000000
#include <xc.h>
#include "LCD.h"
//Autor electrosome

void Lcd_Port(char a) {
    PORTD = a;
}

void Lcd_Cmd(char a) {
    RS = 0; // => RS = 0
    Lcd_Port(a);
    EN = 1; // => E = 1
    __delay_ms(4);
    EN = 0; // => E = 0
}

void Lcd_Clear() {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp;
    if (a == 1) {
        temp = 0x80 + b - 1;
        Lcd_Cmd(temp);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        Lcd_Cmd(temp);
    }
}

void Lcd_Init() {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x30);
    __delay_ms(5);
    Lcd_Cmd(0x30);
    __delay_ms(11);
    Lcd_Cmd(0x30);
    /////////////////////////////////////////////////////
    Lcd_Cmd(0x38);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x6);
}

void Lcd_Write_Char(char a) {
    RS = 1; // => RS = 1
    Lcd_Port(a); //Data transfer
    EN = 1;
    __delay_us(40);
    EN = 0;
}

void Lcd_Write_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right() {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left() {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}
//Autor embedded life
void convert(char *data, float a, int place) //definition
{
    int temp = a;
    float x = 0.0;
    int digits = 0;
    int i = 0, mu = 1;
    int j = 0;
    if (a < 0) {
        a = a*-1;
        data[i] = '-';
        i++;
    }
    //exponent component
    while (temp != 0) {
        temp = temp / 10;
        digits++;
    }
    while (digits != 0) {
        if (digits == 1)mu = 1;
        else for (j = 2; j <= digits; j++)mu = mu * 10;

        x = a / mu;
        a = a - ((int) x * mu);
        data[i] = 0x30 + ((int) x);
        i++;
        digits--;
        mu = 1;
    }
    //mantissa component
    data[i] = '.';
    i++;
    digits = 0;
    for (j = 1; j <= place; j++)mu = mu * 10;
    x = (a - (int) a) * mu; //shift places
    a = x;
    temp = a;
    x = 0.0;
    mu = 1;
    digits = place;
    while (digits != 0) {
        if (digits == 1)mu = 1;
        else for (j = 2; j <= digits; j++)mu = mu * 10;

        x = a / mu;
        a = a - ((int) x * mu);
        data[i] = 0x30 + ((int) x);
        i++;
        digits--;
        mu = 1;
    }

    data[i] = '\n';
}