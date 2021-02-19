/*Slave 2_Contador de 8 bits
 * File:   main.c
 * Author: Daniel Mundo
 * Created on 11 de febrero de 2021, 05:43 PM
 */
//**********************************Librerias***********************************
#include <xc.h>
#include <stdint.h>
#include "SPI.h"
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
//********************************Variables*************************************
uint8_t toogle = 0;
uint8_t antireb = 0;
uint8_t antireb_ant = 0;
//******************************************************************************
//******************************Interrupciones**********************************

void __interrupt() ISR(void) {
    if (INTCONbits.RBIF == 1) { //Se trabaja con botones en config. pull-up
        antireb = antireb_ant;
        antireb_ant = PORTB;
        if ((antireb_ant & 0b00000011) == 3) { //Se asegura que B0 y B1 fueron soltados.
            if ((antireb & 0b00000011) == 2) { //Compruba si en la anterior interrupción B0 fue presionado.
                PORTD++; //Aumenta el valor del puerto
            }
            if ((antireb & 0b00000011) == 1) {//Compruba si en la anterior interrupción B1 fue presionado.
                PORTD--; //Disminuye el valor del puerto
            }
        }
        INTCONbits.RBIF = 0;
    }
    if (SSPIF == 1) { //Si se selecciona como el esclavo activo
        spiWrite(PORTD); //se envia el valor del puerto
        SSPIF = 0;
    }
}
//******************************************************************************

void main(void) {
    //-----------------------Interrupciones-------------------------------------
    INTCONbits.GIE = 1; //Se habilitan ISR globales.
    INTCONbits.PEIE = 1; //Se habilitan ISR perifericas.
    INTCONbits.RBIE = 1; //Se habilita la ISR del por cambios.
    INTCONbits.RBIF = 0;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1; //Se habilita individualmente la lectura de los cambios.
    OPTION_REGbits.T0CS = 0; //Reloj interno
    //--------------------------Canal Analogico---------------------------------
    ANSEL = 0;
    ANSELH = 0; //Puerto A y B como digitales
    //--------------------------Comunicacion SPI--------------------------------
    INTCONbits.GIE = 1; // Habilitamos interrupciones
    INTCONbits.PEIE = 1; // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0; // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1; // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1; // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //--------------------------Puerto Entrada/salida---------------------------
    TRISD = 0; //Puerto D como salidas
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1; //Pines B0 & B1 como entrada
    //-------------------------------Limpieza de puertos------------------------
    PORTB = 0;
    PORTD = 0; //Se limpian los puertos
    while (1) {

    }
}