#include "msp.h"
/**
 * main.c
 */
char character;
char lock;
uint8_t countDigits;
char array[4];
uint16_t multiplier;
#define myCS BIT0
#define GAIN BIT5
#define SHUTDN BIT4
void main(void)
{
    int i;
    uint16_t data;
    countDigits = 0;
    lock = 0;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    //configure UART pins
    P1->SEL0 |= BIT2|BIT3; //puts pins into UART mode

    //configure serial device, similar to EUSCBI
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;//reset mode
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST|EUSCI_A_CTLW0_SSEL__SMCLK;//configure to use SMCLK

    EUSCI_A0->BRW = 1;
    EUSCI_A0->MCTLW = (10<<EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;//table 22.4
    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_SWRST); //clear reset mode
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; //clear receive interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE; //enable interrupts

    P1->SEL0 |= BIT5;
    P1->SEL1 &= ~BIT5;
    P1->SEL0 |= BIT6;
    P1->SEL1 &= ~BIT6;

    P6->DIR |= myCS;
    P6->OUT |= myCS;

    P2->DIR |= BIT0|BIT1|BIT2;
    P2->OUT &= ~(BIT0|BIT1|BIT2);

//            EUSCI_A0->TXBUF = 'H';
//            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//            EUSCI_A0->TXBUF = 'e';
//            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//            EUSCI_A0->TXBUF = 'l';
//            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//            EUSCI_A0->TXBUF = 'l';
//            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//            EUSCI_A0->TXBUF = 'o';

    __enable_interrupt();

    NVIC->ISER[0] = 1<< ((EUSCIA0_IRQn) & 31);
//    while(1){
//        EUSCI_A0->TXBUF = 'H';
//        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//        EUSCI_A0->TXBUF = 'e';
//        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//        EUSCI_A0->TXBUF = 'l';
//        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//        EUSCI_A0->TXBUF = 'l';
//        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//        EUSCI_A0->TXBUF = 'o';
//    }
    while(1) {
        if (character != -1) {
            uint8_t hiByte, loByte;
            if (character == '\n') {//if 'return'
                data = 0;
                if (countDigits == 1) {
                    multiplier = 1;
                }
                else if (countDigits == 2) {
                    multiplier = 10;
                }
                else if (countDigits == 3) {
                    multiplier = 100;
                }
                else if (countDigits == 4) {
                    multiplier = 1000;
                }
                for (i = 0; i < countDigits; i++) {
                    data += (multiplier) * array[i];
                    multiplier /= 10;
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

//                EUSCI_A0->TXBUF = 'H';
//                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//                EUSCI_A0->TXBUF = 'e';
//                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//                EUSCI_A0->TXBUF = 'l';
//                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//                EUSCI_A0->TXBUF = 'l';
//                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
//                EUSCI_A0->TXBUF = 'o';

                P2->OUT &= ~(BIT0|BIT2|BIT3);

                countDigits = 0;
            }
            else if (character > 47 && character < 58) {
                if (countDigits == 0){
                    P2->OUT &= ~(BIT0|BIT2|BIT3);
                    P2->OUT |= BIT0;
                }
                else if (countDigits == 1){
                    P2->OUT &= ~(BIT0|BIT2|BIT3);
                    P2->OUT |= BIT1;
                }
                else if (countDigits == 2){
                    P2->OUT &= ~(BIT0|BIT2|BIT3);
                    P2->OUT |= BIT2;
                }
                else if (countDigits == 4){
                    P2->OUT &= ~(BIT0|BIT2|BIT3);
                    P2->OUT |= 0;
                }

                array[countDigits] = character - 48;
                countDigits++;
            }
        }
    }
}

void EUSCIA0_IRQHandler(){
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));//if it's not an IFG or an IFG is still going, wait
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF; //transmit?
        character = EUSCI_A0->RXBUF;
    }
}
