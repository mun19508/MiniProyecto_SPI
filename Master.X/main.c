/*Master
 * File:   main.c
 * Author: Daniel Mundo
 *
 * Created on 16 de febrero de 2021, 11:21 AM
 */
//**********************************Librerias***********************************
#include <xc.h>
#include <stdint.h>
#include "LCD.h"
#include "UART.h"
#include "SPI.h"
//******************************************************************************
//******************************Definiciones************************************
#define _XTAL_FREQ 4000000
#define Slave1 PORTCbits.RC0 
#define Slave2 PORTCbits.RC1 
#define Slave3 PORTCbits.RC2 
//******************************************************************************
//***************************Bits de ConFig*************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
//******************************************************************************
//*****************************Prototipos***************************************
char int2String(uint8_t value);
//******************************************************************************
//********************************Variable**************************************
uint8_t voltageADC = 0;
uint8_t cont8bits = 0;
uint8_t temperature = 0;
char string_voltageADC[5];
char* string_cont8bits;
char string_temperature[5];
float conv_ADC = 0;
//******************************************************************************
void main(void) {
    //--------------------------Canal Analogico---------------------------------
    ANSEL = 0;
    ANSELH = 0; //Puerto A y B como digitales
    //--------------------------Comunicacion UART-------------------------------
    UARTInit(9600, 1);
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    //--------------------------Comunicacion SPI-------------------------------
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    Slave1 = 0;
    Slave2 = 1;
    Slave3 = 1;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //--------------------------Puerto Entrada/salida---------------------------
    TRISB = 0;
    TRISD = 0;
    TRISE = 0; //Puerto D & E como salidas 
    //-------------------------------Limpieza de puertos------------------------   
    PORTD = 0;
    PORTE = 0; //Se limpian los puertos
    //-------------------------------LCD----------------------------------------
    Lcd_Init(); //se inicia la configuración de la LCD
    Lcd_Clear();
    //--------------------------Loop principal----------------------------------
    while (1) {
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("S1:");
        Lcd_Set_Cursor(1, 7);
        Lcd_Write_String("S2:");
        Lcd_Set_Cursor(1, 13);
        Lcd_Write_String("S3:");
        //------------------------Recepcion de datos via SPI---------------------------- 
        Slave3 = 1; //Slave deselect
        Slave1 = 0; //Slave select
        __delay_ms(1);
        spiWrite(1);
        voltageADC = spiRead();
        __delay_ms(1);
        Slave1 = 1; //Slave deselect
        Slave2 = 0; //Slave select 
        __delay_ms(1);
        spiWrite(2);
        cont8bits = spiRead();
        __delay_ms(1);
        Slave2 = 1; //Slave deselect 
        Slave3 = 0; //Slave select
        __delay_ms(1);
        spiWrite(3);
        temperature = spiRead();
        //-----------------------------Slave 1----------------------------------
        conv_ADC = (voltageADC / (float) 255)*5; //Se consigue el porcentaje con respecto al valor 
        //maximo que un puerto puede tener, despues se multiplica por 5 para conocer el voltaje actual del puerto                                          
        convert(string_voltageADC, conv_ADC, 2); //se convierte el valor actual a un valor ASCII.
        Lcd_Set_Cursor(2, 1); 
        Lcd_Write_String(string_voltageADC);//Muestra los valores en la LCD. 
        Lcd_Set_Cursor(2, 5);
        Lcd_Write_String("V");
        UARTSendChar('|'); //Comunicacion via UART
        UARTSendString("S1", 6);
        UARTSendChar(':');
        UARTSendChar(' ');
        UARTSendString(string_voltageADC, 5);
        UARTSendChar('V');
        UARTSendChar(',');
        UARTSendChar(' ');
        //------------------------Slave 2---------------------------------------
        string_cont8bits = int2String(cont8bits);
        Lcd_Set_Cursor(2, 7); 
        Lcd_Write_String(string_cont8bits); //Muestra el contador en la LCD
        UARTSendString("S2", 6); //Comunicacion via UART
        UARTSendChar(':');
        UARTSendChar(' ');
        UARTSendString(string_cont8bits, 4);
        UARTSendChar(',');
        UARTSendChar(' ');
        //-----------------------------Slave 3----------------------------------
        convert(string_temperature, temperature, 1);
        Lcd_Set_Cursor(2, 12);
        Lcd_Write_String(string_temperature);//Se muestra el valor en la LCD
        Lcd_Set_Cursor(2, 16);
        Lcd_Write_String("C");
        __delay_ms(20);
        UARTSendString("S3", 6); //Comunicacion via UART
        UARTSendChar(':');
        UARTSendChar(' ');
        UARTSendString(string_temperature, 4);
        UARTSendChar('C');
        UARTSendChar('|');
        UARTSendChar(' ');
    }
}
char int2String(uint8_t value) {
    char cifras[4]; 
    uint8_t x;
    x = value / 100; //se obtiene el valor de las centanas 
    cifras[0] = x + 48; //el numero obtenido se pasa ASCII
    value -= x * 100; //se resta el valor obtenido de las  centanas al valor 
    // se hace lo mismo con las decenas y unidades
    cifras[1] = (value / 10) + 48;
    cifras[2] = (value % 10) + 48;
    cifras[3] = '\0';
    return cifras;
}
