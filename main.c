/*
 * ESL Lab, IIT Dharwad - Lab 5 Part1 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem: Toggle the RED LED every 500 ms using SysTick on TM4C123GH6PM
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

// RED LED is connected to PF1
#define RED_LED (1<<1)

// SysTick registers
#define ST_CTRL_R    (*((volatile uint32_t *)0xE000E010))  // Control and Status Register
#define ST_RELOAD_R  (*((volatile uint32_t *)0xE000E014))  // Reload Value Register
#define ST_CURRENT_R (*((volatile uint32_t *)0xE000E018))  // Current Value Register

#define CLK_FREQ 16000000       // System clock frequency = 16 MHz
#define DELAY 0.5               // Desired delay in seconds
#define RELOAD_VALUE (uint32_t)((DELAY * CLK_FREQ) - 1)  // SysTick reload value for 500ms
#define ENABLE 1

int main(void) {
    int x;

    // Enable clock for Port F (0x20 corresponds to Port F)
    SYSCTL_RCGC2_R |= 0x20;
    volatile unsigned long delay = SYSCTL_RCGC2_R;  // Delay for clock to stabilize

    // Set PF1 as output (RED LED)
    GPIO_PORTF_DIR_R |= RED_LED;
    GPIO_PORTF_DEN_R |= RED_LED;  // Enable digital function for PF1

    // Configure SysTick
    ST_RELOAD_R = RELOAD_VALUE;   // Set reload value for 500 ms
    ST_CTRL_R |= (1 << 2);        // Select system clock (16 MHz) as SysTick clock source

    while (1) {
        // Start SysTick timer
        ST_CTRL_R |= 1;  // Enable SysTick

        x = 0;
        // Wait until SysTick count flag is set (timer expires)
        while (x == 0) {
            x = ST_CTRL_R & (1 << 16);  // COUNT flag is bit 16
        }

        // Toggle RED LED
        GPIO_PORTF_DATA_R ^= RED_LED;

        // Reset and reload SysTick for next cycle
        ST_CTRL_R &= ~ENABLE;  // Disable SysTick
        ST_CURRENT_R = 0;      // Clear current value
        ST_CTRL_R |= ENABLE;   // Enable SysTick again
    }
}
