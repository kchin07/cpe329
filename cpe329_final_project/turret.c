#include "msp.h"
#include "delay.h"
#include "turret.h"
#define ASCII_OFFSET 48

void pull_trigger(){
     TIMER_A0->CCR[1] = MAXLEFT; //pull trigger

     delay_ms(1000, FREQ_24MHz);//wait for servo to rotate

     TIMER_A0->CCR[1] = MAXRIGHT; //release trigger

     delay_ms(1000, FREQ_24MHz);//wait for servo to rotate
}

void motor_on(){
    delay_ms(1000, FREQ_24MHz);

    TIMER_A0->CCR[2] = SERVOCENTER + FOURTYDEGREES; //activate gun motor

    delay_ms(1000, FREQ_24MHz);//wait for servo to rotate
}

void motor_off(){
    delay_ms(1000, FREQ_24MHz);

    TIMER_A0->CCR[2] = SERVOCENTER; //activate gun motor

    delay_ms(1000, FREQ_24MHz);//wait for servo to rotate
}

void turret_init(){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

        CS->KEY = CS_KEY_VAL; //unlocked, now can modify clock system
        CS->CTL0 = 0; //reset clock tuning
        CS->CTL0 = CS_CTL0_DCORSEL_4;//set the frequency to 1.5MHz, CS_CTL0_DCORSEL_4 for 24 MHz, CS_CTL0_DCORSEL_0 for 1.5MHz
        CS->CTL1 |= CS_CTL1_DIVS_5; //div clk by 2, CS_CTL1_DIVS_5 to div by 32, CS_CTL1_DIVS_1 to div by 2
        CS->KEY = 0; //lock clock system

        //set P2.4 to PWM for TimerA
        P2->SEL0 |= BIT4;
        P2->SEL1 &= ~BIT4;
        P2->DIR |= BIT4; //set P2.4 as output

        //set P2.6 to PWM for TimerA
        P2->SEL0 |= BIT6;
        P2->SEL1 &= ~BIT6;
        P2->DIR |= BIT6; //set P2.6 as output

        //set P2.5 to PWM for TimerA
        P2->SEL0 |= BIT5;
        P2->SEL1 &= ~BIT5;
        P2->DIR |= BIT5; //set P2.5 as output

        //initiliaze TimerA in capture mode
        TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_2; //set/reset mode, pulling the trigger
        TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_2; //set/reset mode, motor
        TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_2; //set/reset mode, rotating the gun
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, up mode
                TIMER_A_CTL_MC__UP;
        TIMER_A0->CCR[0] = 18000; // want a 24ms period, so 24ms/(1/(0.75*10^6)) = 18000

        TIMER_A0->CCR[1] = MAXRIGHT; //duty cycle, start right
        TIMER_A0->CCR[2] = SERVOCENTER; //duty cycle, start center
        TIMER_A0->CCR[3] = MAXRIGHT; //duty cycle, start left
//        delay_ms(1000, FREQ_24MHz);
//        TIMER_A0->CCR[3] = MAXRIGHT; //duty cycle, start left
//        delay_ms(1000, FREQ_24MHz);
//        TIMER_A0->CCR[3] = MAXLEFT; //duty cycle, start left

}

void LCD_Keypad_init(){
    LCD_INIT();//initializes the LCD screen
    LCD_CMD(0x80); //sets LCD to output to top row of screen

    /*initializes P4.0-P4.6 to general purpose I/O*/
    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);

    P4->DIR = 0x78; //make P4.3-P4.6 outputs, rest are inputs
    P4->REN = 0x07; //make P4.0-P4.2 resistors enabled
    P4->OUT = 0x07; //make P4.0-P4.2 resistors set as pull-up

    delay_ms(1,FREQ_24MHz);
}

void LCD_ammo_prompt(){
    char* string1 = "enter capacity";
    Write_string_LCD(string1);
}

int convert_ammo_string_to_int(char* ammoCountString, int ammoCountNums){
    int i;
    int ammoCountInt = 0;
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
    return ammoCountInt;
}

int reload_turret(){

    Clear_LCD();
    Home_LCD();
    LCD_ammo_prompt();
    LCD_CMD(0xC0);

    char index;
    int startGun = 0;
    int ammoCountNums = 0;
    char ammoCountString[2];
    int ammoCountInt = 0;

    while(startGun == 0){
        index = get_pin();
        if(index > 0){
            if(index == 12){
                startGun = 1;
            }
            else if(index < 10 || index == 11){
                if(ammoCountNums< 2){
                    Write_char_LCD(index + ASCII_OFFSET);
                    delay_ms(200, FREQ_24MHz);
                    if(index == 11){
                        index = 0;
                    }
                    ammoCountString[ammoCountNums] = index + ASCII_OFFSET;
                    ammoCountNums++;
                }
            }
        }
    }

    Clear_LCD();
    Home_LCD();

    char* string2 = "ammo count:";
    Write_string_LCD(string2);
    Write_string_LCD(ammoCountString);

    ammoCountInt = convert_ammo_string_to_int(ammoCountString, ammoCountNums);

    return ammoCountInt;
}

void LCD_display_ammo_count(int ammoCount){
    Clear_LCD();
    Home_LCD();
    char* string2 = "ammo count:";
    Write_string_LCD(string2);
    int tens = 0;
    if(ammoCount > 9){
        tens = ammoCount/10;
        Write_char_LCD(tens+ASCII_OFFSET);
    }
    ammoCount -= (tens*10);
    Write_char_LCD(ammoCount+ASCII_OFFSET);
}


