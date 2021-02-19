/*Slave 3_Sensor de Temperatura 
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
volatile int voltageLM35 = 0;
float temperature = 0.00;
#define green PORTDbits.RD0 
#define yellow PORTDbits.RD1 
#define red PORTDbits.RD2 
//******************************************************************************
//***************************Código de Interrupción***************************** 
void __interrupt() isr(void) {
    if (PIR1bits.ADIF == 1) {
        voltageLM35 = (ADRESH << 2 | ADRESL >> 6); //se guarda el valor del ADC en la variable
        PIR1bits.ADIF = 0;
        __delay_us(50);
        ADCON0bits.GO = 1;
    }
    if (SSPIF == 1) { //Si se selecciona como el esclavo activo
        spiWrite(temperature); //se envia el valor de la temperatura del LM35 
        SSPIF = 0;
    }
}
//******************************************************************************

void main(void) {
    //--------------------------Canal Analogico---------------------------------
    ANSEL = 0;
    ANSELH = 0; //Puerto A y B como digitales
    start_adc(2, 1, 0, 0); //Fosc/8, No ISR de ADC, Ref Vdd y Vcc, a la izquierda
    start_ch(12); //Habilita el pin del Puerto RB0.
    Select_ch(12); //Selecciona el canal e inicia la conversion.
    //--------------------------Comunicacion SPI--------------------------------
    INTCONbits.GIE = 1; // Se habilita las interrupciones
    INTCONbits.PEIE = 1; // Se habilita las interrupciones PEIE
    PIR1bits.SSPIF = 0; // Se borra la bandera interrupción MSSP
    PIE1bits.SSPIE = 1; // Se habilita la interrupción MSSP
    TRISAbits.TRISA5 = 1; // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    //--------------------------Puerto Entrada/salida---------------------------
    TRISD = 0; //Puerto D como salida 
    TRISB = 1; //RB0 como unica entrada
    //-------------------------------Limpieza de puertos------------------------   
    PORTB = 0;
    PORTD = 0; //Se limpian los puertos
    while (1) {
        temperature = (voltageLM35 * 0.4888); //La operacion corresponde 
        //(volLeido*Vcc*100°C/mV)/resoluciondelADC 
        if (temperature < 25) { //si el resultado de la operacion es menor a 25°C
            PORTD = 0; //se asegura que unicamente este prendido el LED requerido
            green = 1; //se enciende el LED verde
        } else if (temperature < 36) { //si es mayor a 25°C y menor a 36°C
            PORTD = 0;
            yellow = 1; //se enciende el LED amarillo
        } else { //en caso sea mayor a 36°C
            PORTD = 0;
            red = 1; //se enciende el LED de color rojo
        }
    }
}