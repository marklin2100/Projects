/*****************************************************************************

    Timer module

    This module implments timer related software.
  Timer 0 implements the 36kHz IR carrier.
  Timer 1 implements the IR bit periods and delay routines.

    David Annett, david@annett.co.nz, 2 August 2008

 ******************************************************************************/

//====================== Includes =============================================

#include <avr/interrupt.h>
#include "Timer.h"


//====================== Constants ============================================


//====================== Types ================================================


//====================== Public vars ==========================================


//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;


//====================== Functions ============================================



/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  TCCR0A = 0;
  TCCR0B = ((1 << CS00) | (1 << CS01)); // Prescaler divide by 64 = 9600000 / 64 =  150000 tick
  TIMSK0 = (1 << OCIE0A); // imer/Counter0 Output Compare Match A Interrupt Enable
  TIFR0 =   (1 << OCF0A) | (1 << OCF0B) | (1 << TOV0);  // Clear off interrupt flags
}



/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
 */
void DelayMS(Uint16 Time)
{
	//Uint32 i;
  CountDown = Time;   // CountDown is decremented in ISR every millisecond
  while(CountDown);
}



/*
  Timer_Clear routine

  This routine clears the 1mS timer.
 */
void Timer_Clear(void)
{
  CountUp = 0;
}



/*
  Timer_Read routine

  This routine reads the current value of the 1mS timer.
 */
Uint16 Timer_Read(void)
{
  return CountUp;
}


//====================== Interrupt service routines============================

/*
  Timer 0 output compare A interrupt service

  This timer is used for software PWM timing.
 */
ISR(TIM0_COMPA_vect)
{
  OCR0A = OCR0A + 150;   // Reload for next tick, 9600000 / 64 = 150000, 150 = 1ms
  // Timer user for DelayMS
  if (CountDown)
    CountDown--;

  // Timer used for Timer_Clear and Timer_Read
  CountUp++;
}
