/*
 * File:   ADC_LIB.h
 * Author: Daniel Mundo
 *Descripcion: Se definen los prototipos de funciones a emplear por la libreria.
 */
#ifndef ADC_LIB_H
#define	ADC_LIB_H

#include <xc.h> 
#include <stdint.h>
void start_adc(uint8_t frec, uint8_t isr, uint8_t Vref, uint8_t justRL);
void Select_ch(uint8_t channel);
void start_ch(uint8_t channel);

#endif	/* ADC_LIB_H */
