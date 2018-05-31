#include "msp.h"
#include "delay.h"
#include "turret.h"
#include "LCD.h"

#define NUMBER_TIMER_CAPTURES 20

#define ASCII_OFFSET 48

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

void timer_init() {
}

//2.7 - trigger, output
//2.5 - echo, input

void sensor_init() {
    P4->SEL0 &= ~(BIT1|BIT2);
    P4->SEL1 &= ~(BIT1|BIT2);
    P4->DIR = BIT1;
    TIMER32_1->LOAD = 24000000;
    TIMER32_1->CONTROL = 0xC2;
}

uint32_t getDistance() {
    uint32_t highTime, lowTime;
    P4->OUT |= BIT1;
    __delay_cycles(3000); // delay cycles for TTL
    P4->OUT &= ~BIT1;

    while ((P4->IN & BIT2) == 0); // while still input low
    // now is high
    highTime = TIMER32_1->VALUE;
    while ((P4->IN & BIT2) == BIT2); // while still input high
    //now is low
    lowTime = TIMER32_1->VALUE;

    while ((TIMER32_1->RIS & 1) == 0);
    TIMER32_1->INTCLR = 0;

    return (highTime - lowTime) / (24 * 58);
}

void main(void){

    char index;

    LCD_Keypad_init();

    LCD_ammo_prompt();

    int startGun = 0;
    int ammoCountNums = 0;
    char ammoCountString[2];
    int ammoCountInt = 0;

    while(startGun == 0){
        index = get_pin();
        if(index > 0){
            index = index;
            if(index == 12){
                startGun = 1;
            }
            else if(ammoCountNums< 2){
                Write_char_LCD(index + ASCII_OFFSET);
                delay_ms(200, FREQ_24MHz);
                ammoCountString[ammoCountNums] = index + ASCII_OFFSET;
                ammoCountNums++;
            }
        }
    }

    Clear_LCD();
    Home_LCD();

    char* string2 = "ammo count:";
    Write_string_LCD(string2);
    Write_string_LCD(ammoCountString);

    int i;
    for(i = 0; i < 2; i++){
        if(ammoCountNums == 1){
            if(i == 0){
                ammoCountInt += (ammoCountString[i] - ASCII_OFFSET);
            }
        }
        else{
            if(i == 0){
                ammoCountInt += (ammoCountString[i] - ASCII_OFFSET)  * 10;
            }
            else{
                ammoCountInt += (ammoCountString[i]- ASCII_OFFSET);
            }
        }
    }

//    ammoCountInt = convert_ammo_string_to_int(ammoCountString, ammoCountNums);



    while(1){
        ammoCountInt = ammoCountInt;
    }

    turret_init();
    uart_init();
    sensor_init();
    uint32_t distanceCms;

    /************************************testing***********************************************/

//    motor_on();
//
//    delay_ms(3000, FREQ_24MHz);
//
//    motor_off();

    /************************************************************************************/

//    motor_on();

    int count = 0;
    char direction = 0; //0 = left, 1 = right
    int start = MAXLEFT;

    int cycles = 0;
    int iterations = 0;

	while(iterations != 1){
	    //distance detection
	     distanceCms = getDistance();

	     set_cursor(1,1);
	     erase_line();

	     print_string("centimeters: ");
	     print_int32(distanceCms);
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
	}

//	motor_off();

}
