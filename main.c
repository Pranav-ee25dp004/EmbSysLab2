#include <stdint.h>
#include "tm4c123gh6pm.h"
void delayMs(int n);
int main(void) {
// 1. Enable clock to GPIOF
SYSCTL_RCGCGPIO_R |= 0x20; // Enable clock for Port F
while((SYSCTL_PRGPIO_R & 0x20) == 0); // Wait until Port F is ready
// 2. Unlock PF0 (if needed) and configure pins
GPIO_PORTF_LOCK_R = 0x4C4F434B; // Unlock GPIO Port F
GPIO_PORTF_CR_R |= 0x1F; // Enable changes to PF4-0
GPIO_PORTF_DIR_R |= 0x02; // Set PF1 (Red LED) as output
GPIO_PORTF_DEN_R |= 0x02; // Enable digital function for PF1
// 3. Blink loop
while(1) {
GPIO_PORTF_DATA_R |= 0x02; // Turn on Red LED (PF1)
delayMs(500); // Delay 500 ms
GPIO_PORTF_DATA_R &= ~0x02; // Turn off Red LED (PF1)
delayMs(500); // Delay 500 ms
}
}
// Simple delay function (~1ms per iteration at 16MHz)
void delayMs(int n) {
int i, j;
for(i = 0; i < n; i++)
for(j = 0; j < 3180; j++) {} // Approx 1ms delay
}
