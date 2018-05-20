#ifndef ADC_H_
#define ADC_H_

#include "msp.h"
#define NUMPOINTS 400

void write_to_terminal(char byte);
void write_voltage(double voltage);
double adc_get_voltage(uint32_t num);

#endif