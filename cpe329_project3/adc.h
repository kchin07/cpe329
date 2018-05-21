#ifndef ADC_H_
#define ADC_H_

#include "msp.h"
#define NUMPOINTS 15000

void write_to_terminal(char byte);
void print_voltage(double voltage);
double adc_get_voltage(uint32_t num);

#endif
