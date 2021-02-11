/*
 * File: 
 * Author: Daniel Mundo
 * Created on 4 de febrero de 2021, 03:05 PM
 */
//**********************************Librerias***********************************
#include <xc.h>
#include <stdint.h>
#include "ADC_LIB.h"
#define _XTAL_FREQ 4000000
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
//********************************Variable**************************************
uint8_t canal_act = 0;
volatile uint8_t var_adc0 = 0;
volatile uint8_t var_adc1 = 0;
float cont_uart = 0;
char string_uart[10];
char valor_uart = 0;
char adc0[10];
char adc1[10];
float conv0 = 0;
float conv1 = 0;
//******************************************************************************
void main(void) {
    //--------------------------Canal Analogico---------------------------------
    ANSEL = 0;
    ANSELH = 0; //Puerto A y B como digitales
    start_adc(2, 0, 0, 0); //Fosc/8, No ISR de ADC, Ref Vdd y Vcc, a la izquierda
    start_ch(12); //Habilita el pin del Puerto RB0.
    start_ch(10); //Habilita el pin del Puerto RB1.
    Select_ch(12); //Selecciona el canal e inicia la conversion.
    //--------------------------Puerto Entrada/salida---------------------------
    TRISA = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    TRISD = 0;
    TRISE = 0; //Puerto A, C, D & E como salidas 
    TRISB = 255;
    //-------------------------------Limpieza de puertos------------------------   
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0; //Se limpian los puertos
    //--------------------------Loop principal----------------------------------
    while (1) {
        if (PIR1bits.ADIF == 1) {
            if (canal_act == 0) {
                var_adc0 = ADRESH; //se guarda el valor convertido en la variable
                Select_ch(10);
                canal_act++;
            } else {
                var_adc1 = ADRESH; //se guarda el valor convertido en la variable
                Select_ch(12);
                canal_act--;
            }
            PIR1bits.ADIF = 0;
        }
    }
}