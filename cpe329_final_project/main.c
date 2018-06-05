#include "msp.h"
#include "delay.h"
#include "turret.h"
#include "LCD.h"

#define NUMBER_TIMER_CAPTURES 20

#define ASCII_OFFSET 48

int ammoCount;

volatile uint16_t timerAcaptureValues[NUMBER_TIMER_CAPTURES] = {0};
uint16_t timerAcapturePointer = 0;

void uart_init() {
    // configure UART pins
    P1->SEL0 |= BIT2|BIT3; // puts pins into UART mode
    P1->SEL1 |= ~(BIT2|BIT3);
    // configure serial device, similar to EUSCBI
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // reset mode
    EUSCI_A0->MCTLW = 0;
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST|EUSCI_A_CTLW0_SSEL__SMCLK;
    // configure to use SMCLK

    EUSCI_A0->BRW = 4;
    EUSCI_A0->MCTLW = (14<<EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_SWRST);
}

void write_to_terminal(char byte) {
    EUSCI_A0->TXBUF = byte;
    while(!(EUSCI_A0->IFG&EUSCI_A_IFG_TXIFG)); //add a delay
    EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
}

char hexTable[16] = {'0','1','2','3','4','5','6','7','8','9',
                     'a','b','c','d','e','f'};

void set_cursor(uint8_t row, uint8_t col) {
  uint8_t character;
  write_to_terminal(27);//ESC
  write_to_terminal('[');

  character = row / 100;
  write_to_terminal(character + 48);
  row -= (row/100) * 100;
  character = row / 10;
  write_to_terminal(character + 48);
  row -= (row/10) * 10;
  character = row / 1;
  write_to_terminal(character + 48);

  write_to_terminal(';');

  character = col / 100;
  write_to_terminal(character + 48);
  col -= (col/100) * 100;
  character = col / 10;
  write_to_terminal(character + 48);
  col -= (col/10) * 10;
  character = col / 1;
  write_to_terminal(character + 48);

  write_to_terminal('H');
}

void clear_screen() {
  write_to_terminal(27);
  write_to_terminal('[');
  write_to_terminal('2');
  write_to_terminal('J');

  write_to_terminal(27);
  write_to_terminal('[');
  write_to_terminal('H');
}

void print_string(char* s) {
  while (*s != 0) {
    write_to_terminal((uint8_t)*s);
    s++;
  }
}

void print_int32(uint32_t i) {
  uint32_t highestPowerOf10 = 1000000000;

  if (i == 0) {
    write_to_terminal((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf10 == 0) {//find highestPowerOf10
    highestPowerOf10 /= 10;
  }

  while (highestPowerOf10 > 0) {
    write_to_terminal((uint8_t)hexTable[i/highestPowerOf10]);
    i -= (i/highestPowerOf10) * highestPowerOf10;
    highestPowerOf10 /= 10;
  }
}

void erase_line() {
  write_to_terminal(27);
  write_to_terminal('[');
  write_to_terminal('2');
  write_to_terminal('K');
}

void set_color(uint8_t color) {
  uint8_t character;
  write_to_terminal(27);
  write_to_terminal('[');

  character = color / 10;
  write_to_terminal(character+48);
  character = color - ((color/10) * 10);
  write_to_terminal(character+48);

  write_to_terminal('m');
}

void sensor_init() {
    P6->SEL0 &= ~(BIT4|BIT5);
    P6->SEL1 &= ~(BIT4|BIT5);
    P6->DIR = BIT4;
    TIMER32_1->LOAD = 24000000;
    TIMER32_1->CONTROL = 0xC2;
}

uint32_t getDistance() {
    uint32_t highTime, lowTime;
    P6->OUT |= BIT4;
    __delay_cycles(3000); // delay cycles for TTL
    P6->OUT &= ~BIT4;

    while ((P6->IN & BIT5) == 0); // while still input low
    // now is high
    highTime = TIMER32_1->VALUE;
    while ((P6->IN & BIT5) == BIT5); // while still input high
    //now is low
    lowTime = TIMER32_1->VALUE;

    while ((TIMER32_1->RIS & 1) == 0);
    TIMER32_1->INTCLR = 0;

    return (highTime - lowTime) / (24 * 58);
}

void main(void){

    turret_init();
    uart_init();
    sensor_init();
    LCD_Keypad_init();
    motor_off();

    P2->SEL0 &= ~BIT0;
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0;
    P2->OUT &= ~BIT0;

    uint32_t distanceCms;

    Clear_LCD();
    Home_LCD();
    LCD_ammo_prompt();

    ammoCount = 0;
    ammoCount = reload_turret();

    int rotateCount = 0;
    char direction = 1; //0 = left, 1 = right
    int start = MAXRIGHT;
    char index;
    char motorOnFlag = 0;
//    motor_off();
    motor_on();
    while(1){
            if(ammoCount > 0){
                distanceCms = getDistance();
                set_cursor(1,1);
                erase_line();
                print_string("centimeters: ");
                print_int32(distanceCms);

                if(distanceCms < 25){
                    P2->OUT |= BIT0;
                    pull_trigger();
                    ammoCount--;
                    LCD_display_ammo_count(ammoCount);
                }
                else{
                    P2->OUT &= ~BIT0;

                    //switch direction
                    if(rotateCount == DEGREES180){
                        rotateCount = 0;
                        direction ^= 1;
                    }
                    //rotate 180 degrees
                    else{
                        if(direction == 0){
                            start -= TENDEGREES;
                        }
                        else{
                            start += TENDEGREES;
                        }
                        TIMER_A0->CCR[3] = start;
                    }
                    rotateCount++;
                }
            }
            else{
                motor_off();
                ammoCount = reload_turret();
                motor_on();
            }
    }


    /************************************testing***********************************************/

//    motor_on();
//
//    delay_ms(3000, FREQ_24MHz);
//
//    motor_off();

    /************************************************************************************/

//    motor_on();

//    int count = 0;
//    char direction = 0; //0 = left, 1 = right
//    int start = MAXLEFT;
//
//    int cycles = 0;
//    int iterations = 0;
//
//	while(iterations != 1){
//	    //distance detection
//	     distanceCms = getDistance();
//
//	     set_cursor(1,1);
//	     erase_line();
//
//	     print_string("centimeters: ");
//	     print_int32(distanceCms);
//
//	     if(distanceCms < 25){//
//	         while(distanceCms < 50){
//
//	             pull_trigger();
//
////	             delay_ms(1000, FREQ_24MHz);//pause
//
//	             distanceCms = getDistance();
//	         }
//	     }
//	     else{
//            //switch direction
//            if(count == DEGREES180){
//                count = 0;
//                direction ^= 1;
//                cycles++;
//            }
//
//            //rotate 180 degrees
//            else{
//                if(direction == 0){
//                    start -= FIVEDEGREES;
//                }
//                else{
//                    start += FIVEDEGREES;
//                }
//                TIMER_A0->CCR[3] = start;
//            }
//            count ++;
//    //	    delay_ms(100, FREQ_24MHz);//controls how fast the servo rotates
//	     }
//	}

//	motor_off();

}
