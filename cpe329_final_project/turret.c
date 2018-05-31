#include "msp.h"
#include "delay.h"
#include "turret.h"

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
        TIMER_A0->CCR[3] = MAXLEFT; //duty cycle, start left
}

