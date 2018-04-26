#include "msp.h"
#include "stdint.h"
#define GAIN BIT5
#define HUTDN BIT4
#define CS BIT1

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	uint16_t data;
	    uint8_t hiByte, loByte;
	    P1->SEL0 |= BIT5|BIT6|BIT7; //eUSCI_B_SPI
	    P4->DIR |= CS;
	    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //put SPI into reset
	    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST|EUSCI_B_CTLW0_MST|EUSCI_B_CTLW0_SYNC|EUSCI_B_CTLW0_CKPL|UCSSEL_2|EUSCI_B_CTLW0_MSB;
	    EUSCI_B0->BRW = 0x02;
	    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

	    P1->SEL0 |= BIT5;
	    P1->SEL1 &= ~BIT5;
        P1->SEL0 |= BIT6;
        P1->SEL1 &= ~BIT6;

        P4->DIR |= BIT1;
        P4->OUT |= BIT1;

	    while(1){
	        for(data = 0; data < 4096; data++){
	            loByte = 0xFF & data;
	            hiByte = 0x0f & (data >> 8);

	            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG)){
	                EUSCI_B0->TXBUF = hiByte;
	            }
	            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG)){
	                EUSCI_B0->TXBUF = loByte;
	            }
	            while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG)){
	                P4->OUT |= CS;
	            }
	       }
	    }

}
