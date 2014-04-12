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
#include "Globals.h"
#include "LampSerial.h"
#include "BoardComms.h"


//====================== Constants ============================================


//====================== Types ================================================


//====================== Public vars ==========================================

volatile Uint16 RS485TXDrop;     // Timer for RS485 transmission drop
volatile Uint16 RS485RXTimeout;  // Timer for RS485 packet reset



//====================== Vars =================================================

volatile Uint16 CountDown;
volatile Uint16 CountUp;
volatile Uint8  PulseScaler;

//====================== Functions ============================================



/*
  Timer_Init routine

  This routine initialise the timer module.
 */
void Timer_Init(void)
{
  // Timer 0, 36kHz pulses using CTC mode and pin toggle while blasting.  Tick is Xtal/8 = 400nS.
  CountUp = 0;
  CountDown = 0;
  // Timer 1, normal mode. Tick is Xtal/8 = 400nS period for maximum delay of 26mS

  TCCR1A = 0;
  TCCR1B = (1 << CS11);                                 // Prescaler divide by 8 = 2.5MHz/400nS tick
  TIMSK1 =  (1 << OCIE1A) | (1 << OCIE1B);              // Enable delay
  TIFR1 =   (1 << OCF1A) | (1 << OCF1B) | (1 << ICF1);  // Clear off interrupt flags
}



/*
  DelayMS routine

  This routine will delay for the supplied number of milliseconds.
 */
void DelayMS(Uint16 Time)
{
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
  Timer 1 output compare B interrupt service

  This timer is used for a 1mS periodic interrupt.
 */
ISR(TIMER1_COMPB_vect)
{
  OCR1B = OCR1B + 3000;   // Reload for next tick, 2500 x 400nS = 1mS

  // Timer user for DelayMS

  if (CountDown)
    CountDown--;

  // Timer used for Timer_Clear and Timer_Read

  CountUp++;

  // Do count down for RS485 transmit stop

  if (RS485TXDrop)
    if (!--RS485TXDrop) {
      ClearBit (RS485DEPort, RS485DE);      // Turn off RS485 driver
      ClearBit (RS485DEPort,RS485DE2);
      BCTXBusy = false;
    }

  // Do count down for RS485 receive timeout

  if (RS485RXTimeout)
    if (!--RS485RXTimeout) {
      BCRXPos = 0;                          // Dump any data and start again
    }

}



/*
  Timer 0 output compare A interrupt service

  This timer is used for software PWM timing.
 */
ISR(TIMER0_COMPA_vect)
{

}


/*
  Timer 1 output compare A interrupt service

  This timer is used for IR send bit timing.
 */
ISR(TIMER1_COMPA_vect)
{

}
