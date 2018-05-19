#include "msp.h"
#include "adc.h"
#include "vtFuncs.h"
#include <math.h>
#include <stdlib.h>

uint32_t dataTable[NUMPOINTS];
uint32_t newData[NUMPOINTS];
uint32_t count = 0;
uint8_t ready = 0;
uint8_t flag = 0;
uint32_t windowSize = 10;

uint32_t findMinIndex() {
    uint32_t i;
    for (i = 1; i < NUMPOINTS - 1 - (windowSize-1); i++) {
        if ((newData[i] <= newData[i-1] && newData[i] < newData[i+1]) ||
            (newData[i] < newData[i-1] && newData[i] <= newData[i+1])) {
            return i;
        }
    }
    return 0;
}

uint32_t findMaxIndex(uint32_t start) {
    uint32_t i;
    for (i = start; i < NUMPOINTS - 1 - (windowSize-1); i++) {
        if ((newData[i] >= newData[i-1] && newData[i] > newData[i+1]) ||
            (newData[i] > newData[i-1] && newData[i] >= newData[i+1])) {
            return i;
        }
    }
    return 0;
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
    TIMER_A0->CCR[0] = 60;
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
    uint32_t minIndex, maxIndex;
    uint8_t cont = 1;
    uint32_t a;
    while (1) {
        if (ready == 1 && cont == 1) {
            __disable_irq();

            //movingAverage
            for (a = 0; a < NUMPOINTS-(windowSize-1); a++) {
                newData[a] = (dataTable[a]
                             +dataTable[a+1]
                             +dataTable[a+2]
                             +dataTable[a+3]
                             +dataTable[a+4]) / windowSize;
            }

            for (i = 0; i < NUMPOINTS - windowSize + 1; i++) {
                print_int32(i);
                write_to_terminal(':');
                write_to_terminal(' ');
                write_to_terminal('[');
                print_int32(newData[i]);
                write_to_terminal(']');
                write_to_terminal('.');
                write_to_terminal(' ');
                write_to_terminal(' ');
            }
            minIndex = findMinIndex();
            maxIndex = findMaxIndex(minIndex+5);
            print_int32(minIndex);
            write_to_terminal(' ');
            print_int32(maxIndex);
            write_to_terminal(' ');
            cont = 0;
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
