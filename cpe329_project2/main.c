#include "msp.h"
#include "stdint.h"
#include <math.h>
#include <stdlib.h>
#include "delay.h"
#include "keypad.h"

#define GAIN BIT5
#define SHUTDN BIT4
#define myCS BIT0
#define VOLTAGE 2048 // voltage for half the maximum

volatile uint16_t count; // variable for spot in lookup table
volatile uint16_t* table; // variable for lookup table
volatile char mode;// 0 - square, 1 - sin, 2 - sawtooth
volatile int intervals; // variable for number of discrete values in shape
volatile int dutyCycle; // duty cycle
volatile char lock;
volatile char pinId;

void initSquareLookup() {
    table = malloc(sizeof(uint16_t) * intervals);
    int i;
    int changePoint = (int)(intervals * ((float)dutyCycle / 100.0));

    for (i = 0; i < changePoint; i++) {
        table[i] = VOLTAGE;
    }
    for (i = changePoint; i < intervals; i++) {
        table[i] = 0;
    }
}

void initSinLookup() {
    table = malloc(sizeof(uint16_t) * intervals);
    int temp = intervals / 2;
    float i;
    int counter = 0;
    for (i = 0; i < 2 * M_PI; i += M_PI / temp) {
        table[counter++] = (int)((VOLTAGE / 2) * (sin(i)+1));
    }
}

void initSawtoothLookup() {
    table = malloc(sizeof(uint16_t) * intervals);
    int i;
    for (i = 0; i < intervals; i++) {
        table[i] = (int)(VOLTAGE * ((float)i / (float)intervals));
    }
}

void changeWaveform() {
    free(table);
    if (mode == 0) {
        initSquareLookup();
    }
    else if (mode == 1) {
        initSinLookup();
    }
    else if (mode == 2) {
        initSawtoothLookup();
    }
}

void initSystem() {
    count = 0; // start of lookup
    mode = 0; // square mode
    intervals = 500; // 100 Hz frequency
    dutyCycle = 50; // 50% duty cycle
    lock = 0;
    initSquareLookup();
}

void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initSystem(); // initializations

    P4->DIR = 0x78;//MAKE P4.3-P4.6 outputs, rest are inputs
    P4->REN = 0x07;//make P4.0-P4.2 resistors enabled
    P4->OUT = 0x07;//make P4.0-P4.2 resistors set as pull-up

    P1->SEL0 |= BIT5|BIT6|BIT7; //eUSCI_B_SPI
    P6->DIR |= myCS;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //put SPI into reset
    EUSCI_B0->CTLW0 =
        EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC |
        EUSCI_B_CTLW0_CKPL | UCSSEL_2 | EUSCI_B_CTLW0_MSB;
    EUSCI_B0->BRW = 0x02;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = CS_CTL0_DCORSEL_4;//24 MHz
    CS->KEY = 0;

    //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; //Enable sleep on exit from ISR

    P1->SEL0 |= BIT5;
    P1->SEL1 &= ~BIT5;
    P1->SEL0 |= BIT6;
    P1->SEL1 &= ~BIT6;

    P6->DIR |= myCS;
    P6->OUT |= myCS;

    P2->DIR |= BIT2|BIT1|BIT0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; //TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 480;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | //SMCLK, up mode
        TIMER_A_CTL_MC__UP;

    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while (1)
    {
        //__disable_irq();
        pinId = get_pin();
        if (pinId == 0) {
            lock = 0;
        }
        displayAndLight(pinId);
        //__enable_irq();
        //__sleep();
        //__no_operation(); //For debugger
    }
}

void TA0_0_IRQHandler(void) {
    uint16_t data;
    uint8_t hiByte, loByte;

    if(pinId == 1){ //change to 100 Hz
        intervals = 500;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 2){ //change to 200 Hz
        intervals = 250;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 3){ //change to 300 Hz
        intervals = 167;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 4){ //change to 400 Hz
        intervals = 125;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 5){ //change to 500 Hz
        intervals = 100;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 7){//square
        mode = 0;
        free(table);
        initSquareLookup();
        count = 0;
    }
    else if(pinId == 8){//sine
        mode = 1;
        free(table);
        initSinLookup();
        count = 0;
    }
    else if(pinId == 9){//sawtooth
        mode = 2;
        free(table);
        initSawtoothLookup();
        count = 0;
    }
    else if(pinId == 10){//decrease
        if (dutyCycle > 10 && lock == 0) {
            dutyCycle -= 10;
            lock = 1;
        }
        changeWaveform();
        count = 0;
        delay_ms(500,FREQ_24MHz);// debounce effect
    }
    else if(pinId == 11){//reset to 50%
        dutyCycle = 50;
        changeWaveform();
        count = 0;
    }
    else if(pinId == 12){//increase
        if (dutyCycle < 90 && lock == 0) {
            dutyCycle += 10;
            lock = 1;
        }
        changeWaveform();
        delay_ms(500,FREQ_24MHz); // debounce effect
        count = 0;
    }

    if (count >= intervals) {
        count = 0;
    }

    data = table[count++];

    //send it
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
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}
