/*
 * ESL Lab, IIT Dharwad - Lab 4 Assignment
 * Author(s): 1) Pranav Deshpande (EE25DP004), 2) Sujit Nayak (EE25MR009)
 * Problem: Toggle onboard RGB LEDs (PF1–PF3) on TM4C123GH6PM
 *          - Use custom macros for GPIO register access
 *          - Use bit-specific addressing (read-modify-write safe)
 *          - Blink LEDs one by one: Red → Blue → Green
 */

#include <stdint.h>

// Base address of GPIO Port F
#define GPIOF_BASE 0x40025000

// LED bit positions
#define RED_LED    (1U << 1)   // PF1
#define BLUE_LED   (1U << 2)   // PF2
#define GREEN_LED  (1U << 3)   // PF3

// LED DATA register macros (derived from datasheet formula: BASE + (MASK << 2))
#define RED_LED_DATA    (*((volatile uint32_t *)(GPIOF_BASE + (RED_LED << 2))))
#define BLUE_LED_DATA   (*((volatile uint32_t *)(GPIOF_BASE + (BLUE_LED << 2))))
#define GREEN_LED_DATA  (*((volatile uint32_t *)(GPIOF_BASE + (GREEN_LED << 2))))

// LED ON/OFF macros
#define RED_LED_ON      (RED_LED_DATA = RED_LED)
#define RED_LED_OFF     (RED_LED_DATA = 0x00)

#define BLUE_LED_ON     (BLUE_LED_DATA = BLUE_LED)
#define BLUE_LED_OFF    (BLUE_LED_DATA = 0x00)

#define GREEN_LED_ON    (GREEN_LED_DATA = GREEN_LED)
#define GREEN_LED_OFF   (GREEN_LED_DATA = 0x00)

// System control registers
#define SYSCTL_RCGCGPIO_R  (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_PRGPIO_R    (*((volatile uint32_t *)0x400FEA08))

// GPIO Port F registers
#define GPIO_PORTF_DIR_R   (*((volatile uint32_t *)(GPIOF_BASE + 0x400)))
#define GPIO_PORTF_DEN_R   (*((volatile uint32_t *)(GPIOF_BASE + 0x51C)))
#define GPIO_PORTF_LOCK_R  (*((volatile uint32_t *)(GPIOF_BASE + 0x520)))
#define GPIO_PORTF_CR_R    (*((volatile uint32_t *)(GPIOF_BASE + 0x524)))

// Delay (~1ms per loop at 16 MHz)
void delayMs(int n){
    int i,j;
    for(i=0;i<n;i++)
        for(j=0;j<3180;j++){}
}

int main(void){
    // 1. Enable Port F clock
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0);

    // 2. Unlock PF0, commit PF0–PF4
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x1F;

    // 3. Set PF1–PF3 as outputs and enable digital
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED);
    GPIO_PORTF_DEN_R |= (RED_LED | BLUE_LED | GREEN_LED);

    // 4. Blink loop
    while(1){
        RED_LED_ON; delayMs(250);
        RED_LED_OFF; delayMs(250);

        BLUE_LED_ON; delayMs(250);
        BLUE_LED_OFF; delayMs(250);

        GREEN_LED_ON; delayMs(250);
        GREEN_LED_OFF; delayMs(250);
    }
}
