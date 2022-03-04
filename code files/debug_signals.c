#include "gpio_defs.h"
#include <MKL25Z4.h>

void init_debug_signals(void) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTD_MASK; /* enable clock for port D */

	/* Select GPIO for pins connected to debug signals*/
	PORTD->PCR[DBG_ISR_POS] |= PORT_PCR_MUX(1);	
	PORTD->PCR[DBG_MAIN_POS] |= PORT_PCR_MUX(1);	
	
	/* Set bits to outputs */
	PTD->PDDR |= MASK(DBG_ISR_POS) | MASK(DBG_MAIN_POS); 
	
	/* Clear output signals initially */
	PTD->PCOR |= MASK(DBG_ISR_POS) | MASK(DBG_MAIN_POS);
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
