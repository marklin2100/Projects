/*****************************************************************************

  Timer module

  This module implements timer related software.
  Timer 0 implements the 1mS interrupt.

  David Annett, david@lynxinnovation.com, 10 February 2012

****************************************************************************/

#include "Timer.h"
#include "Globals.h"


// Constants

#define T0Reload  (255-40)


// Types


// Vars

volatile Uint16 Time;



/*
  Timer0 Overflow interrupt
*/
ISR(TIM0_OVF_vect)
{
  TCNT0 = T0Reload; // Reload counter
  if (Time)
    Time--;
}



/*
  Timer0 Compare A interrupt

  Should be called every 1mS
*/
ISR(TIM0_COMPA_vect)
{
}



/*
  Timer0 Compare B interrupt
*/
ISR(TIM0_COMPB_vect)
{
}



/*
  Timer0_init routine
 */
void Timer0_Init()
{
  TCCR0B = (1<<CS02);  // Timer0 settings, prescaler = clkio/256
//  OCR0A = T0Reload;    // Init counter
  TCNT0 = T0Reload;    // Init counter

//  TIMSK0 = (1<<OCIE0A); // Timer0 output compare A Interrupt Enable
  TIMSK0 = (1<<TOIE0); // Timer0 Overflow Interrupt Enable
  TIFR0  = (1 << OCF0A) | (1 << OCF0B) | (1 << TOV0);  // Clear off interrupt flags
}



/*
  DelayMS routine

  This routine will delay for the indicated number milliseconds.
  It will attempt to save power while waiting by entering the 'Idle' sleep mode.
 */
void DelayMS(Uint16 Delay)
{
  Time =  Delay;
  while (Time) {
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  }
}
