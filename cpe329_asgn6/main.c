#include "msp.h"
#include "stdint.h"
#define GAIN BIT5
#define SHUTDN BIT4
#define myCS BIT1
#define TWOVOLTS 2482
#define ON 1
#define OFF 0

int state = ON;
void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    P1->SEL0 |= BIT5|BIT6|BIT7; //eUSCI_B_SPI
    P4->DIR |= BIT1;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //put SPI into reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC|EUSCI_B_CTLW0_CKPL|UCSSEL_2|EUSCI_B_CTLW0_MSB;
    EUSCI_B0->BRW = 0x02;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = CS_CTL0_DCORSEL_0;//1.5 MHz
    CS->KEY = 0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 15122;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
            TIMER_A_CTL_MC__UP;

    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR

    P1->SEL0 |= BIT5;
    P1->SEL1 &= ~BIT5;
    P1->SEL0 |= BIT6;
    P1->SEL1 &= ~BIT6;

    P4->DIR |= BIT1;
    P4->OUT |= myCS;

    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while (1)
    {
        __sleep();
        __no_operation();                   // For debugger
    }
}

void TA0_0_IRQHandler(void) {
    uint16_t data;
    uint8_t hiByte, loByte;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    if (state == ON) {
        data = 2482;
        state = OFF;
    }
    else {
        data = 0;
        state = ON;
    }

    loByte = 0xFF & data;
    hiByte = 0x0F & (data >> 8);
    hiByte |= GAIN|SHUTDN;
    P4->OUT &= ~myCS;

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = hiByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = loByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));

    P4->OUT |= myCS;
}
