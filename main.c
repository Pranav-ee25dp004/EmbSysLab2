/*
 * ESL Lab, IIT Dharwad - Lab 2 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem II: Sequential LED color change on TM4C123GH6PM using a single onboard pushbutton.
 *   - SW1 (PF4) → cycle through Red (PF1) -> Green (PF3) -> Blue (PF2) on each button press
 *   - Implements edge detection (falling edge) to trigger color change on button press
 *   - Includes simple software debouncing
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

/* Pin definitions */
#define RED_LED    0x02  // PF1
#define GREEN_LED  0x08  // PF3
#define BLUE_LED   0x04  // PF2
#define SW1        0x10  // PF4 (user switch)

int main(void)
{
    /* 1. Enable clock to GPIO Port F */
    SYSCTL_RCGC2_R |= 0x00000020;

    /* 2. Unlock GPIOCR register to allow changes on PF0-4 */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;

    /* 3. Set digital enable and direction for PORTF pins */
    GPIO_PORTF_DEN_R = 0x1E;            // Enable PF1-PF3 (LEDs) and PF4 (switch)
    GPIO_PORTF_DIR_R = 0x0E;            // PF1-PF3 as output (LEDs), PF4 as input (switch)
    GPIO_PORTF_PUR_R = 0x10;            // Enable pull-up resistor for PF4 switch

    int i;                               // Variable for software delay loop
    int edgeCnt = 0;                            // LED sequence counter
    int prev = 0, present = 0, toggleState;           // Variables to detect switch edge

    /* Start with Red LED ON */
    GPIO_PORTF_DATA_R = RED_LED;

    while(1)
    {
        /* Read current state of switch PF4 */
        present = GPIO_PORTF_DATA_R & SW1;

        /* Detect falling edge (button press) */
        if(prev == 0x10 & present == 0x0)
        {
            edgeCnt = edgeCnt + 1;                    // Increment LED sequence index

            /* Simple software debouncing */
            for(i = 0; i < 800; i++) {}
        }

         toggleState = edgeCnt % 3;

        /* Update LED output based on sequence index modulo 3 */
        if(toggleState == 0)
        {
            GPIO_PORTF_DATA_R = RED_LED;   // Red LED
        }
        if(toggleState == 1)
        {
            GPIO_PORTF_DATA_R = GREEN_LED; // Green LED
        }
        if(toggleState == 2)
        {
            GPIO_PORTF_DATA_R = BLUE_LED;  // Blue LED
        }

        /* Remember previous switch state for edge detection */
        prev = present;
    }

}
