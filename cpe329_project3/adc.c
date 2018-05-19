#include "msp.h"
#include "adc.h"

void write_to_terminal(char byte) {
    EUSCI_A0->TXBUF = byte;
    while(!(EUSCI_A0->IFG&EUSCI_A_IFG_TXIFG)); //add a delay
    EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
}

void write_voltage(double voltage) {
    voltage *= 10000;
    int newVoltage = (int)voltage;
    unsigned char output[6];
    int divisor = 10000;
    int i;
    for (i = 0; i < 6; i++) {
        if (newVoltage < 100 && i == 0) {
            output[i] = '0';
            divisor /= 10;
        }
        else if (i == 1) {
            output[i] = '.';
        }
        else {
            output[i] = (newVoltage / divisor) + 48;
            newVoltage -= (output[i]-48)* divisor;
            divisor /= 10;
        }
    }

    for (i = 0; i < 6; i++) {
        write_to_terminal(output[i]);
    }
    write_to_terminal(' ');
}

double adc_get_voltage(uint32_t num) {
    return (num * 3.3) / 1023;
}
