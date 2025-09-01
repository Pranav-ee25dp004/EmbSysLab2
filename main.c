/*
 * ESL Lab, IIT Dharwad - Lab 4 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem: Toggle onboard RGB LEDs (PF1–PF3) on TM4C123GH6PM
 *          - Use custom macros for GPIO register access
 *          - Use read-modify-write (RMW) approach to not disturb other bits
 *          - Blink LEDs one by one: Red → Blue → Green
 */

#include <stdint.h>

// Base address of GPIO Port F
#define GPIOF_BASE 0x40025000

// Macro to access the Port F DATA register (all pins PF0-PF7)
// Derived using datasheet formula: address = GPIO_BASE + (MASK << 2)
// MASK = 0xFF for full port (PF0–PF7)
#define GPIOF_DATA (*((volatile uint32_t *)(GPIOF_BASE + (0xFF << 2))))

// System control registers for GPIO clock
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *)0x400FE608))  // Run mode clock gating
#define SYSCTL_PRGPIO_R    (*((volatile uint32_t *)0x400FEA08))  // Peripheral ready

// GPIO Port F configuration registers
#define GPIO_PORTF_DIR_R   (*((volatile uint32_t *)(GPIOF_BASE + 0x400))) // Direction
#define GPIO_PORTF_DEN_R   (*((volatile uint32_t *)(GPIOF_BASE + 0x51C))) // Digital enable
#define GPIO_PORTF_LOCK_R  (*((volatile uint32_t *)(GPIOF_BASE + 0x520))) // Lock register
#define GPIO_PORTF_CR_R    (*((volatile uint32_t *)(GPIOF_BASE + 0x524))) // Commit register

// LED pin macros (bit masks)
#define RED_LED    (1U << 1)   // PF1
#define BLUE_LED   (1U << 2)   // PF2
#define GREEN_LED  (1U << 3)   // PF3

// Simple blocking delay (~1ms per iteration at 16 MHz)
void delayMs(int n){
    int i,j;
    for(i=0;i<n;i++)
        for(j=0;j<3180;j++){}
}

int main(void){
    // 1. Enable clock to GPIO Port F
    SYSCTL_RCGCGPIO_R |= 0x20;              // Set bit 5 to enable Port F
    while((SYSCTL_PRGPIO_R & 0x20) == 0);   // Wait until Port F is ready

    // 2. Unlock PF0 (if needed) and allow changes to PF4-0
    GPIO_PORTF_LOCK_R = 0x4C4F434B;         // Unlock GPIOCR register
    GPIO_PORTF_CR_R |= 0x1F;                // Allow commit for PF0–PF4

    // 3. Set PF1–PF3 as outputs (LEDs) and enable digital function
    GPIO_PORTF_DIR_R |= RED_LED | BLUE_LED | GREEN_LED;  // Output pins
    GPIO_PORTF_DEN_R |= RED_LED | BLUE_LED | GREEN_LED;  // Enable digital pins

    // 4. Main loop: blink Red, Blue, Green LEDs one by one
    while(1){
        // Turn on Red LED (PF1)
        GPIOF_DATA |= RED_LED;
        delayMs(500);
        GPIOF_DATA &= ~RED_LED;  // Turn off Red LED
        delayMs(500);

        // Turn on Blue LED (PF2)
        GPIOF_DATA |= BLUE_LED;
        delayMs(500);
        GPIOF_DATA &= ~BLUE_LED; // Turn off Blue LED
        delayMs(500);

        // Turn on Green LED (PF3)
        GPIOF_DATA |= GREEN_LED;
        delayMs(500);
        GPIOF_DATA &= ~GREEN_LED; // Turn off Green LED
        delayMs(500);
    }
}
