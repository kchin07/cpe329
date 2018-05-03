#include "msp.h"
#include "stdint.h"
#define GAIN BIT5
#define SHUTDN BIT4
#define myCS BIT0
#define TWOVOLTS 2482

//freq count eq: count = 756336/freq
#define ONEHUNDREDFREQ 7561
#define TWOHUNDREDFREQ 3780
#define THREEHUNDREDFREQ 2520
#define FOURHUNDREDFREQ 1890
#define FIVEHUNDREDFREQ 1511

//4096
#define ON 1
#define OFF 0

int state = ON;
char keypadfreq = 1;
void main(void) {
    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);

    P4->DIR = 0x78;//MAKE P4.3-P4.6 outputs, rest are inputs
    P4->REN = 0x07;//make P4.0-P4.2 resistors enabled
    P4->OUT = 0x07;//make P4.0-P4.2 resistors set as pull-up

    P2->SEL1 &= ~(BIT0|BIT1|BIT2);  /* configure P2.0-P2.2 as simple I/O */
    P2->SEL0 &= ~(BIT0|BIT1|BIT2);
    P2->DIR |= BIT0|BIT1|BIT2;          /* P2.0-P2.2 set as output pin */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    P1->SEL0 |= BIT5|BIT6|BIT7; //eUSCI_B_SPI
    P6->DIR |= myCS;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //put SPI into reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC|EUSCI_B_CTLW0_CKPL|UCSSEL_2|EUSCI_B_CTLW0_MSB;
    EUSCI_B0->BRW = 0x02;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

   //CS->KEY = CS_KEY_VAL;
//    CS->CTL0 = CS_CTL0_DCORSEL_0;//1.5 MHz
//    CS->KEY = 0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = THREEHUNDREDFREQ * 10; //starting number for timer to count up to aka frequency
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
            TIMER_A_CTL_MC__UP;

    P1->SEL0 |= BIT5;
    P1->SEL1 &= ~BIT5;
    P1->SEL0 |= BIT6;
    P1->SEL1 &= ~BIT6;

    P6->DIR |= myCS;
    P6->OUT |= myCS;

    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while (1)
    {
        keypadfreq = get_pin();
        displayAndLight(keypadfreq);
//        __sleep();
//        __no_operation();                 // For debugger
    }
}

void TA0_0_IRQHandler(void) {
    uint16_t data;
    uint8_t hiByte, loByte;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    if(keypadfreq == 1){
        TIMER_A0->CCR[0] = ONEHUNDREDFREQ * 10;
    }
    else if(keypadfreq == 2){
        TIMER_A0->CCR[0] = TWOHUNDREDFREQ;
    }
    else if(keypadfreq == 3){
        TIMER_A0->CCR[0] = THREEHUNDREDFREQ;
    }
    else if(keypadfreq == 4){
        TIMER_A0->CCR[0] = FOURHUNDREDFREQ;
    }
    else if(keypadfreq == 5){
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 6){
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 7){//square
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 8){//sine
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 9){//sawtooth
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 10){//decrease
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 11){//reset to 50%
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }
    else if(keypadfreq == 12){//increase
        TIMER_A0->CCR[0] = FIVEHUNDREDFREQ;
    }

    if (state == ON) {
        data = TWOVOLTS;
        state = OFF;
    }
    else {
        data = 0;
        state = ON;
    }

    loByte = 0xFF & data;
    hiByte = 0x0F & (data >> 8);
    hiByte |= GAIN|SHUTDN;
    P6->OUT &= ~myCS;

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = hiByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = loByte;
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    P6->OUT |= myCS;

}
