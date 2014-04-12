/*****************************************************************************

  Timer module

  This module implements timer related software.
    Timer 0 implements the 36kHz IR carrier.
    Timer 1 implements the IR bit periods and delay routines.

  David Annett, david@annett.co.nz, 2 August 2008

 ****************************************************************************/

#include <avr/interrupt.h>
#include "Globals.h"
#include "Timer.h"

//------------ VARS -------------------------------------



/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  // Timer 1, normal mode. Tick is Xtal/8 = 400nS period for maximum delay of 26mS

  TCCR1A = 0;
  TCCR1B = (1 << CS11);                                 // Prescaler divide by 8 = 2.5MHz/400nS tick
  //TIMSK1 = (1 << OCIE1A);                              // Enable delay
  TIFR1  = 0;//(1 << OCF1A) | (1 << OCF1B) | (1 << ICF1);  // Clear off interrupt flags
}



/*
  Delay routine

  This routine will wait for the supplied number of 400nS periods.
  It uses timer 1 output compare B.
 */
void Delay(Uint16 Time)
{
  Uint8 sreg;


  TIFR1 = 1 << OCF1B;  // Clear timer 1 output compare B flag

  // Set target time

  sreg = SREG;                // Save Global Interrupt Flag
  cli();                      // Disable interrupts
  OCR1B = TCNT1 + Time;   // Set target time
  SREG = sreg;                // Restore Global Interrupt Flag

  while (!(TIFR1 & (1 << OCF1B)));  // Wait for timer 1 output compare B flag
}



/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
 */
void DelayMS(Uint16 Time)
{
  Uint16 Now;


  for (Now = 0; Now < Time; Now++)
    Delay(2500);
}



/*
  Timer 0 output compare A interrupt service

  This timer is used for PWN timing.
 */
ISR(TIMER0_COMPA_vect)
{
  //Phase++;
}


/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
 */
ISR(TIMER1_COMPA_vect)
{
}
