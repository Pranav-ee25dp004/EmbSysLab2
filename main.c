/*
 * ESL Lab, IIT Dharwad - Lab 2 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem I: Control LEDs on TM4C123GH6PM using two onboard pushbuttons.
 *   - SW1 (PF4) → Red LED (PF1)
 *   - SW2 (PF0) → Blue LED (PF2)
 *   - Both pressed → Red + Blue
 */

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

/* Pin masks */
#define RED_LED   0x02  // PF1
#define BLUE_LED  0x04  // PF2
#define SW2       0x01  // PF0
#define SW1       0x10  // PF4
#define LED_MASK  (RED_LED | BLUE_LED) // Only PF1 & PF2

int main(void)
{
    /* 1. Enable clock for Port F */
    SYSCTL_RCGC2_R  |= 0x20;  // Enable Port F clock; we can also use SYSCTL_RCGCGPIO_R
    volatile unsigned long delay = SYSCTL_RCGC2_R;  // Section 5.2.6: wait 3 cycles after RCGC enable before GPIO access
                                                    // dummy read to allow clock to stabilize
    /* 2. Unlock PF0 and commit changes */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;  // Allow changes to PF0–PF4

    /* 3. Configure directions: PF1 & PF2 outputs, PF0 & PF4 inputs */
    GPIO_PORTF_DIR_R &= ~(SW1 | SW2);  // PF0 & PF4 inputs
    GPIO_PORTF_DIR_R |= LED_MASK;      // PF1 & PF2 outputs

    /* 4. Enable digital function for PF0, PF1, PF2, PF4 */
    GPIO_PORTF_DEN_R |= (SW1 | SW2 | LED_MASK);

    /* 5. Enable pull-up resistors for switches (active-low) */
    GPIO_PORTF_PUR_R |= (SW1 | SW2);

    while (1)
    {
        /* Read switches (invert because active-low) */
        uint8_t switches = ~GPIO_PORTF_DATA_R & (SW1 | SW2);
        uint8_t leds = 0;

        switch (switches)
        {
            case SW2:            // Only SW2 pressed → Blue
                leds = BLUE_LED;
                break;
            case SW1:            // Only SW1 pressed → Red
                leds = RED_LED;
                break;
            case (SW1 | SW2):    // Both pressed → Red + Blue
                leds = RED_LED | BLUE_LED;
                break;
            default:             // None pressed → Off
                leds = 0;
                break;
        }

        /* Update only LED bits, keep inputs untouched */
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~LED_MASK) | leds;
    }
}
