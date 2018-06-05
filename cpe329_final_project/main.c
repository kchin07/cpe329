#include "msp.h"
#include "delay.h"
#include "turret.h"
#include "LCD.h"
#include "keypad.h"

int ammoCount;

void main(void){

    turret_init();
    sensor_init();
    LCD_Keypad_init();
    motor_off();

    //initialize LED pin for signaling object detection
    P2->SEL0 &= ~BIT0;
    P2->SEL1 &= ~BIT0;
    P2->DIR |= BIT0;
    P2->OUT &= ~BIT0;


    Clear_LCD();
    Home_LCD();
    LCD_ammo_prompt();

    ammoCount = 0;
    ammoCount = reload_turret();
    int rotateCount = 0;
    char direction = 1; //0 = left, 1 = right
    int start = MAXRIGHT;
    char motorOnFlag = 0;
    uint32_t distanceCms;

//    motor_off();
    motor_on();

    while(1){

        //checks that the turret has ammo to use
        if(ammoCount > 0){

            distanceCms = getDistance();

            //if object is within firing range
            if(distanceCms < 25){
                P2->OUT |= BIT0;
                pull_trigger();
                ammoCount--;
                LCD_display_ammo_count(ammoCount);
            }
            else{
                P2->OUT &= ~BIT0;

                //switch rotating direction
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
        //turret is out of ammo
        else{
            motor_off();
            ammoCount = reload_turret();
            distanceCms = 10000;
            motor_on();
        }
    }
}
