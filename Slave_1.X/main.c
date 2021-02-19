/*Slave 1_Voltaje del ADC
 * File: main.c
 * Author: Daniel Mundo
 * Created on 11 de febrero de 2021, 03:05 PM
 */
//**********************************Librerias***********************************
#include <xc.h>
#include <stdint.h>
#include "ADC_LIB.h"
#include "SPI.h"
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
volatile uint8_t voltageADC = 0;
//******************************************************************************
//***************************Código de Interrupción***************************** 
void __interrupt() isr(void) {
    if (PIR1bits.ADIF == 1) {
        voltageADC = ADRESH; //se guarda el valor convertido en la variable
        PIR1bits.ADIF = 0;
        __delay_us(50);
        ADCON0bits.GO = 1;
    }
    if (SSPIF == 1) {//Si se selecciona como el esclavo activo
        spiWrite(voltageADC); //se envia el valor del ADC
        SSPIF = 0;
    }
}
//******************************************************************************
void main(void) {
    //--------------------------Canal Analogico---------------------------------
    ANSEL = 0;
    ANSELH = 0; //Puerto A y B como digitales
    start_adc(2, 1, 0, 0); //Fosc/8, si ISR de ADC, Ref Vdd y Vcc, a la izquierda
    start_ch(12); //Habilita el pin del Puerto RB0.
    Select_ch(12); //Selecciona el canal e inicia la conversion.
    //--------------------------Comunicacion SPI--------------------------------
    INTCONbits.GIE = 1; // Habilitamos interrupciones
    INTCONbits.PEIE = 1; // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0; // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1; // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1; // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //--------------------------Puerto Entrada/salida---------------------------
    TRISBbits.TRISB0 = 1; //Unica entrada RB0
    //-------------------------------Limpieza de puertos------------------------   
    PORTB = 0; //Se limpian los puertos
    while (1) {
        
    }
}