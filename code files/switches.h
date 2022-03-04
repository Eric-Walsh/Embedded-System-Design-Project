#ifndef SWITCHES_H
#define SWITCHES_H
#include "gpio_defs.h"

// Switches is on port A for interrupt support
#define SW1_POS (1) 
#define SW2_POS (2)
#define SW3_POS (3)
#define SW4_POS (4)

// Function prototypes
extern void init_switch(void);
void init_Switch(void);

// Shared variables
extern volatile unsigned count;

#endif
// *****************************************************************************   
