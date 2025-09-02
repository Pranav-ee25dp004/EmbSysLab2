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
#define DELAY 0.5
#define DELAY_CNT 4
#define RELOAD_VALUE (uint32_t)((DELAY*CLK_FREQ)-1)
#define ENABLE 1


volatile int tick_count =0;

// SysTick Interrupt Service Routine
void SysTickIntHandler(void) {

    tick_count++;
    if(tick_count == 4){
        // Toggle RED LED
        GPIO_PORTF_DATA_R ^= RED_LED;
        tick_count = 0;
    }
}

void Systick_Init(){

    // SysTick setup
    ST_RELOAD_R = RELOAD_VALUE;
    ST_CURRENT_R = 0x00;
    ST_CTRL_R = (1<<2) | (1<<1) | 1;  // // ENABLE + TICKINT
    //__asm(" CPSIE i");   // enable global interrupts

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

    // Initialize SysTick
    Systick_Init();
    __asm(" CPSIE i");   // enable interrupts globally



    while(1) {
        // Read switches (active-low)
        uint8_t switches = ~GPIO_PORTF_DATA_R & (SW1 | SW2);


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
