#include "msp.h"
#include "adc.h"
#include "vtFuncs.h"
#include <math.h>
#include <stdlib.h>

#define STATE0 0
#define STATE1 1

uint16_t dataTable[NUMPOINTS];
char newData[NUMPOINTS];
char state = STATE0; // start at STATE0
uint32_t count = 0;
uint8_t ready = 0;
uint32_t windowSize = 5;

double true_rms(int32_t interval){
    int i = 0;
    double sum = 0;
    uint32_t temp;
    for(i = 10; i <= interval+10; i++){
        temp = dataTable[i];
//        print_int32(temp);
//        write_to_terminal(' ');
        sum += temp*temp;
    }
    sum = sum/(interval);
    sum = sqrt(sum);

    return sum;
}

uint32_t convert_to_freq(int32_t interval){
    double inverseInterval = 1.0/interval;
    uint32_t frequency;
    if(state == STATE1){
        frequency = (7551.7 * inverseInterval) + 0.0019;
    }
    else{
        frequency = (197988 *inverseInterval) + 0.0624;
    }
    return frequency;
}

uint32_t findFallingEdgeIndex(uint32_t start) {
    uint32_t i;
    for (i = start; i < NUMPOINTS-1; i++) {
        if (newData[i] == 1 && newData[i+1] == 0) {
            return i;
        }
    }
    return 0;
}

uint32_t findRisingEdgeIndex(uint32_t start) {
    uint32_t i;
    for (i = start; i < NUMPOINTS-1; i++) {
        if (newData[i] == 0 && newData[i+1] == 1) {
            return i;
        }
    }
    return 0;
}

void filter() {
    uint32_t i;
    for (i = 0; i < NUMPOINTS - windowSize + 1; i++) {
        dataTable[i] = (dataTable[i]
                      +dataTable[i+1]
                      +dataTable[i+2]
                      +dataTable[i+3]
                      +dataTable[i+4]) / windowSize;
    }
}

uint32_t findMin() {
    uint32_t i;
    uint32_t minIndex = 0;
    for (i = 1; i < NUMPOINTS; i++) {
        if (dataTable[minIndex] > dataTable[i]) {
            minIndex = i;
        }
    }
    return dataTable[minIndex];
}

uint32_t findMax() {
    uint32_t i;
    uint32_t maxIndex = 0;
    for (i = 1; i < NUMPOINTS; i++) {
        if (dataTable[maxIndex] < dataTable[i]) {
            maxIndex = i;
        }
    }
    return dataTable[maxIndex];
}

void uart_init() {
    // configure UART pins
    P1->SEL0 |= BIT2|BIT3; // puts pins into UART mode
    P1->SEL1 |= ~(BIT2|BIT3);
    // configure serial device, similar to EUSCBI
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // reset mode
    EUSCI_A0->MCTLW = 0;
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST|EUSCI_A_CTLW0_SSEL__SMCLK;
    // configure to use SMCLK

    EUSCI_A0->BRW = 13;//13    n 73
    EUSCI_A0->MCTLW = (73<<EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16; // table 22.4
    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_SWRST); // clear reset mode
}

int main(void) {
    uint32_t i;
    WDT_A->CTL = WDT_A_CTL_PW |   // Stop WDT
                 WDT_A_CTL_HOLD;
    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_4;
    CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) | CS_CTL1_SELM_3;
    CS->KEY = 0;

    uart_init();

    // GPIO Setup
    P5->SEL1 |= BIT4;   // Configure P5.4 for ADC
    P5->SEL0 |= BIT4;

    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_3 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_3;
    // Use sampling timer, 14-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    // A1 ADC input select; Vref=AVCC
    //ADC14->IER0 |= ADC14_IER0_IE0;
    // Enable ADC conv complete interrupt

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; //TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 60; // 60, 3200
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | //SMCLK, up mode
        TIMER_A_CTL_MC__UP;

    // Enable global interrupt
    __enable_irq();

    // Enable ADC interrupt in NVIC module
    //NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    //clear_screen();
    //set_cursor(1,1);
    uint32_t fall1, fall2;
    uint16_t min, max, mid;
    int32_t interval;
    uint8_t cont = 1;
    uint32_t a;
    uint32_t frequency;
    while (1) {
        if (ready == 1 && cont == 1) {
            __disable_irq();
            filter();
            min = findMin();
            max = findMax();
            mid = (min + max) / 2;

            for (i = 0; i < NUMPOINTS; i++) {
                if ((int32_t)dataTable[i] - (int32_t)mid >= 0) {
                    newData[i] = 1;
                }
                else {
                    newData[i] = 0;
                }
            }

            fall1 = findFallingEdgeIndex(1);
            fall2 = findFallingEdgeIndex(fall1+10);

            interval = abs((int32_t)fall1 - (int32_t)fall2);
            if ((interval > 5000 && state == STATE0) || interval < 150) { // SWITCH TO STATE1
                state = STATE1;
                TIMER_A0->CCR[0] = 3200;
                TIMER_A0->R = 0;
                ready = 0;
                count = 0;
            }
            else { // WE GOOD
                frequency = convert_to_freq(interval);
                clear_screen();
                print_int32(frequency);
                print_string("Hz");
                state = STATE0;
                TIMER_A0->CCR[0] = 60;
                TIMER_A0->R = 0;
                ready = 0;
                count = 0;
            }
            __enable_irq();
        }
    }
    return 1;
}

void TA0_0_IRQHandler(void) {
    dataTable[count++] = ADC14->MEM[0];
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    if (count >= NUMPOINTS) {
        ready = 1;
    }
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

// ADC14 interrupt service routine
/*void ADC14_IRQHandler(void) {
    dataTable[count++] = ADC14->MEM[0];
    flag = 1;
}*/
