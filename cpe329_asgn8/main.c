#include "msp.h"
/**
 * main.c
 */
char character;
char lock;
uint8_t countDigits;
char array[4];
char arrayIndex;
uint16_t data;
uint16_t multiplier;
#define myCS BIT0
#define GAIN BIT5
#define SHUTDN BIT4
void main(void)
{
    int i;
    countDigits = 0;
    lock = 0;
    arrayIndex = 0;
    data = 0;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    P1->SEL0 |= BIT5|BIT6|BIT7; //eUSCI_B_SPI
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //put SPI into reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC|EUSCI_B_CTLW0_CKPL|UCSSEL_2|EUSCI_B_CTLW0_MSB;
    EUSCI_B0->BRW = 0x02;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

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

    __enable_interrupt();

    NVIC->ISER[0] = 1<< ((EUSCIA0_IRQn) & 31);

    while(1) {
        __disable_irq();
        uint8_t hiByte, loByte;
        if (lock == 1) {//if 'return'
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
            countDigits = 0;
            lock = 0;
        }

        __enable_irq();
    }
}

void EUSCIA0_IRQHandler(){
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));//if it's not an IFG or an IFG is still going, wait
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF; //transmit?
        character = EUSCI_A0->RXBUF;
        if(character == 13){
            lock = 1;
        }
        else if (character > 47 && character < 58) {
            array[countDigits] = character - 48;
            countDigits++;
        }
    }
}
