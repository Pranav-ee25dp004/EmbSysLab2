/*
 * ESL Lab, IIT Dharwad - Lab 5 Part1 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem: RED LED toggles every 2 s, BLUE and GREEN LED controlled by switches on TM4C123GH6PM
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED_LED   (1<<1)  // PF1
#define BLUE_LED  (1<<2)  // PF2
#define GREEN_LED (1<<3)  // PF3
#define SW1        0x10   // PF4
#define SW2        0x01   // PF0

#define ST_CTRL_R    (*((volatile uint32_t *)0xE000E010))
#define ST_RELOAD_R  (*((volatile uint32_t *)0xE000E014))
#define ST_CURRENT_R (*((volatile uint32_t *)0xE000E018))

#define CLK_FREQ 16000000
#define DELAY 0.5  // 2 seconds
#define DELAY_CNT 4
#define RELOAD_VALUE (uint32_t)((DELAY*CLK_FREQ)-1)
#define ENABLE 1

void SysTick_Wait(void) {
    int x;

    // Start SysTick
    ST_CTRL_R |= 1;      // enable counting
    x = 0;
    while(x == 0) {      // wait for COUNTFLAG
        x = ST_CTRL_R & (1<<16);
    }

    // Reload SysTick
    ST_CTRL_R &= ~ENABLE;
    ST_CURRENT_R = 0;
    ST_CTRL_R |= ENABLE;
}


int main(void) {

    // Enable Port F clock
    SYSCTL_RCGC2_R |= 0x20;
    volatile unsigned long delay = SYSCTL_RCGC2_R;

    // Unlock PF0
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;

    // Configure directions
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED); // outputs
    GPIO_PORTF_DIR_R &= ~(SW1 | SW2);                     // inputs

    // Enable digital function
    GPIO_PORTF_DEN_R |= RED_LED | BLUE_LED | GREEN_LED | SW1 | SW2;

    // Pull-up resistors
    GPIO_PORTF_PUR_R |= SW1 | SW2;

    // SysTick setup
    ST_RELOAD_R = RELOAD_VALUE;
    ST_CTRL_R |= (1<<2);  // use system clock

    while(1) {
        // Read switches (active-low)
        uint8_t switches = ~GPIO_PORTF_DATA_R & (SW1 | SW2);


        // Call delay 4 times
        int i;
        for (i = 0; i < DELAY_CNT; i++) {
            SysTick_Wait();
        }

        // Toggle RED and set GREEN and/or BLUE based on switch status
        // Toggle RED LED
        GPIO_PORTF_DATA_R ^= RED_LED;

        // BLUE controlled by SW1
        if (switches & SW1)
            GPIO_PORTF_DATA_R |= BLUE_LED;   // ON
        else
            GPIO_PORTF_DATA_R &= ~BLUE_LED;  // OFF

        // GREEN controlled by SW2
        if (switches & SW2)
            GPIO_PORTF_DATA_R |= GREEN_LED;  // ON
        else
            GPIO_PORTF_DATA_R &= ~GREEN_LED; // OFF

    }
}
