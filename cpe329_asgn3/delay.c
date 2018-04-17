#include "delay.h"
#include "msp.h"

int set_dco(int input) { //function that sets the DCO clock to one of 6 given
                         //frequencies that are defined in delay.h
    CS->KEY = CS_KEY_VAL;//unlock key register
    CS->CTL0 = 0;
    if (input < 1.5 || input > 48) {//setting frequency based on input
        return -1;
    }
    else if (input == FREQ_1P5MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_0;
    }
    else if (input == FREQ_3MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_1;
    }
    else if (input == FREQ_6MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_2;
    }
    else if (input == FREQ_12MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_3;
    }
    else if (input == FREQ_24MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_4;
    }
    else if (input == FREQ_48MHz) {
        CS->CTL0 = CS_CTL0_DCORSEL_5;
        CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) | CS_CTL1_SELM_3;//special statement for 48 MHz frequency
    }
    else {
        return -1;
    }
    CS->KEY = 0;
    return 0;
}


void delay_ms(int numSec, int sysFreq) {
    set_dco(sysFreq);//part of the delay calculated in this function
    int numCycles = (numSec * sysFreq)/(0.0033 * 6);
    int i;
    for (i = 0; i < numCycles; i++) {
        __delay_cycles(10);
    }
}

void delay_us(int numSec, int sysFreq) {
    return;
    int numCycles = 2 * numSec + 15;
    int i;
    for (i = 0; i < numCycles; i++) {
        ;
    }
}
