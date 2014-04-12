#ifndef timer_h
#define timer_h
/*****************************************************************************

    Timer module

    This module implments timer related software

    David Annett, david@annett.co.nz, 2 August 2008

****************************************************************************/

#include "CommonTypes.h"
#include <avr/interrupt.h>

// ------- Constants ---------

//------------ VARS -------------------------------------




/*
  Timer_Init routine

  This routine initialise the timer module.
*/
void Timer_Init(void);


/*
  Delay routine

  This routine will wait for the supplied number of 400nS periods.
  It uses timer 1 output compare B.
*/
void Delay(Uint16 Time);


/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
*/
void DelayMS(Uint16 Time);

#endif
