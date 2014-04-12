#ifndef timer_h
#define timer_h
/*****************************************************************************

  Timer module

  This module implements timer related software

  David Annett, david@lynxinnovation.com, 10 February 2012

****************************************************************************/

#include "CommonTypes.h"
#include <avr/interrupt.h>

// ------- Constants ---------


/*
  Timer_Init routine

  This routine initialise the timer module.
*/
void Timer0_Init(void);


/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
*/
void DelayMS(Uint16 Time);

#endif
