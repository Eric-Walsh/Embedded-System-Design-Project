#ifndef LEDS_H
#define LEDS_H

// PRoject LEDS position on port E
#define LED1_POS (0)
#define LED2_POS (1)

// function prototypes
void init_LEDs(void);
void control_LEDs(unsigned int led1_on, unsigned int led2_on);
void toggle_LEDs(unsigned int led1_on, unsigned int led2_on);

#endif 

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
